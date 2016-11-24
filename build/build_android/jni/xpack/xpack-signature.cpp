//
//  xpack
//
//  Created by Luwei.
//  Copyright (c) 2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy
//

#include "xpack-signature.h"
#include "xpack-context.h"
#include "xpack-stream.h"
#include "xpack-base.h"
#include "xpack-def.h"
#include <assert.h>

using namespace xpack;

SignatureSegment::SignatureSegment(Context *ctx)
:m_context(ctx)
,m_signature({0})
{
    assert(ctx);
    torch::HeapCounterRetain();
}

SignatureSegment::~SignatureSegment()
{
    torch::HeapCounterRelease();
    m_context = nullptr;
}

bool SignatureSegment::SearchFromStream()
{
    Context *ctx = m_context;
    assert(m_context->stream);
    
    uint32_t partoffset = 0;
    size_t fsize = ctx->stream->Size();
    MetaSignature signaturebuf = {0};

    while (partoffset < fsize) {
        if (!ctx->stream->GetSignature(&signaturebuf, partoffset)) {
            return false;
        }
        // Search success
        if (signaturebuf.signature == xpack::SIGNATURE) {
            ctx->offset = partoffset;
            m_signature = signaturebuf;
            return true;
        }
        partoffset += xpack::SIGNATURE_ALIGNED;
    }
    
    XPACK_ERROR(xpack::Error::Format);
    return false;
}

bool SignatureSegment::ReadFromStream()
{
    Context *ctx = m_context;
    if (!ctx->stream->GetSignature(&m_signature, ctx->offset)) {
        return false;
    }
    // Verify signature
    if (m_signature.signature != xpack::SIGNATURE) { 
        XPACK_ERROR(xpack::Error::Format);
        return false;
    }
    return true;
}

bool SignatureSegment::WriteToStream()
{
    Context *ctx = m_context;
    if (!ctx->stream->PutSignature(&m_signature, ctx->offset)) { 
        return false;
    }
    return true;
}

bool SignatureSegment::IsValid()
{
    if (m_signature.signature != xpack::SIGNATURE) {
        XPACK_ERROR(xpack::Error::Format);
        return false;
    }
    // Version not support
    if (m_signature.version != xpack::VERSION) { 
        XPACK_ERROR(xpack::Error::Version);
        return false;
    }
    return true;
}

SignatureSegment* SignatureSegment::Initialize()
{
    m_signature.signature = xpack::SIGNATURE;
    m_signature.version = xpack::VERSION;
    return this;
}

MetaSignature* SignatureSegment::Metadata()
{
    return &m_signature;
}

