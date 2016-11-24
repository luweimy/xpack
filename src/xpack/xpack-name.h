//
//  xpack
//
//  Created by Luwei.
//  Copyright (c) 2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy
//

#ifndef __XPACK__NAME__
#define __XPACK__NAME__

#include <stdio.h>
#include "xpack-def.h"
#include "torch/torch.h"

namespace xpack {
    
    class Context;
    class NameSegment {
    public:
        
        NameSegment(Context *ctx);
        ~NameSegment();
    
        /*
         * 读取区域数据
         * 说明：根据HashSegment确定位置，NameSegment位于其后面
         */
        bool ReadFromStream();
        
        /*
         * 将区域数据写入
         * 说明：写到HashSegment后面
         */
        bool WriteToStream();
        
        /*
         * 检测是否存在合法的Name区域
         */
        bool IsValid();
        
        /*
         * 获得name数据区域的字节数
         */
        uint32_t Size();
        
        /*
         * 获得存储的Name指针
         * 参数：
         *  - offset, size: 偏移量和Name字节数
         *  - metahash: 根据metahash取Name
         * 返回值：不包含'\0'结尾，不可修改，不用释放内存
         */
        const char* GetNamePtr(uint32_t offset, uint16_t size);
        const char* GetNamePtr(MetaHash *metahash);
        
        /*
         * 获得存储的Name字符串
         * 参数：
         *  - offset, size: 偏移量和Name字节数
         *  - metahash: 根据metahash取Name
         * 返回值：Name字符串
         */
        std::string GetName(uint32_t offset, uint16_t size);
        std::string GetName(MetaHash *metahash);
        
        /*
         * 新增Name记录
         * 参数：
         *  - name: 新增的Name
         *  - metahash: 记录Name的offset和size的MetaHash项
         */
        void AddName(const std::string &name, MetaHash *metahash);
        
        /*
         * 新增Name记录(推荐使用AddName(name, metahash))
         * 返回值：新增name的相对偏移量offset，要将其记录到MetaHash中
         */
        uint32_t AddName(const std::string &name);

        /*
         * 删除Name记录
         * 注意：只有当删除Name后，不会导致其他Name的offset发生改变才能删除成功，否则删除无效
         */
        void RemoveNameSafely(uint32_t offset, uint16_t size);
        void RemoveNameSafely(MetaHash *metahash);

        /*
         * 清理所有的无效名称
         * 注意：此接口会修改metahash和metaheader，务必在metaheader写入之前调用
         */
        void CleanupNames();
        
        /*
         * 获得Name存储的原始数据
         */
        const torch::Data& GetRawNames();
        
    private:
        torch::Data  m_names;
        Context     *m_context;
    };
    
}

#endif /* __XPACK__NAME__ */
