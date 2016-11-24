//
//  Torch
//
//  Created by Luwei.
//  Copyright (c) 2014-2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy/Torch
//

#ifndef __TORCH__COLLECTION__UTIL__
#define __TORCH__COLLECTION__UTIL__

#include <set>
#include <algorithm>
#include <vector>

namespace torch { namespace collection {
    
    // 集合的交集
    template<typename _Tp>
    std::set<_Tp> SetIntersection(const std::set<_Tp> &s1, const std::set<_Tp> &s2) {
        std::set<_Tp> res;
        std::set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), std::insert_iterator<std::set<_Tp> >(res, res.begin()));
        return std::move(res);
    }
    
    // 集合的并集
    template<typename _Tp>
    std::set<_Tp> SetUnion(const std::set<_Tp> &s1, const std::set<_Tp> &s2) {
        std::set<_Tp> res;
        std::set_union(s1.begin(), s1.end(), s2.begin(), s2.end(), std::insert_iterator<std::set<_Tp> >(res, res.begin()));
        return std::move(res);
    }
    
    // 集合的差集
    template<typename _Tp>
    std::set<_Tp> SetDifference(const std::set<_Tp> &s1, const std::set<_Tp> &s2) {
        std::set<_Tp> res;
        std::set_difference(s1.begin(), s1.end(), s2.begin(), s2.end(), std::insert_iterator<std::set<_Tp> >(res, res.begin()));
        return std::move(res);
    }
    
    // 升序数组的交集
    template<typename _Tp>
    std::vector<_Tp> SortedVectorIntersection(const std::vector<_Tp> &v1, const std::vector<_Tp> &v2) {
        std::vector<_Tp> res(std::min(v1.size(), v2.size()));
        auto it = std::set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), res.begin());
        res.resize(it - res.begin());
        return std::move(res);
    }
    
    // 升序数组的并集
    template<typename _Tp>
    std::vector<_Tp> SortedVectorUnion(const std::vector<_Tp> &v1, const std::vector<_Tp> &v2) {
        std::vector<_Tp> res(v1.size() + v2.size());
        auto it = std::set_union(v1.begin(), v1.end(), v2.begin(), v2.end(), res.begin());
        res.resize(it - res.begin());
        return std::move(res);
    }
    
    // 升序数组的差集
    template<typename _Tp>
    std::vector<_Tp> SortedVectorDifference(const std::vector<_Tp> &v1, const std::vector<_Tp> &v2) {
        std::vector<_Tp> res(std::max(v1.size(), v2.size()));
        auto it = std::set_difference(v1.begin(), v1.end(), v2.begin(), v2.end(), res.begin());
        res.resize(it - res.begin());
        return std::move(res);
    }

} }


#endif /* __TORCH__COLLECTION__UTIL__ */
