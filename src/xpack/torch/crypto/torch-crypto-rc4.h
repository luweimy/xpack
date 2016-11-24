//
//  Torch
//
//  Created by Luwei.
//  Copyright (c) 2014-2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy/Torch
//

#ifndef __TORCH__CRYPTO__RC4__
#define __TORCH__CRYPTO__RC4__

#include <stdio.h>
#include <string>
#include "../torch-data.h"
#include "../deps/xyssl/include/arc4.h"

namespace torch { namespace crypto {
    
    /*
     * RC4加密算法
     * 注意：
     *  - RC4算法具有实现简单，加密数度快，对硬件资源消耗费用低等优点
     *  - RC4是一种对称密码算法
     *  - RC4算法加密和解密使用同一个接口
     *  - RC4加密前后数据大小不变
     *  - 提供了在源数据内存直接加密的接口
     */
    class RC4
    {
    public:
        /*
         * 设置RC4加密算法的密钥，内部会将密钥copy一份
         */
        RC4& SetSecretKey(const unsigned char *skey, size_t length);
        
        /*
         * 加密/解密
         * 说明：
         *  - 会将加密后的数据放入新申请的内存中，不会改动源数据。
         *  - 加密和解密都调用同一个接口
         */
        Data CryptoCopy(const unsigned char *input, size_t length);
        Data CryptoCopy(const Data &input);
        void CryptoCopy(const Data &input, Data &output);

        /*
         * 加密/解密
         * 说明：
         *  - 加密后的数据直接覆盖元数据，不会申请新的内存
         *  - 加密和解密都调用同一个接口
         */
        void CryptoNoCopy(unsigned char *input, size_t length);
        void CryptoNoCopy(Data &input);
        
        /*
         * 加密/解密文件的内容
         * 注意：
         *  - 接口若发生错误，则返回空的Data对象
         */
        Data CryptoWithFile(const std::string &path);
        
    private:
        torch::Data  m_skey;
        arc4_context m_rc4ctx;
    };
    
} }

#endif /* __TORCH__CRYPTO__RC4__ */
