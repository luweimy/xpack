//
//  xpack
//
//  Created by Luwei.
//  Copyright (c) 2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy
//

#include "xpack-hash.h"
#include "xpack-header.h"
#include "xpack-name.h"
#include "xpack-context.h"
#include "xpack-stream.h"
#include "xpack-util.h"
#include "xpack-base.h"
#include "xpack-def.h"
#include <vector>
#include <unordered_set>

using namespace xpack;

HashSegment::HashSegment(Context *ctx)
:m_context(ctx)
,m_slatcursor(0)
{
    assert(ctx);
    torch::HeapCounterRetain();
}

HashSegment::~HashSegment()
{
    torch::HeapCounterRelease();
    m_context = nullptr;
}

bool HashSegment::ReadFromStream()
{
    if (!this->IsValid()) {
        return true;
    }
    
    Context *ctx = m_context;
    
    uint32_t hashoffset = ctx->offset + ctx->header->Metadata()->hash_offset;
    uint32_t hashcount  = ctx->header->Metadata()->hash_count;
    
    torch::Data rb(hashcount * sizeof(MetaHash));
    MetaHash *hashptr = (MetaHash *)rb.GetBytes();
    
    if (!ctx->stream->GetHashs(hashptr, hashoffset, hashcount)) {
        return false;
    }
    
    // Decrypt hashs data
    ctx->crypto->CryptoNoCopy((unsigned char*)rb.GetBytes(), (int)rb.GetSize());
    
    if (!this->IsValid()) {
        XPACK_ERROR(xpack::Error::Format);
        return false;
    }
    
    for (int i = 0; i < hashcount; i++) {
        MetaHash *metahash = (MetaHash *)m_hashpool.Alloc();
        memcpy(metahash, hashptr++, sizeof(MetaHash));
        if (!m_hashmap.Set(metahash->hash, metahash)) {
            goto error;
        }
    }
    return true;
    
error:
    m_hashmap.Clear();
    XPACK_ERROR(xpack::Error::Format);
    return false;
}

bool HashSegment::WriteToStream()
{
    Context *ctx = m_context;
    ctx->header->UpdateMetadata();
    
    uint32_t hashoffset = ctx->offset + ctx->header->Metadata()->hash_offset;

    torch::Data wb;
    wb.Reserve(m_hashmap.Size() * sizeof(MetaHash));
    
    for (auto it : m_hashmap.GetIterator()) {
        MetaHash *metahash = it.second;
        assert(!(metahash->flags & int(HashFlags::Unused)));
        wb.Append(metahash, sizeof(MetaHash));
    }
    
    // Encrypt hashs data
    ctx->crypto->CryptoNoCopy((unsigned char*)wb.GetBytes(), (int)wb.GetSize());

    if (!ctx->stream->PutHashs(wb.GetBytes(), hashoffset, wb.GetSize() / sizeof(MetaHash))) {
        return false;
    }
    
    return true;
}

bool HashSegment::IsValid()
{
    Context *ctx = m_context;
    MetaHeader *metaheader = ctx->header->Metadata();
    
    if (metaheader->hash_offset <= 0 || metaheader->hash_count <= 0) {
        return false;
    }
    if (metaheader->hash_offset < sizeof(MetaSignature) + sizeof(MetaHeader)) {
        return false;
    }

    return true;
}

MetaHash* HashSegment::QueryByName(const std::string &name)
{
    Context  *ctx = m_context;
    uint32_t hashid = ctx->HashMaker(name, 0);
    MetaHash *metahash = this->GetById(hashid);
    
    // Name not exists
    if (!metahash) {
        XPACK_ERROR(xpack::Error::NotExists);
        return nullptr;
    }

    // Conflict hashs chain
    while (metahash && (metahash->flags & int(HashFlags::Conflict))) {
        assert(!(metahash->flags & int(HashFlags::Unused)));
        uint32_t hashid = ctx->HashMaker(name, metahash->salt);
        metahash = this->GetById(hashid);
    }

    // Check storage name
    if (!metahash || name != ctx->name->GetName(metahash)) {
        XPACK_ERROR(xpack::Error::NotExists);
        return nullptr;
    }
    return metahash;
}

MetaHash* HashSegment::GetById(const uint32_t hashid)
{
    return m_hashmap.Get(hashid);
}

bool HashSegment::IsHashExist(const std::string &name)
{
    return this->QueryByName(name) != nullptr;
}

MetaHash* HashSegment::AddNew(const std::string &name)
{
    if (this->IsHashExist(name)) {
        XPACK_ERROR(xpack::Error::AlreadyExists);
        return nullptr;
    }
    return this->InternalAddNew(name);
}

MetaHash* HashSegment::InternalAddNew(const std::string &name, uint8_t seed)
{
    Context *ctx = m_context;
    uint32_t hashid = ctx->HashMaker(name, seed);
    MetaHash *metahash = this->GetById(hashid);
    
    // Detect hashid conflicts
    if (metahash) {
        // Current metahash was already marked `conflict` before
        if (metahash->flags & int(HashFlags::Conflict)) {
            metahash->conflict_refc++;
            // Use current metahash's salt as next search name's seed
            return HashSegment::InternalAddNew(name, metahash->salt);
        }
        // Current metahash now will be marked `conflict`
        else {
            std::string originalname = ctx->name->GetName(metahash);

            // Name duplication error
            assert(name != originalname);

            // Use the next slat seed
            int slatseed = ++m_slatcursor;
            
            // Move conflicted metahash to other
            MetaHash* metahashnew = HashSegment::InternalAddNew(originalname, slatseed);
            if (!metahashnew) {
                return nullptr;
            }
            int32_t hashidnew = metahashnew->hash;
            memcpy(metahashnew, metahash, sizeof(MetaHash));
            metahashnew->hash = hashidnew;
            
            // Mark current metahash `conflict`
            int32_t hashidcur = metahash->hash;
            memset(metahash, 0, sizeof(MetaHash));
            metahash->hash          = hashidcur;
            metahash->block_index   = -1;
            metahash->conflict_refc = 2;
            metahash->salt          = slatseed;
            metahash->flags        |= int(HashFlags::Conflict);
            
            return HashSegment::InternalAddNew(name, slatseed);
        }
    }
    
    // Add new metahash
    MetaHash *metahashnew = (MetaHash*)m_hashpool.Alloc();
    memset(metahashnew, 0, sizeof(MetaHash));

    if (!m_hashmap.Set(hashid, metahashnew)) {
        m_hashpool.Free(metahashnew);
        XPACK_ERROR(xpack::Error::AlreadyExists);
        return nullptr;
    }
    
    metahashnew->hash = hashid;
    metahashnew->block_index = -1;
    ctx->header->Metadata()->hash_count++;
    ctx->header->UpdateMetadata();
    
    return metahashnew;
}

void HashSegment::RemoveByName(const std::string &name)
{
    Context *ctx = m_context;

    MetaHash *metahash = this->GetById(ctx->HashMaker(name, 0));
    if (!metahash) {
        return;
    }
    
    assert(!(metahash->flags & int(HashFlags::Unused)));
    
    std::vector<MetaHash*> removehashs;
    removehashs.push_back(metahash);
    while (metahash->flags & int(HashFlags::Conflict)) {
        metahash = this->GetById(ctx->HashMaker(name, metahash->salt));
        assert(metahash && !(metahash->flags & int(HashFlags::Unused)));
        removehashs.push_back(metahash);
    }
    
    for (auto metahash : removehashs) {
        if (metahash->conflict_refc <= 1) {
            m_hashmap.Remove(metahash->hash);
            
            memset(metahash, 0, sizeof(MetaHash));
            metahash->block_index = -1;
            metahash->flags |= int(HashFlags::Unused);
            
            m_hashpool.Free(metahash);
            m_context->header->Metadata()->hash_count--;
        }
        else {
            metahash->conflict_refc--;
        }
    }
    m_context->header->UpdateMetadata();
}

void HashSegment::Clear()
{
    m_slatcursor = 0;
    
    m_hashpool.Clear();
    m_hashmap.Clear();

    // Update header data
    m_context->header->Metadata()->hash_count = 0;
    m_context->header->Metadata()->name_size = 0;
    m_context->header->UpdateMetadata();
}

HashSegment::MetaHashMap* HashSegment::GetMetaHashMap()
{
    return &m_hashmap;
}

std::string HashSegment::DumpHash()
{
    return std::move(DumpUtils::DumpHashs(m_context));
}

std::string HashSegment::DumpHashById(const uint32_t hashid)
{
    return std::move(DumpUtils::DumpHash(m_context, hashid));
}

std::string HashSegment::DumpHashByName(const std::string &name)
{
    return std::move(DumpUtils::DumpHash(m_context, name));
}

