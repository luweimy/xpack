//
//  Torch
//
//  Created by Luwei.
//  Copyright (c) 2014-2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy/Torch
//

#ifndef __TORCH__BASE__
#define __TORCH__BASE__

#include <string>
#include <vector>
#include <typeinfo>
#include <limits>
#include "torch-endian.h"

namespace torch {
    
    /*
     * 内存管理函数：申请/释放...
     * 说明：
     *  - 内部维护着计数器，可用于检测内存泄露，保证申请和释放次数一致
     *  - 函数行为与ANSI-C一致
     */
    void *HeapMalloc(size_t size);
    void *HeapCalloc(size_t count, size_t size);
    void *HeapReAlloc(void *ptr, size_t size);
    void  HeapFree(void *ptr);
    
    /*
     * 内存管理函数计数器
     * 说明：
     *  - 当出现其他库申请的内存时，或者其他库释放了内存，导致的计数器不准，可以使用下列函数校准计数器
     */
    int HeapCounter();
    int HeapCounterRetain();
    int HeapCounterRelease();
    
    /*
     * 判断是否是当前类的对象(RTTI)
     * 使用：
     *  - InstanceOf<Object>(ptr);
     */
    template<typename _CTp, typename _OTp>
    bool InstanceOf(_OTp* obj) {
        return typeid(_CTp) == typeid(*obj);
    }
    
    /*
     * 获得一个类型的唯一ID(RTTI)
     * 注意：
     *  - 不同环境下获得的ID可能不同，但保证唯一
     * 使用：
     *  - HashID<Object>();
     *  - HashID<Array>();
     *  - HashID<int>();
     */
    template <typename _Tp>
    size_t HashID() {
        return typeid(_Tp).hash_code();
    }
    
    /*
     * 获得一个类型的唯一名称(RTTI)
     * 注意：
     *  - 不同编译器获得同一类型名称可能不同，但保证名称唯一
     * 使用：
     *  - TypeName<Object>();
     *  - TypeName<Array>();
     *  - TypeName<int>();
     */
    template <typename _Tp>
    const char* TypeName() {
        return typeid(_Tp).name();
    }
    
    /*
     * 获得一个类型的最大值
     * 使用：
     *  - GetMax<int>();
     */
    template<typename _Tp>
    _Tp GetMax() {
        return std::numeric_limits<_Tp>::max();
    }
    
    /*
     * 获得一个类型的最小值
     * 使用：
     *  - GetMin<int>();
     */
    template<typename _Tp>
    _Tp GetMin() {
        return std::numeric_limits<_Tp>::min();
    }

    /*
     * 转换为二进制或者十六进制可显示的数字的字符串
     * 参数：
     *  - byteblank: 每隔多少个字节加一个空格，若byteblank<=0则无空格
     */
    std::string ToHex   (const char *data, size_t length, int byteblank = 0);
    std::string ToBinary(const char *data, size_t length, int byteblank = 0);
    template<typename _Tp>
    std::string ToHex   (_Tp v, int byteblank = 0) {
        return torch::ToHex((char*)&v, sizeof(v), byteblank);
    }
    template<typename _Tp>
    std::string ToBinary(_Tp v, int byteblank = 0) {
        return torch::ToBinary((char*)&v, sizeof(v), byteblank);
    }
    template<typename _Tp>
    std::string ToHexHumanReadable(_Tp v, int byteblank = 0) {
        v=torch::Endian::ToNet((_Tp)v); return torch::ToHex((char*)&v, sizeof(v), byteblank);
    }
    template<typename _Tp>
    std::string ToBinaryHumanReadable(_Tp v, int byteblank = 0) {
        v=torch::Endian::ToNet((_Tp)v); return torch::ToBinary((char*)&v, sizeof(v), byteblank);
    }
    
    
    /*
     * 打印二进制或者十六进制
     * 参数：
     *  - byteblank: 每隔多少个字节加一个空格，若byteblank<=0则无空格
     */
    void DumpHex   (const char *data, size_t length, int byteblank = 0);
    void DumpBinary(const char *data, size_t length, int byteblank = 0);
    template<typename _Tp>
    void DumpHex   (_Tp v, int byteblank = 0) {
        torch::DumpHex((char*)&v, sizeof(v), byteblank);
    }
    template<typename _Tp>
    void DumpBinary(_Tp v, int byteblank = 0) {
        torch::DumpBinary((char*)&v, sizeof(v), byteblank);
    }
    template<typename _Tp>
    void DumpHexHR(_Tp v, int byteblank = 0) {
        v=torch::Endian::ToNet((_Tp)v); torch::DumpHex((char*)&v, sizeof(v), byteblank);
    }
    template<typename _Tp>
    void DumpBinaryHR(_Tp v, int byteblank = 0) {
        v=torch::Endian::ToNet((_Tp)v); torch::DumpBinary((char*)&v, sizeof(v), byteblank);
    }
    
}

#endif
