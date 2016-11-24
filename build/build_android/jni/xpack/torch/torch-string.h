//
//  Torch
//
//  Created by Luwei.
//  Copyright (c) 2014-2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy/Torch
//

#ifndef __TORCH__STRING__
#define __TORCH__STRING__


#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>

namespace torch {
    
    class String
    {
    public:
        enum { NotFound = -1 };
        enum { FORMAT_BUFFSIZE = 1024 };

        String();
        String(const char c, const size_t count = 1);
        String(const char* fmt, ...);
        String(const std::string &str);
        String(const String& other);
        String(String&& other);
        String(const long v);
        String(const double v);
        String(const double v, int ndigit); // ndigit - dot precision
        ~String();
        
        String& operator=(const String& other);
        String& operator=(String&& other);
        
        String& SetString(const char c, const size_t count = 1);
        String& SetString(const char* fmt, ...);
        String& SetString(const std::string &str);
        String& SetString(const String &str);
        String& SetString(const long v);
        String& SetString(const double v);
        String& SetString(const double v, int ndigit);
        
        size_t GetLength();
        bool IsEmpty();
        
        /*
         * 获得字符或者字符串
         * 参数：
         *  - index：支持负向索引，-1代表末字节
         *  - count：要获取的字符个数
         */
        char GetCharAt(const int index);
        const char* GetCString();
        const std::string& GetCppString();
        
        /*
         * 截取字符串
         * 参数：
         *  - from：支持负向索引，-1代表末字节
         *  - to：支持负向索引，-1代表末字节
         *  - index：支持负向索引，-1代表末字节
         * 注意：
         *  - 截取范围是全包含关系，[from, to]
         */
        std::string SubstrFromTo(const int from, const int to);
        std::string SubstrFrom(const int from);
        std::string SubstrTo(const int to);
        std::string Substr(const int index, const size_t count);
        static std::string SubstrFromTo(const std::string &str, const int from, const int to);
        static std::string SubstrFrom(const std::string &str, const int from);
        static std::string SubstrTo(const std::string &str, const int to);
        static std::string Substr(const std::string &str, const int index, const size_t count);

        /*
         * 替换指定范围的字符串
         * 参数：
         *  - from：支持负向索引，-1代表末字节
         *  - to：支持负向索引，-1代表末字节
         *  - index：支持负向索引，-1代表末字节
         * 注意：
         *  - 替换范围是全包含关系，[from, to]
         */
        String& ReplaceFromTo(const int from, const int to, const std::string &str);
        String& ReplaceFrom(const int from,  const std::string &str);
        String& ReplaceTo(const int to, const std::string &str);
        String& Replace(const int index, const size_t count, const std::string &str);
        String& ReplaceAll(const std::string &oldstr, const std::string &str);
        String& ReplaceFirst(const std::string &oldstr, const std::string &str);
        String& ReplaceLast(const std::string &oldstr, const std::string &str);
        
        /*
         * 在字符串末尾追加
         */
        String& Append(const char *cstr);
        String& Append(const std::string &str);
        String& AppendFloat(double v);
        String& AppendFloat(double v, int ndigit);
        String& AppendInteger(long v);
        String& AppendCharacter(char c, size_t count=1);
        String& AppendFormat(const char *fmt, ...);
        static std::string AppendFloat(const std::string &str, double v);
        static std::string AppendFloat(const std::string &str, double v, int ndigit);
        static std::string AppendInteger(const std::string &str, long v);
        static std::string AppendCharacter(const std::string &str, char c, size_t count=1);

        /*
         * 在字符串指定位置插入
         * 参数：
         *  - index：支持负向索引，-1代表末字节
         */
        String& Insert(const int index, const std::string &str);
        String& Insert(const int index, double v);
        String& Insert(const int index, double v, int ndigit);
        String& InsertInteger(const int index, long v);
        String& InsertCharacter(const int index, char c, size_t count = 1);
        String& InsertFormat(const int index, const char *fmt, ...);
        
        /*
         * 删除指定范围的字符串
         * 参数：
         *  - from：支持负向索引，-1代表末字节
         *  - to：支持负向索引，-1代表末字节
         *  - index：支持负向索引，-1代表末字节
         * 注意：
         *  - 范围是全包含关系，[from, to]
         */
        String& EraseFromTo(const int from, const int to);
        String& EraseFrom(const int index);
        String& EraseTo(const int index);
        String& Erase(const int index, const size_t count);
        String& EraseAll(const std::string &substr);
        String& EraseFirst(const std::string &substr);
        String& EraseLast(const std::string &substr);
        
        /*
         * 在字符串两端填充
         * 参数：
         *  - width：填充的个数
         *  - fillchar：要填充的字符
         * 注意：
         *  - [Padding]填充时，是左右分别填充width个字符
         */
        String& LeftPad(const int width, const char fillchar);
        String& RightPad(const int width, const char fillchar);
        static std::string LeftPad(const std::string &str, const int width, const char fillchar);
        static std::string RightPad(const std::string &str, const int width, const char fillchar);

        /*
         * 在字符串两端清除指定字符
         * 参数：
         *  - charset：要清除的字符集合
         * 说明：
         *  - 默认清除的空白字符，包含: ' ', '\t', '\r', '\n', '\v', '\f'
         */
        String& TrimLeft();
        String& TrimLeftBy(const char c);
        String& TrimLeftBy(const std::string &charset);
        
        String& TrimRight();
        String& TrimRightBy(const char c);
        String& TrimRightBy(const std::string &charset);
        
        String& Trim();
        String& TrimBy(const char c);
        String& TrimBy(const std::string &charset);
        
        static std::string TrimLeft(const std::string &str);
        static std::string TrimRight(const std::string &str);
        static std::string Trim(const std::string &str);

        /*
         * 字符串查找
         * 参数：
         *  - from：支持负向索引，-1代表末字节
         *  - to：支持负向索引，-1代表末字节
         *  - icase：忽略字符串大小写，默认false(不忽略)
         * 注意：
         *  - 查找范围是全包含关系，[from, to]
         *  - 若没有找到返回String::NotFound(-1)
         */
        int IndexOfFromTo(const int from, const int to, const std::string &str, bool icase = false);
        int IndexOfFrom(const int from, const std::string &str, bool icase = false);
        int IndexOfTo(const int to, const std::string &str, bool icase = false);
        int IndexOf(const char c, bool icase = false);
        int IndexOf(const std::string &str, bool icase = false);
        
        /*
         * 字符串逆序查找
         * 参数：
         *  - icase：忽略字符串大小写，默认false(不忽略)
         * 注意：
         *  - 若没有找到返回String::NotFound(-1)
         */
        int ReverseIndexOf(const char c, bool icase = false);
        int ReverseIndexOf(const std::string &str, bool icase = false);
        
        /*
         * 字符串分割
         *  - delimiter：以字符分割
         *  - str：以字符串整体作为分割的依据
         * 说明：
         *  - 保留空字符串，可以通过Contact复原，例如："he":('h')=> "", "e"
         *  - SplitByString不同于SplitByCharacterSet，SplitByString将字符串作为一个整体为分割依据
         */
        std::vector<std::string> SplitByCharacter(const char delimiter);
        std::vector<std::string> SplitByString(const std::string &delimiter);
        static std::vector<std::string> SplitByCharacter(const std::string &str, const char delimiter);
        static std::vector<std::string> SplitByString(const std::string &str, const std::string &delimiter);

        /*
         * 字符串分割
         * 参数：
         *  - delimiterset：分割字符集合，每一个字符都是分割依据
         * 说明：
         *  - 不会保留空字符串，不可复原，例如："he":("h")=> "e"
         */
        std::vector<std::string> SplitByCharacterSet(const std::string &delimiterset);
        static std::vector<std::string> SplitByCharacterSet(const std::string &str, const std::string &delimiterset);

        /*
         * 字符串连接
         * 参数：
         *  - str：在连接点添加的字符串
         *  - c：在连接点添加的字符
         * 说明：
         *  - 将字符串两两连接，并且在连接点添加分割字符或者字符串
         */
        static std::string Concat(const std::vector<std::string> &array, const char *str);
        static std::string Concat(const std::vector<std::string> &array, const char c);
        
        /*
         * 字符串判等
         * 参数：
         *  - icase：是否忽略大小写，默认不忽略
         */
        bool IsEqual(const std::string &str, bool icase = false);
        
        /*
         * 判断的每个字符
         * IsUpper - 都是大写字符
         * IsLower - 都是小写字符
         * IsAlpha - 都是英文字母
         * IsDigit - 都是阿拉伯数字
         * IsSpace - 都是空白字符[' ', '\t', '\r', '\n', '\v', '\f']
         */
        bool IsUpper();
        bool IsLower();
        bool IsAlpha();
        bool IsDigit();
        bool IsSpace();
        
        /*
         * 判断的字符串的头或尾是否包含指定字符串
         */
        bool HasPrefix(const char prefix);
        bool HasPrefix(const std::string &prefix);
        bool HasSuffix(const char suffix);
        bool HasSuffix(const std::string &suffix);
        
        static bool HasPrefix(const std::string &str, const std::string &prefix);
        static bool HasSuffix(const std::string &str, const std::string &suffix);

        /*
         * 获取两个字符串的相同头或者尾
         * 参数：
         *  - icase(ignorecase)：忽略大小写，默认不忽略
         */
        std::string CommonPrefix(const std::string &str, bool icase = false);
        std::string CommonSuffix(const std::string &str, bool icase = false);

        static std::string CommonPrefix(const std::string &str1, const std::string &str2, bool icase = false);
        static std::string CommonSuffix(const std::string &str1, const std::string &str2, bool icase = false);

        /*
         * 字符串逆序
         */
        String& Reverse();
        
        /*
         * 字符串重复count次
         */
        String& Repeat(size_t count);
        
        /*
         * 获取字符串的32位或者64位的Hash值
         * 说明：
         *  - 内部使用的Murmur算法计算Hash
         */
        uint32_t Hash32();
        uint64_t Hash64();
        std::string Hash32String();
        std::string Hash64String();
        
        /*
         * 将字符串转化为十六进制或者二进制的字符串
         * 参数：
         *  - blankByte：每隔多少个字节加一个空格，若blankByte<=0则无空格
         */
        std::string ToHex(int blankByte = 0);
        std::string ToBinary(int blankByte = 0);
        
        /*
         * 大小写处理
         * Capitalized - 首字母大写，其余字母不变
         * ToLower - 全部转为小写
         * ToUpper - 全部转为大写
         */
        String& Capitalized();
        String& ToLower();
        String& ToUpper();
        
        /*
         * 字符串转数字
         */
        double ToDouble();
        int    ToInt();
        long   ToLong();
        bool   ToBool();
        
        /*
         * 基本数据类型转字符串
         * 参数：
         *  - ndigit: 取小数的位数
         *  - uppercase: 是否转成大写
         */
        template<typename _Tp>
        inline static String ToString(const _Tp &v);
        template<typename _Tp>
        inline static std::string ToCppString(const _Tp &v);
        
        static String ToString(bool v, bool uppercase=true);
        static std::string ToCppString(bool v, bool uppercase=true);
        
        static String ToString(const double v, int ndigit);
        static std::string ToCppString(const double v, int ndigit);
        
        static String ToString(const std::vector<std::string> &args);
        static std::string ToCppString(const std::vector<std::string> &args);
        
        /*
         * 格式化字符串
         */
        static std::string Format(const char *fmt, ...);
        
        /*
         * 格式化字符串
         * 参数：
         *  - buffsize: 指定创建的格式化缓存区大小，默认是`String::FORMAT_BUFFSIZE(1024)`，若过小则会截断
         * 注意：
         *  - 调用前和调用后要分别调用va_start()和va_end()
         *  - 若使用GetFormatVarLength()获得buffsize则，在调用本接口前必须再次调用va_start()
         */
        static std::string FormatVar(const char *fmt, va_list varlist, size_t buffsize = String::FORMAT_BUFFSIZE);
        
        /*
         * 获取FormatVar格式化后的结果长度，不包含'\0'结尾的长度
         * 注意：调用前和调用后要分别调用va_start()和va_end()
         */
        static size_t GetFormatVarLength(const char *fmt, va_list varlist);

    private:
        
        std::string m_string;
    };
    
    template<typename _Tp>
    String String::ToString(const _Tp &v) {
        std::ostringstream os; os << v;
        return os.str();
    }
    
    template<typename _Tp>
    std::string String::ToCppString(const _Tp &v) {
        std::ostringstream os; os << v;
        return os.str();
    }
    
}

#endif
