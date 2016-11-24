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
        Allocator(const Allocator& other);
        Allocator(Allocator&& other);
        Allocator& operator=(const Allocator& other);
        Allocator& operator=(Allocator&& other);
        ~Allocator();
        
        /*
         * 申请/释放内存
         * 说明：内部实际会缓存内存，同时实际申请内存一般会略大于需申请内存大小
         */
        Allocator& Free();
        Allocator& Alloc(size_t size);
        Allocator& ReAlloc(size_t size);
        
        /*
         * GetSize：获得用户申请的内存的大小
         * GetCapacity：获得实际申请的内存的容量大小
         */
        size_t GetSize() const;
        size_t GetCapacity() const;

        /*
         * 将用户申请的内存控制权交给Allocator管理
         * 注意：内存不可在手动释放
         */
        Allocator& ShallowSet(void* pMem, size_t size);
        
        /*
         * 将内容的控制权分离出去
         * 注意：内存的释放要由用户自己完成
         */
        void* Detach();
        
        /*
         * 获得内存指针
         * 注意：不可释放内容，不要持有内存，因为再次申请内存时，内存指针可能改变
         */
        void* GetPtr() const;
        
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