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
         * BKDR算法
         * 说明：BKDR是一种简单快捷的hash算法，效率略高于DJB，也是Java目前采用的字符串的Hash算法
         */
        static uint32_t BKDR(const char* data,  size_t length);
        
        /*
         * DJB算法(time33)
         * 说明：几乎所有的流行的HashMap都采用了DJB Hash，其有着极快的计算效率和很好的哈希分布
         */
        static uint32_t DJB(const char* data, size_t length);
        
        /*
         * APHash算法
         */
        static uint32_t AP(const char* data, size_t length);
        
        /*
         * JenkinsHash算法(lookup3.c)
         */
        static uint64_t Jenkins64(const char* data, size_t length);
        static uint32_t Jenkins32(const char* data, size_t length, int seed = 131);
        
        /*
         * XXHash算法
         */
        static uint32_t XXHash32(const char* data, size_t length, int seed = 131);
        static uint64_t XXHash64(const char* data, size_t length, int seed = 131);
        
        /*
         * 适用于32位系统的Hash
         */
        class x86 {
        public:
            /** 32-bit hash for 32-bit platforms */
            
            /*
             * Murmur算法(推荐Murmur64)
             * 说明：
             *  - Murmur是高运算性能，低碰撞率的Hash算法，现已应用到Hadoop、libstdc++、nginx、libmemcached等开源系统
             *  - 从计算速度上来看，MurmurHash只适用于已知长度的、长度比较长的字符
             */
            static uint32_t Murmur32(const char* data, size_t length, int seed = 131);
            static uint64_t Murmur64(const char* data, size_t length, int seed = 131);
            
        };
        
        /*
         * 适用于64位系统的Hash
         */
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
