//
//  xpack
//
//  Created by Luwei.
//  Copyright (c) 2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy
//

#include "xpack-stream.h"
#include "xpack-def.h"
#include "xpack-base.h"
#include <assert.h>

using namespace xpack;

/// Stream

Stream::Stream()
{
    torch::HeapCounterRetain();
}

Stream::~Stream()
{
    torch::HeapCounterRelease();
}

bool Stream::GetContent(torch::Data &content, size_t offset)
{
    return this->GetContent(content.GetBytes(), content.GetSize(), offset);
}

bool Stream::GetSignature(MetaSignature *buffer, size_t offset)
{
    bool ok = this->GetContent(buffer, sizeof(MetaSignature), offset);
    if (ok) {
        buffer->signature = torch::Endian::ToHost(buffer->signature);
        buffer->version   = torch::Endian::ToHost(buffer->version);
    }
    return ok;
}

bool Stream::PutSignature(MetaSignature *buffer, size_t offset)
{
    MetaSignature convdate = *buffer;
    convdate.signature = torch::Endian::ToNet(buffer->signature);
    convdate.version   = torch::Endian::ToNet(buffer->version);
    return this->PutContent(&convdate, sizeof(MetaSignature),  offset);
}

bool Stream::GetHeader(MetaHeader *buffer, size_t offset)
{
    bool ok = this->GetContent(buffer, sizeof(MetaHeader), offset);
    if (ok) {
        buffer->archive_size = torch::Endian::ToHost(buffer->archive_size);
        buffer->content_offset = torch::Endian::ToHost(buffer->content_offset);
        buffer->content_size = torch::Endian::ToHost(buffer->content_size);
        buffer->block_offset = torch::Endian::ToHost(buffer->block_offset);
        buffer->block_count  = torch::Endian::ToHost(buffer->block_count);
        buffer->hash_offset  = torch::Endian::ToHost(buffer->hash_offset);
        buffer->hash_count   = torch::Endian::ToHost(buffer->hash_count);
        buffer->name_size    = torch::Endian::ToHost(buffer->name_size);
    }
    return ok;
}

bool Stream::PutHeader(MetaHeader *buffer, size_t offset)
{
    MetaHeader tmp = *buffer;
    tmp.archive_size = torch::Endian::ToNet(buffer->archive_size);
    tmp.content_offset = torch::Endian::ToNet(buffer->content_offset);
    tmp.content_size = torch::Endian::ToNet(buffer->content_size);
    tmp.block_offset = torch::Endian::ToNet(buffer->block_offset);
    tmp.block_count  = torch::Endian::ToNet(buffer->block_count);
    tmp.hash_offset  = torch::Endian::ToNet(buffer->hash_offset);
    tmp.hash_count   = torch::Endian::ToNet(buffer->hash_count);
    tmp.name_size    = torch::Endian::ToNet(buffer->name_size);
    return this->PutContent(&tmp, sizeof(MetaHeader),  offset);
}

bool Stream::GetHashs(void *buffer, size_t offset, size_t count)
{
    bool ok = this->GetContent(buffer, sizeof(MetaHash) * count, offset);
    if (ok) {
        MetaHash *p = (MetaHash *)buffer;
        for (int i = 0; i < count; i++) {
            p->hash        = torch::Endian::ToHost(p->hash);
            p->crc         = torch::Endian::ToHost(p->crc);
            p->block_index = torch::Endian::ToHost(p->block_index);
            p->unpacked_size = torch::Endian::ToHost(p->unpacked_size);
            p->name_offset = torch::Endian::ToHost(p->name_offset);
            p->name_size   = torch::Endian::ToHost(p->name_size);
            p->conflict_refc = torch::Endian::ToHost(p->conflict_refc);
            p->salt        = torch::Endian::ToHost(p->salt);
            p->flags       = torch::Endian::ToHost(p->flags);
            p++; // Move to next hash item
        }
    }
    return ok;
}

bool Stream::PutHashs(void *buffer, size_t offset, size_t count)
{
    MetaHash tmp = {0};
    MetaHash *p = (MetaHash *)buffer;
    for (int i = 0; i < count; i++) {
        tmp.hash        = torch::Endian::ToNet(p->hash);
        tmp.crc         = torch::Endian::ToNet(p->crc);
        tmp.block_index = torch::Endian::ToNet(p->block_index);
        tmp.unpacked_size = torch::Endian::ToNet(p->unpacked_size);
        tmp.name_offset = torch::Endian::ToNet(p->name_offset);
        tmp.name_size   = torch::Endian::ToNet(p->name_size);
        tmp.conflict_refc = torch::Endian::ToNet(p->conflict_refc);
        tmp.salt        = torch::Endian::ToNet(p->salt);
        tmp.flags       = torch::Endian::ToNet(p->flags);
        bool ok = this->PutContent(&tmp, sizeof(MetaHash), offset + sizeof(MetaHash) * i);
        if (!ok) {
            XPACK_ERROR(xpack::Error::IO);
            return false;
        }
        p++;
    }
    return true;
}

bool Stream::GetBlocks(void *buffer, size_t offset, size_t count)
{
    bool ok = this->GetContent(buffer, sizeof(MetaBlock) * count, offset);
    if (ok) {
        MetaBlock *p = (MetaBlock *)buffer;
        for (int i = 0; i < count; i++) {
            p->offset        = torch::Endian::ToHost(p->offset);
            p->size          = torch::Endian::ToHost(p->size);
            p->next_index    = torch::Endian::ToHost(p->next_index);
            p->flags         = torch::Endian::ToHost(p->flags);
            p++; // move to next hash item
        }
    }
    return ok;
}

bool Stream::PutBlocks(void *buffer, size_t offset, size_t count)
{
    MetaBlock tmp = {0};
    MetaBlock *p = (MetaBlock *)buffer;
    for (int i = 0; i < count; i++) {
        tmp.offset        = torch::Endian::ToNet(p->offset);
        tmp.size          = torch::Endian::ToNet(p->size);
        tmp.next_index    = torch::Endian::ToNet(p->next_index);
        tmp.flags         = torch::Endian::ToNet(p->flags);
        bool ok = this->PutContent(&tmp, sizeof(MetaBlock), offset + sizeof(MetaBlock) * i);
        if (!ok) {
            XPACK_ERROR(xpack::Error::IO);
            return false;
        }
        p++;
    }
    return true;
}

/// FileStream

FileStream::FileStream()
{
}

FileStream::~FileStream()
{
    bool ok = m_fstream.Close();
    if (!ok) {
        XPACK_ERROR(Error::IO);
    }
}

bool FileStream::Open(const std::string &path, bool readonly)
{
    std::string mode = "rb+"; // read + write
    if (readonly) {
        mode = "rb"; // read only
    }
    
    m_fstream.Close();
    bool ok = m_fstream.Open(path, mode);
    if (!ok) {
        XPACK_ERROR(Error::IO);
        return false;
    }
    return true;
}

bool FileStream::IsEnd()
{
    return m_fstream.IsEOF();
}

bool FileStream::Flush()
{
    return m_fstream.Flush();
}

size_t FileStream::Size()
{
    return m_fstream.GetSize();
}

bool FileStream::ReSize(size_t size)
{
    return m_fstream.ReSize(size);
}

bool FileStream::GetContent(void *buffer, size_t size, size_t offset)
{
    bool ok = m_fstream.SeekSet(offset);
    if (!ok) {
        XPACK_ERROR(Error::IO);
        return false;
    }
    
    size_t rsize = m_fstream.Read(buffer, size);
    if (rsize != size) {
        XPACK_ERROR(Error::Format);
        return false;
    }
    
    return true;
}

bool FileStream::PutContent(void *buffer, size_t size, size_t offset)
{
    bool ok = m_fstream.SeekSet(offset);
    if (!ok) {
        XPACK_ERROR(Error::IO);
        return false;
    }
    
    size_t wsize = m_fstream.Write(buffer, size);
    if (wsize != size) {
        XPACK_ERROR(Error::IO);
        return false;
    }
    
    return true;
}


