//
//  xpack
//
//  Created by Luwei.
//  Copyright (c) 2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy
//

#ifndef __XPACK__XPACK__
#define __XPACK__XPACK__

#include <stdio.h>
#include <string>
#include <vector>
#include "torch/torch.h"
#include "xpack-def.h"
#include "xpack-base.h"
#include "xpack-context.h"

namespace xpack {
    class SignatureSegment;
    class HeaderSegment;
    class BlockSegment;
    class HashSegment;
    class NameSegment;
    class Stream;
    class Context;
    
    class Package {
    public:
        
        Package();
        ~Package();
        Package(const Package &) = delete;
        Package& operator=(const Package &) = delete;

        /*
         * 打开包文件(使用xpack::FileStream)
         * 参数：
         *  - path: 包的路径
         *  - readonly: 是否以只读模式打开包，若以只读方式打开则所有修改操作全部无效
         * 注意：包必须存在，而且必须是合法的包，若想构建一个新包请使用OpenNew()
         */
        bool Open(const std::string &path, bool readonly = true);
        
        /*
         * 创建并打开一个新的包(使用xpack::FileStream)
         * 参数：
         *  - path: 包的路径
         * 注意：
         *  - 若文件存在且非空，则会在文件内部添加寄生包，否则创建一个空的包
         *  - 目录必须存在(文件可不存在)，否则会创建失败
         */
        bool OpenNew(const std::string &path);

        /*
         * 打开包(自定义输入输出流)
         * 参数：
         *  - stm: 传入流对象，继承自xpack::Stream，内部会在Close时自动释放此对象
         *  - path: 包的路径
         *  - readonly: 是否以只读模式打开包，若以只读方式打开则所有修改操作全部无效
         * 注意：
         *  - 谨慎使用自定义输入输出流，参考FileStream实现
         *  - 包必须存在，而且必须是合法的包，若想构建一个新包请使用OpenNew()
         */
        bool OpenWithStream(Stream *stm, const std::string &path, bool readonly = true);

        /*
         * 关闭包，释放资源，析构函数中会自动调用，自动将更新写入包中
         */
        void Close();
        
        /*
         * 校验是否是合法的包
         */
        bool IsValid();
        
        /*
         * 设置是否开启CRC校验，默认开启
         * 注意：
         *  - 写入时开启CRC，读取时可以关闭，但是写入时关闭，读取时不可开启
         *  - 开启CRC校验会保障数据的一致性，但是会减慢数据的读取和写入
         */
        void SetNeedCrcVerify(bool need);
        
        /*
         * 设置是否自动减小包的磁盘占用空间，默认开启
         * 说明：
         *  - 禁用后，对包的实际内容不会产生影响，只是不会减少包占用磁盘的体积大小(文件末尾会存在无效数据)
         */
        void SetNeedAutoShrink(bool need);

        /*
         * 设置元数据加密密钥(若不设置也有默认的密钥)
         * 说明：
         *  - 在Open数据包之前设置正确的密钥才能打开，若要修改密码，则关闭数据包之前重新设置新密码即可
         *  - 此密钥为加密元数据(Metadata)的密钥设置
         *  - 此密钥可与内容加密密钥不同，即SetSecretKey设置的密钥
         *  - 内部使用流式加密算法RC4
         */
        void SetMetadataSecretKey(const unsigned char *skey, const int length);

        /*
         * 设置密钥(若不设置也有默认的密钥)
         * 说明：
         *  - 读取文件时，必须保证密钥设置正确，才能正确读取(会自动判断是否加密)
         *  - 可以为不同的文件设置不同的密钥，只要保障读取文件时密钥设置正确即可
         *  - 内部使用流式加密算法RC4
         */
        void SetSecretKey(const unsigned char *skey, const int length);

        /*
         * 向包内新增数据项
         * 参数：
         *  - name: 存储在包内的项名
         *  - data: 存储的文件内容
         *  - crypto: 是否对文件内容加密(密钥可以通过SetSecretKey接口设置，不设置则使用默认密钥)
         *  - compress: 是否压缩
         * 注意：
         *  - 若存在同名文件会新增失败
         */
        bool AddEntry(const std::string &name, const torch::Data &data, bool crypto = false, bool compress = false);

        /*
         * 从包内删除数据项
         * 参数：
         *  - name: 存储在包内的项名
         * 返回值：
         *  - 若不存在，则会返回false
         * 说明：
         *  - 内部会在不影响性能的情况下尽量清除无效数据，但是并不保证包会变小，删除过的数据会被标记为无效数据并允许后续写覆盖
         */
        bool RemoveEntry(const std::string &name);
        
        /*
         * 将包信息写入到文件(包关闭时自动触发)
         * 注意：此方法性能不高，不要频繁调用
         */
        bool Flush();

        /*
         * 获得包内存储项的尺寸大小(单位:Byte)
         * 参数：
         *  - name: 存储在包内的项名
         * 返回值：
         *  - 若未找到则返回0
         */
        uint32_t GetEntrySizeByName(const std::string &name);
        
        /*
         * 获得文件的解包后的尺寸(即原文件尺寸大小)
         * 参数：
         *  - name: 存储在包内的项名
         * 返回值：
         *  - 若未找到则返回0
         * 说明：
         *  - 压缩会造成包内文件大小的改变
         */
        uint32_t GetUnpackedEntrySizeByName(const std::string &name);

        /*
         * 从包内读取文件内容转为字符串
         * 参数：
         *  - name: 存储在包内的项名
         * 注意：
         *  - 请在确认文件内容是文本的情况下使用此接口
         */
        std::string GetEntryStringByName(const std::string &name);
        
        /*
         * 从包内读取存储项内容
         * 参数：
         *  - name: 存储在包内的项名
         *  - outdata: 获取的文件内容，其size会根据读取的大小自动调整
         * 返回值：
         *  - bool:读取是否成功，错误信息使用xpack::GetLastError()获取
         * 注意：
         *  - GetEntryDataByName(name, outdata)接口可以重复使用一个Data作为缓冲区，性能更高
         */
        bool GetEntryDataByName(const std::string &name, torch::Data &outdata);
        torch::Data GetEntryDataByName(const std::string &name);
        
        /*
         * 判断包内是否存在指定的存储项
         * 参数：
         *  - name: 存储在包内的项名
         */
        bool IsEntryExist(const std::string &name);
        
        /*
         * 遍历存储项名
         * 参数：
         *  - callback: 每个存储项调用一次，若返回false，则会中断遍历
         * 返回值：
         *  - 若遍历过程中callback返回false，则本接口也会返回false，否则返回true
         * 注意：
         *  - 不可以在遍历过程中中删除进行操作
         */
        bool ForeachEntryNames(std::function<bool(const std::string &name)> callback);
        
        /*
         * 获得所有存储项名
         */
        std::vector<std::string> GetEntryNames();
        
        /*
         * 获得包整体的大小
         * 注意：
         *  - 包的大小并不一定等于文件大小，包也可以是文件中的一部分，返回的只是包的整体大小。
         */
        size_t GetPackageSize();
                
        /*
         * 获得内部数据接口(谨慎操作)
         */
        Context* GetContxt();
        Stream*  GetStream();

        /*
         * 获得版本号信息，如: 0.9
         */
        static std::string GetVersion();

    private:
        const torch::Data* ProcessingBeforeWriting(MetaHash *sct, const torch::Data &data, bool crypto, bool compress);
        bool ProcessingAfterReading(MetaHash *sct, torch::Data &data);

    private:
        Stream  *m_stream;
        Context *m_context;
        
        bool     m_modify;
        bool     m_needcrc;
        bool     m_needshrink;
        
        torch::Data         m_compressbuffer;
        torch::Data         m_cryptobuffer;
        torch::crypto::RC4 *m_rc4crypto;
    };
    
    class PackageHelper {
    public:
        typedef std::function<bool(const std::string &name, bool status)> StatusCallback;
                
        /*
         * 创建一个空包
         * 说明：
         *  - 若文件存在且非空，则会在文件内部添加寄生包，否则创建一个空的包
         */
        static bool MakeNew(const std::string &path);
        
        /*
         * 向一个已经存在的包添加文件
         * 参数：
         *  - path: 要添加的文件的路径
         *  - name: 添加到包内的文件名称，若为空则直接使用参数path
         *  - force: 是否覆盖同名文件
         */
        static bool AddTo(const std::string &package, const std::string &path, const std::string &name = std::string(), bool force = false);
        static bool AddTo(Package &package, const std::string &path, const std::string &name = std::string(), bool force = false);

        /*
         * 提取所有文件到指定路径下
         * 参数：
         *  - pathto: 解包位置的根目录
         *  - force: 强制文件覆盖，若解包存在同名文件是否覆盖
         *  - callback: callback(name:文件名, status:当前文件解包状态)->bool:返回false则终止解包
         * 返回值：
         *  - 是否成功执行，若通过callback返回false终端执行，则本方法也会返回false
         */
        static bool ExtractTo(const std::string &package, const std::string &pathto, bool force = false, StatusCallback callback = nullptr);
        static bool ExtractTo(Package &package, const std::string &pathto, bool force = false, StatusCallback callback = nullptr);

        /*
         * 合并包的内容
         * 参数：
         *  - force: 强制文件覆盖，若存在同名文件则覆盖
         *  - callback: callback(name:文件名, status:当前文件解包状态)->bool:返回false则终止解包
         * 返回值：
         *  - 是否成功执行，若通过callback返回false终端执行，则本方法也会返回false
         * 说明：
         *  - 将other中的存储项合并到main中，other不会被改变
         */
        static bool Merge(const std::string &main, const std::string &other, bool force = false, StatusCallback callback = nullptr);
        static bool Merge(Package &main, Package &other, bool force = false, StatusCallback callback = nullptr);
        
    };
        
}

#endif /* __XPACK__XPACK__ */
