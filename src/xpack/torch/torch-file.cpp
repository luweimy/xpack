//
//  Torch
//
//  Created by Luwei.
//  Copyright (c) 2014-2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy/Torch
//

#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include "torch-file.h"
#include "torch-path.h"

#define TORCH_CP_BUFFERSIZE 4096

using namespace torch;

// [File]

File::File()
:m_fstream(nullptr)
{
}

File::~File()
{
    this->Close();
}

bool File::Open(const std::string &path, const std::string &mode)
{
    if (m_fstream) {
        return false;
    }
    if (path.empty()) {
        return false;
    }
    if (mode.empty()) {
        return false;
    }
    
    m_fstream = fopen(path.c_str(), mode.c_str());
    if (!m_fstream) {
        return false;
    }
    m_path = path;
    return true;
}

bool File::OpenTemp()
{
    this->Close();
    m_fstream = tmpfile();
    if (!m_fstream) {
        return false;
    }
    return true;
}

bool File::Attach(FILE *pFile)
{
    assert(pFile);
    bool ret = this->Close();
    if (ret) {
        m_fstream = pFile;
    }
    return ret;
}

bool File::Close()
{
    if (!this->IsOpen()) {
        return true;
    }
    
    int c = fclose(m_fstream);
    if (c != 0) {
        return false;
    }
    m_fstream = nullptr;
    m_path = "";
    return true;
}

bool File::Flush()
{
    if (!this->IsOpen()) {
        return false;
    }
    return fflush(m_fstream) == 0;
}

bool File::IsOpen() const
{
    return (m_fstream != nullptr);
}

bool File::IsEOF() const
{
    if (!this->IsOpen()) {
        return true;
    }
    if (feof(m_fstream)) {
        return true;
    }
    return false;
}

long File::GetSize() const
{
    if (!this->IsOpen()) {
        return 0;
    }
    long old_pos = this->Tell();
    this->SeekEnd();
    long fsize = this->Tell();
    this->SeekSet(old_pos);
    return fsize;
}

const std::string& File::GetPath() const
{
    return m_path;
}

size_t File::Read(void *buffer, size_t size, size_t count) const
{
    if (!this->IsOpen()) {
        return 0;
    }
    return fread(buffer, size, count, m_fstream);
}

size_t File::Read(void *buffer, size_t size) const
{
    if (!this->IsOpen()) {
        return 0;
    }
    return fread(buffer, 1, size, m_fstream);
}

Data File::Read(size_t bytes) const
{
    Data d;
    d.ReSize(bytes);
    size_t size = this->Read(d.GetBytes(), 1, bytes);
    d.ReSize(size);
    return std::move(d);
}

void File::Read(Data &outdata, size_t bytes) const
{
    outdata.ReSize(bytes);
    size_t size = this->Read(outdata.GetBytes(), 1, bytes);
    outdata.ReSize(size);
}

size_t File::Write(const void *buffer, size_t size, size_t count) const
{
    if (!this->IsOpen()) {
        return 0;
    }
    return fwrite(buffer, size, count, m_fstream);
}

size_t File::Write(const void *buffer, size_t size) const
{
    if (!this->IsOpen()) {
        return 0;
    }
    return fwrite(buffer, 1, size, m_fstream);
}

size_t File::Write(const Data &data) const
{
    return this->Write(data.GetBytes(), 1, data.GetSize());
}

long File::Tell() const
{
    if (!this->IsOpen()) {
        return -1;
    }
    return ftell(m_fstream);
}

bool File::SeekSet() const
{
    return this->Seek(0L, Location::Set);
}

bool File::SeekEnd() const
{
    return this->Seek(0L, Location::End);
}

bool File::SeekSet(long offset) const
{
    return this->Seek(offset, Location::Set);
}

bool File::SeekCur(long offset) const
{
    return this->Seek(offset, Location::Cur);
}

bool File::SeekEnd(long offset) const
{
    return this->Seek(offset, Location::End);
}

bool File::Seek(long offset, Location where) const
{
    if (!this->IsOpen()) {
        return false;
    }
    int res = fseek(m_fstream, offset, (int)where);
    if (res != 0) {
        return false;
    }
    return true;
}

bool File::ReSize(size_t size)
{
    return ::ftruncate(this->GetFileDescriptor(), size) == 0;
}

int File::GetFileDescriptor()
{
    return fileno(m_fstream);
}

std::string File::GetString(size_t length) const
{
    if (!this->IsOpen()) {
        return std::string();
    }
    
    size_t outlen = 0;
    char* buffer = (char*)this->GetBytes(&outlen, length);
    if (!buffer) {
        return std::string();
    }
    
    std::string str(buffer, outlen);
    torch::HeapFree(buffer);

    return std::move(str);
}

torch::Data File::GetBytes(size_t length) const
{
    torch::Data data;
    size_t outlen = 0;
    void *buffer = this->GetBytes(&outlen, length);
    data.ShallowSet(buffer, outlen);
    return std::move(data);
}

void* File::GetBytes(size_t *outlen, size_t length) const
{
    assert(outlen);
    *outlen = 0;
    
    if (!this->IsOpen()) {
        return nullptr;
    }
    
    long curoffset = this->Tell();
    if (length == 0) {
        length = this->GetSize() - curoffset;
    }
    
    size_t allocsize = sizeof(char) * length;
    char *buffer = (char *)torch::HeapMalloc(allocsize);
    
    if (!buffer) {
        return nullptr;
    }
    memset(buffer, 0, allocsize);
    
    // return really bytes had read
    *outlen = this->Read(buffer, sizeof(char), length);
    
    this->SeekSet(curoffset);
    
    return buffer;
}

void File::DumpHex(size_t length) const
{
    if (!this->IsOpen())
        return;
    
    size_t outlen = 0;
    unsigned char* buffer = (unsigned char*)this->GetBytes(&outlen, length);
    
    for (size_t i = 0; i < outlen; i++) {
        unsigned char uc = buffer[i];
        printf("%02X", uc);
        if ((i+1) % 2 == 0) {
            putchar(' ');
        }
    }
    putchar('\n');
    torch::HeapFree(buffer);
}

void File::DumpBinary(size_t length) const
{
    if (!this->IsOpen()) {
        return;
    }
    
    size_t outlen = 0;
    unsigned char* buffer = (unsigned char*)this->GetBytes(&outlen, length);
    
    for (size_t i = 0; i < outlen; i++) {
        unsigned char uc = buffer[i];
        unsigned char index = 0x80;
        while (index > 0) {
            putchar(uc & index ? '1' : '0');
            index >>= 1;
        }
        putchar(' ');
    }
    putchar('\n');
    torch::HeapFree(buffer);
}

torch::Data File::GetBytes(const std::string &path, size_t length)
{
    bool ok = true;
    
    // read data from disk
    File fstream;
    ok = fstream.Open(path, "rb");
    if (!ok) {
        return Data::Null;
    }
    
    Data buffer = fstream.GetBytes(length);
    fstream.Close();

    return std::move(buffer);
}

bool File::WriteBytes(const std::string &path, const Data &data, WriteMode mode)
{
    return File::WriteBytes(path, (char *)data.GetBytes(), data.GetSize(), mode);
}

bool File::WriteBytes(const std::string &path, const char *s, size_t length, WriteMode mode)
{
    std::string smode = "wb";
    if (mode == WriteMode::CreateOrAppend) {
        smode = "ab";
    }
    
    File fstream;
    bool ok = fstream.Open(path, smode);
    if (!ok) {
        return false;
    }
    
    size_t sz = fstream.Write(s, length);
    fstream.Close();
    
    return sz == length;
}

///
/// FileSystem
///

time_t FileSystem::GetLastModifyTime(const std::string &path)
{
    struct stat buf;
    if (lstat(path.c_str(), &buf) == -1) {
        return 0;
    }
    time_t modify_time = buf.st_mtime;
    return modify_time;
}


bool FileSystem::IsDir(const std::string &path)
{
    struct stat buf;
    if (lstat(path.c_str(), &buf) == -1) {
        return 0;
    }
    return S_ISDIR(buf.st_mode);
}

bool FileSystem::IsFile(const std::string &path)
{
    struct stat buf;
    if (lstat(path.c_str(), &buf) == -1) {
        return 0;
    }
    return S_ISREG(buf.st_mode);
}

bool FileSystem::IsLink(const std::string &path)
{
    struct stat buf;
    if (lstat(path.c_str(), &buf) == -1) {
        return 0;
    }
    return S_ISLNK(buf.st_mode);
}

bool FileSystem::IsPathExist(const std::string &path)
{
    return (access(path.c_str(), F_OK) == 0);
}

bool FileSystem::MakeFile(const std::string &path)
{
    bool ok = false;
    File stream;
    
    // file exist
    ok = FileSystem::IsPathExist(path);
    if (ok) {
        return false;
    }
    
    // create empty file
    ok = stream.Open(path, "wb");
    if (!ok) {
        return false;
    }
    ok = stream.Close();
    if (!ok) {
        return false;
    }
    
    return true;
}

bool FileSystem::MakeDirectory(const std::string &path)
{
    return (::mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0);
}

bool FileSystem::MakeDeepDirectory(const std::string &path)
{
    const std::vector<std::string> components = Path(path).Normalize().Split();
    
    char sep = Path::GetSeparator();
    std::string chdir_path(1, sep);
    for (auto com : components) {
        if (com.empty()) {
            continue;
        }
        chdir_path += com + sep;
        if (FileSystem::IsPathExist(chdir_path)) {
            continue;
        }
        if (!FileSystem::MakeDirectory(chdir_path)) {
            return false;
        }
    }
    return true;
}

bool FileSystem::Remove(const std::string &path)
{
    return (::remove(path.c_str()) == 0);
}

bool FileSystem::Rename(const std::string &path, const std::string &newname)
{
    std::string dir = Path(path).GetDirectory();
    return FileSystem::Move(path, dir+newname);
}

bool FileSystem::Move(const std::string &srcpath, const std::string &dstpath)
{
    return (::rename(srcpath.c_str(), dstpath.c_str()) == 0);
}

bool FileSystem::Copy(const std::string &srcpath, const std::string &dstpath)
{
    bool ok = true;
    
    File fsrc, fdst;
    ok &= fsrc.Open(srcpath, "rb");
    ok &= fdst.Open(dstpath, "wb");
    if (!ok) {
        return false;
    }
    
    // copy ...
    char buffer[TORCH_CP_BUFFERSIZE];
    while (!fsrc.IsEOF()) {
        // read --> write
        size_t count = fsrc.Read(buffer, sizeof(char), sizeof(buffer));
        fdst.Write(buffer, sizeof(char), count);
    }
    
    return true;
}


