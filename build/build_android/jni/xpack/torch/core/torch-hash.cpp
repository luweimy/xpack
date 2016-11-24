//
//  Torch
//
//  Created by Luwei.
//  Copyright (c) 2014-2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy/Torch
//

#include <string.h>
#include <inttypes.h>
#include "torch-hash.h"
#include "../deps/xxhash/xxhash.h"
#include "../deps/jenkins/lookup.h"

using namespace torch;

uint32_t Hash::BKDR(const char* data,  size_t length)
{
    unsigned int seed = 131; /* 31 131 1313 13131 131313 etc.. */
    unsigned int hash = 0;
    unsigned int i    = 0;
    
    for (i = 0; i < length; data++, i++) {
        hash = (hash * seed) + (*data);
    }
    
    return hash;
}

uint32_t Hash::DJB(const char* data, size_t length)
{
    unsigned int hash = 5381;
    unsigned int i    = 0;
    
    for (i = 0; i < length; data++, i++) {
        hash = ((hash << 5) + hash) + (*data);
    }
    
    return hash;
}

uint32_t Hash::AP(const char* data, size_t length)
{
    unsigned int hash = 0xAAAAAAAA;
    unsigned int i    = 0;
    
    for (i = 0; i < length; data++, i++) {
        hash ^= ((i & 1) == 0) ? (  (hash <<  7) ^ (*data) * (hash >> 3)) :
        (~((hash << 11) + ((*data) ^ (hash >> 5))));
    }
    
    return hash;
}

uint64_t Hash::Jenkins64(const char* data, size_t length)
{
    uint32_t primary_hash = 1, secondary_hash = 1;
    hashlittle2(data, length, &secondary_hash, &primary_hash);
    return ((uint64_t)primary_hash << 0x20) | (uint64_t)secondary_hash;
}

uint32_t Hash::Jenkins32(const char* data, size_t length, int seed)
{
    return hashlittle(data, length, seed);
}

uint32_t Hash::XXHash32(const char* data, size_t length, int seed)
{
    return XXH32(data, length, seed);
}

uint64_t Hash::XXHash64(const char* data, size_t length, int seed)
{
    return XXH64(data, length, seed);
}

uint32_t Hash::x86::Murmur32(const char* data, size_t length, int seed)
{
    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;
    const uint32_t r1 = 15;
    const uint32_t r2 = 13;
    const uint32_t m = 5;
    const uint32_t n = 0xe6546b64;
    
    uint32_t hash = seed;
    
    const int nblocks = (int)length / 4;
    const uint32_t *blocks = (const uint32_t *)data;
    int i;
    uint32_t k;
    for (i = 0; i < nblocks; i++) {
        k = blocks[i];
        k *= c1;
        k = ((k << r1) | (k >> (32 - r1)));
        k *= c2;
        
        hash ^= k;
        hash = ((hash << r2) | (hash >> (32 - r2))) * m + n;
    }
    
    const uint8_t *tail = (const uint8_t *) (data + nblocks * 4);
    uint32_t k1 = 0;
    
    switch (length & 3) {
        case 3:
            k1 ^= tail[2] << 16;
        case 2:
            k1 ^= tail[1] << 8;
        case 1:
            k1 ^= tail[0];
            
            k1 *= c1;
            k1 = ((k1 << r1) | (k1 >> (32 - r1)));
            k1 *= c2;
            hash ^= k1;
    }
    
    hash ^= length;
    hash ^= (hash >> 16);
    hash *= 0x85ebca6b;
    hash ^= (hash >> 13);
    hash *= 0xc2b2ae35;
    hash ^= (hash >> 16);
    
    return hash;
}

uint64_t Hash::x86::Murmur64(const char* data, size_t length, int seed)
{
    const unsigned int m = 0x5bd1e995;
    const int r = 24;
    
    unsigned int h1 = seed ^ (unsigned)length;
    unsigned int h2 = 0;
    
    const unsigned int * buf = (const unsigned int *)data;
    
    while (length >= 8) {
        unsigned int k1 = *buf++;
        k1 *= m; k1 ^= k1 >> r; k1 *= m;
        h1 *= m; h1 ^= k1;
        length -= 4;
        
        unsigned int k2 = *buf++;
        k2 *= m; k2 ^= k2 >> r; k2 *= m;
        h2 *= m; h2 ^= k2;
        length -= 4;
    }
    
    if (length >= 4) {
        unsigned int k1 = *buf++;
        k1 *= m; k1 ^= k1 >> r; k1 *= m;
        h1 *= m; h1 ^= k1;
        length -= 4;
    }
    
    switch (length) {
        case 3: h2 ^= ((unsigned char*)buf)[2] << 16;
        case 2: h2 ^= ((unsigned char*)buf)[1] << 8;
        case 1: h2 ^= ((unsigned char*)buf)[0];
            h2 *= m;
    };
    
    h1 ^= h2 >> 18; h1 *= m;
    h2 ^= h1 >> 22; h2 *= m;
    h1 ^= h2 >> 17; h1 *= m;
    h2 ^= h1 >> 19; h2 *= m;
    
    uint64_t h = h1;
    
    h = (h << 32) | h2;
    
    return h;
}

uint64_t Hash::x64::Murmur64(const char* data, size_t length, int seed)
{
    const uint64_t m = 0xc6a4a7935bd1e995;
    const int r = 47;
    
    uint64_t h = seed ^ (length * m);
    
    const uint64_t * buf = (const uint64_t *)data;
    const uint64_t * end = buf + (length/8);
    
    while (buf != end) {
        uint64_t k = *buf++;
        
        k *= m;
        k ^= k >> r;
        k *= m;
        
        h ^= k;
        h *= m;
    }
    
    const unsigned char * buf2 = (const unsigned char*)buf;
    
    switch (length & 7) {
        case 7: h ^= uint64_t(buf2[6]) << 48;
        case 6: h ^= uint64_t(buf2[5]) << 40;
        case 5: h ^= uint64_t(buf2[4]) << 32;
        case 4: h ^= uint64_t(buf2[3]) << 24;
        case 3: h ^= uint64_t(buf2[2]) << 16;
        case 2: h ^= uint64_t(buf2[1]) << 8;
        case 1: h ^= uint64_t(buf2[0]);
            h *= m;
    };
    
    h ^= h >> r;
    h *= m;
    h ^= h >> r;
    
    return h;
}

std::string Hash::ToString(uint32_t hash)
{
    char buf[10];
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%08X", hash);
    return std::string(buf);
}

std::string Hash::ToString(uint64_t hash)
{
    char buf[20];
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%llu", hash);
    return std::string(buf);
}
