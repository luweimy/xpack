//
//  xpack
//
//  Created by Luwei.
//  Copyright (c) 2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy
//

#include "xpack-util.h"
#include "xpack.h"
#include "xpack-base.h"
#include "xpack-context.h"
#include "xpack-stream.h"
#include "xpack-name.h"
#include "xpack-header.h"
#include "xpack-hash.h"
#include "xpack-block.h"
#include "xpack-signature.h"
#include "xpack-content.h"
#include "torch/torch.h"
#include <unordered_map>

using namespace xpack;

// Utils

std::vector<uint32_t> Utils::GetHashIDChainByEntryName(Context *ctx, const std::string &name)
{
    assert(ctx);
    
    uint32_t hashid = ctx->HashMaker(name, 0);
    MetaHash *metahash = ctx->hash->GetById(hashid);
    std::vector<uint32_t> ret;
    
    // Entry not exists
    if (!metahash) {
        return ret;
    }
    
    // Follow the conflict chain
    while (metahash->flags & int(HashFlags::Conflict)) {
        ret.push_back(metahash->hash);
        uint32_t hashid = ctx->HashMaker(name, metahash->salt);
        metahash = ctx->hash->GetById(hashid);
        assert(metahash); 
    }
    ret.push_back(metahash->hash);
    return std::move(ret);
}

std::vector<uint32_t> Utils::GetAllHashIDMarkedConflict(Context *ctx)
{
    assert(ctx);
    std::vector<uint32_t> ret;
    for (auto x : ctx->hash->GetMetaHashMap()->GetIterator()) {
        MetaHash *metahash = x.second;
        if (metahash->conflict_refc > 0) {
            ret.push_back(metahash->hash);
        }
    }
    return std::move(ret);
}

std::vector<int32_t> Utils::GetBlockIndexChainByMetablock(Context *ctx, MetaBlock *metablock)
{
    assert(ctx && metablock);
    
    std::vector<int32_t> ret;
    if ((metablock->flags & int(BlockFlags::NotStart))) {
        return ret;
    }
    
    ret.push_back(ctx->block->ConvertBlockPtrToIndex(metablock));
    
    auto cur = metablock;
    while (cur->next_index >= 0) {
        ret.push_back(cur->next_index);
        cur = ctx->block->GetByIndex(cur->next_index);
    }
    
    return ret;
}

std::string Utils::GetNameByBlockIndex(Context *ctx, int32_t index)
{
    assert(ctx);
    for (auto x : ctx->hash->GetMetaHashMap()->GetIterator()) {
        MetaHash *metahash = x.second;
        int32_t cursorindex = metahash->block_index;
        while (cursorindex >= 0) {
            if (cursorindex == index) {
                return ctx->name->GetName(metahash);
            }
            cursorindex = ctx->block->GetByIndex(cursorindex)->next_index;
        }
    }
    return std::string();
}

// DumpUtils

std::string DumpUtils::DumpHashIDChainByEntryName(Context *ctx, const std::string &name)
{
    std::vector<uint32_t> ret = Utils::GetHashIDChainByEntryName(ctx, name);
    
    std::string display;
    for (int i = 0; i < ret.size(); i++) {
        display += torch::String::Format("%llu", ret[i]);
        if (i < ret.size() - 1) {
            display += "->";
        }
    }
    return display;
}

std::string DumpUtils::DumpAllHashIDMarkedConflict(Context *ctx)
{
    std::vector<uint32_t> ret = Utils::GetAllHashIDMarkedConflict(ctx);
    
    std::string display;
    for (int i = 0; i < ret.size(); i++) {
        display += torch::String::Format("%llu", ret[i]);
        if (i < ret.size() - 1) {
            display += ", ";
        }
    }
    return display;
}

std::string DumpUtils::DumpAllHashIDChain(Context *ctx)
{
    std::string display;

    HashSegment::MetaHashMap *hashmap = ctx->hash->GetMetaHashMap();
    for (auto x : hashmap->GetIterator()) {
        MetaHash *metahash = x.second;
        assert(!(metahash->flags & int(HashFlags::Unused)));
        if (metahash->flags & int(HashFlags::Conflict)) {
            continue;
        }
        std::string name = ctx->name->GetName(metahash);
        display += name + " : " + DumpUtils::DumpHashIDChainByEntryName(ctx, name) + '\n';
    }
    return display;
}

std::string DumpUtils::DumpData(Context *ctx, uint32_t offset, uint32_t size)
{
    assert(ctx);
    torch::Data d;
    d.ReSize(size);
    ctx->stream->GetContent(d, ctx->offset + offset);
    return std::move(d.ToString());
}

std::string DumpUtils::DumpData(Context *ctx)
{
    assert(ctx);
    auto metaheader = ctx->header->Metadata();
    return DumpUtils::DumpData(ctx, metaheader->content_offset, metaheader->content_size);
}

std::string DumpUtils::DumpBlock(Context *ctx, MetaBlock *metablock)
{
    assert(ctx && metablock);
    uint32_t offset = metablock->offset;
    uint32_t size = metablock->size;
    int32_t  next_index = metablock->next_index;
    uint8_t  flags = metablock->flags;
    std::string stringflags;
    std::unordered_map<int, std::string> flagsmap {
        {int(BlockFlags::UnusedContent), "UnusedContent"},
        {int(BlockFlags::UnusedBlock), "UnusedBlock"},
        {int(BlockFlags::NotStart), "NotStart"},
    };
    for (auto x : flagsmap) {
        stringflags += (metablock->flags & x.first) ? x.second + ",": "";
    }
    
    int32_t blockindex = ctx->block->ConvertBlockPtrToIndex(metablock);
    std::string blockname = Utils::GetNameByBlockIndex(ctx, blockindex);
    
    std::string display;
    std::vector<std::string> keys = {"offset", "size", "next_index", "flags"};
    std::vector<std::string> vals = {
        torch::String::Format("%u", offset),
        torch::String::Format("%u(%s)", size, torch::ByteToHumanReadableString(size).c_str()),
        torch::String::Format("%d(index:%d, name:%s)", next_index, blockindex, blockname.c_str()),
        torch::String::Format("%d(%s)[%s]", flags, torch::ToBinary<uint8_t>(flags).c_str(), stringflags.c_str()),
    };
    for (int i = 0; i < keys.size(); i++) {
        display += torch::String::RightPad(keys[i], KEY_LENGTH, ' ') + ": " + vals[i] + '\n';
    }
    return display;
}

std::string DumpUtils::DumpBlock(Context *ctx, const std::string &name)
{
    assert(ctx);
    auto metahash = ctx->hash->QueryByName(name);
    auto mateblock = ctx->block->GetByIndex(metahash->block_index);
    assert(!(mateblock->flags & int(BlockFlags::NotStart)));
    
    std::string display;
    while (mateblock) {
        assert(!(mateblock->flags & int(BlockFlags::UnusedContent)));
        display += DumpUtils::DumpBlock(ctx, mateblock) + '\n';

        if (mateblock->next_index < 0) {
            break;
        }
        mateblock = ctx->block->GetByIndex(mateblock->next_index);
    }
    return display;
}

std::string DumpUtils::DumpBlock(Context *ctx, int32_t index)
{
    assert(ctx);
    return std::move(DumpUtils::DumpBlock(ctx, ctx->block->GetByIndex(index)));
}

std::string DumpUtils::DumpHash(Context *ctx, MetaHash *metahash)
{
    assert(ctx && metahash);
    uint32_t hash = metahash->hash;
    uint32_t crc = metahash->crc;
    int32_t  block_index = metahash->block_index;
    int32_t  unpacked_size = metahash->unpacked_size;
    uint32_t name_offset = metahash->name_offset;
    uint16_t name_size = metahash->name_size;
    uint8_t  conflict_refc = metahash->conflict_refc;
    uint8_t  salt = metahash->salt;
    uint8_t  flags = metahash->flags;
    
    std::string stringflags;
    std::unordered_map<int, std::string> flagsmap {
        {int(HashFlags::Unused), "Unused"},
        {int(HashFlags::Conflict), "Conflict"},
        {int(HashFlags::CryptoRC4), "CryptoRC4"},
        {int(HashFlags::Compressed), "Compressed"},
    };
    for (auto x : flagsmap) {
        stringflags += (metahash->flags & x.first) ? x.second + ",": "";
    }

    std::string display;
    std::vector<std::string> keys = {"hash", "crc", "block_index", "unpacked_size", "name_offset", "name_size", "conflict_refc", "salt", "flags"};
    std::vector<std::string> vals = {
        torch::String::Format("%u(0x%s)", hash, torch::ToHexHumanReadable<uint32_t>(hash).c_str()),
        torch::String::Format("%u(0x%s)", crc, torch::ToHexHumanReadable<uint32_t>(crc).c_str()),
        torch::String::Format("%d", block_index),
        torch::String::Format("%u(%s)", unpacked_size, torch::ByteToHumanReadableString(unpacked_size).c_str()),
        torch::String::Format("%u(name:%s)", name_offset, ctx->name->GetName(metahash).c_str()),
        torch::String::Format("%u", name_size),
        torch::String::Format("%d", conflict_refc),
        torch::String::Format("%d", salt),
        torch::String::Format("%d(%s)[%s]", flags, torch::ToBinary<uint8_t>(flags).c_str(), stringflags.c_str()),
    };
    for (int i = 0; i < keys.size(); i++) {
        display += torch::String::RightPad(keys[i], KEY_LENGTH, ' ') + ": " + vals[i] + '\n';
    }
    return display;
}

std::string DumpUtils::DumpHash(Context *ctx, const std::string &name)
{
    assert(ctx);
    return std::move(DumpUtils::DumpHash(ctx, ctx->hash->QueryByName(name)));
}

std::string DumpUtils::DumpHash(Context *ctx, const uint32_t hashid)
{
    assert(ctx);
    return std::move(DumpUtils::DumpHash(ctx, ctx->hash->GetById(hashid)));
}

std::string DumpUtils::DumpSignature(Context *ctx)
{
    assert(ctx);
    xpack::SignatureSegment *object = ctx->signature;
    uint64_t signature = object->Metadata()->signature;
    uint16_t version = object->Metadata()->version;
    
    std::string display;
    std::vector<std::string> keys = {"signature", "version"};
    std::vector<std::string> vals = {
        torch::String::Format("%llu(0x%s)", signature, torch::ToHexHumanReadable<uint64_t>(signature).c_str()),
        torch::String::Format("%d(%s)", version, torch::ToBinary<uint16_t>(version, 1).c_str())
    };
    for (int i = 0; i < keys.size(); i++) {
        display += torch::String::RightPad(keys[i], KEY_LENGTH, ' ') + ": " + vals[i] + '\n';
    }
    return display;
    
}

std::string DumpUtils::DumpHeader(Context *ctx)
{
    assert(ctx);
    xpack::HeaderSegment *object = ctx->header;
    uint32_t archive_size = object->Metadata()->archive_size;
    uint32_t content_offset = object->Metadata()->content_offset;
    uint32_t content_size = object->Metadata()->content_size;
    uint32_t block_offset = object->Metadata()->block_offset;
    uint32_t block_count = object->Metadata()->block_count;
    uint32_t hash_offset = object->Metadata()->hash_offset;
    uint32_t hash_count = object->Metadata()->hash_count;
    uint32_t name_size = object->Metadata()->name_size;
    size_t name_offset = hash_offset + hash_count * sizeof(MetaHash);
    
    std::string display;
    std::vector<std::string> keys = {
        "archive_size",
        "content_offset",
        "content_size",
        "block_offset",
        "block_count",
        "hash_offset",
        "hash_count",
        "name_size"
    };
    std::vector<std::string> vals = {
        torch::String::Format("%u(%s)", archive_size, torch::ByteToHumanReadableString(archive_size).c_str()),
        torch::String::Format("%u", content_offset),
        torch::String::Format("%u(%s)", content_size, torch::ByteToHumanReadableString(content_size).c_str()),
        torch::String::Format("%u", block_offset),
        torch::String::Format("%u(%u*%u=%u)", block_count, block_count, sizeof(MetaBlock), block_count * sizeof(MetaBlock)),
        torch::String::Format("%u", hash_offset),
        torch::String::Format("%u(%u*%u=%u)", hash_count, hash_count, sizeof(MetaHash), hash_count * sizeof(MetaHash)),
        torch::String::Format("%u(offset=%u)", name_size, name_offset),
    };
    for (int i = 0; i < keys.size(); i++) {
        display += torch::String::RightPad(keys[i], KEY_LENGTH, ' ') + ": " + vals[i] + '\n';
    }
    return display;
}

std::string DumpUtils::DumpBlocks(Context *ctx, bool containUnused)
{
    assert(ctx);
    std::string display;
    for (int i = 0; i < ctx->block->GetBlockNumber(); i++) {
        MetaBlock *metablock = ctx->block->GetByIndex(i);
        if (containUnused) {
            display += DumpUtils::DumpBlock(ctx, i) + '\n';
        }
        else if (!(metablock->flags & (int)BlockFlags::UnusedBlock) && !(metablock->flags & (int)BlockFlags::UnusedContent)) {
            display += DumpUtils::DumpBlock(ctx, i) + '\n';
        }
    }
    return display;
}

std::string DumpUtils::DumpBlockUnused(Context *ctx)
{
    assert(ctx);
    std::string display;
    auto blockUnusedPool = ctx->block->GetBlockReuser();
    for (auto index : blockUnusedPool->GetIterator()) {
        display += DumpUtils::DumpBlock(ctx, index) + '\n';
    }
    return display;
}

std::string DumpUtils::DumpBlockContentUnused(Context *ctx)
{
    assert(ctx);
    std::string display;
    auto contentUnusedPool = ctx->block->GetContentReuser();
    for (auto x : contentUnusedPool->GetOrderdByOffsetIncreaseIterator()) {
        display += DumpUtils::DumpBlock(ctx, x.value) + '\n';
    }
    return display;
}

std::string DumpUtils::DumpHashs(Context *ctx)
{
    assert(ctx);
    std::string display;
    for (auto x: ctx->hash->GetMetaHashMap()->GetIterator()) {
        MetaHash *metahash = x.second;
        display += DumpUtils::DumpHash(ctx, metahash) + '\n';
    }
    return display;
}

std::string DumpUtils::DumpNames(Context *ctx)
{
    assert(ctx);
    torch::Data names = ctx->name->GetRawNames();
    return names.ToString() + '\n';
}


