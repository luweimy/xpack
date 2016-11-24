//
//  xpack
//
//  Created by Luwei.
//  Copyright (c) 2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy
//

#ifndef __XPACK__SIGNATURE__
#define __XPACK__SIGNATURE__

#include <stdio.h>
#include "xpack-def.h"

namespace xpack {
    
class Context;
class SignatureSegment {
public:
    
    SignatureSegment(Context *ctx);
    ~SignatureSegment();

    /*
     * 搜索xpack包的标记(xpack::SIGNATURE)
     * 说明：
     *  - signaure的偏移位置以xpack::SIGNATURE_ALIGNED(512)对齐
     *  - 搜到后即会读取，并将搜到的offset设置到ctx->offset
     */
    bool SearchFromStream();
    
    /*
     * 读取区域数据
     * 说明：只能读取Signature区域在文件的开头的情况，否则出现数据错误会返回false
     */
    bool ReadFromStream();
    
    /*
     * 将区域数据写入
     * 说明：写入的偏移量是根据ctx->offset确定的
     */
    bool WriteToStream();
    
    /*
     * 校验签名的正确性
     */
    bool IsValid();
    
    /*
     * 使用xpack::SIGNATURE和xpack::VERSION来初始化签名标记和版本号信息
     */
    SignatureSegment* Initialize();
    
    /*
     * 获得指向MetaSignature结构体的指针
     */
    MetaSignature* Metadata();
    
private:
    MetaSignature m_signature;
    Context      *m_context;
};

}


#endif /* __XPACK__SIGNATURE__ */
