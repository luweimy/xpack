//
//  Torch
//
//  Created by Luwei.
//  Copyright (c) 2014-2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy/Torch
//

#include "torch-base.h"
#include <stdlib.h>
#include <assert.h>
#include <unordered_map>
#include <sstream>

using namespace torch;

static int s_allocateCounter = 0;

void *torch::HeapMalloc(size_t size) {
    ++ s_allocateCounter;
    return ::malloc(size);
}

void *torch::HeapCalloc(size_t count, size_t size) {
    ++ s_allocateCounter;
    return ::calloc(count, size);
}

void *torch::HeapReAlloc(void *ptr, size_t size) {
    if (ptr == nullptr) {
        ++ s_allocateCounter;
    }
    if (size == 0) {
        -- s_allocateCounter;
    }
    return ::realloc(ptr, size);
}

void torch::HeapFree(void *ptr) {
    if (ptr) {
        -- s_allocateCounter;
        return ::free(ptr);
    }
}

int torch::HeapCounter() {
    return s_allocateCounter;
}

int torch::HeapCounterRetain() {
    return ++s_allocateCounter;
}

int torch::HeapCounterRelease() {
    return --s_allocateCounter;
}

std::string torch::ToHex(const char *data, size_t length, int blankByte)
{
    static char s_hextable[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    std::ostringstream osstream;
    
    for (size_t i = 0; i < length; i++) {
        unsigned char uc = data[i];
        osstream<< s_hextable[(uc>>4)] << s_hextable[(uc&0x0f)];
        if ( blankByte>0 && ((i+1)%blankByte == 0) && (i+1)<length ) {
            osstream<<(' ');
        }
    }
    return osstream.str();
}

std::string torch::ToBinary(const char *data, size_t length, int blankByte)
{
    std::ostringstream osstream;
    
    for (size_t i = 0; i < length; i++) {
        unsigned char uc = data[i];
        unsigned char index = 0x80;
        while (index > 0) {
            osstream<<(uc & index ? '1' : '0');
            index >>= 1;
        }
        if ( blankByte>0 && ((i+1)%blankByte == 0) && (i+1)<length ) {
            osstream<<(' ');
        }
    }
    return osstream.str();
}

void torch::DumpHex(const char *data, size_t length, int blankByte)
{
    printf("%s\n", torch::ToHex(data, length, blankByte).c_str());
}

void torch::DumpBinary(const char *data, size_t length, int blankByte)
{
    printf("%s\n", torch::ToBinary(data, length, blankByte).c_str());
}
