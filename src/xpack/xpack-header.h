//
//  xpack
//
//  Created by Luwei.
//  Copyright (c) 2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy
//

#ifndef __XPACK__HEADER__
#define __XPACK__HEADER__

#include <stdio.h>
#include <string>
#include "xpack-def.h"

namespace xpack {
    
class Context;
class HeaderSegment {
public:
    
    HeaderSegment(Context *ctx);
    ~HeaderSegment();
    
    /*
     * 读取区域数据
     * 说明：根据ctx->offset确定读取位置，若数据校验失败返回false
     */
    bool ReadFromStream();
    
    /*
     * 将区域数据写入
     * 说明：写入的偏移量是根据ctx->offset确定的
     */
    bool WriteToStream();
    
    /*
     * 校验正确性
     */
    bool IsValid();
    
    /*
     * 将数据初始化
     */
    HeaderSegment* Initialize();
    
    /*
     * 更新Header区段元信息
     * 说明：每次使用其他(hash,block,name,content)修改完数据后，都会自动更新，读写时请视情况使用
     */
    HeaderSegment* UpdateMetadata();
    
    /*
     * 获得指向MetaHeader结构体的指针
     */
    MetaHeader* Metadata();
    
    /*
     * 内部调试接口
     */
    std::string DumpHeader();
    
private:
    
    MetaHeader m_header;
    Context   *m_context;
};
    
}

#endif /* __XPACK__HEADER__ */
