//
//  xpack
//
//  Created by Luwei.
//  Copyright (c) 2016年 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy
//

#ifndef __TEST__H__
#define __TEST__H__

#include <iostream>
#include <vector>
#include <string>

#define TEST_TRUE(_C) assert(_C)

static void ErrorLog(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
}

static void InfoLog(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
}

static bool IsVectorContainString(const std::vector<std::string> &array, const std::string &str) {
    for (auto s : array) {
        if (s == str) {
            return true;
        }
    }
    return false;
}

#endif
