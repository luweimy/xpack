//
//  Torch
//
//  Created by Luwei.
//  Copyright (c) 2014-2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy/Torch
//

#ifndef __TORCH__COMPRESS__ZIP__
#define __TORCH__COMPRESS__ZIP__

#include <stdio.h>
#include "../deps/minizip/unzip.h"
#include "../deps/minizip/zip.h"
#include "../torch-data.h"

namespace torch { namespace compress {
    
    class ZipUtil {
    public:
        /*
         * 获得压缩后的最大尺寸，用于提前申请缓冲区
         * 参数：
         *  - len: 压缩之前的尺寸大小
         * 注意：只是压缩后的最大可能的尺寸，并不准确
         */
        static size_t GetCompressedMaxSize(size_t len);
        
        enum class CompressLevel {
            NoCompression   = Z_NO_COMPRESSION,
            Default         = Z_DEFAULT_COMPRESSION,
            BestSpeed       = Z_BEST_SPEED,
            BestCompression = Z_BEST_COMPRESSION,
        };
        
        /*
         * 压缩/解压缩
         * 参数：
         *  - dst: 要提前申请好缓冲区，若是压缩则使用GetCompressedSize()获得压缩后最大的尺寸，若是解压缩则需自行存储压缩前的尺寸
         *  - dstlen: 返回压缩/解压缩后的真实数据大小
         *  - level: 压缩等级
         */
        static bool Compress(const char *src, size_t srclen, char *dst, size_t *dstlen, CompressLevel level=CompressLevel::Default);
        static bool Decompress(const char *src, size_t srclen, char *dst, size_t *dstlen);
        
        /*
         * 压缩/解压缩
         * 参数：
         *  - output: 要提前申请好缓冲区，若是压缩则使用GetCompressedMaxSize()获得压缩后最大的尺寸，若是解压缩则需自行存储压缩前的尺寸
         *  - level: 压缩等级
         */
        static bool Compress(const torch::Data &input, torch::Data &output, CompressLevel level=CompressLevel::Default);
        static bool Decompress(const torch::Data &input, torch::Data &output);
    };
    
    /*
     * Zip
     * Zip压缩操作
     * 注意：
     *  - 本类不提供删除操作，若想删除则新建一个Zip包，然后将不删除的内容全部写入新Zip包中
     *  - 复制Zip文件中数据时，使用Zip::OpenEntryRaw和Unzip::OpenCurrentRaw速度更快，避免了数据的解压和重新压缩
     */
    class Zip {
    public:
        enum class OpenMode {
            Create = APPEND_STATUS_CREATE,     /* 文件不存在则创建文件，文件存在则清空文件 */
            AddIn  = APPEND_STATUS_ADDINZIP  /* 文件不存在则打开失败，文件存在则在原ZIP文件中添加，若存在同名文件则覆盖 */
        };
        
        Zip();
        Zip(const std::string &path, OpenMode append=OpenMode::AddIn);
        ~Zip();
        
        Zip(const Zip &other) = delete;
        Zip& operator=(const Zip &other) = delete;
        
        /*
         * 打开Zip文件
         * 参数：
         *  - append: OpenMode类型的枚举，默认OpenMode::OPEN_ADDINZIP
         * 说明：
         *  - OpenMode::Create：文件不存在则创建文件，文件存在则清空文件
         *  - OpenMode::AddIn：文件不存在则打开失败，文件存在则在原ZIP文件中添加，若存在同名文件则覆盖
         */
        bool Open(const std::string &path, OpenMode append=OpenMode::AddIn);
        bool Close();
        bool IsOpen();
        int  GetError();
        bool HasError();
        
        /*
         * 设置一个项的内容，若存在则覆盖
         */
        bool SetContent(const std::string &filename, const Data &content);
        
        /*
         * 打开条目/向打开的条目写内容/关闭条目
         */
        bool OpenEntry(const std::string &filename);
        bool WriteEntry(void *buffer, size_t length);
        bool CloseEntry();
        
        /*
         * 纯数据方式打开文件，写入数据不会压缩处理，对应Unzip::OpenCurrentRaw使用
         * 参数：
         *  - method：压缩方法
         *  - level：压缩级别
         * 注意：
         *  - 其他写入和关闭API可以通用：WriteEntry/CloseEntry
         */
        bool OpenEntryRaw(const std::string &filename, int method, int level);
        
    private:
        zipFile  m_zipFile;
        int      m_errcode;
    };
    
    /*
     * Unzip
     * zip压缩包解压
     */
    class Unzip {
    public:
        typedef unzFileNameComparer Comparer;
        
        Unzip();
        Unzip(const std::string &path);
        ~Unzip();
        
        Unzip(const Unzip &other) = delete;
        Unzip& operator=(const Unzip &other) = delete;
        
        bool Open(const std::string &path);
        bool Close();
        bool IsOpen();
        int  GetError();
        bool HasError();
        
        /*
         * 获取条目数量
         * 说明：
         *  - 不区分文件和文件夹，都是一条记录，所以输出的条目可能与文件数不符合
         */
        int GetEntryCount();
        
        /*
         * 将遍历游标指向第一个文件
         */
        bool SetFirst();
        
        /*
         * 将遍历游标指向下一个文件
         * 说明：
         *  - 若没有下一个文件，则返回false
         */
        bool Next();
        
        /*
         * 直接定位到一个文件
         * 说明：
         *  - Locate之后会指向定位的文件，若要遍历则要先设置SetFirst
         *  - 模糊定位会定位到第一个合适的文件
         * 描述：
         *  - Locate(const std::string& name)精确定位，name必须完全一致
         *  - Locate(const std::string& name, Comparer comp)允许自定义文件名比较函数
         *  - LocateFuzzyIgnoreCase 模糊定位匹配-忽略大小写
         *  - LocateFuzzyContain 模糊匹配-包含匹配，可以匹配到包含name的条目，如：'cde'可以匹配到'abcdefg'
         */
        bool Locate(const std::string& name);
        bool Locate(const std::string& name, Comparer comp);
        bool LocateFuzzyIgnoreCase(const std::string& name);
        bool LocateFuzzyContain(const std::string& name);
        
        /*
         * 判断当前指向的条目为文件/文件夹
         * 说明：
         *  - 内部是通过条目名称是否以路径分隔符结尾来判断的
         */
        bool IsCurrentDirectory();
        bool IsCurrentFile();
        
        /*
         * 获得当前的条目文件路径名
         * 说明：
         *  - 获取的是文件路径名，包含文件夹路径。如：__MACOSX/unzip101/._crypt.h
         *  - 纯路径也会出现，以路径分隔符结尾
         */
        std::string GetCurrentName();
        
        /*
         * 获得条目内容的字节数(压缩后/未压缩)
         * 说明：
         *  - 一般只用未压缩的就可以
         */
        size_t GetCurrentCompressedSize();
        size_t GetCurrentUncompressedSize();
        
        /*
         * 获得条目内容(未压缩)
         */
        Data GetCurrnetContent();
        
        /*
         * 打开当前条目/读取当前条目/关闭当前条目
         */
        bool OpenCurrent();
        bool OpenCurrent(const std::string& password);
        bool CloseCurrent();
        /*
         * 读取当前条目
         * 参数：
         *  - buffer: 读取内容存放的内存
         *  - length: buffer的尺寸大小
         * 返回值：
         *  - 真实读取的字节数
         */
        int  ReadCurrent(void *buffer, size_t length);
        
        /*
         * 纯数据方式打开文件，读取数据不会解压处理，，对应Zip::OpenEntryRaw使用
         * 参数：
         *  - outmethod：获得压缩方法
         *  - outlevel：获得压缩级别
         * 注意：
         *  - 其他读取和关闭API可以通用：ReadCurrent/CloseCurrent
         */
        bool OpenCurrentRaw(int *outmethod, int *outlevel);
        
        /*
         * 定位并读取条目内容
         * 参数：
         *  - name: 条目的名字，必须是精确匹配
         * 返回值：
         *  - 若定位失败，则返回空的Data对象
         * 注意：
         *  - 内部会使用Locate定位，所以不要在遍历ZIP条目中使用本接口
         */
        Data GetContent(const std::string &name);
        
        /*
         * 获得文件名信息
         * 描述：
         *  - GetFilenameList: 获得文件的条目名称
         *  - GetDirList: 获得路径条目名称
         *  - GetNameList: 获得所有条目的名称
         */
        std::vector<std::string> GetFilenameList();
        std::vector<std::string> GetDirList();
        std::vector<std::string> GetNameList();
        
    private:
        unzFile  m_unzFile;
        
        int      m_errcode;
        static const int s_maxFilenameSize = 512;
    };
    
} }


#endif /* __TORCH__COMPRESS__ZIP__ */
