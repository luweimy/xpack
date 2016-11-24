//
//  xpack
//
//  Created by Luwei.
//  Copyright (c) 2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy
//

#ifndef __XPACK__CONTENT__
#define __XPACK__CONTENT__

#include <stdio.h>
#include "xpack-def.h"
#include "torch/torch.h"

namespace xpack {
    
    class Context;
    class ContentSegment {
    public:
        
        ContentSegment(Context *ctx);
        ~ContentSegment();
        
        /*
         * 检测是否存在合法的Content区域
         */
        bool IsValid();
        
        /*
         * 根据给定的block写入数据到指定的位置
         * 参数：
         *  - data: 要写入的数据
         * 说明：要写入的数据大小必须和block链代表的content大小之和相等
         */
        bool OverallWrite(const MetaHash *metahash, const torch::Data &data);
        
        /*
         * 根据给定的block读取数据
         * 参数：
         *  - outdata: 读取的数据，其size会根据读取的大小自动调整
         */
        bool OverallRead(const MetaHash *metahash, torch::Data &outdata);
        
    private:
        
        Context *m_context;
    };
    
}

#endif /* __XPACK__CONTENT__ */
