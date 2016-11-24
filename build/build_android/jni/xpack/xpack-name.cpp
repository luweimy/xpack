//
//  xpack
//
//  Created by Luwei.
//  Copyright (c) 2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy
//

#include "xpack-name.h"
#include "xpack-header.h"
#include "xpack-hash.h"
#include "xpack-context.h"
#include "xpack-stream.h"
#include "xpack-base.h"
#include "xpack-def.h"
#include <assert.h>

using namespace xpack;

NameSegment::NameSegment(Context *ctx)
:m_context(ctx)
{
    assert(ctx);
    torch::HeapCounterRetain();
}

NameSegment::~NameSegment()
{
    torch::HeapCounterRelease();
    m_context = nullptr;
}

bool NameSegment::ReadFromStream()
{
    // No name section
    if (!this->IsValid()) { 
        return true;
    }

    Context *ctx = m_context;

    MetaHeader *metaheader = ctx->header->Metadata();
    uint32_t offset = ctx->offset + metaheader->hash_offset + metaheader->hash_count * sizeof(MetaHash);
    uint32_t size   = ctx->header->Metadata()->name_size;

    m_names.Alloc(size);
    if (!ctx->stream->GetContent(m_names, offset)) {
        return false;
    }
    if (m_names.GetSize() < size) {
        XPACK_ERROR(xpack::Error::Format);
        return false;
    }

    // Decrypt names data
    ctx->crypto->CryptoNoCopy((unsigned char*)m_names.GetBytes(), (int)m_names.GetSize()); 

    return true;
}

bool NameSegment::WriteToStream()
{
    Context *ctx = m_context;
    ctx->header->UpdateMetadata();
    MetaHeader *metaheader = ctx->header->Metadata();
    torch::Data wb = m_names;

    // Encrypt names data
    ctx->crypto->CryptoNoCopy((unsigned char*)wb.GetBytes(), (int)wb.GetSize()); 

    uint32_t baseoffset = ctx->offset + metaheader->hash_offset + metaheader->hash_count * sizeof(MetaHash);
    uint32_t size = (uint32_t)m_names.GetSize();
    if (!ctx->stream->PutContent(wb.GetBytes(), size, baseoffset)) {
        return false;
    }
    return true;
}

bool NameSegment::IsValid()
{
    Context *ctx = m_context;
    MetaHeader *metaheader = ctx->header->Metadata();
    
    if (metaheader->name_size <= 0) {
        return false;
    }
    
    return true;
}

uint32_t NameSegment::Size()
{
    return (uint32_t)m_names.GetSize();
}

const char* NameSegment::GetNamePtr(uint32_t offset, uint16_t size)
{
    if (offset + size > m_names.GetSize()) {
        return nullptr;
    }
    return (char*)m_names.GetBytes() + offset;
}

const char* NameSegment::GetNamePtr(MetaHash *metahash)
{
    assert(metahash);
    return this->GetNamePtr(metahash->name_offset, metahash->name_size);
}

std::string NameSegment::GetName(uint32_t offset, uint16_t size)
{
    if (offset + size > m_names.GetSize()) {
        return std::string();
    }
    std::string name((char*)m_names.GetBytes() + offset, size);
    return std::move(name);
}

std::string NameSegment::GetName(MetaHash *metahash)
{
    assert(metahash);
    return this->GetName(metahash->name_offset, metahash->name_size);
}

void NameSegment::AddName(const std::string &name, MetaHash *hs)
{
    uint32_t offset = this->AddName(name);
    hs->name_offset = offset;
    hs->name_size   = name.size();
    m_context->header->UpdateMetadata();
}

uint32_t NameSegment::AddName(const std::string &name)
{
    uint32_t offset = (uint32_t)m_names.GetSize();
    m_names.Append(name.c_str(), name.size());
    m_context->header->UpdateMetadata();
    return offset;
}

void NameSegment::RemoveNameSafely(uint32_t offset, uint16_t size)
{
    size_t nsize = m_names.GetSize();
    if (offset + size >= nsize) {
        m_names.Erase(offset, size);
        m_context->header->UpdateMetadata();
    }
}

void NameSegment::RemoveNameSafely(MetaHash *metahash)
{
    assert(metahash);
    this->RemoveNameSafely(metahash->name_offset, metahash->name_size);
}

void NameSegment::CleanUnusedNameSafely()
{
    HashSegment::MetaHashMap *hashmap = m_context->hash->GetMetaHashMap();
    
    uint32_t max = 0;
    for (auto x : hashmap->GetIterator()) {
        MetaHash *metahash = x.second;
        uint32_t pos = metahash->name_offset + metahash->name_size;
        if (pos > max) {
            max = pos;
        }
    }
    
    m_names.Erase(max, m_names.GetSize() - max);
    m_context->header->UpdateMetadata();
}

void NameSegment::RemoveName(uint32_t offset, uint16_t size)
{
    m_names.Erase(offset, size);
    m_context->header->UpdateMetadata();
}

void NameSegment::RemoveName(MetaHash *metahash)
{
    assert(metahash);
    this->RemoveName(metahash->name_offset, metahash->name_size);
}

const torch::Data& NameSegment::GetRawNames()
{
    return m_names;
}

