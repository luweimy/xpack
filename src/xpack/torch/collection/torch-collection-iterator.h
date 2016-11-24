//
//  Torch
//
//  Created by Luwei.
//  Copyright (c) 2014-2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy/Torch
//

#ifndef __TORCH__COLLECTION__ITERATOR__
#define __TORCH__COLLECTION__ITERATOR__
#include <stdio.h>
#include <unordered_map>

namespace torch { namespace collection {
    
    template <typename _Tp>
    class Iterator {
    public:
        typedef _Tp CollectionType;
        
        Iterator(CollectionType *obj) : m_object(obj)
        {}
        
        typename CollectionType::iterator begin() { return m_object ? m_object->begin() : typename CollectionType::iterator(); }
        typename CollectionType::const_iterator begin() const { return m_object ? m_object->begin() : typename CollectionType::iterator(); }
        typename CollectionType::iterator end() { return m_object ? m_object->end() : typename CollectionType::iterator(); }
        typename CollectionType::const_iterator end() const { return m_object ? m_object->end() : typename CollectionType::iterator(); }
        
    protected:
        CollectionType *m_object;
    };
    
    template <typename _Tp>
    class OrderedIterator : public Iterator<_Tp> {
    public:
        typedef _Tp CollectionType;
        
        OrderedIterator(CollectionType *obj) : Iterator<_Tp>(obj)
        {}
        
        typename CollectionType::reverse_iterator rbegin() { return this->m_object ? this->m_object->rbegin() : typename CollectionType::reverse_iterator(); }
        typename CollectionType::const_reverse_iterator rbegin() const { return this->m_object ? this->m_object->rbegin() : typename CollectionType::reverse_iterator(); }
        typename CollectionType::reverse_iterator rend() { return this->m_object ? this->m_object->rend() : typename CollectionType::reverse_iterator(); }
        typename CollectionType::const_reverse_iterator rend() const { return this->m_object ? this->m_object->rend() : typename CollectionType::reverse_iterator(); }
    };
    
} }


#endif /* __TORCH__COLLECTION__ITERATOR__ */
