//
//  xpack
//
//  Created by Luwei.
//  Copyright (c) 2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy
//

#ifndef __XPACK__UTIL__
#define __XPACK__UTIL__

#include <stdio.h>
#include <string>
#include <vector>
#include "torch/torch.h"
#include "xpack-def.h"

namespace xpack {
    class SignatureSegment;
    class HeaderSegment;
    class BlockSegment;
    class HashSegment;
    class NameSegment;
    class Stream;
    class Context;

    class Utils {
    public:
        /*
         * 获得指定项的冲突跳转链
         * 返回值：hashid的数组，hashid可以在HashSegment中获得对应得MetaHash
         */
        static std::vector<uint32_t> GetHashIDChainByEntryName(Context *ctx, const std::string &name);
        
        /*
         * 获得文件中所有标记为冲突MetaHash的hashid
         * 返回值：hashid的数组
         */
        static std::vector<uint32_t> GetAllHashIDMarkedConflict(Context *ctx);

        /*
         * 获得指定block项的index链
         * 注意：必须是从链首开始获取
         * 返回值：blockindex数组
         */
        static std::vector<int32_t> GetBlockIndexChainByMetablock(Context *ctx, MetaBlock *metablock);
        
        /*
         * 获得指定block项的name
         * 参数：
         *  - index: 支持非链表头index
         * 返回值：block对应的名称
         */
        static std::string GetNameByBlockIndex(Context *ctx, int32_t index);

    };
    
    
    class DumpUtils {
    public:
        enum { KEY_LENGTH = 15 };
        
        static std::string DumpHashIDChainByEntryName(Context *ctx, const std::string &name); // If no conflicts return empty string
        static std::string DumpAllHashIDMarkedConflict(Context *ctx);
        static std::string DumpAllHashIDChain(Context *ctx);

        static std::string DumpData(Context *ctx, uint32_t offset, uint32_t size);
        static std::string DumpData(Context *ctx);

        static std::string DumpBlock(Context *ctx, MetaBlock *metablock);
        static std::string DumpBlock(Context *ctx, const std::string &name);
        static std::string DumpBlock(Context *ctx, int32_t index);
        static std::string DumpHash(Context *ctx, MetaHash *metahash);
        static std::string DumpHash(Context *ctx, const std::string &name);
        static std::string DumpHash(Context *ctx, const uint32_t hashid);

        static std::string DumpSignature(Context *ctx);
        static std::string DumpHeader(Context *ctx);
        static std::string DumpBlocks(Context *ctx, bool containUnused = false);
        static std::string DumpBlockUnused(Context *ctx);
        static std::string DumpBlockContentUnused(Context *ctx);
        static std::string DumpHashs(Context *ctx);
        static std::string DumpNames(Context *ctx);

    };
    
}

#endif /* __XPACK__UTIL__ */
