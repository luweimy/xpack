//
//  Torch
//
//  Created by Luwei.
//  Copyright (c) 2014-2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy/Torch
//

#ifndef __TORCH__PATH__
#define __TORCH__PATH__


#include <iostream>
#include <vector>

namespace torch {
    
    /*
     * 规范：
     *   - 以路径分隔符结尾的是目录路径，否则是文件路径
     *   - 以路径分隔符开头的是绝对路径
     */
    class Path
    {
    public:
        
        Path(const std::string& path);
        
        /*
         * 获得路径字符串
         */
        std::string GetPath() const;
        
        /*
         * 判断是否是绝对路径
         * 说明：
         *  - 绝对路径就是以路径分隔符开头的路径
         */
        bool IsAbsolute();
        
        /*
         * 获取文件后缀名/文件名，
         * 注意：
         *  - 注意只要路径结尾不是'/'，则认为其最后一个路径节点是文件，并不会真实的调用系统接口去测试是否为文件
         *  - 若要获取的文件名或者后缀名不存在，则返回空字符串
         */
        std::string GetExtension() const;
        std::string GetFilename() const;
        
        /*
         * 获得路径的目录层，若路径指向文件，则返回文件所在目录的路径，否则直接返回原路径
         * 注意：
         *  - 返回的路径，必然是以'/'结尾的路径
         */
        std::string GetDirectory() const;
        
        /*
         * 获得路径的节点
         * 参数：
         *  - index：从0开始，也支持负数索引，-1代表最后一个，以此类推
         * 注意：
         *  - 返回的路径节点不会存在路径分隔符
         *  - 使用Split分隔后最后一项可能是空白，但是通过本接口获取的最后一个路径节点不会是空白，路径为空的情况除外。
         */
        std::string GetComponent(int index) const;
        std::string GetLastComponent() const;
        
        /*
         * 追加路径节点
         * 注意：
         *  - 会自动补齐或者删除连接点多余的路径分隔符
         *  - 不会修改追加的路径节点本身，即不管pathcom是否以分隔符结尾都不会修改
         */
        Path& Append(const std::string &pathcom);
        
        /*
         * 删除路径的最后一级节点
         * 参数：
         *  - index：从0开始，也支持负数索引，-1代表最后一个，以此类推
         *  - count：删除的个数
         * 注意：
         *  - 使用Split分隔后最后一项可能是空白，但是通过本接口删除的不是空白节点，而是真实的路径最后一个节点，若存在。
         *  - 删除时不会删除最后一个节点和前面路径的分隔符
         *  - 根路径删除不会产生效果
         */
        Path& RemoveComponent(int index);
        Path& RemoveLastComponent(int count = 1);
        
        /*
         * 修改指定的路径节点
         * 参数：
         *  - index：从0开始，也支持负数索引，-1代表最后一个，以此类推
         * 注意：
         *  - 路径中的所有空白项都不属于路径节点，比如：A//B，理解为A/B即可
         */
        Path& ChangeComponent(int index, const std::string &component);
        Path& ChangeLastComponent(const std::string &component);
        
        /*
         * 修改文件名和修改后缀名
         * 注意：
         *  - 不同于ChangeLastComponent，本接口文件名若不存在，则不会修改
         *  - extension带不带'.'都可以
         */
        Path& ChangeFilename(const std::string &filename);
        Path& ChangeExtension(const std::string &extension);
        
        /*
         * 规范化一个路径
         * 说明：
         *  - 去除重复的路径分割符号，例如：root//a => root/a
         *  - 去除路径两侧空白字符
         *  - 格式化路径，例如：root/path/../a => root/a，root/./a => root/a，
         */
        Path& Normalize();
        
        /*
         * 将路径分拆为路径节点
         * 注意：
         *  - 会保留空的路径节点，通过[Join]可以完全复原分拆前的路径
         */
        std::vector<std::string> Split() const;
        static std::vector<std::string> Split(const std::string &path);
        
        /*
         * 拼接路径节点
         * 注意：
         *  - 仅会在两个节点间添加路径分割符号，一般目录的路径Split后，最后一个会是空格，所以Join会在路径末尾添加分隔符
         */
        static Path Join(const std::vector<std::string> &components);
        static Path Join(const std::string &path, const std::string &com);
        
        /*
         * GetHomeDir - 获取用户根目录
         * GetTemp - 获取一个随机的临时路径，指向一个临时文件名
         * GetSeparator - 获取路径分隔符
         */
        static std::string GetHomeDir();
        static std::string GetTemp();
        static char GetSeparator();
        
    private:
        std::string m_origin;
    };
    
}

#endif
