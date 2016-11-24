//
//  Torch
//
//  Created by Luwei.
//  Copyright (c) 2014-2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy/Torch
//

#include "torch-path.h"
#include "torch-string.h"
#include <assert.h>
#include <cstdlib>

using namespace torch;

#define TORCH_PATH_BACK      ".."
#define TORCH_PATH_CURRENT   "."

// [Path]

Path::Path(const std::string& path)
{
    m_origin = path;
}

std::string Path::GetPath() const
{
    return m_origin;
}

bool Path::IsAbsolute()
{
    char psep = Path::GetSeparator();
    if (m_origin.length() > 0) {
        return (m_origin.front() == psep);
    }
    return false;
}

std::string Path::GetExtension() const
{
    std::string filename = this->GetFilename();
    size_t pos = filename.find_last_of('.');
    if ( (std::string::npos == pos) || (pos >= filename.length() - 1) ) {
        return std::string();
    }
    return filename.substr(pos + 1);
}

std::string Path::GetFilename() const
{
    char psep = Path::GetSeparator();
    size_t pos = m_origin.find_last_of(psep);
    if ( (std::string::npos == pos) && (pos >= m_origin.length() - 1) ) {
        return std::string();
    }
    return m_origin.substr(pos + 1);
}

std::string Path::GetDirectory() const
{
    std::string filename = this->GetFilename();
    if (filename.length() <= 0) {
        return m_origin;
    }
    return m_origin.substr(0, m_origin.length() - filename.length());
}

std::string Path::GetComponent(int index) const
{
    std::vector<std::string> components = this->Split();
    int comsize = (int)components.size();
    if (comsize <= 0) {
        return std::string();
    }
    
    std::vector<int> commap(comsize);
    
    int cindex = 0;
    for (int i = 0; i < (int)components.size(); i++) {
        std::string item = components.at(i);
        commap[i] = -1;
        if (item.length() > 0) {
            commap[cindex++] = i;
        }
    }
    
    if (index < 0) {
        index = cindex + index;
    }
    if (index < 0 || index >= (int)commap.size()) {
        return std::string();
    }
    if (commap[index] >= 0) {
        int ri = commap[index];
        return components[ri];
    }
    return std::string();
}

std::string Path::GetLastComponent() const
{
//    char psep = Path::GetSeparator();
//    int rpos = (int)m_origin.length() - 1;
//   
//    while (rpos >= 0 && psep == m_origin.at(rpos) ) {
//        rpos--;
//    }
//    
//    int lpos = rpos;
//    while ( lpos >= 0 && psep != m_origin.at(lpos)  ) {
//        lpos--;
//    }
//    
//    if (lpos >= 0) {
//        return m_origin.substr(lpos + 1, rpos-lpos);
//    }
//    return std::string();
    return this->GetComponent(-1);
}

Path& Path::Append(const std::string &pathcom)
{
    char sep  = Path::GetSeparator();
    char last = m_origin.back();
    
    std::string com = pathcom;
    while (com.front() == sep) {
        com = com.substr(1, com.length() - 1);
    }
    
    if (last == sep) {
        m_origin.append(com);
    }
    else {
        m_origin = m_origin.append(sep + com);
    }
        
    return *this;
}

Path& Path::RemoveComponent(int index)
{
    std::vector<std::string> components = this->Split();
    int comsize = (int)components.size();
    if (comsize <= 0) {
        return *this;
    }
    
    std::vector<int> commap(comsize);
    
    int cindex = 0;
    for (int i = 0; i < (int)components.size(); i++) {
        std::string item = components.at(i);
        commap[i] = -1;
        if (item.length() > 0) {
            commap[cindex++] = i;
        }
    }
    
    if (index < 0) {
        index = cindex + index;
    }
    if (index < 0 || index >= (int)commap.size()) {
        return *this;
    }
    if (commap[index] >= 0) {
        int rmi = commap[index];
        if (rmi == (int)components.size() - 1) {
            components[rmi] = std::string();
        }
        else {
            components.erase(components.begin() + rmi);
        }
    }
    m_origin = Path::Join(components).GetPath();
    
    return *this;
}

Path& Path::RemoveLastComponent(int count)
{
//    char psep = Path::GetSeparator();
//    int rpos = (int)m_origin.length() - 1;
//    
//    while (rpos >= 0 && psep == m_origin.at(rpos) ) {
//        rpos--;
//    }
//    
//    int lpos = rpos;
//    while ( lpos >= 0 && psep != m_origin.at(lpos)  ) {
//        lpos--;
//    }
//    
//    if (lpos >= 0) {
//        m_origin = m_origin.substr(0, lpos + 1);
//    }
//    return *this;
    while (count-- > 0) {
        this->RemoveComponent(-1);
    }
    return *this;
}

Path& Path::ChangeComponent(int index, const std::string &component)
{
    std::vector<std::string> components = this->Split();
    int comsize = (int)components.size();
    if (comsize <= 0) {
        return *this;
    }
    
    std::vector<int> commap(comsize);
    
    int cindex = 0;
    for (int i = 0; i < (int)components.size(); i++) {
        std::string item = components.at(i);
        commap[i] = -1;
        if (item.length() > 0) {
            commap[cindex++] = i;
        }
    }
    
    if (index < 0) {
        index = cindex + index;
    }
    if (index < 0 || index >= (int)commap.size()) {
        return *this;
    }
    if (commap[index] >= 0) {
        components[commap[index]] = component;
    }
    m_origin = Path::Join(components).GetPath();

    return *this;
}

Path& Path::ChangeLastComponent(const std::string &component)
{
    this->ChangeComponent(-1, component);
    return *this;
}

Path& Path::ChangeFilename(const std::string &filename)
{
    std::string oldname = this->GetFilename();
    if (oldname.length() > 0) {
        this->ChangeComponent(-1, filename);
    }
    return *this;
}

Path& Path::ChangeExtension(const std::string &extension)
{
    std::string oldext = this->GetExtension();
    std::string oldname = this->GetFilename();

    if (oldext.length() > 0 && oldname.length() > 0) {
        int namelen = (int)oldname.length() - (int)oldext.length();
        std::string name = oldname.substr(0, namelen);
        this->ChangeComponent(-1, name + String(extension).Trim().TrimLeftBy('.').GetCppString());
    }
    return *this;
}

Path& Path::Normalize()
{
    char psep = Path::GetSeparator();
    
    bool isab = m_origin.front() == psep;
    bool isdir = m_origin.back() == psep;
    
    std::vector<std::string> components = this->Split();
    
    for (int i = 0; i < (int)components.size(); i++) {
        std::string com = components[i];
        if (com.length() <= 0) {
            components.erase(components.begin() + i--);
            continue;
        }
        if (com == TORCH_PATH_BACK) {
            components.erase(components.begin() + i);
            if (i - 1 >= 0) {
                components.erase(components.begin() + i - 1);
            }
            i = std::max(i-3, -1);
        }
        else if (com == TORCH_PATH_CURRENT) {
            components.erase(components.begin() + i--);
        }
    }
    
    String path = String(Path::Join(components).GetPath()).TrimBy(psep);
    if (isdir && path.GetLength() > 0) {
        path.AppendCharacter(psep);
    }
    if (isab) {
        path.InsertCharacter(0, psep);
    }
    m_origin = path.GetCppString();
    return *this;
}

std::vector<std::string> Path::Split() const
{
    return String(m_origin).SplitByCharacter(Path::GetSeparator());
}

std::vector<std::string> Path::Split(const std::string &path)
{
    return Path(path).Split();
}

Path Path::Join(const std::vector<std::string> &components)
{
    char psep = Path::GetSeparator();
    return String::Concat(components, psep);
}

Path Path::Join(const std::string &path, const std::string &com)
{
    return Path(path).Append(com);
}

std::string Path::GetHomeDir()
{
    return std::getenv("HOME");
}

std::string Path::GetTemp()
{
    return ::tmpnam(nullptr);
}

char Path::GetSeparator()
{
#ifdef _WIN32
    return '\\';
#else
    return '/';
#endif
}
