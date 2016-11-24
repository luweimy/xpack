//
//  xpack
//
//  Created by Luwei.
//  Copyright (c) 2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy
//

#ifndef __XPACK__HASH__
#define __XPACK__HASH__

#include <stdio.h>
#include "xpack-def.h"
#include "torch/torch.h"

namespace xpack {

class Context;
class HashSegment {
public:
    typedef torch::collection::HashMap<uint32_t, MetaHash*> MetaHashMap;
    typedef torch::FixedMemoryPool<sizeof(MetaHash)> MetaHashPool;

    HashSegment(Context *ctx);
    ~HashSegment();

    /*
     * 读取区域数据
     * 说明：
     *  - 根据MetaHeader.hash_offset确定位置
     *  - 若不存在或者格式错误，则返回false
     *  - 读取完毕后，会建立索引表以便于查询
     */
    bool ReadFromStream();
    
    /*
     * 将区域数据写入
     * 说明：
     *  - 根据MetaHeader.hash_offset确定位置
     *  - 不会写入被标记为unused的项
     */
    bool WriteToStream();
    
    /*
     * 检测是否存在Hash区域
     */
    bool IsValid();
    
    /*
     * 根据name查找对应的MetaHash
     * 注意：
     *  - 如果存在hashid冲突，则会跳转间接查找，知道返回对应的MetaHash
     *  - 若未找到则返回null
     */
    MetaHash* QueryByName(const std::string &name);
    
    /*
     * 根据hashid获得对应的MetaHash(直接返回hashmap中的存储项)
     * 注意：
     *  - 若未找到则返回null
     */
    MetaHash* GetById(const uint32_t hashid);

    /*
     * 是否存在指定名称的项
     */
    bool IsHashExist(const std::string &name);
    
    /*
     * 新增指定名称的MetaHash项
     * 说明：
     *  - 名字不可重复，若重复则失败并返回null
     *  - 内部会在hashid冲突时，多级跳转存储
     *  - 会实时更新MetaHeader->hash_count
     *  - 若存在可重用的项，则优先重用
     */
    MetaHash* AddNew(const std::string &name);
    
    /*
     * 删除指定名称的MetaHash项
     * 说明：
     *  - 若冲突链删除，则会将中转节点也全部删除
     *  - 会实时更新MetaHeader->hash_count
     *  - 删除的项，会加入重用集合
     */
    void RemoveByName(const std::string &name);
    
    /*
     * 清空所有内容
     */
    void Clear();
    
    /*
     * 获取存储MetaHash的Map，请不要修改
     */
    MetaHashMap* GetMetaHashMap();
    
    /*
     * 内部调试接口
     */
    std::string DumpHash();
    std::string DumpHashById(const uint32_t hashid);
    std::string DumpHashByName(const std::string &name);

private:
    MetaHash* InternalAddNew(const std::string &name, uint8_t seed = 0);

private:
    
    Context     *m_context;
    uint8_t      m_slatcursor;
    
    MetaHashMap   m_hashmap;  // Only used hash entries(hashmap<hashid, hashptr>)
    MetaHashPool  m_hashpool; // All hash struct entries
};
    
}

#endif /* __XPACK__HASH__ */
