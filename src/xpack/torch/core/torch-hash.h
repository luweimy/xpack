//
//  Torch
//
//  Created by Luwei.
//  Copyright (c) 2014-2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy/Torch
//

#ifndef __TORCH__HASH__
#define __TORCH__HASH__


#include <iostream>

namespace torch {
    
    class Hash
    {
    public:
        /*
         * - BKDR：BKDR是一种简单快捷的hash算法，效率略高于DJB，也是Java目前采用的字符串的Hash算法
         * - DJB(time33)：大多主流编程语言都采用的哈希算法是DJB，.Net中就使用DJB，其有着极快的计算效率和很好的哈希分布
         * - APHash
         * - JenkinsHash(lookup3.c)
         * - XXHash
         * - Murmur
         */

        static uint32_t BKDR(const char* data,  size_t length);
        static uint32_t DJB(const char* data, size_t length);
        static uint32_t AP(const char* data, size_t length);
        static uint64_t Jenkins64(const char* data, size_t length);
        static uint32_t Jenkins32(const char* data, size_t length, int seed = 131);
        static uint32_t XXHash32(const char* data, size_t length, int seed = 131);
        static uint64_t XXHash64(const char* data, size_t length, int seed = 131);
        
        class x86 {
        public:
            /*
             * Murmur算法(推荐Murmur64)
             * 说明：
             *  - Murmur是高运算性能，低碰撞率的Hash算法，现已应用到Hadoop、libstdc++、nginx、libmemcached等开源系统
             *  - 从计算速度上来看，MurmurHash只适用于已知长度的、长度比较长的字符
             */
            static uint32_t Murmur32(const char* data, size_t length, int seed = 131);
            static uint64_t Murmur64(const char* data, size_t length, int seed = 131);
            
        };
        
        class x64 {
        public:
            /*
             * Murmur算法(推荐)
             * 说明：
             *  - Murmur是高运算性能，低碰撞率的Hash算法，现已应用到Hadoop、libstdc++、nginx、libmemcached等开源系统
             *  - 从计算速度上来看，MurmurHash只适用于已知长度的、长度比较长的字符
             */
            static uint64_t Murmur64(const char* data, size_t length, int seed = 131);
            
        };
        
        static std::string ToString(uint32_t hash);
        static std::string ToString(uint64_t hash);
    };
    
}

#endif /* defined(__Torch__torch_hash__) */
