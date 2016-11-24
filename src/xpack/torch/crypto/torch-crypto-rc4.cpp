//
//  Torch
//
//  Created by Luwei.
//  Copyright (c) 2014-2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy/Torch
//

#include "torch-crypto-rc4.h"
#include "../torch-file.h"

using namespace torch::crypto;

RC4& RC4::SetSecretKey(const unsigned char *skey, size_t length)
{
    m_skey.CopyFrom(skey, length);
    return *this;
}

torch::Data RC4::CryptoCopy(const unsigned char *input, size_t length)
{
    // 每次调用arc4_crypt之前都要重新设置arc4_setup
    arc4_setup(&m_rc4ctx, (unsigned char *)m_skey.GetBytes(), (int)m_skey.GetSize());

    Data data((char*)input, length);
    arc4_crypt(&m_rc4ctx, (unsigned char*)data.GetBytes(), (int)data.GetSize());
    return std::move(data);
}

torch::Data RC4::CryptoCopy(const torch::Data &input)
{
    arc4_setup(&m_rc4ctx, (unsigned char *)m_skey.GetBytes(), (int)m_skey.GetSize());

    Data data(input);
    arc4_crypt(&m_rc4ctx, (unsigned char*)data.GetBytes(), (int)data.GetSize());
    return std::move(data);
}

void RC4::CryptoCopy(const Data &input, Data &output)
{
    arc4_setup(&m_rc4ctx, (unsigned char *)m_skey.GetBytes(), (int)m_skey.GetSize());
    
    output.CopyFrom(input);
    arc4_crypt(&m_rc4ctx, (unsigned char*)output.GetBytes(), (int)output.GetSize());
}

void RC4::CryptoNoCopy(unsigned char *input, size_t length)
{
    arc4_setup(&m_rc4ctx, (unsigned char *)m_skey.GetBytes(), (int)m_skey.GetSize());
    arc4_crypt(&m_rc4ctx, input, (int)length);
}

void RC4::CryptoNoCopy(torch::Data &input)
{
    arc4_setup(&m_rc4ctx, (unsigned char *)m_skey.GetBytes(), (int)m_skey.GetSize());
    arc4_crypt(&m_rc4ctx, (unsigned char*)input.GetBytes(), (int)input.GetSize());
}

torch::Data RC4::CryptoWithFile(const std::string &path)
{
    Data fdata = File::GetBytes(path);
    this->CryptoNoCopy(fdata);
    return std::move(fdata);
}