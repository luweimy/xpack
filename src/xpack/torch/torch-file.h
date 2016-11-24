//
//  Torch
//
//  Created by Luwei.
//  Copyright (c) 2014-2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy/Torch
//

#ifndef __TORCH__FILE__
#define __TORCH__FILE__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "torch-data.h"

namespace torch {
    
    class File
    {
    public:
        enum class Location {
            Set = SEEK_SET, // 文件开头
            Cur = SEEK_CUR, // 当前位置
            End = SEEK_END, // 文件结尾
        };
        
        File();
        ~File();
        File(const File&) = delete;
        File& operator=(const File&) = delete;
        
        bool Open(const std::string &path, const std::string &mode);
        bool OpenTemp();
        bool Attach(FILE *pFile);
        bool Close();
        bool Flush();
        bool IsOpen() const;
        bool IsEOF()  const;
        long GetSize() const;
        
        /*
         * 当OpenTemp或者Attach方式打开时，获得的路径为空
         */
        const std::string& GetPath() const;
        
        /*
         * 文件读写接口
         */
        size_t Read(void *buffer, size_t size, size_t count) const;
        size_t Read(void *buffer, size_t size) const;
        Data   Read(size_t bytes) const;
        void   Read(Data &outdata, size_t bytes) const;
        size_t Write(const void *buffer, size_t size, size_t count) const;
        size_t Write(const void *buffer, size_t size) const;
        size_t Write(const Data &data) const;
        
        /*
         * 文件指针定位及相关接口
         */
        long Tell()    const;
        bool SeekSet() const;
        bool SeekEnd() const;
        bool SeekSet(long offset) const;
        bool SeekCur(long offset) const;
        bool SeekEnd(long offset) const;
        bool Seek(long offset, Location where) const;
        
        /*
         * 改变文件大小
         * 说明：必须是以写入模式打开的文件，如果resize后文件变小则直接截断内容
         */
        bool ReSize(size_t size);
        
        /*
         * 获得文件描述符fd(FileDescriptor)
         * 说明：必须是正常打开的文件
         */
        int GetFileDescriptor();
        
        /*
         * 从文件指针当前位置开始获取文件内容
         * 参数：
         *  - length: 想要获取的文件内容长度，若为0则代表获取整个文件内容
         * 注意：
         *  - 从文件指针当前位置开始向后获取内容，若要从文件开始位置获取，请调用SeekSet()
         */
        std::string GetString(size_t length = 0) const;
        Data GetBytes(size_t length = 0) const;
        
        /*
         * 从文件指针当前位置开始，以十六进制/二进制打印文件内容
         * 参数：
         *  - length: 想要打印的文件内容长度，若为0则代表打印整个文件内容
         * 注意：
         *  - 从文件指针当前位置开始向后打印内容，若要从文件开始位置打印，请调用SeekSet()
         */
        void DumpHex(size_t length = 0) const;
        void DumpBinary(size_t length = 0) const;
        
        /*
         * 从文件指针当前位置开始获取文件内容(不建议)
         * 参数：
         *  - length: 想要获取的文件内容长度，若为0则代表获取整个文件内容
         *  - outlen: 文件真实返回的长度
         * 注意：
         *  - 必须调用torch::HeapFree()手动释放内存
         *  - 从文件指针当前位置开始向后获取内容，若要从文件开始位置获取，请调用SeekSet()
         */
        void* GetBytes(size_t *outlen, size_t length = 0) const;
        
        /*
         * 获取文件内容，可以指定获取的长度
         * 参数：
         *  - length: 想要要获取的长度，为0代表获取整个文件的长度
         */
        static Data GetBytes(const std::string &path, size_t length = 0);

        /*
         * 写入文件内容，若文件存在则清空
         * 参数：
         *  - mode: 写入模式，覆盖OR追加
         */
        enum class WriteMode {
            CreateOrOverWrite,  // 创建文件或者覆盖已存在文件
            CreateOrAppend,     // 创建文件或者在已存在文件末尾追加
        };
        static bool WriteBytes(const std::string &path, const Data &data, WriteMode mode = WriteMode::CreateOrOverWrite);
        static bool WriteBytes(const std::string &path, const char *s, size_t length, WriteMode mode = WriteMode::CreateOrOverWrite);

    private:
        FILE*       m_fstream;
        std::string m_path;
    };
    
    class FileSystem {
    public:
        /*
         * 返回时间戳，以秒为单位
         */
        static time_t GetLastModifyTime(const std::string &path);
        
        /*
         * 判断是否为目录/文件/链接文件
         */
        static bool IsDir(const std::string &path);
        static bool IsFile(const std::string &path);
        static bool IsLink(const std::string &path);
        
        /*
         * 判断文件或者文件夹是否存在
         */
        static bool IsPathExist(const std::string &path);
        
        /*
         * 创建文件
         */
        static bool MakeFile(const std::string &path);
        
        /*
         * 创建文件夹
         * 注意：
         *  - 若其父文件夹不存在，则不会创建成功
         *  - 若存在同名文件或者文件夹均不能成功创建
         *  - 错误原因可以用perror()函数打印
         */
        static bool MakeDirectory(const std::string &path);
        
        /*
         * 创建目录，若其父文件夹不存在，则会强制创建
         */
        static bool MakeDeepDirectory(const std::string &path);

        /*
         * 删除/改名/移动/复制文件
         */
        static bool Remove(const std::string &path);
        static bool Rename(const std::string &path, const std::string &newname);
        static bool Move(const std::string &srcpath, const std::string &dstpath);
        static bool Copy(const std::string &srcpath, const std::string &dstpath);
        
    };
    
}

#endif /* __TORCH__FILE__ */
