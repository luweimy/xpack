//
//  xpack
//
//  Created by Luwei.
//  Copyright (c) 2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy
//

#ifndef __XPACK__BASE__
#define __XPACK__BASE__

#include <stdio.h>
#include <stdint.h>
#include <string>

namespace xpack {
        
    /*
     * 计算字符串的Hash值(32位)
     * 参数：
     *  - seed: 种子序号，内部会根据序号查表找到一个素数，做为计算Hash值的真正种子
     */
    uint32_t HashString(const std::string &s, uint8_t seed = 0);

    /*
     * 设置/获取错误代码
     */
    void SetLastError(int errcode);
    void SetLastError(int errcode, const char *where);
    int GetLastError();

    const char* GetLastErrorMessage();
    
}

#endif /* __XPACK__BASE__ */
