//
//  xpack
//
//  Created by Luwei.
//  Copyright (c) 2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy
//

#ifndef __XPACK__CONTEXT__
#define __XPACK__CONTEXT__

#include <stdio.h>
#include "xpack-def.h"
#include "xpack.h"

namespace xpack {
    
    class Stream;
    class SignatureSegment;
    class HeaderSegment;
    class ContentSegment;
    class BlockSegment;
    class HashSegment;
    class NameSegment;

    class Context {
    public:
        Context(Stream *stm);
        ~Context();
        
        Stream             *stream;
        
        SignatureSegment   *signature;
        HeaderSegment      *header;
        ContentSegment     *content;
        BlockSegment       *block;
        HashSegment        *hash;
        NameSegment        *name;
        
        uint32_t            offset;    /* xpack archive offset */

        torch::crypto::RC4 *crypto;

        uint32_t (*HashMaker)(const std::string &s, uint8_t seed);

    public:
        /*
         * 设置偏移量
         * 说明：
         * - 内部会将ctx->offset偏移对齐到xpack::SIGNATURE_ALIGNED(512)
         *  - 可以直接设置ctx->offset，其会在内部保障数据存储位置的对齐
         */
        void SetAlignedOffset(uint32_t offset);
    };

}

#endif /* __XPACK__CONTEXT__ */
