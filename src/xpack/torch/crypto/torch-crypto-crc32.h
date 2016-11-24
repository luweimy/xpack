//
//  Torch
//
//  Created by Luwei.
//  Copyright (c) 2014-2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy/Torch
//

#ifndef __TORCH__CRYPTO__CRC32__
#define __TORCH__CRYPTO__CRC32__

#include <stdio.h>
#include <string>
#include "../torch-data.h"

namespace torch { namespace crypto {
    
    /*
     * CRC32(32位循环冗余校验)
     * 注意：
     *  - 本类不用实例化对象
     *  - CRC32是信息校验算法，检错能力极强，开销小
     *  - 本类提供的是标准CRC32算法，结果与其他标准CRC32算法结果一致
     *  - 注意最终结果一般转为大端字节序的十六进制
     */
    class Crc32
    {
    public:
        /*
         * 实例化对象后，分块累加计算crc32值
         * 注意：
         *  - 可以分块传入累加计算，最终计算结果与一次性计算结果相同，通过[GetCrc32()]获得
         */
        Crc32();
        void ComputeBlock(const unsigned char *input, size_t length);
        void ComputeBlock(const Data &input);
        uint32_t GetCrc32();
        
        /*
         * 计算crc32值，返回4字节(32位)长度的校验数据
         * 注意：
         *  - 接口[ComputeWithFile()]若发生错误，则返回0
         */
        static uint32_t Compute(const unsigned char *input, size_t length);
        static uint32_t Compute(const Data &input);
        static uint32_t ComputeWithFile(const std::string &path);
        
        /*
         * 将crc32校验值转为字符串(大端字节序，十六进制，大写)
         */
        static std::string ToString(uint32_t crc32);
        
    private:
        uint32_t m_crc32;
    };
    
} }

#endif /* __TORCH__CRYPTO__CRC32__ */
