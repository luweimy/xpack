//
//  Torch
//
//  Created by Luwei.
//  Copyright (c) 2014-2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy/Torch
//

#ifndef __TORCH__COLLECTION__SORTEDSET__
#define __TORCH__COLLECTION__SORTEDSET__

#include "torch-collection-iterator.h"
#include <unordered_map>
#include <set>

namespace torch { namespace collection {
    
    template<typename _Tp>
    class SortedSet {
    public:
        typedef _Tp ValueType;
        struct SetItem {
            ValueType value; float score;
            bool operator<(const SetItem& o) const { return this->score < o.score || (this->score == o.score && this->value < o.value); }
        };
        typedef std::set<SetItem> Set;
        typedef std::unordered_map<ValueType, typename std::set<SetItem>::iterator> SetMap;
        static ValueType ValueNull;
        
        /*
         * 更新排序分数(添加值到集合)
         * 参数：
         *  - val : 添加到集合的值，若存在重复则更新其分数，不存在则直接添加到集合
         *  - score: 排序的分数，排序依据此值，升序排序
         */
        void Update(ValueType val, float score);
        
        /*
         * 从集合中删除指定的元素
         * 参数：val代表要删除的值
         * 说明：若值不存在则不发生任何改变
         */
        void Remove(ValueType val);
        
        /*
         * 从集合中删除第一个/最后一个元素
         * 说明：若集合为空则此操作不会造成任何影响
         */
        void PopFirst();
        void PopLast();
        
        /*
         * 判断集合中是否存在指定的值
         */
        bool Contains(ValueType val) { return (m_setsmap.find(val) != m_setsmap.end()); }
        
        void Clear() { m_setsmap.clear(); m_zset.clear(); }
        size_t Size() { return m_zset.size(); }
        
        /*
         * 获得集合中最大/最小的分数
         * 说明：若集合为空则返回0
         */
        float GetMinScore();
        float GetMaxScore();
        
        /*
         * 获得集合中最大/最小的分数
         * 说明：若集合为空则返回0
         */
        ValueType GetMinScoreValue();
        ValueType GetMaxScoreValue();
        
        /*
         * 从集合中获取第一个/最后一个元素
         * 说明：若集合为空则返回空值
         */
        ValueType GetFirst();
        ValueType GetLast();
                
        /*
         * 获得迭代器，用于遍历Set
         */
        OrderedIterator<Set> GetIterator() { return OrderedIterator<Set>(&m_zset); }
        
    private:
        Set     m_zset;
        SetMap  m_setsmap;
    };
    
    template<typename _Tp> _Tp SortedSet<_Tp>::ValueNull;
    
    template<typename _Tp>
    void SortedSet<_Tp>::Update(ValueType val, float score) {
        auto it = m_setsmap.find(val);
        if (it != m_setsmap.end()) { // already exist
            if (it->second->score == score) {
                return;
            }
            m_zset.erase(it->second);
        }
        SetItem e;
        e.value = val;
        e.score = score;
        
        auto r = m_zset.insert(e);
        m_setsmap[val] = r.first;
    }
    
    template<typename _Tp>
    void SortedSet<_Tp>::Remove(ValueType val) {
        auto it = m_setsmap.find(val);
        if (it != m_setsmap.end()) { // exist
            m_zset.erase(it->second);
            m_setsmap.erase(it);
        }
    }
    
    template<typename _Tp>
    void SortedSet<_Tp>::PopFirst() {
        if (!m_zset.empty()) { // exist
            auto it = m_zset.begin();
            m_setsmap.erase(it->value);
            m_zset.erase(it);
        }
    }
    
    template<typename _Tp>
    void SortedSet<_Tp>::PopLast() {
        if (!m_zset.empty()) { // exist
            auto it = --m_zset.end();
            m_setsmap.erase(it->value);
            m_zset.erase(it);
        }
    }
    
    template<typename _Tp>
    float SortedSet<_Tp>::GetMinScore() {
        if (m_zset.empty())
            return 0;
        return m_zset.begin()->score;
    }
    
    template<typename _Tp>
    float SortedSet<_Tp>::GetMaxScore() {
        if (m_zset.empty())
            return 0;
        return (--m_zset.end())->score;
    }
    
    template<typename _Tp>
    typename SortedSet<_Tp>::ValueType SortedSet<_Tp>::GetMinScoreValue() {
        if (m_zset.empty())
            return ValueNull;
        return m_zset.begin()->value;
    }
    
    template<typename _Tp>
    typename SortedSet<_Tp>::ValueType SortedSet<_Tp>::GetMaxScoreValue() {
        if (m_zset.empty())
            return ValueNull;
        return (--m_zset.end())->value;
    }
    
    template<typename _Tp>
    typename SortedSet<_Tp>::ValueType SortedSet<_Tp>::GetFirst() {
        if (m_zset.empty())
            return ValueNull;
        return m_zset.begin()->value;
    }
    
    template<typename _Tp>
    typename SortedSet<_Tp>::ValueType SortedSet<_Tp>::GetLast() {
        if (m_zset.empty())
            return ValueNull;
        return (--m_zset.end())->value;
    }
    
} }

#endif /* __TORCH__COLLECTION__SORTEDSET__ */
