//
//  xpack
//
//  Created by Luwei.
//  Copyright (c) 2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy
//

#include "xpack-context.h"
#include "xpack-stream.h"
#include "xpack-signature.h"
#include "xpack-header.h"
#include "xpack-content.h"
#include "xpack-block.h"
#include "xpack-hash.h"
#include "xpack-name.h"

#include <math.h>

using namespace xpack;

Context::Context(Stream *stm)
:stream(stm)
,signature(nullptr)
,header(nullptr)
,content(nullptr)
,block(nullptr)
,hash(nullptr)
,name(nullptr)
,offset(0)
{
    torch::HeapCounterRetain();
    
    HashMaker = xpack::HashString;

    signature = new SignatureSegment(this);
    header    = new HeaderSegment(this);
    content   = new ContentSegment(this);
    block     = new BlockSegment(this);
    hash      = new HashSegment(this);
    name      = new NameSegment(this);
    crypto    = new torch::crypto::RC4();
    
    uint32_t skey[] = {
        0x43415058, 0x77073096, 0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988
    };
    crypto->SetSecretKey((unsigned char*)skey, sizeof(skey));
}

Context::~Context()
{
    torch::HeapCounterRelease();
    
    if (signature)  { delete signature; }
    if (header)     { delete header; }
    if (content)    { delete content; }
    if (block)      { delete block; }
    if (hash)       { delete hash; }
    if (name)       { delete name; }
    if (crypto)     { delete crypto; }
}

void Context::SetAlignedOffset(uint32_t offset)
{
    double alignedsize = (double)xpack::SIGNATURE_ALIGNED;
    this->offset = ceil(offset / alignedsize) * alignedsize;
}

