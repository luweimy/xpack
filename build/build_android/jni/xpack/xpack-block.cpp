//
//  xpack
//
//  Created by Luwei.
//  Copyright (c) 2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy
//

#include "xpack-block.h"
#include "xpack-header.h"
#include "xpack-name.h"
#include "xpack-context.h"
#include "xpack-stream.h"
#include "xpack-util.h"
#include "xpack-base.h"
#include "xpack-def.h"
#include <vector>

using namespace xpack;

BlockSegment::BlockSegment(Context *ctx)
:m_context(ctx)
{
    assert(ctx);
    torch::HeapCounterRetain();
    
    m_blocks.Reserve(sizeof(MetaBlock) * RESERVE_COUNT);
}

BlockSegment::~BlockSegment()
{
    torch::HeapCounterRelease();
    m_context = nullptr;
}

bool BlockSegment::ReadFromStream()
{
    if (!this->IsValid()) {
        return true;
    }
    
    Context *ctx = m_context;
    
    uint32_t blockoffset = ctx->offset + ctx->header->Metadata()->block_offset;
    uint32_t blockcount  = ctx->header->Metadata()->block_count;
    
    m_blocks.Alloc(blockcount * sizeof(MetaBlock));
    if (!ctx->stream->GetBlocks(m_blocks.GetBytes(), blockoffset, blockcount)) {
        return false;
    }
    
    // Decrypt blocks data
    ctx->crypto->CryptoNoCopy((unsigned char*)m_blocks.GetBytes(), (int)m_blocks.GetSize());
    
    // Build reusing pool
    uint32_t count = this->GetBlockNumber();
    for (int i = 0; i < count; i++) {
        MetaBlock *metablock = this->GetByIndex(i);
        assert(!((metablock->flags & int(BlockFlags::UnusedBlock)) && (metablock->flags & int(BlockFlags::UnusedContent))));
        if (metablock->flags & int(BlockFlags::UnusedBlock)) {
            m_blockReuser.UpdateEntry(i);
        }
        else if (metablock->flags & int(BlockFlags::UnusedContent)) {
            m_contentReuser.UpdateEntry(i, metablock);
        }
    }
    return true;
}

bool BlockSegment::WriteToStream()
{
    Context *ctx = m_context;
    ctx->header->UpdateMetadata();
    
    torch::Data wb = m_blocks;
    
    // Encrypt blocks data
    ctx->crypto->CryptoNoCopy((unsigned char*)wb.GetBytes(), (int)wb.GetSize());
    
    uint32_t blockoffset = ctx->offset + ctx->header->Metadata()->block_offset;
    if (!ctx->stream->PutBlocks(wb.GetBytes(), blockoffset, this->GetBlockNumber())) {
        return false;
    }
    return true;
}

bool BlockSegment::IsValid()
{
    Context *ctx = m_context;
    MetaHeader *metaheader = ctx->header->Metadata();
    
    if (metaheader->block_offset <= 0 || metaheader->block_count <= 0) {
        return false;
    }
    
    size_t headerend = sizeof(MetaSignature) + sizeof(MetaHeader);
    if (metaheader->block_offset < headerend) {
        return false;
    }
    
    return true;
}

void BlockSegment::RemoveByIndex(int32_t index)
{
    MetaBlock  *metablock  = this->GetByIndex(index);
    MetaHeader *metaheader = m_context->header->Metadata();
    
    // Must be head of block chain
    assert(metablock && !(metablock->flags & int(BlockFlags::NotStart)));
    
    // Reusing the content
    while (metablock) {
        assert(!(metablock->flags & int(BlockFlags::UnusedContent)));
        
        int32_t curindex  = this->ConvertBlockPtrToIndex(metablock);
        int32_t nextindex = metablock->next_index;
        
        metablock->next_index = -1;
        metablock->flags = 0;
        metablock->flags |= int(BlockFlags::UnusedContent);
        
        if (metablock->size > 0) {
            m_contentReuser.UpdateEntry(curindex, metablock);
        }
        else {
            this->InternalAddingBlockToReuserByIndex(curindex);
        }
        
        metablock = this->GetByIndex(nextindex);
    }
    
    // Remove the unused tail in content section
    while (m_contentReuser.Size() > 0) {
        int32_t index = m_contentReuser.GetMaximumOffsetEntry();
        MetaBlock *metablock = this->GetByIndex(index);
        assert(metablock->offset + metablock->size <= metaheader->content_offset + metaheader->content_size);
        if (metablock->offset + metablock->size < metaheader->content_offset + metaheader->content_size) {
            break;
        }
        metaheader->content_size -= metablock->size;
        this->InternalAddingBlockToReuserByIndex(index);
    }
    
    // Remove the unused tail in block section
    while (m_blockReuser.Size() > 0) {
        int32_t index = m_blockReuser.GetMaximumIndexEntry();
        if (this->GetBlockNumber() != index + 1) {
            break;
        }
        m_blocks.ReSize(m_blocks.GetSize() - sizeof(MetaBlock));
        m_context->header->Metadata()->block_count = this->GetBlockNumber();
        m_blockReuser.RemoveEntry(index);
    }
    
    m_context->header->UpdateMetadata();
}

MetaBlock* BlockSegment::GetByIndex(int32_t index)
{
    if (index < 0 || index >= m_blocks.GetSize() / sizeof(MetaBlock)) {
        return nullptr;
    }
    return ((MetaBlock *)m_blocks.GetBytes()) + index;
}

int32_t BlockSegment::AllocLinkedBlock(uint32_t size)
{
    int32_t     headindex  = -1;
    uint32_t    totalsize  = 0;
    MetaBlock  *prevblock  = nullptr;
    MetaHeader *metaheader = m_context->header->Metadata();
    
    // Find block in content reuser, smaller block size first
    while (m_contentReuser.Size() > 0) {
        int32_t curbindex = m_contentReuser.GetMinimumSizeEntry();
        
        MetaBlock *metablock = this->GetByIndex(curbindex);
        metablock->flags = 0;
        
        // Current metablock is too big to reusing
        if (totalsize + metablock->size > size) {
            uint32_t needsize = size - totalsize;
            assert(metablock->size > needsize);
            
            // Separate out extra space and recorded in new block
            int32_t    extraindex = this->InternalGetOrCreate();
            MetaBlock *extrablock = this->GetByIndex(extraindex);
            extrablock->offset = metablock->offset + metablock->size;
            extrablock->size   = metablock->size - needsize;
            extrablock->flags |= int(BlockFlags::UnusedContent);
            m_contentReuser.UpdateEntry(extraindex, extrablock);
            
            metablock->size = needsize;
        }
        
        // Make blocks linked by field `next_index`
        if (headindex >= 0) {
            prevblock->next_index = curbindex;
            metablock->flags |= int(BlockFlags::NotStart);
        }
        else {
            headindex = curbindex;
        }

        m_contentReuser.RemoveEntry(curbindex);
        totalsize += metablock->size;
        assert(totalsize <= size);
        
        if (totalsize == size) {
            return headindex;
        }
        
        prevblock = metablock;
    }
    
    uint32_t    needsize   = size - totalsize;
    int32_t     finalindex = this->InternalGetOrCreate();
    MetaBlock  *finalblock = this->GetByIndex(finalindex);
    
    finalblock->offset = metaheader->content_offset + metaheader->content_size;
    finalblock->size   = needsize;
    metaheader->content_size += needsize;
    
    if (prevblock) {
        prevblock->next_index = finalindex;
        finalblock->flags |= int(BlockFlags::NotStart);
    }
    else {
        headindex = finalindex;
    }
    
    m_context->header->UpdateMetadata();
    return headindex;
}

void BlockSegment::Clear()
{
    // Clear reuse pool
    m_blockReuser.Clear();
    m_contentReuser.Clear();
    
    // Clear blocks memory
    m_blocks.Free();
    
    // Update header data
    m_context->header->Metadata()->content_size = 0;
    m_context->header->Metadata()->block_count = 0;
    m_context->header->UpdateMetadata();
}

uint32_t BlockSegment::GetBlockNumber()
{
    return (uint32_t)m_blocks.GetSize() / sizeof(MetaBlock);
}

int32_t BlockSegment::ConvertBlockPtrToIndex(MetaBlock *metablock)
{
    assert(metablock);
    return (int32_t)(metablock - (MetaBlock *)m_blocks.GetBytes());
}

ContentReuser* BlockSegment::GetContentReuser()
{
    return &m_contentReuser;
}

BlockReuser* BlockSegment::GetBlockReuser()
{
    return &m_blockReuser;
}

void BlockSegment::InternalAddingBlockToReuserByIndex(int32_t index)
{
    MetaBlock *metablock = this->GetByIndex(index);
    metablock->offset = 0;
    metablock->size = 0;
    metablock->next_index = -1;
    metablock->flags = 0;
    metablock->flags |= int(BlockFlags::UnusedBlock);
    m_blockReuser.UpdateEntry(index);
    m_contentReuser.RemoveEntry(index);
}

int32_t BlockSegment::InternalGetOrCreate()
{
    uint32_t   count     = this->GetBlockNumber();
    int32_t    bindexnew = -1;
    MetaBlock *blocknew  = nullptr;
    
    // Reusing cached metablock
    if (m_blockReuser.Size() > 0) {
        // Reusing min bindex better, that we can remove the unused tail in block section
        bindexnew = m_blockReuser.GetMinimumIndexEntry();
        blocknew  = this->GetByIndex(bindexnew);
        m_blockReuser.RemoveEntry(bindexnew);
    }
    else {
        // Adding new block
        size_t offset = m_blocks.GetSize();
        m_blocks.ReSize(offset + sizeof(MetaBlock));
        blocknew = (MetaBlock *)((char*)m_blocks.GetBytes() + offset);
        bindexnew = (++count) - 1;
    }
    
    blocknew->offset = 0;
    blocknew->size = 0;
    blocknew->next_index = -1;
    blocknew->flags = 0;
    
    m_context->header->Metadata()->block_count = count;
    return bindexnew;
}

std::string BlockSegment::DumpBlock(bool cotainUnused)
{
    return std::move(DumpUtils::DumpBlocks(m_context, cotainUnused));
}

std::string BlockSegment::DumpBlockByIndex(const int32_t index)
{
    return std::move(DumpUtils::DumpBlock(m_context, index));
}

std::string BlockSegment::DumpBlockByName(const std::string &name)
{
    return std::move(DumpUtils::DumpBlock(m_context, name));
}

std::string BlockSegment::DumpBlockUnused()
{
    return std::move(DumpUtils::DumpBlockUnused(m_context));
}

std::string BlockSegment::DumpBlockContentUnused()
{
    return std::move(DumpUtils::DumpBlockContentUnused(m_context));
}
