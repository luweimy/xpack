//
//  Torch
//
//  Created by Luwei.
//  Copyright (c) 2014-2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy/Torch
//

#include <assert.h>
#include <string.h>
#include "torch-compress-zip.h"
#include "../torch-path.h"
#include "../torch-string.h"


using namespace torch::compress;

// ZipUtil

size_t ZipUtil::GetCompressedMaxSize(size_t len)
{
    return compressBound(len);
}

bool ZipUtil::Compress(const char *src, size_t srclen, char *dst, size_t *dstlen, CompressLevel level)
{
    return compress2((unsigned char*)dst, (uLongf*)dstlen, (unsigned char*)src, srclen, (int)level) == Z_OK;
}

bool ZipUtil::Decompress(const char *src, size_t srclen, char *dst, size_t *dstlen)
{
    return uncompress((unsigned char*)dst, (uLongf*)dstlen, (unsigned char*)src, srclen) == Z_OK;
}

bool ZipUtil::Compress(const torch::Data &input, torch::Data &output, CompressLevel level)
{
    size_t bsize = ZipUtil::GetCompressedMaxSize(input.GetSize());
    output.ReSize(bsize);
    
    size_t rsize = bsize;
    bool ok = ZipUtil::Compress((char*)input.GetBytes(), input.GetSize(), (char*)output.GetBytes(), &rsize, level);
    output.ReSize(rsize);
    return ok;
}

bool ZipUtil::Decompress(const torch::Data &input, torch::Data &output)
{
    size_t rsize = output.GetSize();
    bool ok = ZipUtil::Decompress((char*)input.GetBytes(), input.GetSize(), (char*)output.GetBytes(), &rsize);
    output.ReSize(rsize);
    return ok;
}

// Zip

Zip::Zip()
:m_zipFile(nullptr)
,m_errcode(ZIP_OK)
{
}

Zip::Zip(const std::string &path, OpenMode append)
:m_zipFile(nullptr)
,m_errcode(ZIP_OK)
{
    this->Open(path, append);
}

Zip::~Zip()
{
    this->Close();
}

bool Zip::Open(const std::string &path, OpenMode append)
{
    m_zipFile = zipOpen(path.c_str(), (int)append);
    return (m_zipFile != nullptr);
}

bool Zip::Close()
{
    if (m_zipFile) {
        m_errcode = zipClose(m_zipFile, nullptr);
        m_zipFile = nullptr;
        return m_errcode == UNZ_OK;
    }
    return true;
}

bool Zip::IsOpen()
{
    return m_zipFile != nullptr;
}

int  Zip::GetError()
{
    return m_errcode;
}

bool Zip::HasError()
{
    return m_errcode != ZIP_OK;
}

bool Zip::SetContent(const std::string &filename, const Data &content)
{
    bool ok = true;
    do {
        ok = this->OpenEntry(filename);
        if (!ok) break;
        ok = this->WriteEntry(content.GetBytes(), content.GetSize());
        if (!ok) break;
        ok = this->CloseEntry();
        if (!ok) break;
    } while (false);
    return ok;
}

bool Zip::OpenEntry(const std::string &filename)
{
    zip_fileinfo zinfo;
    memset (&zinfo, 0, sizeof(zinfo));
    m_errcode = zipOpenNewFileInZip(m_zipFile, filename.c_str(), &zinfo, nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED, Z_DEFAULT_COMPRESSION);
    return m_errcode == ZIP_OK;
}

bool Zip::WriteEntry(void *buffer, size_t length)
{
    m_errcode = zipWriteInFileInZip(m_zipFile, buffer, (unsigned)length);
    return m_errcode == ZIP_OK;
}

bool Zip::CloseEntry()
{
    m_errcode = zipCloseFileInZip(m_zipFile);
    return m_errcode == ZIP_OK;
}

// Unzip

inline int locatefuzzycomparer_ignorecase(unzFile file, const char *filename1, const char *filename2)
{
    return strcasecmp(filename1, filename2);
}

inline int locatefuzzycomparer_contain(unzFile file, const char *filename1, const char *filename2)
{
    std::string name1(filename1);
    if (std::string::npos != name1.find(filename2)){
        return 0;
    }
    return -1;
}

Unzip::Unzip()
:m_unzFile(nullptr)
,m_errcode(UNZ_OK)
{
}

Unzip::Unzip(const std::string &path)
:m_unzFile(nullptr)
,m_errcode(UNZ_OK)
{
    this->Open(path);
}

Unzip::~Unzip()
{
    this->Close();
}

bool Unzip::Open(const std::string &path)
{
    m_unzFile = unzOpen(path.c_str());
    return (m_unzFile != nullptr);
}

bool Unzip::Close()
{
    if (m_unzFile) {
        m_errcode = unzClose(m_unzFile);
        m_unzFile = nullptr;
        return m_errcode == UNZ_OK;
    }
    return true;
}

bool Unzip::IsOpen()
{
    return m_unzFile != nullptr;
}

int Unzip::GetError()
{
    return m_errcode;
}

bool Unzip::HasError()
{
    return m_errcode != UNZ_OK;
}

int Unzip::GetEntryCount()
{
    unz_global_info pglobal_info;
    m_errcode = unzGetGlobalInfo(m_unzFile, &pglobal_info);
    if (m_errcode != UNZ_OK) {
        return -1;
    }
    return (int)pglobal_info.number_entry;
}

bool Unzip::SetFirst()
{
    m_errcode = unzGoToFirstFile(m_unzFile);
    return m_errcode == UNZ_OK;
}

bool Unzip::Next()
{
    m_errcode = unzGoToNextFile(m_unzFile);
    return m_errcode == UNZ_OK;
}

bool Unzip::Locate(const std::string& name)
{
    m_errcode = unzLocateFile(m_unzFile, name.c_str(), nullptr);
    return m_errcode == UNZ_OK;
}

bool Unzip::Locate(const std::string& name, Comparer comp)
{
    m_errcode = unzLocateFile(m_unzFile, name.c_str(), comp);
    return m_errcode == UNZ_OK;
}

bool Unzip::LocateFuzzyIgnoreCase(const std::string& name)
{
    m_errcode = unzLocateFile(m_unzFile, name.c_str(), locatefuzzycomparer_ignorecase);
    return m_errcode == UNZ_OK;
}

bool Unzip::LocateFuzzyContain(const std::string& name)
{
    m_errcode = unzLocateFile(m_unzFile, name.c_str(), locatefuzzycomparer_contain);
    return m_errcode == UNZ_OK;
}

bool Unzip::IsCurrentDirectory()
{
    std::string name = this->GetCurrentName();
    return name.back() == Path::GetSeparator();
}

bool Unzip::IsCurrentFile()
{
    return not this->IsCurrentDirectory();
}

std::string Unzip::GetCurrentName()
{
    static char filename[s_maxFilenameSize];
    memset(filename, 0, sizeof(filename));

    unz_file_info info;
    m_errcode = unzGetCurrentFileInfo(m_unzFile, &info, filename, s_maxFilenameSize, nullptr, 0, nullptr, 0);
    assert(info.size_filename <= s_maxFilenameSize);
    if (m_errcode == UNZ_OK) {
        return std::string(filename);
    }
    return std::string();
}

size_t Unzip::GetCurrentCompressedSize()
{
    unz_file_info info;
    m_errcode = unzGetCurrentFileInfo(m_unzFile, &info, nullptr, 0, nullptr, 0, nullptr, 0);
    if (m_errcode == UNZ_OK) {
        return info.compressed_size;
    }
    return 0;
}

size_t Unzip::GetCurrentUncompressedSize()
{
    unz_file_info info;
    m_errcode = unzGetCurrentFileInfo(m_unzFile, &info, nullptr, 0, nullptr, 0, nullptr, 0);
    if (m_errcode == UNZ_OK) {
        return info.uncompressed_size;
    }
    return 0;
}

torch::Data Unzip::GetCurrnetContent()
{
    size_t uncompressSize = this->GetCurrentUncompressedSize();
    torch::Data d(uncompressSize);
    bool ok = true;
    do {
        ok = this->OpenCurrent();
        if (!ok) break;

        int size = this->ReadCurrent(d.GetBytes(), uncompressSize);
        ok = (size == (int)uncompressSize);
        if (!ok) break;

        ok = this->CloseCurrent();
        if (!ok) break;
    } while (false);

    if (!ok) {
        return torch::Data::Null;
    }
    return std::move(d);
}

bool Unzip::OpenCurrent()
{
    m_errcode = unzOpenCurrentFile(m_unzFile);
    return m_errcode == UNZ_OK;
}

bool Unzip::OpenCurrentRaw(int *outmethod, int *outlevel)
{
    m_errcode = unzOpenCurrentFile2(m_unzFile, outmethod, outlevel, 1);
    return m_errcode == UNZ_OK;
}

bool Unzip::OpenCurrent(const std::string& password)
{
    m_errcode = unzOpenCurrentFilePassword(m_unzFile, password.c_str());
    return m_errcode == UNZ_OK;
}

bool Unzip::CloseCurrent()
{
    m_errcode = unzCloseCurrentFile(m_unzFile);
    return m_errcode == UNZ_OK;
}

int Unzip::ReadCurrent(void *buffer, size_t length)
{
    return unzReadCurrentFile(m_unzFile, buffer, (unsigned)length);
}

torch::Data Unzip::GetContent(const std::string &name)
{
    this->Locate(name);
    return std::move(this->GetCurrnetContent());
}

std::vector<std::string> Unzip::GetFilenameList()
{
    std::vector<std::string> vec;
    this->SetFirst();
    do {
        std::string name = this->GetCurrentName();
        bool isfile = this->IsCurrentFile();
        if (isfile) {
            vec.push_back(name);
        }
    } while (this->Next());
    return vec;
}

std::vector<std::string> Unzip::GetDirList()
{
    std::vector<std::string> vec;
    this->SetFirst();
    do {
        std::string name = this->GetCurrentName();
        bool isdir = this->IsCurrentDirectory();
        if (isdir) {
            vec.push_back(name);
        }
    } while (this->Next());
    return vec;
}

std::vector<std::string> Unzip::GetNameList()
{
    std::vector<std::string> vec;
    this->SetFirst();
    do {
        std::string name = this->GetCurrentName();
        vec.push_back(name);
    } while (this->Next());
    return vec;
}
