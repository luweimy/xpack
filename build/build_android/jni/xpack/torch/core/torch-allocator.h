//
//  Torch
//
//  Created by Luwei.
//  Copyright (c) 2014-2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy/Torch
//

#ifndef __TORCH__ALLOCATOR__
#define __TORCH__ALLOCATOR__

#include <stdio.h>
#include "torch-base.h"

namespace torch {
    
    /*
     * 内存分配器（内存缓存）
     * 说明：
     *  - 内部会减少对内存的释放，若下次能重用则直接重用，不能重用的话才会释放原有内存重新申请内存
     *  - 内部不会保存多块内存，仅仅保存一块内存
     *  - 返回内存实际可能大于申请的内存，但是通过[GetSize()]获得的就是申请内存的大小
     */
    class Allocator
    {
    public:
        enum { CAPACITY_FACTOR = 256 };
        Allocator();
        Allocator(size_t size);
        ~Allocator();

        Allocator(const Allocator& other);
        Allocator(Allocator&& other);
        Allocator& operator=(const Allocator& other);
        Allocator& operator=(Allocator&& other);
        
        Allocator& Free();
        Allocator& Alloc(size_t size);
        Allocator& ReAlloc(size_t size);
        
        /*
         * 将用户申请的内存控制权交给Allocator管理
         */
        Allocator& ShallowSet(void* pMem, size_t size);
        
        /*
         * 将内容的控制权分离出去，内存的释放要由用户自己完成
         */
        void* Detach();
        
        void* GetPtr() const;
        size_t GetSize() const;
        
    private:
    public:
        struct { 
            size_t cap; void *mem; 
        } m_chunk;
        
        size_t m_size;
        void*  m_mem;
    };
    
}

#endif