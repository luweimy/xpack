//
//  xpack
//
//  Created by Luwei.
//  Copyright (c) 2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy
//

#include "xpack.h"
#include "xpack-stream.h"
#include "xpack-name.h"
#include "xpack-header.h"
#include "xpack-hash.h"
#include "xpack-block.h"
#include "xpack-signature.h"
#include "xpack-content.h"
#include "xpack-util.h"
#include "torch/torch.h"

using namespace xpack;

// Package

Package::Package()
:m_stream(nullptr)
,m_context(nullptr)
,m_rc4crypto(nullptr)
,m_modify(false)
,m_needcrc(true)
,m_needshrink(true)
{
    m_rc4crypto =  new torch::crypto::RC4();
    
    uint32_t skey[] = {
        0x43415058, 0x72073096, 0x0EDB97D2, 0x09ADB8A4, 0xE0D5B8A4, 0x97D2EDB9
    };
    this->SetSecretKey((unsigned char*)skey, sizeof(skey));
}

Package::~Package()
{
    this->Close();
    
    if (m_rc4crypto) {
        delete m_rc4crypto;
    }
}

bool Package::Open(const std::string &path, bool readonly)
{
    return this->OpenWithStream(new FileStream, path, readonly);
}

bool Package::OpenNew(const std::string &path)
{
    this->Close();
    
    if (!torch::FileSystem::IsPathExist(path)) {
        torch::FileSystem::MakeFile(path);
    }

    m_stream = new xpack::FileStream;
    if (!m_stream || !m_stream->Open(path, false /* r+w */)) {
        return false;
    }

    m_context = new xpack::Context(m_stream);
    if (!m_context) {
        return false;
    }
    
    m_context->SetAlignedOffset((uint32_t)m_stream->Size());
    
    if (!m_context->signature->Initialize()->WriteToStream()) {
        return false;
    }
    if (!m_context->header->Initialize()->WriteToStream()) {
        return false;
    }
    
    m_modify = true;
    return true;
}

bool Package::OpenWithStream(Stream *stm, const std::string &path, bool readonly)
{
    this->Close();
    
    m_stream = stm;
    if (!m_stream) {
        return false;
    }
    if (!m_stream->Open(path, readonly)) {
        return false;
    }
    
    m_context = new xpack::Context(m_stream);
    if (!m_context) {
        return false;
    }
    
    if (!m_context->signature->SearchFromStream() && !m_context->signature->IsValid()) {// search signature
        return false;
    }
    
    if (!m_context->header->ReadFromStream() && !m_context->header->IsValid()) {
        return false;
    }
    
    if (!m_context->block->ReadFromStream()) {
        return false;
    }
    
    if (!m_context->hash->ReadFromStream()) {
        return false;
    }
    
    if (!m_context->name->ReadFromStream()) {
        return false;
    }
    
    return true;
}

void Package::Close()
{
    if (m_modify && m_stream && m_context) {
        this->Flush();
    }
    if (m_stream) {
        delete m_stream;
    }
    if (m_context) {
        delete m_context;
    }
    
    m_context = nullptr;
    m_stream = nullptr;
}

bool Package::IsValid()
{
    assert(m_context);
    return m_context->signature->IsValid() && m_context->header->IsValid();
}

void Package::SetNeedCrcVerify(bool need)
{
    m_needcrc = need;
}

void Package::SetMetadataSecretKey(const unsigned char *skey, const int length)
{
    m_context->crypto->SetSecretKey(skey, length);
}

void Package::SetSecretKey(const unsigned char *skey, const int length)
{
    m_rc4crypto->SetSecretKey(skey, length);
}

bool Package::AddEntry(const std::string &name, const torch::Data &data, bool crypto, bool compress)
{
    // get hash -> get block -> write content -> write name
    assert(m_context);
    
    MetaHash *metahash = m_context->hash->AddNew(name);
    if (!metahash) {
        return false; // Duplicate name
    }
    
    const torch::Data *processeddata = this->ProcessingBeforeWriting(metahash, data, crypto, compress);
    if (!processeddata) {
        m_context->hash->RemoveByName(name);
        return false;
    }
    
    int32_t bindex = m_context->block->AllocLinkedBlock((uint32_t)(*processeddata).GetSize());
    assert(bindex >= 0);
    
    // Must before overall-write
    metahash->block_index = bindex; 
    metahash->unpacked_size = (uint32_t)data.GetSize();

    if (!m_context->content->OverallWrite(metahash, *processeddata)) {
        m_context->hash->RemoveByName(name);
        m_context->block->RemoveByIndex(bindex);
        return false;
    }
    
    m_context->name->AddName(name, metahash);
    m_context->header->UpdateMetadata();
    m_modify = true;

    return true;
}

bool Package::RemoveEntry(const std::string &name)
{
    // remove block -> remove hash -> remove name
    assert(m_context);
    
    MetaHash *metahash = m_context->hash->QueryByName(name);
    if (!metahash) {
        return false; // Not exists
    }
    
    m_context->block->RemoveByIndex(metahash->block_index);
    m_context->name->RemoveNameSafely(metahash);
    m_context->hash->RemoveByName(name);
    m_context->header->UpdateMetadata();
    m_modify = true;
    return true;
}

bool Package::Flush()
{
    assert(m_context);
    m_context->header->UpdateMetadata();
    
    if (!m_context->header->WriteToStream()) {
        return false;
    }
    if (!m_context->block->WriteToStream()) {
        return false;
    }
    if (!m_context->hash->WriteToStream()) {
        return false;
    }
    if (!m_context->name->WriteToStream()) {
        return false;
    }
    
    if (m_needshrink) {
        // Ftruncate
        m_context->stream->ReSize(m_context->offset + m_context->header->Metadata()->archive_size);
    }
    
    m_modify = false;
    return true;
}

uint32_t Package::GetEntrySizeByName(const std::string &name)
{
    assert(m_context);

    MetaHash *metahash = m_context->hash->QueryByName(name);
    if (!metahash || metahash->block_index<0) {
        return 0;
    }

    MetaBlock *metablock = m_context->block->GetByIndex(metahash->block_index);
    size_t size = 0;
    while (metablock) {
        size += metablock->size;
        metablock = m_context->block->GetByIndex(metablock->next_index);
    }
    return (uint32_t)size;
}

uint32_t Package::GetUnpackedEntrySizeByName(const std::string &name)
{
    assert(m_context);
    
    MetaHash *metahash = m_context->hash->QueryByName(name);
    if (!metahash || metahash->block_index<0) {
        return 0;
    }

    return metahash->unpacked_size;
}

std::string Package::GetEntryStringByName(const std::string &name)
{
    assert(m_context);
    torch::Data buf;
    MetaHash *metahash = m_context->hash->QueryByName(name);
    if (!metahash || metahash->block_index<0) {
        return std::string();
    }
    if (!m_context->content->OverallRead(metahash, buf)) {
        return std::string();
    }
    if (!this->ProcessingAfterReading(metahash, buf)) {
        return std::string();
    }
    return std::move(buf.ToString());
}

torch::Data Package::GetEntryDataByName(const std::string &name)
{
    assert(m_context);
    torch::Data rdata;
    MetaHash *metahash = m_context->hash->QueryByName(name);
    if (!metahash || metahash->block_index<0) {
        return torch::Data::Null;
    }
    if (!m_context->content->OverallRead(metahash, rdata)) {
        return torch::Data::Null;
    }
    if (!this->ProcessingAfterReading(metahash, rdata)) {
        return torch::Data::Null;
    }
    return std::move(rdata);
}

bool Package::GetEntryDataByName(const std::string &name, torch::Data &outdata)
{
    assert(m_context);
    MetaHash *metahash = m_context->hash->QueryByName(name);
    if (!metahash || metahash->block_index<0) {
        return false;
    }
    if (!m_context->content->OverallRead(metahash, outdata)) {
        return false;
    }
    return this->ProcessingAfterReading(metahash, outdata);
}

bool Package::IsEntryExist(const std::string &name)
{
    assert(m_context);
    return m_context->hash->IsHashExist(name);
}

bool Package::ForeachEntryNames(std::function<bool(const std::string &name)> callback)
{
    assert(m_context);
    HashSegment::MetaHashMap *hashmap = m_context->hash->GetMetaHashMap();
    for (auto x : hashmap->GetIterator()) {
        MetaHash *metahash = x.second;
        assert(!(metahash->flags & int(HashFlags::Unused)));
        if (metahash->flags & int(HashFlags::Conflict)) {
            continue;
        }
        if (!callback(m_context->name->GetName(metahash))) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> Package::GetEntryNames()
{
    assert(m_context);
    std::vector<std::string> names;
    HashSegment::MetaHashMap *hashmap = m_context->hash->GetMetaHashMap();
    for (auto x : hashmap->GetIterator()) {
        MetaHash *metahash = x.second;
        assert(!(metahash->flags & int(HashFlags::Unused)));
        if (metahash->flags & int(HashFlags::Conflict)) {
            continue;
        }
        names.push_back(m_context->name->GetName(metahash));
    }
    return names;
}

size_t Package::GetPackageSize()
{
    assert(m_context);
    return m_context->header->Metadata()->archive_size;
}

Context* Package::GetContxt()
{
    return m_context;
}

Stream* Package::GetStream()
{
    return m_stream;
}

std::string Package::GetVersion()
{
    return torch::String::Format("%d.%d", (xpack::VERSION >> 8) & 0x00ff, xpack::VERSION & 0x00ff);
}

const torch::Data* Package::ProcessingBeforeWriting(MetaHash *metahash, const torch::Data &data, bool crypto, bool compress)
{
    // Crc32 check at first
    if (m_needcrc) {
        metahash->crc = torch::crypto::Crc32::Compute(data); 
    }
    
    const torch::Data *dataptr = &data;
    
    if (compress) {
        metahash->flags |= (int)HashFlags::Compressed;
        if (!torch::compress::ZipUtil::Compress(*dataptr, m_compressbuffer)) {
            XPACK_ERROR(xpack::Error::Compress); return nullptr;
        }
        dataptr = &m_compressbuffer;
    }
    
    if (crypto) {
        metahash->flags |= (int)HashFlags::CryptoRC4;
        m_rc4crypto->CryptoCopy(*dataptr, m_cryptobuffer);
        dataptr = &m_cryptobuffer;
    }
    return dataptr;
}

bool Package::ProcessingAfterReading(MetaHash *metahash, torch::Data &data)
{
    if (metahash->flags & (int)HashFlags::CryptoRC4) {
        m_rc4crypto->CryptoNoCopy(data);
    }

    if (metahash->flags & (int)HashFlags::Compressed) {
        m_compressbuffer.ReSize(metahash->unpacked_size);
        if (!torch::compress::ZipUtil::Decompress(data, m_compressbuffer)) {
            XPACK_ERROR(xpack::Error::Compress); return false;
        }
        data.CopyFrom(m_compressbuffer);
    }
    
    // Crc32 check at last
    if (m_needcrc) {
        if (metahash->crc != torch::crypto::Crc32::Compute(data)) { 
            XPACK_ERROR(xpack::Error::CRC);
            return false;
        }
    }
    return true;
}

// PackageHelper

bool PackageHelper::MakeNew(const std::string &path)
{
    return xpack::Package().OpenNew(path);
}

bool PackageHelper::AddTo(const std::string &package, const std::string &path, const std::string &name, bool force)
{
    Package pack;
    if (!pack.Open(package, false)) {
        return false;
    }
    return PackageHelper::AddTo(pack, path, name, force);
}

bool PackageHelper::AddTo(Package &package, const std::string &path, const std::string &name, bool force)
{
    std::string key = name;
    if (key.length() <= 0) {
        key = path;
    }
    if (force) {
        package.RemoveEntry(key);
    }
    return package.AddEntry(key, torch::File::GetBytes(path));
}

bool PackageHelper::ExtractTo(const std::string &package, const std::string &pathto, bool force, StatusCallback callback)
{
    Package pack;
    if (!pack.Open(package)) {
        return false;
    }
    return PackageHelper::ExtractTo(pack, pathto, force, callback);
}

bool PackageHelper::ExtractTo(Package &package, const std::string &pathto, bool force, StatusCallback callback)
{
    bool ok = true;
    torch::FileSystem::MakeDeepDirectory(pathto);

    package.ForeachEntryNames([&](const std::string &name)->bool {
        torch::File f;
        torch::Data d;
        torch::Path p = torch::Path(pathto).Append(name);
        ok = true;

        if (force) {
            torch::FileSystem::MakeDeepDirectory(p.GetDirectory());
        }
        else if (torch::FileSystem::IsPathExist(p.GetPath())) {
            ok = false; XPACK_ERROR(xpack::Error::AlreadyExists);
        }

        ok = ok ? f.Open(p.GetPath(), "wb") : ok;
        ok = ok ? package.GetEntryDataByName(name, d) : ok;
        ok = ok ? f.Write(d) == d.GetSize() : ok;
        if (callback) {
            if (!callback(name, ok)) {
                return false;
            }
        }
        return ok;
    });

    return ok;
}

bool PackageHelper::Merge(const std::string &main, const std::string &other, bool force, StatusCallback callback)
{
    Package mainpack, otherpack;
    if (!mainpack.Open(main, false))  {
        return false;
    }
    if (!otherpack.Open(other)) {
        return false;
    }
    return PackageHelper::Merge(mainpack, otherpack, force, callback);
}

bool PackageHelper::Merge(Package &main, Package &other, bool force, StatusCallback callback)
{
    bool ok = true;
    torch::Data buffer;
    other.ForeachEntryNames([&](const std::string &name){
        if (force && main.IsEntryExist(name)) {
            main.RemoveEntry(name);
        }
        other.GetEntryDataByName(name, buffer.Free());
        ok = main.AddEntry(name, buffer);
        if (callback) {
            if (!callback(name, ok)) {
                return false;
            }
        }
        return ok;
    });
    return ok;
}


