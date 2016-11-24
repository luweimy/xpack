//
//  Torch
//
//  Created by Luwei.
//  Copyright (c) 2014-2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy/Torch
//

#ifndef __TORCH__DATA__
#define __TORCH__DATA__

#include <stdio.h>
#include <string>
#include "core/torch-allocator.h"

namespace torch {
    
    class Data
    {
    public:
        static const Data Null;
        enum { NotFound = -1 };
        
        Data();
        Data(size_t size);
        Data(const char* str);
        Data(const void* pMem, size_t size);
        Data(const Data& other);
        Data(Data&& other);
        ~Data();
        
        Data& operator=(const Data &other);
        Data& operator=(Data &&other);
        
        Data& Free();
        Data& Alloc(size_t size);
        Data& Memset(int value);
        
        /*
         * 改变尺寸
         * 说明：
         *  - 若新尺寸大于原尺寸，则原内容不变，默认空白处填充0(fillchar)
         *  - 若新尺寸小于原尺寸，则截断
         */
        Data& ReSize(size_t size, int fillchar = 0);
        
        /*
         * 预分配空间
         * 说明：
         *  - 调用后会进行内存的申请和释放，注意数据安全
         *  - 调用此接口申请后，对外显示size为0，内存为空
         *  - 适用于预先无法确定需要内存大小，但是频繁Append和Insert的情况，最终获得的size仍会是实际数据的size，而非申请内存的size
         */
        Data& Reserve(size_t size);
        
        /*
         * 将数据拷贝过来，会申请新内存，会删除原有内存(若存在)
         */
        Data& CopyFrom(const Data &other);
        Data& CopyFrom(const void *mem, size_t size);
        Data& CopyFrom(const void *str);
        
        /*
         * 浅赋值，仅仅是将指针赋值，并不会申请新内存，但是会删除原有内存(若存在)
         */
        Data& ShallowSet(void* pMem, size_t size);
        
        /*
         * 将内容的控制权分离出去，内存的释放要由用户自己完成
         */
        void* Detach();
        
        /*
         * 对比两个Data的数据是否相等
         */
        bool IsEqualDeep(const Data &other) const;
        
        /*
         * 填充为指定字符
         */
        Data& Fill(int index, size_t size, char ch);
        Data& Fill(int index, char ch);
        Data& Fill(char ch = 0);

        /*
         * 将数据插入到指定的index
         * 参数：
         *  - index: 支持负向索引，-1代表末字节，默认为0
         * 注意：
         *  - 当使用负向索引插入时，-1代表插入到当前最末字符的位置，但是插入后，原位置的字符会后移，所以最终插入的字符不会在末尾
         */
        Data& Insert(int index, char ch);
        Data& Insert(int index, Data &other);
        Data& Insert(int index, const void *mem, size_t size);
        
        /*
         * 在末尾追加数据
         */
        Data& Append(char ch);
        Data& Append(Data &other);
        Data& Append(const void *mem, size_t size);
        
        /*
         * 替换指定范围的数据
         * 参数：
         *  - index: 支持负向索引，-1代表末字节，默认为0
         * 说明：
         *  - 替换指定的index处的count个字节，包含index索引指向的字节
         */
        Data& Replace(int index, size_t count, char ch);
        Data& Replace(int index, size_t count, Data &other);
        Data& Replace(int index, size_t count, const void *mem, size_t size);
        
        /*
         * 删除指定范围数据
         * 参数：
         *  - index: 支持负向索引，-1代表末字节，默认为0
         * 说明：
         *  - 从index开始，删除count个字节，超出数据范围的部分直接忽略
         */
        Data& Erase(int index, size_t count);
        
        /*
         * 查找指定的数据
         * 参数：
         *  - ch：要查找的字符
         *  - start： 开始查找的起始索引，支持负向索引，-1代表末字节，默认为0
         * 说明：
         *  - 在内存中正序查找指定的字符，并返回找到的第一个字符的index
         *  - 可以指定开始查找的位置，会从index开始向后查找
         *  - 若没有找到则返回Data::NotFound(-1)
         */
        int IndexOf(char ch, int start = 0) const;
        
        /*
         * 倒序查找指定的数据
         * 参数：
         *  - ch：要查找的字符
         *  - start： 开始查找的起始索引，支持负向索引，-1代表末字节，默认为-1
         * 说明：
         *  - 在内存中逆序查找指定的字符，并返回找到的第一个字符的index
         *  - 可以指定开始查找的位置，会从index开始向前遍历
         *  - 若没有找到则返回Data::NotFound(-1)
         */
        int LastIndexOf(char ch, int start = -1) const;
        
        /*
         * 是否为空，内部仅判断内存指针为空，没有判断size
         */
        bool IsNull() const;
        
        /*
         * 获取数据的大小(单位：字节)和指针
         */
        size_t GetSize() const;
        void*  GetBytes() const;
        
        /*
         * 获得指定位置的指针
         * 参数：
         *  - index： 支持负向索引，-1代表末字节
         */
        void* GetPtr(int index) const;
        
        /*
         * 获得指定位置的字节
         * 参数：
         *  - index：支持负向索引，-1代表末字节
         */
        unsigned char GetByte(int index) const;
        
        /*
         * 将数据转为字符串
         */
        std::string ToString() const;

        /*
         * 转化为十六进制或者二进制的字符串
         * 参数：
         *  - byteblank：每隔多少个字节加一个空格，若byteblank<=0则无空格
         */
        std::string ToHex(int byteblank = 0) const;
        std::string ToBinary(int byteblank = 0) const;
        
        /*
         * 打印转化为十六进制或者二进制
         * 参数：
         *  - byteblank：每隔多少个字节加一个空格，若byteblank<=0则无空格
         */
        void DumpHex(int byteblank = 0) const;
        void DumpBinary(int byteblank = 0) const;
        
    private:
        Allocator  m_allocator;
    };
    
}

#endif
