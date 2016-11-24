//
//  Torch
//
//  Created by Luwei.
//  Copyright (c) 2014-2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy/Torch
//

#ifndef __TORCH__MEMORYPOOL__
#define __TORCH__MEMORYPOOL__

#include <stdio.h>
#include <list>
#include <string.h>
#include "core/torch-base.h"

namespace torch {
    
    /*
     * 固定尺寸的内存池
     * 说明：
     *  - 只适用于需要大量固定尺寸内存块的情况
     * 模板参数
     *  - MemoryBlockSize: 内存块的大小
     *  - MemoryPageSize: 一个MemoryPage的大小
     */
    template<size_t MemoryBlockSize, size_t MemoryPageSize = 1024>
    class FixedMemoryPool
    {
    public:
        enum { ITEM_COUNT = MemoryPageSize / MemoryBlockSize };
        
        FixedMemoryPool();
        ~FixedMemoryPool();
        
        FixedMemoryPool(const FixedMemoryPool &);
        FixedMemoryPool& operator=(const FixedMemoryPool &) = delete;
        
        /*
         * 从内存池获得一个内存块
         * 注意：
         *  - 用完后最好在放回内存池中(有利于提高性能)
         *  - 不能使用::free()释放内存，内存池会统一释放池中的内存块
         */
        void* Alloc();
        
        /*
         * 将内存块放回内存池中
         * 注意：必须是从内存池申请的内存块才能放回去
         */
        void Free(void *mem);
        
        /*
         * 清空内存池
         * 注意：调用此接口后，通过Alloc申请的内存块也会被释放
         */
        void Clear();
        
        /*
         * 获得内存池总大小(单位Byte)
         */
        size_t GetCapacity();
        
        /*
         * 获得内存块的大小(单位Byte)
         */
        size_t GetMemoryBlockSize();
        
    private:
        struct MemoryPage;
        MemoryPage* InternalMakeMemoryPage();
        
    private:
        union MemoryPageItem {
            MemoryPageItem*  next;
            uint8_t          mem[MemoryBlockSize];
        };
        struct MemoryPage { MemoryPageItem itemArray[ITEM_COUNT]; };
        
        MemoryPageItem*          m_rootItem;
        std::list<MemoryPage *>  m_memoryPageList;
    };
    
    template<size_t MemoryBlockSize, size_t MemoryPageSize>
    FixedMemoryPool<MemoryBlockSize, MemoryPageSize>::FixedMemoryPool() {
        m_rootItem = nullptr;
        MemoryPage *page = this->InternalMakeMemoryPage();
        if (!page) {
            return;
        }
        m_memoryPageList.push_back(page);
        m_rootItem = page->itemArray;
    }
    
    template<size_t MemoryBlockSize, size_t MemoryPageSize>
    FixedMemoryPool<MemoryBlockSize, MemoryPageSize>::~FixedMemoryPool() {
        this->Clear();
    }
    
    template<size_t MemoryBlockSize, size_t MemoryPageSize>
    FixedMemoryPool<MemoryBlockSize, MemoryPageSize>::FixedMemoryPool(const FixedMemoryPool &) {
        m_rootItem = nullptr;
        MemoryPage *page = this->InternalMakeMemoryPage();
        if (!page) {
            return;
        }
        m_memoryPageList.push_back(page);
        m_rootItem = page->itemArray;
    }
    
    template<size_t MemoryBlockSize, size_t MemoryPageSize>
    void* FixedMemoryPool<MemoryBlockSize, MemoryPageSize>::Alloc() {
        if (!m_rootItem) {
            MemoryPage *page = this->InternalMakeMemoryPage();
            if (!page) { 
                return nullptr;
            }
            m_memoryPageList.push_back(page);
            m_rootItem = page->itemArray;
        }
        void *item = m_rootItem;
        m_rootItem = m_rootItem->next;
        memset(item, 0, MemoryBlockSize);
        return item;
    }
    
    template<size_t MemoryBlockSize, size_t MemoryPageSize>
    void FixedMemoryPool<MemoryBlockSize, MemoryPageSize>::Free(void *mem) {
        if (!mem) { 
            return;
        }
        MemoryPageItem *item = static_cast<MemoryPageItem*>(mem);
        item->next = m_rootItem;
        m_rootItem = item;
    }
    
    template<size_t MemoryBlockSize, size_t MemoryPageSize>
    void FixedMemoryPool<MemoryBlockSize, MemoryPageSize>::Clear() {
        for (MemoryPage *page : m_memoryPageList) {
            torch::HeapFree(page);
        }
        m_memoryPageList.clear();
    }
    
    template<size_t MemoryBlockSize, size_t MemoryPageSize>
    size_t FixedMemoryPool<MemoryBlockSize, MemoryPageSize>::GetCapacity() {
        return m_memoryPageList.size() * ITEM_COUNT * MemoryBlockSize;
    }
    
    template<size_t MemoryBlockSize, size_t MemoryPageSize>
    size_t FixedMemoryPool<MemoryBlockSize, MemoryPageSize>::GetMemoryBlockSize() {
        return MemoryBlockSize;
    }
    
    template<size_t MemoryBlockSize, size_t MemoryPageSize>
    typename FixedMemoryPool<MemoryBlockSize, MemoryPageSize>::MemoryPage* FixedMemoryPool<MemoryBlockSize, MemoryPageSize>::InternalMakeMemoryPage() {
        typedef typename FixedMemoryPool<MemoryBlockSize, MemoryPageSize>::MemoryPage MemoryPage;
        MemoryPage *memoryPage = (MemoryPage *)torch::HeapMalloc(sizeof(MemoryPage));
        if (!memoryPage) { 
            return nullptr; 
        }
        for (int i = 0; i < ITEM_COUNT - 1; i++) {
            memoryPage->itemArray[i].next = &(memoryPage->itemArray[i+1]);
        }
        memoryPage->itemArray[ITEM_COUNT-1].next = nullptr;
        return memoryPage;
    }
    
}

#endif /* __TORCH__MEMORYPOOL__ */
