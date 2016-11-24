//
//  xpack
//
//  Created by Luwei.
//  Copyright (c) 2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy
//

#include "xpack-content.h"
#include "xpack-header.h"
#include "xpack-hash.h"
#include "xpack-context.h"
#include "xpack-stream.h"
#include "xpack-block.h"
#include "xpack-base.h"
#include "xpack-def.h"

using namespace xpack;

ContentSegment::ContentSegment(Context *ctx)
:m_context(ctx)
{
    assert(ctx);
    torch::HeapCounterRetain();
}

ContentSegment::~ContentSegment()
{
    torch::HeapCounterRelease();
    m_context = nullptr;
}

bool ContentSegment::IsValid()
{
    Context *ctx = m_context;
    MetaHeader *metaheader = ctx->header->Metadata();
    
    if (metaheader->content_offset <= 0 || metaheader->content_size <= 0) {
        return false;
    }
    
    size_t headerend = sizeof(MetaSignature) + sizeof(MetaHeader);
    if (metaheader->content_offset < headerend) {
        return false;
    }
    
    return true;
}

bool ContentSegment::OverallWrite(const MetaHash *metahash, const torch::Data &data)
{
    Context   *ctx      = m_context;
    MetaBlock *blockptr = ctx->block->GetByIndex(metahash->block_index);
    char *dataptr       = (char*)data.GetBytes();
    
    size_t wrotesize = 0;
    while (blockptr) {
        if (!ctx->stream->PutContent(dataptr + wrotesize, blockptr->size, ctx->offset + blockptr->offset)) {
            return false;
        }
        wrotesize += blockptr->size;
        if (blockptr->next_index >= 0) {
            blockptr = ctx->block->GetByIndex(blockptr->next_index);
        }
        else {
            blockptr = nullptr;
        }
    }

    // Wrote size != data size
    assert(wrotesize == data.GetSize()); 
    return true;
}

bool ContentSegment::OverallRead(const MetaHash *metahash, torch::Data &outdata)
{
    Context   *ctx      = m_context;
    MetaBlock *blockptr = ctx->block->GetByIndex(metahash->block_index);
    
    outdata.Reserve(metahash->unpacked_size);
    char *dataptr = (char*)outdata.GetBytes();
    
    size_t readsize = 0;
    while (blockptr) {
        outdata.ReSize(readsize + blockptr->size);
        dataptr = (char*)outdata.GetBytes();
        if (!ctx->stream->GetContent(dataptr + readsize, blockptr->size, ctx->offset + blockptr->offset)) { 
            return false;
        }
        readsize += blockptr->size;
        if (blockptr->next_index >= 0) {
            blockptr = ctx->block->GetByIndex(blockptr->next_index);
        }
        else {
            blockptr = nullptr;
        }
    }
    
    return true;
}
