//
//  Torch
//
//  Created by Luwei.
//  Copyright (c) 2014-2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy/Torch
//

#ifndef __TORCH__UTIL__
#define __TORCH__UTIL__

#include <stdio.h>
#include <string>
#include <vector>

#include "torch-data.h"

namespace torch {
    
    void DumpBool(bool x);
    
    void Dump();
    void Dump(std::string s1);
    void Dump(std::string s1, std::string s2);
    void Dump(std::string s1, std::string s2, std::string s3);
    void Dump(std::string s1, std::string s2, std::string s3, std::string s4);
    void Dump(std::string s1, std::string s2, std::string s3, std::string s4, std::string s5);
    
    void Dump(const char *raw, const size_t size);
    
    std::string ByteToHumanReadableString(unsigned long long bytes);
    float ByteToKB(unsigned long long bytes);
    float ByteToMB(unsigned long long bytes);
    float ByteToGB(unsigned long long bytes);
    
}

#endif
