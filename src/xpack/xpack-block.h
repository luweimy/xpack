//
//  xpack
//
//  Created by Luwei.
//  Copyright (c) 2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy
//

#ifndef __XPACK__BLOCK__
#define __XPACK__BLOCK__

#include <stdio.h>
#include <vector>
#include <unordered_set>
#include "xpack-def.h"
#include "torch/torch.h"

namespace xpack {
    
    /* 
     * 记录block-content可重用的区域
     * 负责记录block-content可重用的blockindex并且分别按照block的size和block的offset分别排序
     * size排序用于重用决策时，选择合适尺寸的block-content
     * offset排序用于删除块时，将位于文件末尾的无效数据块全部清除
     */
    class ContentReuser {
    public:
        void UpdateEntry(int32_t index, MetaBlock *metablock) {
            m_orderdByOffset.Update(index, metablock->offset); m_orderdBySize.Update(index, metablock->size);
        }
        void RemoveEntry(int32_t index) {
            m_orderdByOffset.Remove(index); m_orderdBySize.Remove(index);
        }
        void Clear() {
            m_orderdByOffset.Clear(); m_orderdBySize.Clear();
        }
        int32_t GetMinimumSizeEntry() {
            return m_orderdBySize.GetMinScoreValue();
        }
        int32_t GetMaximumOffsetEntry() {
            return m_orderdByOffset.GetMaxScoreValue();
        }
        size_t Size() {
            assert(m_orderdBySize.Size() == m_orderdByOffset.Size());
            return m_orderdByOffset.Size();
        }
        bool Contains(int32_t index) {
            return m_orderdBySize.Contains(index);
        }
        // iterator
        torch::collection::OrderedIterator<torch::collection::SortedSet<int32_t>::Set> GetOrderdByOffsetAscendingIterator() {
            return m_orderdBySize.GetIterator();
        }
        torch::collection::OrderedIterator<torch::collection::SortedSet<int32_t>::Set> GetOrderdBySizeAscendingIterator() {
            return m_orderdBySize.GetIterator();
        }
        
    private:
        torch::collection::SortedSet<int32_t> m_orderdByOffset;
        torch::collection::SortedSet<int32_t> m_orderdBySize;
    };
    
    class BlockReuser {
    public:
        void UpdateEntry(int32_t index) {
            m_orderedByIndex.insert(index);
        }
        void RemoveEntry(int32_t index) {
            m_orderedByIndex.erase(index);
        }
        void Clear() {
            m_orderedByIndex.clear();
        }
        int32_t GetMinimumIndexEntry() {
            return *m_orderedByIndex.begin();
        }
        int32_t GetMaximumIndexEntry() {
            return *(--m_orderedByIndex.end());
        }
        size_t Size() {
            return m_orderedByIndex.size();
        }
        bool Contains(int32_t index) {
            return m_orderedByIndex.find(index) != m_orderedByIndex.end();
        }
        torch::collection::OrderedIterator<std::set<int32_t>> GetOrderedByIndexAscendingIterator() {
            return torch::collection::OrderedIterator<std::set<int32_t>>(&m_orderedByIndex);
        }
    private:
        std::set<int32_t> m_orderedByIndex; // ordered set(asc)
    };
    
    class Context;
    class BlockSegment {
    public:
        enum { RESERVE_COUNT = 2000 };
        
        BlockSegment(Context *ctx);
        ~BlockSegment();
        
        /*
         * 读取区域数据
         * 说明：
         *  - 根据MetaHeader.block_offset确定位置
         *  - 构建重用池
         *  - 若不存在或者格式错误，则返回false
         */
        bool ReadFromStream();
        
        /*
         * 将区域数据写入
         * 说明：
         *  - 根据MetaHeader.block_offset确定位置
         *  - 内容会被全部写入
         */
        bool WriteToStream();
        
        /*
         * 检测是否存在合法的Block区域
         */
        bool IsValid();
        
        /*
         * 删除指定index的Block及其代表的内容
         * 说明：
         *  - 删除内容的节点必须为Block链的起始节点，然后会将整条链都放入Content重用池，以记录内容区域的重用
         *  - 优化：将无用的Content区域末尾内容删除，会减小MetaHeader.content_size
         *  - 优化：将记录Content区域过小的Block记录抛弃
         *  - 优化：将无用的Block区域末尾项删除，会减小MetaHeader.block_count
         */
        void RemoveByIndex(int32_t index);

        /*
         * 根据index返回对应的MetaBlock
         * 注意：
         *  - 若超出范围则返回null
         */
        MetaBlock* GetByIndex(int32_t index);
        
        /*
         * 申请指定大小尺寸的Block链表
         * 参数：
         *  - size: 要申请的尺寸大小(Byte)
         * 返回：链表头的block_index
         * 说明：
         *  - 其获得的Block可能是一个Block链，Block链上的所有Block代表的区域大小之和等于给定的大小
         *  - 优先利用Content重用池中的小尺寸块
         *  - 当没有可重用时，才会去创建新Block，此时会影响MetaHeader中的content_size,block_offset,hash_offset
         */
        int32_t AllocLinkedBlock(uint32_t size);
        
        /*
         * 清空所有内容
         */
        void Clear();

        /*
         * 获得所有的Block个数，包含被标记为可重用的
         */
        uint32_t GetBlockNumber();
        
        /*
         * 给定block指针，返回此block的block_index
         */
        int32_t ConvertBlockPtrToIndex(MetaBlock *metablock);

        /*
         * 获得重用池中的复用Block
         */
        ContentReuser* GetContentReuser();
        BlockReuser*   GetBlockReuser();

        /*
         * 内部调试接口
         */
        std::string DumpBlock(bool cotainUnused=false);
        std::string DumpBlockByIndex(const int32_t index);
        std::string DumpBlockByName(const std::string &name);
        std::string DumpBlockUnused();
        std::string DumpBlockContentUnused();

    private: 
        void InternalAddingBlockToReuserByIndex(int32_t index);
        int32_t InternalGetOrCreate();

    private:
        torch::Data   m_blocks; // Metablock entries memory
        Context      *m_context;
        
        ContentReuser m_contentReuser; // Content unused
        BlockReuser   m_blockReuser;   // Metablock unused
    };
}

#endif /* __XPACK__BLOCK__ */
