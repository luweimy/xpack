//
//  xpack
//
//  Created by Luwei.
//  Copyright (c) 2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy
//

#include "xpack-header.h"
#include "xpack-context.h"
#include "xpack-stream.h"
#include "xpack-base.h"
#include "xpack-name.h"
#include "xpack-util.h"
#include "xpack-def.h"
#include "torch/torch.h"

using namespace xpack;

HeaderSegment::HeaderSegment(Context *ctx)
:m_context(ctx)
,m_header({0})
{
    assert(ctx);
    torch::HeapCounterRetain();
}

HeaderSegment::~HeaderSegment()
{
    torch::HeapCounterRelease();
    m_context = nullptr;
}

bool HeaderSegment::ReadFromStream()
{
    Context *ctx = m_context;
    this->UpdateMetadata();

    size_t headeroffset = ctx->offset + sizeof(MetaSignature);
    if (!ctx->stream->GetHeader(&m_header, headeroffset)) {
        return false;
    }
    
    // Decrypt header data
    ctx->crypto->CryptoNoCopy((unsigned char*)&m_header, sizeof(MetaHeader)); 
    
    if (!this->IsValid()) {
        XPACK_ERROR(xpack::Error::Format);
        return false;
    }
    return true;
}

bool HeaderSegment::WriteToStream()
{
    this->UpdateMetadata();
    Context *ctx = m_context;
    MetaHeader encryptbuffer = m_header;

    // Encrypt header data
    ctx->crypto->CryptoNoCopy((unsigned char*)&encryptbuffer, sizeof(MetaHeader)); 
    
    size_t headeroffset = ctx->offset + sizeof(MetaSignature);
    if (!ctx->stream->PutHeader(&encryptbuffer, headeroffset)) { 
        return false;
    }
    return true;
}

bool HeaderSegment::IsValid()
{
    MetaHeader &metaheader = m_header;
    return  (metaheader.archive_size >= sizeof(MetaSignature) + sizeof(MetaHeader)) &&
            (metaheader.block_offset >= metaheader.content_offset) &&
            (metaheader.hash_offset >= metaheader.block_offset) &&
            (metaheader.block_count * sizeof(MetaBlock) + metaheader.hash_count * sizeof(MetaHash) + metaheader.content_size + metaheader.name_size) <= metaheader.archive_size;
}

HeaderSegment* HeaderSegment::Initialize()
{
    m_header.archive_size = sizeof(MetaSignature) + sizeof(MetaHeader);
    m_header.content_offset = m_header.archive_size;
    m_header.content_size = 0;
    m_header.block_offset = m_header.archive_size;
    m_header.block_count = 0;
    m_header.hash_offset = m_header.archive_size;
    m_header.hash_count = 0;
    m_header.name_size = 0;
    return this;
}

HeaderSegment* HeaderSegment::UpdateMetadata()
{
    m_header.name_size    = m_context->name->Size();
    
    uint32_t blocksize    = m_header.block_count * sizeof(MetaBlock);
    uint32_t hashsize     = m_header.hash_count * sizeof(MetaHash);

    m_header.block_offset = m_header.content_offset + m_header.content_size;
    m_header.hash_offset  = m_header.block_offset + blocksize;
    m_header.archive_size = sizeof(MetaSignature) + sizeof(MetaHeader) + m_header.content_size + blocksize + hashsize + m_header.name_size;
    
    return this;
}

MetaHeader* HeaderSegment::Metadata()
{
    return &m_header;
}

std::string HeaderSegment::DumpHeader()
{
    return std::move(DumpUtils::DumpHeader(m_context));
}