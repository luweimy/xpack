//
//  Torch
//
//  Created by Luwei.
//  Copyright (c) 2014-2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy/Torch
//

#include <sstream>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include "torch-string.h"
#include "torch-util.h"
#include "core/torch-hash.h"


using namespace torch;

inline int ConvertNegativeIndex(const int index, const size_t length) {
    if (index < 0) {
        return (int)length + index ;
    }
    assert(index>=0 && index<=(int)length);
    return index;
}

String::String()
{
}

String::String(const char c, const size_t count)
{
    this->SetString(c, count);
}

String::String(const char* fmt, ...)
{
    va_list ap;
    
    va_start(ap, fmt);
    size_t l = String::GetFormatVarLength(fmt, ap);
    va_end(ap);

    va_start(ap, fmt);
    std::string s = String::FormatVar(fmt, ap, l + 1);
    va_end(ap);
    
    this->SetString(s);
}

String::String(const std::string &str)
{
    this->SetString(str);
}

String::String(const String& other)
{
    this->SetString(other);
}

String::String(String&& other)
{
    m_string = std::move(other.m_string);
}

String::String(const long v)
{
    this->SetString(v);
}

String::String(const double v)
{
    this->SetString(v);
}

String::String(const double v, int ndigit)
{
    this->SetString(v, ndigit);
}

String::~String()
{
}

String& String::operator=(const String& other)
{
    if (this == &other) {
        return *this;
    }
    m_string = other.m_string;
    return *this;
}

String& String::operator=(String&& other)
{
    m_string = std::move(other.m_string);
    return *this;
}

String& String::SetString(const char c, const size_t count)
{
    m_string = std::string(count, c);
    return *this;
}

String& String::SetString(const char* fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    size_t l = String::GetFormatVarLength(fmt, ap);
    va_end(ap);

    va_start(ap, fmt);
    std::string s = String::FormatVar(fmt, ap, l + 1);
    va_end(ap);
    
    m_string = std::move(s);
    return *this;
}

String& String::SetString(const std::string &str)
{
    m_string = str;
    return *this;
}

String& String::SetString(const String &str)
{
    m_string = str.m_string;
    return *this;
}

String& String::SetString(const long v)
{
    m_string = String::ToString(v).GetCppString();
    return *this;
}

String& String::SetString(const double v)
{
    m_string = String::ToString(v).GetCppString();
    return *this;
}

String& String::SetString(const double v, int ndigit)
{
    m_string = String::ToString(v, ndigit).GetCppString();
    return *this;
}

size_t String::GetLength()
{
    return m_string.length();
}

bool String::IsEmpty()
{
    return m_string.empty() || this->IsSpace();
}

char String::GetCharAt(const int index)
{
    int i = ConvertNegativeIndex(index, m_string.length());
    return m_string.at(i);
}

const char* String::GetCString()
{
    return m_string.c_str();
}

const std::string& String::GetCppString()
{
    return m_string;
}

std::string String::SubstrFromTo(const int from, const int to)
{
    int fromi = ConvertNegativeIndex(from, m_string.length());
    int toi = ConvertNegativeIndex(to, m_string.length());
    assert(toi >= fromi);
    return this->Substr(fromi, toi - fromi + 1);
}

std::string String::SubstrFrom(const int from)
{
    return this->Substr(from, this->GetLength() - from);
}

std::string String::SubstrTo(const int to)
{
    return this->Substr(0, to + 1);
}

std::string String::Substr(const int index, const size_t count)
{
    int i = ConvertNegativeIndex(index, m_string.length());
    return m_string.substr(i, count);
}

std::string String::SubstrFromTo(const std::string &str, const int from, const int to)
{
    return String(str).SubstrFromTo(from, to);
}

std::string String::SubstrFrom(const std::string &str, const int from)
{
    return String(str).SubstrFrom(from);
}

std::string String::SubstrTo(const std::string &str, const int to)
{
    return String(str).SubstrTo(to);
}

std::string String::Substr(const std::string &str, const int index, const size_t count)
{
    return String(str).Substr(index, count);
}

String& String::ReplaceFromTo(const int from, const int to, const std::string &str)
{
    int fromi = ConvertNegativeIndex(from, m_string.length());
    int toi = ConvertNegativeIndex(to, m_string.length());
    assert(toi >= fromi);
    this->Replace(fromi, toi - fromi + 1, str);
    return *this;
}

String& String::ReplaceFrom(const int from, const std::string &str)
{
    int i = ConvertNegativeIndex(from, m_string.length());
    this->Replace(i, this->GetLength() - i, str);
    return *this;
}

String& String::ReplaceTo(const int to,  const std::string &str)
{
    int i = ConvertNegativeIndex(to, m_string.length());
    this->Replace(0, i + 1, str);
    return *this;
}

String& String::Replace(const int index, const size_t count, const std::string &str)
{
    int i = ConvertNegativeIndex(index, m_string.length());
    m_string.replace(i, count, str);
    return *this;
}

String& String::ReplaceAll(const std::string &oldstr, const std::string &str)
{
    size_t size = oldstr.length();
    std::string::size_type index = m_string.find(oldstr);
    while (index != std::string::npos) {
        m_string = m_string.replace(index, size, str);
        index = (int)m_string.find(oldstr);
    }
    return *this;
}

String& String::ReplaceFirst(const std::string &oldstr, const std::string &str)
{
    size_t size = oldstr.length();
    std::string::size_type index = m_string.find(oldstr);
    if (index != std::string::npos) {
        m_string = m_string.replace(index, size, str);
    }
    return *this;
}

String& String::ReplaceLast(const std::string &oldstr, const std::string &str)
{
    size_t size = oldstr.length();
    std::string::size_type index = m_string.rfind(oldstr);
    if (index != std::string::npos) {
        m_string = m_string.replace(index, size, str);
    }
    return *this;
}

String& String::Append(const char *cstr)
{
    m_string.append(cstr);
    return *this;
}

String& String::Append(const std::string &str)
{
    m_string.append(str);
    return *this;
}

String& String::AppendFloat(double v)
{
    m_string.append(String::ToString(v).GetCppString());
    return *this;
}

String& String::AppendFloat(double v, int ndigit)
{
    m_string.append(String::ToString(v, ndigit).GetCppString());
    return *this;
}

String& String::AppendInteger(long v)
{
    m_string.append(String::ToString(v).GetCppString());
    return *this;
}

String& String::AppendCharacter(char c, size_t count)
{
    m_string.append(std::string(count, c));
    return *this;
}

String& String::AppendFormat(const char *fmt, ...)
{
    va_list ap;
    
    va_start(ap, fmt);
    size_t l = String::GetFormatVarLength(fmt, ap);
    va_end(ap);
    
    va_start(ap, fmt);
    std::string s = String::FormatVar(fmt, ap, l + 1);
    this->Append(s);
    va_end(ap);
    
    return *this;
}

std::string String::AppendFloat(const std::string &str, double v)
{
    return std::move(String(str).AppendFloat(v).GetCppString());
}

std::string String::AppendFloat(const std::string &str, double v, int ndigit)
{
    return String(str).AppendFloat(v, ndigit).GetCppString();
}

std::string String::AppendInteger(const std::string &str, long v)
{
    return String(str).AppendInteger(v).GetCppString();
}

std::string String::AppendCharacter(const std::string &str, char c, size_t count)
{
    return String(str).AppendCharacter(c, count).GetCppString();
}

String& String::Insert(const int index, const std::string &str)
{
    int i = ConvertNegativeIndex(index, m_string.length());
    m_string.insert(i, str);
    return *this;
}

String& String::Insert(const int index, double v)
{
    int i = ConvertNegativeIndex(index, m_string.length());
    m_string.insert(i, String::ToString(v).GetCppString());
    return *this;
}

String& String::Insert(const int index, double v, int ndigit)
{
    int i = ConvertNegativeIndex(index, m_string.length());
    m_string.insert(i, String::ToString(v, ndigit).GetCppString());
    return *this;
}

String& String::InsertInteger(const int index, long v)
{
    int i = ConvertNegativeIndex(index, m_string.length());
    m_string.insert(i, String::ToString(v).GetCppString());
    return *this;
}

String& String::InsertCharacter(const int index, char c, size_t count)
{
    int i = ConvertNegativeIndex(index, m_string.length());
    m_string.insert(i, std::string(count, c));
    return *this;
}

String& String::InsertFormat(const int index, const char *fmt, ...)
{
    int i = ConvertNegativeIndex(index, m_string.length());
    
    va_list ap;
    
    va_start(ap, fmt);
    size_t l = String::GetFormatVarLength(fmt, ap);
    va_end(ap);

    va_start(ap, fmt);
    std::string s = String::FormatVar(fmt, ap, l + 1);
    va_end(ap);
    
    m_string.insert(i, s);
    return *this;
}

String& String::EraseFromTo(const int from, const int to)
{
    int fromi = ConvertNegativeIndex(from, m_string.length());
    int toi = ConvertNegativeIndex(to, m_string.length());
    m_string.erase(fromi, toi);
    return *this;
}

String& String::EraseFrom(const int from)
{
    int i = ConvertNegativeIndex(from, m_string.length());
    m_string.erase(i);
    return *this;
}

String& String::EraseTo(const int to)
{
    int i = ConvertNegativeIndex(to, m_string.length());
    m_string.erase(0, i + 1);
    return *this;
}

String& String::Erase(const int index, const size_t count)
{
    int i = ConvertNegativeIndex(index, m_string.length());
    m_string.erase(i, count);
    return *this;
}

String& String::EraseAll(const std::string &substr)
{
    size_t size = substr.length();
    size_t index = m_string.find(substr);
    while (index != std::string::npos) {
        m_string.erase(index, size);
        index = m_string.find(substr);
    }
    return *this;
}

String& String::EraseFirst(const std::string &substr)
{
    size_t index = m_string.find(substr);
    if (index != std::string::npos) {
        m_string.erase(index, substr.length());
    }
    return *this;
}

String& String::EraseLast(const std::string &substr)
{
    size_t index = m_string.rfind(substr);
    if (index != std::string::npos) {
        m_string.erase(index, substr.length());
    }
    return *this;
}

String& String::LeftPad(const int width, const char fillchar)
{
    int diff = width - (int)m_string.length();
    if (diff > 0) {
        std::string fillstr(diff, fillchar);
        m_string = fillstr + m_string;
    }
    return *this;
}

String& String::RightPad(const int width, const char fillchar)
{
    int diff = width - (int)m_string.length();
    if (diff > 0) {
        std::string fillstr(diff, fillchar);
        m_string = m_string + fillstr;
    }
    return *this;
}

std::string String::LeftPad(const std::string &str, const int width, const char fillchar)
{
    std::string pad = str;
    int diff = width - (int)pad.length();
    if (diff > 0) {
        pad = std::string(diff, fillchar) + pad;
    }
    return std::move(pad);
}

std::string String::RightPad(const std::string &str, const int width, const char fillchar)
{
    std::string pad = str;
    int diff = width - (int)pad.length();
    if (diff > 0) {
        pad += std::string(diff, fillchar);
    }
    return std::move(pad);
}

String& String::TrimLeft()
{
    auto condition = std::not1(std::ptr_fun(isspace));
    auto itr = std::find_if(m_string.begin(), m_string.end(), condition);
    m_string.erase(m_string.begin(), itr);
    return *this;
}

String& String::TrimLeftBy(const char c)
{
    m_string.erase(0, m_string.find_first_not_of(c));
    return *this;
}

String& String::TrimLeftBy(const std::string &charset)
{
    int length = (int)charset.length();
    int i = 0;
    for (; i < m_string.length(); i++) {
        char curchar = m_string.at(i);
        int j = 0;
        for (; j < length; j++) {
            if (curchar == charset[j]) {
                break;
            }
        }
        if (j >= length) {
            break;
        }
    }
    m_string.erase(0, i);
    return *this;
}

String& String::TrimRight()
{
    auto condition = std::not1(std::ptr_fun(isspace));
    auto itr = std::find_if(m_string.rbegin(), m_string.rend(), condition);
    m_string.erase(itr.base(), m_string.end());
    return *this;
}

String& String::TrimRightBy(const char c)
{
    m_string.erase(m_string.find_last_not_of(c) + 1);
    return *this;
}

String& String::TrimRightBy(const std::string &charset)
{
    int length = (int)charset.length();
    int i = (int)m_string.length() - 1;
    for (; i >= 0; i--) {
        char curchar = m_string.at(i);
        int j = 0;
        for (; j < length; j++) {
            if (curchar == charset[j]) {
                break;
            }
        }
        if (j >= length) {
            break;
        }
    }
    m_string.erase(i+1);
    return *this;
}

String& String::Trim()
{
    this->TrimLeft().TrimRight();
    return *this;
}

String& String::TrimBy(const char c)
{
    this->TrimLeftBy(c).TrimRightBy(c);
    return *this;
}

String& String::TrimBy(const std::string &charset)
{
    this->TrimLeftBy(charset).TrimRightBy(charset);
    return *this;
}

std::string String::TrimLeft(const std::string &str)
{
    return String(str).TrimLeft().GetCppString();
}

std::string String::TrimRight(const std::string &str)
{
    return String(str).TrimRight().GetCppString();
}

std::string String::Trim(const std::string &str)
{
    return String(str).Trim().GetCppString();
}

int String::IndexOfFromTo(const int from, const int to, const std::string &str, bool icase)
{
    size_t size = m_string.length();
    int fromi = ConvertNegativeIndex(from, size);
    int toi = ConvertNegativeIndex(to, size);
    assert(fromi<=toi);

    if (icase == false) {
        size_t index = m_string.find(str, fromi);
        if (std::string::npos == index || toi < index+str.length()-1) {
            return String::NotFound;
        }
        else {
            return (int)index;
        }
    }
    
    std::string target = m_string;
    std::string substr = str;
    std::transform(target.begin(), target.end(), target.begin(), ::tolower);
    std::transform(substr.begin(), substr.end(), substr.begin(), ::tolower);

    size_t index = target.find(substr, fromi);
    if (std::string::npos == index ||  toi < index+str.length()-1) {
        return String::NotFound;
    }
    else {
        return (int)index;
    }
}

int String::IndexOfFrom(const int from, const std::string &str, bool icase)
{
    return this->IndexOfFromTo(from, (int)m_string.length()-1, str, icase);
}

int String::IndexOfTo(const int to, const std::string &str, bool icase)
{
    return this->IndexOfFromTo(0, to, str, icase);
}

int String::IndexOf(const char c, bool icase)
{
    char cstr[] = {c, '\0'};
    return this->IndexOfFromTo(0, (int)m_string.length()-1, cstr, icase);
}

int String::IndexOf(const std::string &str, bool icase)
{
    return this->IndexOfFromTo(0, (int)m_string.length()-1, str, icase);
}

int String::ReverseIndexOf(const char c, bool icase)
{
    char cstr[] = {c, '\0'};
    return this->ReverseIndexOf(cstr, icase);
}

int String::ReverseIndexOf(const std::string &str, bool icase)
{
    if (icase == false) {
        size_t index = m_string.rfind(str);
        if (std::string::npos == index) {
            return String::NotFound;
        }
        else {
            return (int)index;
        }
    }
    
    std::string target = m_string;
    std::string substr = str;
    std::transform(target.begin(), target.end(), target.begin(), ::tolower);
    std::transform(substr.begin(), substr.end(), substr.begin(), ::tolower);

    size_t index = target.rfind(substr);
    if (std::string::npos == index) {
        return String::NotFound;
    }
    else {
        return (int)index;
    }
}

std::vector<std::string> String::SplitByCharacter(const char delimiter)
{
    if (m_string.length() <= 0) {
        return std::vector<std::string>();
    }
    
    std::string content = m_string;
    content += delimiter;
    
    std::string::size_type x;
    std::vector<std::string> v;
    
    for(size_t i = 0; i < content.size(); i++) {
        x = content.find(delimiter, i);
        if(x < content.size()) {
            std::string s = content.substr(i, x - i);
            v.push_back(s);
            i = x + 1 - 1;
        }
    }
    return std::move(v);
}

std::vector<std::string> String::SplitByString(const std::string &delimiter)
{
    std::string content = m_string;
    content += delimiter;
    
    std::string::size_type x;
    std::vector<std::string> v;
    size_t l = delimiter.length();
    
    for(size_t i = 0; i < content.size(); i++) {
        x = content.find(delimiter, i);
        if(x < content.size()) {
            std::string s = content.substr(i, x - i);
            v.push_back(s);
            i = x + l - 1;
        }
    }
    return std::move(v);
}

std::vector<std::string> String::SplitByCharacter(const std::string &str, const char delimiter)
{
    return std::move(String(str).SplitByCharacter(delimiter));
}

std::vector<std::string> String::SplitByString(const std::string &str, const std::string &delimiter)
{
    return std::move(String(str).SplitByString(delimiter));
}

std::vector<std::string> String::SplitByCharacterSet(const std::string &delimiterset)
{
    std::vector<std::string> v;
    size_t sz = m_string.length()+1;
    
    char *buf = (char*)torch::HeapMalloc(sz);
    memcpy(buf, m_string.c_str(), sz);
    
    char *p;
    v.push_back(strtok(buf, delimiterset.c_str()));
    while((p = strtok(NULL, delimiterset.c_str()))) {
        v.push_back(p);
    }
    
    torch::HeapFree(buf);
    
    return std::move(v);
}

std::vector<std::string> String::SplitByCharacterSet(const std::string &str, const std::string &delimiterset)
{
    return std::move(String(str).SplitByCharacterSet(delimiterset));
}

std::string String::Concat(const std::vector<std::string> &array, const char *str)
{
    size_t size = array.size();
    std::string buf( (size==0) ? "" : array[0] );
    for (size_t i = 1; i < size; i++) {
        buf += str;
        buf += array[i];
    }
    return buf;
}

std::string String::Concat(const std::vector<std::string> &array, const char c)
{
    size_t size = array.size();
    std::string buf( (size==0) ? "" : array[0] );
    for (size_t i = 1; i < size; i++) {
        buf += c;
        buf += array[i];
    }
    return buf;
}

bool String::IsEqual(const std::string &str, bool icase)
{
    if (false == icase) {
        return m_string == str;
    }
    else {
        return strcasecmp(m_string.c_str(), str.c_str()) == 0;
    }
}

bool String::IsUpper()
{
    bool r = true;
    for (char x : m_string) {
        if (!::isupper(x)) r = false;
    }
    return r;
}

bool String::IsLower()
{
    bool r = true;
    for (char x : m_string) {
        if (!::islower(x)) r = false;
    }
    return r;
}

bool String::IsAlpha()
{
    bool r = true;
    for (char x : m_string) {
        if (!::isalpha(x)) r = false;
    }
    return r;
}

bool String::IsDigit()
{
    bool r = true;
    for (char x : m_string) {
        if (!::isdigit(x)) r = false;
    }
    return r;
}

bool String::IsSpace()
{
    bool r = true;
    for (char x : m_string) {
        if (!::isspace(x)) r = false;
    }
    return r;
}

bool String::HasPrefix(const char prefix)
{
    return m_string.front() == prefix;
}

bool String::HasPrefix(const std::string &prefix)
{
    return m_string.compare(0, prefix.length(), prefix) == 0;
}

bool String::HasSuffix(const char suffix)
{
    return m_string.back() == suffix;
}

bool String::HasSuffix(const std::string &suffix)
{
    size_t sz1 = m_string.length();
    size_t sz2 = suffix.length();
    return m_string.compare(sz1 - sz2, sz2, suffix) == 0;
}

bool String::HasPrefix(const std::string &str, const std::string &prefix)
{
    return String(str).HasPrefix(prefix);
}

bool String::HasSuffix(const std::string &str, const std::string &suffix)
{
    return String(str).HasSuffix(suffix);
}

std::string String::CommonPrefix(const std::string &str, bool icase)
{
    long len = std::min(m_string.length(), str.length());

    int i = 0;
    
    if (false == icase) {
        while (i < len) {
            char a1 = m_string.at(i), a2 = str[i];
            if (a1 != a2) break;
            i++;
        }
    }
    else {
        while (i < len) {
            char a1 = m_string.at(i), a2 = str[i];
            if (::toupper(a1) != ::toupper(a2)) break;
            i++;
        }
    }
    return m_string.substr(0, i);
}

std::string String::CommonSuffix(const std::string &str, bool icase)
{
    size_t sz = str.length();
    long i = m_string.length() - 1, j = sz - 1;
    
    if (false == icase) {
        for (; i>=0 && j>=0; i--, j--) {
            char a1 = m_string.at(i), a2 = str[j];
            if (a1 != a2) break;
        }
    }
    else {
        for (; i>=0 && j>=0; i--, j--) {
            char a1 = m_string.at(i), a2 = str[j];
            if (::toupper(a1) != ::toupper(a2)) break;
        }
    }
    return m_string.substr(i + 1, sz);
}

std::string String::CommonPrefix(const std::string &str1, const std::string &str2, bool icase)
{
    return String(str1).CommonPrefix(str2, icase);
}

std::string String::CommonSuffix(const std::string &str1, const std::string &str2, bool icase)
{
    return String(str1).CommonSuffix(str2, icase);
}

String& String::Reverse()
{
    std::reverse(m_string.begin(), m_string.end());
    return *this;;
}

String& String::Repeat(size_t count)
{
    std::string str;
    while (count > 0) {
        str += m_string;
        count--;
    }
    m_string = str;
    return *this;
}

uint32_t String::Hash32()
{
    return Hash::x86::Murmur32(m_string.c_str(), (unsigned int)m_string.length());
}

uint64_t String::Hash64()
{
    return Hash::x64::Murmur64(m_string.c_str(), (unsigned int)m_string.length());
}

std::string String::Hash32String()
{
    return Hash::ToString(this->Hash32());
}

std::string String::Hash64String()
{
    return Hash::ToString(this->Hash64());
}

std::string String::ToHex(int byteblank)
{
    return torch::ToHex(m_string.c_str(), m_string.length(), byteblank);
}

std::string String::ToBinary(int byteblank)
{
    return torch::ToBinary(m_string.c_str(), m_string.length(), byteblank);
}

String& String::Capitalized()
{
    if (m_string.length()>0) {
        m_string[0] = ::toupper(m_string.front());
    }
    return *this;
}

String& String::ToLower()
{
    std::transform(m_string.begin(), m_string.end(), m_string.begin(), ::tolower);
    return *this;
}

String& String::ToUpper()
{
    std::transform(m_string.begin(), m_string.end(), m_string.begin(), ::toupper);
    return *this;
}

double String::ToDouble()
{
    return atof(m_string.c_str());
}

int String::ToInt()
{
    return atoi(m_string.c_str());
}

long String::ToLong()
{
    return atol(m_string.c_str());
}

bool String::ToBool()
{
    std::string tmp = m_string;
    std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);
    return (::atol(tmp.c_str()) != 0) || tmp=="TRUE" || tmp=="YES";
}

String String::ToString(bool v, bool uppercase)
{
    return String::ToCppString(v, uppercase);
}

std::string String::ToCppString(bool v, bool uppercase)
{
    if (uppercase) {
        return (v) ? "TRUE" : "FALSE";
    }
    return (v) ? "true" : "false";
}

String String::ToString(const double v, int ndigit)
{
    return String::ToCppString(v, ndigit);
}

std::string String::ToCppString(const double v, int ndigit)
{
    std::string fmt = torch::String::Format("%%.%df", ndigit);
    char sb[50];
    memset(sb, 0, sizeof(sb));
    sprintf(sb, fmt.c_str(), v);
    return sb;
}

String String::ToString(const std::vector<std::string> &args)
{
    return String::ToCppString(args);
}

std::string String::ToCppString(const std::vector<std::string> &args)
{
    std::string ret = "[";
    for (size_t i = 0; i < args.size(); i++) {
        ret += args[i];
        if (i + 1 < args.size()) {
            ret += ", ";
        }
    }
    return ret + "]";
}

std::string String::Format(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt); // measure the required size
    size_t str_l = vsnprintf(NULL, (size_t)0, fmt, ap);
    assert(str_l >= 0); // vsnprintf return not format need length
    va_end(ap);

    va_start(ap, fmt);
    std::string s = String::FormatVar(fmt, ap, str_l + 1);
    va_end(ap);
    return std::move(s);
}

std::string String::FormatVar(const char *fmt, va_list varlist, size_t buffsize)
{
    static char sb[FORMAT_BUFFSIZE];
    memset(sb, 0, FORMAT_BUFFSIZE);

    char *ptr = sb;
    if (buffsize > FORMAT_BUFFSIZE) {
        ptr = (char*)torch::HeapMalloc(buffsize);
        memset(ptr, 0, buffsize);
    }
    vsprintf(ptr, fmt, varlist);
    std::string s(ptr);
    if (buffsize > FORMAT_BUFFSIZE) {
        torch::HeapFree(ptr);
    }
    return std::move(s);
}

size_t String::GetFormatVarLength(const char *fmt, va_list varlist)
{
    size_t str_l = vsnprintf(NULL, (size_t)0, fmt, varlist);
    assert(str_l >= 0); // vsnprintf return not format need length
    return str_l;
}


