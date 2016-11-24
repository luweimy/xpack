//
//  Torch
//
//  Created by Luwei.
//  Copyright (c) 2014-2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy/Torch
//

#include <stdio.h>
#include <sstream>
#include <inttypes.h>
#include "torch-util.h"
#include "torch-string.h"

using namespace torch;

void torch::DumpBool(bool x)
{
    printf("%s\n", torch::String::ToCppString(x).c_str());
}

void torch::Dump()
{
    printf("\n");
}

void torch::Dump(std::string s1)
{
    printf("%s\n", s1.c_str());
}

void torch::Dump(std::string s1, std::string s2)
{
    printf("%s\t%s\n", s1.c_str(), s2.c_str());
}

void torch::Dump(std::string s1, std::string s2, std::string s3)
{
    printf("%s\t%s\t%s\n", s1.c_str(), s2.c_str(), s3.c_str());
}

void torch::Dump(std::string s1, std::string s2, std::string s3, std::string s4)
{
    printf("%s\t%s\t%s\t%s\n", s1.c_str(), s2.c_str(), s3.c_str(), s4.c_str());
}

void torch::Dump(std::string s1, std::string s2, std::string s3, std::string s4, std::string s5)
{
    printf("%s\t%s\t%s\t%s\t%s\n", s1.c_str(), s2.c_str(), s3.c_str(), s4.c_str(), s5.c_str());
}

void torch::Dump(const char *raw, const size_t size)
{
    for (size_t i = 0; i < size; i++) {
        printf("%c", raw[i]);
    }
    printf("\n");
}

std::string torch::ByteToHumanReadableString(unsigned long long bytes)
{
    if (bytes < 1024) { // Byte
        return torch::String::Format("%lluB", bytes);
    }
    else if (bytes < 1024 * 1024) { // KB
        return torch::String::Format("%.2fK", torch::ByteToKB(bytes));
    }
    else if (bytes < 1024 * 1024 * 1024) { // MB
        return torch::String::Format("%.2fM", torch::ByteToMB(bytes));
    }
    else { // GB
        return torch::String::Format("%.2fG", torch::ByteToGB(bytes));
    }
}

float torch::ByteToKB(unsigned long long bytes)
{
    return bytes / 1024.f;
}

float torch::ByteToMB(unsigned long long bytes)
{
    return bytes / 1024.f / 1024.f;
}

float torch::ByteToGB(unsigned long long bytes)
{
    return bytes / 1024.f / 1024.f / 1024.f;
}

