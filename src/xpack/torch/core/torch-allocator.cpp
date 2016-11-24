//
//  Torch
//
//  Created by Luwei.
//  Copyright (c) 2014-2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy/Torch
//

#include <string.h>
#include "torch-allocator.h"

using namespace torch;

Allocator::Allocator()
:m_chunk({0,nullptr})
,m_size(0)
,m_mem(nullptr)
{
}

Allocator::Allocator(size_t size)
:m_size(0), m_mem(nullptr), m_chunk({0,nullptr})
{
    this->Alloc(size);
}

Allocator::Allocator(const Allocator &other)
:m_chunk({0,nullptr})
,m_size(0)
,m_mem(nullptr)
{
    this->Alloc(other.GetSize());
    memmove(this->GetPtr(), other.GetPtr(), other.GetSize());
}

Allocator::Allocator(Allocator&& other)
:m_size(other.m_size)
,m_mem(other.m_mem)
,m_chunk(other.m_chunk)
{
    other.m_size = 0;
    other.m_mem = nullptr;
    other.m_chunk.cap = 0;
    other.m_chunk.mem = nullptr;
}

Allocator& Allocator::operator=(const Allocator& other)
{
    if (this == &other) {
        return *this;
    }
    this->Alloc(other.GetSize());
    memmove(this->GetPtr(), other.GetPtr(), other.GetSize());
    return *this;
}

Allocator& Allocator::operator=(Allocator&& other)
{
    m_size = other.m_size;
    m_mem = other.m_mem;
    m_chunk = other.m_chunk;
    other.m_size = 0;
    other.m_mem = nullptr;
    other.m_chunk.cap = 0;
    other.m_chunk.mem = nullptr;
    return *this;
}

Allocator::~Allocator()
{
    m_mem = nullptr;
    if (m_chunk.mem) {
        torch::HeapFree(m_chunk.mem);
    }
}

Allocator& Allocator::Free()
{
    m_size = 0;
    m_mem = nullptr;
    return *this;
}

Allocator& Allocator::Alloc(size_t size)
{
    if (m_chunk.cap < size) {
        size_t capacity = Allocator::CAPACITY_FACTOR;
        while(capacity < size) {
            capacity *= 2;
        }
        torch::HeapFree(m_chunk.mem);
        m_chunk.mem = torch::HeapMalloc(capacity);
        m_chunk.cap = capacity;
    }
    m_size = size;
    m_mem = m_chunk.mem;
    return *this;
}

Allocator& Allocator::ReAlloc(size_t size)
{
    if (m_chunk.cap < size) {
        size_t capacity = Allocator::CAPACITY_FACTOR;
        while(capacity < size) {
            capacity *= 2;
        }
        m_chunk.mem = torch::HeapReAlloc(m_chunk.mem, capacity);
        m_chunk.cap = capacity;
    }
    m_size = size;
    m_mem = m_chunk.mem;
    return *this;
}

size_t Allocator::GetSize() const
{
    return m_size;
}

size_t Allocator::GetCapacity() const
{
    return m_chunk.cap;
    
}

Allocator& Allocator::ShallowSet(void* pMem, size_t size)
{
    if (m_chunk.mem == pMem) {
        m_size = size;
        m_mem = pMem;
        return *this;
    }
    if (m_chunk.mem) {
        torch::HeapFree(m_chunk.mem);
        m_chunk.mem = nullptr;
        m_chunk.cap = 0;
    }
    m_chunk.mem = pMem;
    m_chunk.cap = size;
    m_mem = pMem;
    m_size = size;
    return *this;
}

void* Allocator::Detach()
{
    void *p = m_mem;
    m_size = 0;
    m_mem = nullptr;
    m_chunk.cap = 0;
    m_chunk.mem = nullptr;
    return p;
}

void* Allocator::GetPtr() const
{
    return m_mem;
}
