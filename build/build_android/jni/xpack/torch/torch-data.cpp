//
//  Torch
//
//  Created by Luwei.
//  Copyright (c) 2014-2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy/Torch
//

#include "torch-data.h"
#include "torch-util.h"
#include <memory.h>
#include <stdlib.h>
#include <assert.h>
#include <sstream>
#include <iomanip>

using namespace torch;

const Data Data::Null;

/*
 * 监测内存边界，计算安全访问范围
 * 参数：
 *  - index：索引值
 *  - bytes：内存字节数
 *  - size：可用内存大小
 * 说明：有可能index+count超出可用内存范围，其返还真实可用的字节数
 */
inline size_t ConvertSafelySize(size_t index, size_t bytes, size_t memsize) {
    return (index+bytes>memsize) ? bytes-(index+bytes-memsize) : bytes;
}

Data::Data()
{
}

Data::Data(size_t size)
{
    this->Alloc(size);
}

Data::Data(const char* str)
{
    this->CopyFrom(str, strlen(str));
}

Data::Data(const void* pMem, size_t size)
{
    this->CopyFrom(pMem, size);
}

Data::Data(const Data& other)
{
    this->CopyFrom(other);
}

Data::Data(Data&& other)
{
    m_allocator = std::move(other.m_allocator);
}

Data::~Data()
{
    this->Free();
}

Data& Data::operator=(const Data &other)
{
    if (this == &other)
        return *this;
    m_allocator = other.m_allocator;
    return *this;
}

Data& Data::operator=(Data &&other)
{
    m_allocator = std::move(other.m_allocator);
    return *this;
}

Data& Data::Free()
{
    m_allocator.Free();
    return *this;
}

Data& Data::Alloc(size_t size)
{
    m_allocator.Alloc(size);
    return *this;
}

Data& Data::CopyFrom(const Data &other)
{
    return this->CopyFrom(other.GetBytes(), other.GetSize());
}

Data& Data::CopyFrom(const void *mem, size_t size)
{
    if (mem == nullptr) {
        return *this;
    }
    
    this->Free().Alloc(size);
    
    void *ptr = m_allocator.GetPtr();
    if (ptr) {
        ::memcpy(ptr, mem, size);
    }
    return *this;
}

Data& Data::CopyFrom(const void *str)
{
    return this->CopyFrom(str, strlen((char*)str));
}

Data& Data::Memset(int value)
{
    void *ptr = m_allocator.GetPtr();
    if (ptr) {
        ::memset(ptr, value, m_allocator.GetSize());
    }
    return *this;
}

Data& Data::ReSize(size_t size, int fillchar)
{
    size_t memsize = m_allocator.GetSize();
    m_allocator.ReAlloc(size);
    if (size > memsize) {
        char *p = (char*)m_allocator.GetPtr();
        memset(p + memsize, fillchar, size - memsize);
    }
    return *this;
}

Data& Data::Reserve(size_t size)
{
    m_allocator.Alloc(size);
    m_allocator.Free();
    return *this;
}

Data& Data::ShallowSet(void* pMem, size_t size)
{
    this->Free();
    m_allocator.ShallowSet(pMem, size);
    return *this;
}

void* Data::Detach()
{
    return m_allocator.Detach();
}

bool Data::IsEqualDeep(const Data &other) const
{
    if (this->IsNull() || other.IsNull()) {
        if (other.IsNull() && other.IsNull()) {
            return true;
        }
        return false;
    }
    if (this->GetSize() != other.GetSize()) {
        return false;
    }
    return memcmp(this->GetBytes(), other.GetBytes(), this->GetSize()) == 0;
}

Data& Data::Insert(int index, char ch)
{
    return this->Insert(index, &ch, 1);
}

Data& Data::Insert(int index, Data &other)
{
    return this->Insert(index, other.GetBytes(), other.GetSize());
}

Data& Data::Insert(int index, const void *mem, size_t size)
{
    assert(mem);
    size_t memsize = m_allocator.GetSize();
    if (index < 0) {
        index = (int)memsize + index;
    }
    if (index<0 || index>memsize) {
        return *this;
    }
    
    size_t newsize = memsize + size;
    
    m_allocator.ReAlloc(newsize);
    char *m = (char*)m_allocator.GetPtr();
    memmove(m+index+size, m+index, memsize-index);
    memmove(m+index, mem, size);
    
    return *this;
}

Data& Data::Append(char ch)
{
    return this->Insert((int)m_allocator.GetSize(), ch);
}

Data& Data::Append(Data &other)
{
    return this->Insert((int)m_allocator.GetSize(), other);
}

Data& Data::Append(const void *mem, size_t size)
{
    return this->Insert((int)m_allocator.GetSize(), mem, size);
}

Data& Data::Replace(int index, size_t count, char ch)
{
    return this->Replace(index, count, &ch, 1);
}

Data& Data::Replace(int index, size_t count, Data &other)
{
    return this->Replace(index, count, other.GetBytes(), other.GetSize());
}

Data& Data::Replace(int index, size_t count, const void *mem, size_t size)
{
    size_t memsize = m_allocator.GetSize();
    if (index < 0) {
        index = (int)memsize + index;
    }
    if (index<0 || index >= memsize) {
        return *this;
    }

    // number of replace
    size_t replacenum = ConvertSafelySize(index, count, memsize);
    if (replacenum <= 0) {
        return *this;
    }

    size_t newsize = memsize - replacenum + size;
    
    m_allocator.ReAlloc(newsize);
    char *m = (char*)m_allocator.GetPtr();
    memmove(m+index+size, (char*)m+index+replacenum, memsize-index-replacenum);
    memmove(m+index, mem, size);

    return *this;
}

Data& Data::Erase(int index, size_t count)
{
    size_t memsize = m_allocator.GetSize();
    if (index < 0) {
        index = (int)memsize + index;
    }
    if (index<0 || index >= memsize) {
        return *this;
    }
    
    // number of remove
    size_t rmnum = ConvertSafelySize(index, count, memsize);
    if (rmnum <= 0)
        return *this;
    
    size_t newsize = memsize - rmnum;
    
    char *m = (char*)m_allocator.GetPtr();
    memmove(m+index, m+index+rmnum, memsize-index-rmnum);
    m_allocator.ReAlloc(newsize);

    return *this;
}

int Data::IndexOf(char ch, int start) const
{
    char *cur = (char*)this->GetPtr(start);
    char *last = (char*)this->GetPtr(-1);
    long size = last - cur + 1;
    assert(cur&&last);
    
    char *dst = (char*)memchr(cur, ch, size);
    if (dst) {
        return int(dst - (char*)this->GetBytes());
    }
    return NotFound;
}

int Data::LastIndexOf(char ch, int start) const
{
    char *cur = (char*)this->GetPtr(start);
    char *first = (char*)this->GetBytes();
    assert(cur&&first);

    for (;cur >= first; cur--) {
        if (*cur == ch) {
            return (int)(cur - first);
        }
    }
    return NotFound;
}

bool Data::IsNull() const
{
    return (m_allocator.GetPtr() == nullptr);
}

size_t Data::GetSize() const
{
    return m_allocator.GetSize();
}

void* Data::GetBytes() const
{
    return m_allocator.GetPtr();
}

void* Data::GetPtr(int index) const
{
    const int size = (int)this->GetSize();
    if (index < 0) {
        index = size + index;
    }
    if (index < 0) {
        return nullptr;
    }
    if (index >= size) {
        return nullptr;
    }
    
    return (char*)this->GetBytes() + index;
}

unsigned char Data::GetByte(int index) const
{
    void *p = this->GetPtr(index);
    assert(p);
    return *(unsigned char*)p;
}

std::string Data::ToString() const
{
    if (!(char*)m_allocator.GetPtr()) {
        return std::string();
    }
    return std::move(std::string((char*)m_allocator.GetPtr(), m_allocator.GetSize()));
}

std::string Data::ToHex(int blankByte) const
{
    return torch::ToHex((char*)m_allocator.GetPtr(), m_allocator.GetSize(), blankByte);
}

std::string Data::ToBinary(int blankByte) const
{
    return torch::ToBinary((char*)m_allocator.GetPtr(), m_allocator.GetSize(), blankByte);
}

void Data::DumpHex(int blankByte) const
{
    torch::DumpHex((char*)m_allocator.GetPtr(), m_allocator.GetSize(), blankByte);
}

void Data::DumpBinary(int blankByte) const
{
    torch::DumpBinary((char*)m_allocator.GetPtr(), m_allocator.GetSize(), blankByte);
}

