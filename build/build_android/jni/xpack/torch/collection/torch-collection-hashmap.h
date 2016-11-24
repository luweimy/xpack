//
//  Torch
//
//  Created by Luwei.
//  Copyright (c) 2014-2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy/Torch
//

#ifndef __TORCH__COLLECTION__HASHMAP__
#define __TORCH__COLLECTION__HASHMAP__

#include <stdio.h>
#include <vector>
#include <unordered_map>
#include "torch-collection-iterator.h"

namespace torch { namespace collection {
    
    template <typename _Key, typename _Value>
    class HashMap {
    public:
        typedef _Key   KeyType;
        typedef _Value ValueType;
        typedef std::pair<KeyType, ValueType>          UnorderedMapPair;
        typedef std::unordered_map<KeyType, ValueType> UnorderedMap;
        
        static KeyType   KeyNull;
        static ValueType ValueNull;
        
        /*
         * 若存在相同的key，不会替换Value，返回设置失败False
         */
        bool Set(KeyType k, ValueType v) { auto ok = m_map.insert(UnorderedMapPair(k, v)); return ok.second; }
        
        /*
         * 若存在相同的Key，则更新Value，若不存在相同Key则直接设置
         */
        bool Update(KeyType k, ValueType v) { if (m_map.find(k) != m_map.end()) m_map.erase(k); return this->Set(k, v); }
        
        /*
         * 获得指定Key的Value
         * 说明：若元素不存在，则返回ValueNull; 但是若元素不为指针，则返回空的对象。
         */
        ValueType Get(KeyType k) { auto i = m_map.find(k); if (i == m_map.end()) return ValueNull; return i->second; }
        
        /*
         * 获得所有的value值，此方法性能较低
         */
        std::vector<ValueType> AllValues();
        
        /*
         * 获得所有的key，此方法性能较低
         */
        std::vector<KeyType> AllKeys();
        
        /*
         * 获得所有指定值的key，此方法性能较低
         */
        std::vector<KeyType> AllKeysForValue(ValueType v);
        
        KeyType GetKey() { if (m_map.begin() != m_map.end()) return m_map.begin()->first; return KeyNull; }
        
        /*
         * 判断Key是否存在
         */
        bool HasKey(KeyType k) { return !(m_map.find(k) == m_map.end()); }
        
        /*
         * Key存在且将其删除才会返回True，否则返回False
         */
        bool Remove(KeyType k) { return m_map.erase(k) == 1; }
        
        /*
         * 清空所有元素
         */
        void Clear() { m_map.clear(); }
        
        /*
         * 获得Map中的元素数量
         */
        size_t Size() { return m_map.size(); }
        
        /*
         * 获得迭代器，用于遍历Map
         */
        Iterator<UnorderedMap> GetIterator() { return Iterator<UnorderedMap>(&m_map); }
        
    private:
        UnorderedMap m_map;
    };
    
    template <typename _Key, typename _Value> _Key   HashMap<_Key, _Value>::KeyNull;
    template <typename _Key, typename _Value> _Value HashMap<_Key, _Value>::ValueNull;
    
    template <typename _Key, typename _Value>
    std::vector<typename HashMap<_Key, _Value>::ValueType> HashMap<_Key, _Value>::AllValues() {
        std::vector<ValueType> values;
        for (auto x : m_map) {
            values.push_back(x.second);
        }
        return std::move(values);
    }
    
    template <typename _Key, typename _Value>
    std::vector<typename HashMap<_Key, _Value>::KeyType> HashMap<_Key, _Value>::AllKeys() {
        std::vector<KeyType> keys;
        for (auto x : m_map) {
            keys.push_back(x.first);
        }
        return std::move(keys);
    }
    
    template <typename _Key, typename _Value>
    std::vector<typename HashMap<_Key, _Value>::KeyType> HashMap<_Key, _Value>::AllKeysForValue(ValueType v) {
        std::vector<KeyType> keys;
        for (auto x : m_map) {
            if (x.second == v) {
                keys.push_back(x.first);
            }
        }
        return std::move(keys);
    }
    
    
} }

#endif /* __TORCH__COLLECTION__HASHMAP__ */
