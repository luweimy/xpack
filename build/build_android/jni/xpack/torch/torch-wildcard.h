//
//  Torch
//
//  Created by Luwei.
//  Copyright (c) 2014-2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy/Torch
//

#ifndef __TORCH__WILDCARD__
#define __TORCH__WILDCARD__

#include <stdlib.h>
#include <string>

namespace torch {
    
    /*
     * 通配符匹配
     *
     * 语法定义:
     *  - * matches any sequence of characters, including zero.
     *  - ? matches exactly one character which can be anything.
     *  - [abc] matches exactly one character which is a, b or c.
     *  - [a-f] matches anything from a through f.
     *  - [^a-f] matches anything _except_ a through f.
     *  - [-_] matches - or _; [^-_] matches anything else. (The - is
     *    non-special if it occurs immediately after the opening
     *    bracket or ^.)
     *  - [a^] matches an a or a ^. (The ^ is non-special if it does
     *    _not_ occur immediately after the opening bracket.)
     *  - \*, \?, \[, \], \\ match the single characters *, ?, [, ], \.
     *  - All other characters are non-special and match themselves.
     */
    class WildcardMatcher {
    public:
        
        /*
         * 通配符匹配
         * 参数：
         *  - wildcard: 通配符搜索表达式
         *  - target: 要对其进行搜索的目标串
         * 返回值：若匹配成功则返回1，若匹配失败则返回0，若返回小于0则代表通配符表达式有语法错误。
         */
        static int Match(const std::string &wildcard, const std::string &target);
        static const char* FormatError(int error);
    };
    
}

#endif /* __TORCH__WILDCARD__ */
