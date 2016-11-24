//
//  xpack
//
//  Created by Luwei.
//  Copyright (c) 2016年 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy
//

#include <iostream>
#include <unordered_map>
#include <vector>
#include "../src/xpack/torch/torch.h"
#include "../src/xpack/xpack-context.h"
#include "../src/xpack/xpack-stream.h"
#include "../src/xpack/xpack-signature.h"
#include "../src/xpack/xpack-header.h"
#include "../src/xpack/xpack-hash.h"
#include "../src/xpack/xpack-block.h"
#include "../src/xpack/xpack-name.h"
#include "../src/xpack/xpack-def.h"
#include "../src/xpack/xpack-base.h"
#include "test.h"

using namespace xpack;

static std::string LoadNextPackage(xpack::Package &pkg) {
	static int counter = 0;
	std::string homedir = torch::Path::GetHomeDir();
    std::string path = torch::String::Format("%s/.xpacktest/hash%03d", homedir.c_str(), counter++);
    if (torch::FileSystem::IsPathExist(path)) {
        torch::FileSystem::Remove(path);
    }
    bool ok = pkg.OpenNew(path);
    if (!ok) {
    	ErrorLog("%s: open new xpack(%s) failed\n", __FUNCTION__, path.c_str());
    }
    return path;
}

void TestHash_ReadAndWrite() {
    xpack::Package pack;
    std::string packpath = LoadNextPackage(pack);
    
    std::vector<std::string> nameArray = {
        "name1", "name2", "name3", "name4", "name5"
    };
    
    bool ok = true;
    for (auto name : nameArray) {
        ok &= pack.AddEntry(name, torch::Data::Null);
    }
    
    {
        auto hashmap = pack.GetContxt()->hash->GetMetaHashMap();
        TEST_TRUE(hashmap->Size() == nameArray.size());
    }
    
    pack.Close();
    if (!ok) {
        ErrorLog("%s: add entry failed\n", __FUNCTION__);
    }
    
    ok = pack.Open(packpath);
    if (!ok) {
        ErrorLog("%s: open xpack failed\n", __FUNCTION__);
    }

    {
        auto hashmap = pack.GetContxt()->hash->GetMetaHashMap();
        TEST_TRUE(hashmap->Size() == nameArray.size());
        
        for (auto x : pack.GetContxt()->hash->GetMetaHashMap()->GetIterator()) {
            MetaHash *metahash = x.second;
            std::string name = pack.GetContxt()->name->GetName(metahash);
            TEST_TRUE(IsVectorContainString(nameArray, name));
        }
    }
}

uint32_t TestHashString(const std::string &s, uint8_t seed) {
    return torch::Hash::x86::Murmur32(s.c_str(), s.length(), seed) % 20;
}

void DumpConflictNamesWithHashId() {
    torch::collection::HashMap<uint32_t, std::vector<std::string>*> map;
    for (int i = 0; i < 100; i++) {
        std::string name = "name" + torch::String::ToCppString(i);
        uint32_t hashid = TestHashString(name, 0);
        if (!map.HasKey(hashid)) {
            std::vector<std::string> *array = new std::vector<std::string>;
            map.Set(hashid, array);
        }
        std::vector<std::string> *array = map.Get(hashid);
        array->push_back(name);
    }

    for (auto it : map.GetIterator()) {
        std::vector<std::string> *array = it.second;
        std::string line = torch::String::ToCppString(*array);
        InfoLog("%03d -> %s\n", it.first, line.c_str());
        delete array;
    }
}

void TestHash_AddNew() {
    // 测试：
    // 1.添加多个文件时其不同名文件hashid发生冲突的情况
    // 2.发生冲突时，通过文件名查找对应的metahash的正确性
    // 3.发生冲突时，其文件内容的读取的正确性
    // 4.发生冲突时，判断文件是否存在的正确性

    
    { // 3个同时冲突,有一个重名
        xpack::Package pack;
        std::string packpath = LoadNextPackage(pack);
        pack.GetContxt()->HashMaker = TestHashString;

        std::vector<std::string> nameArray = {
            "name53", "name70", "name87", "name87" // hashid -> 10
        };
        
        bool ok = true;
        for (auto name : nameArray) {
            ok &= pack.AddEntry(name, torch::Data(name.c_str()));
            TEST_TRUE(pack.IsEntryExist(name));
        }
        
        // InfoLog("%s\n", pack.GetContxt()->hash->DumpHash().c_str());
        auto hashmap = pack.GetContxt()->hash->GetMetaHashMap();
        TEST_TRUE(hashmap->Size() == 4);
        
        for (auto x : pack.GetContxt()->hash->GetMetaHashMap()->GetIterator()) {
            MetaHash *metahash = x.second;
            if (metahash->flags & (int)HashFlags::Conflict) {
                TEST_TRUE(metahash->hash == 10);
                TEST_TRUE(metahash->conflict_refc == 3);
            }
            else {
                std::string name = pack.GetContxt()->name->GetName(metahash);
                TEST_TRUE(metahash == pack.GetContxt()->hash->QueryByName(name));
            }
        }
        
        for (auto name : nameArray) {
            std::string content = pack.GetEntryStringByName(name);
            TEST_TRUE(content == name);
        }
    }

    
    { // 3个同时冲突
        xpack::Package pack;
        std::string packpath = LoadNextPackage(pack);
        pack.GetContxt()->HashMaker = TestHashString;

        std::vector<std::string> nameArray = {
            "name53", "name70", "name87" // hashid -> 10
        };
        
        bool ok = true;
        for (auto name : nameArray) {
            ok &= pack.AddEntry(name, torch::Data(name.c_str()));
            TEST_TRUE(pack.IsEntryExist(name));
        }
        
        // InfoLog("%s\n", pack.GetContxt()->hash->DumpHash().c_str());
        auto hashmap = pack.GetContxt()->hash->GetMetaHashMap();
        TEST_TRUE(hashmap->Size() == nameArray.size() + 1);
        
        for (auto x : pack.GetContxt()->hash->GetMetaHashMap()->GetIterator()) {
            MetaHash *metahash = x.second;
            if (metahash->flags & (int)HashFlags::Conflict) {
                TEST_TRUE(metahash->hash == 10);
                TEST_TRUE(metahash->conflict_refc == 3);
            }
            else {
                std::string name = pack.GetContxt()->name->GetName(metahash);
                TEST_TRUE(metahash == pack.GetContxt()->hash->QueryByName(name));
            }

        }
        
        for (auto name : nameArray) {
            std::string content = pack.GetEntryStringByName(name);
            TEST_TRUE(content == name);
        }
    }
    
    { // 2个冲突，其中一个跳转后同另一个冲突
        xpack::Package pack;
        std::string packpath = LoadNextPackage(pack);
        pack.GetContxt()->HashMaker = TestHashString;

        std::vector<std::string> nameArray = {
            "name34", "name69",  // hashid -> 9, name34:hashid=7(seed=1)
            "name99" // 7
        };
        
        bool ok = true;
        for (auto name : nameArray) {
            ok &= pack.AddEntry(name, torch::Data(name.c_str()));
            TEST_TRUE(pack.IsEntryExist(name));
        }
        
        // InfoLog("%s\n", pack.GetContxt()->hash->DumpHash().c_str());
        auto hashmap = pack.GetContxt()->hash->GetMetaHashMap();
        TEST_TRUE(hashmap->Size() == nameArray.size() + 2);
        
        for (auto x : pack.GetContxt()->hash->GetMetaHashMap()->GetIterator()) {
            MetaHash *metahash = x.second;
            if (metahash->flags & (int)HashFlags::Conflict) {
                TEST_TRUE(metahash->hash == 9 || metahash->hash == 7);
            }
            else {
                std::string name = pack.GetContxt()->name->GetName(metahash);
                TEST_TRUE(metahash == pack.GetContxt()->hash->QueryByName(name));
            }
        }
        
        for (auto name : nameArray) {
            std::string content = pack.GetEntryStringByName(name);
            TEST_TRUE(content == name);
        }
    }

}


void TestHash_RemoveByName() {
    // 测试：
    // 1.添加多个文件hashid冲突时的删除文件的正确性
    // 2.删除文件时，对于中转metahash的删除的正确性
    // 3.删除过程中，其他共用中转metahash的读取的正确性
    // 4.发生冲突时，判断文件是否存在的正确性

    { // 3个同时冲突情况下删除
        xpack::Package pack;
        std::string packpath = LoadNextPackage(pack);
        pack.GetContxt()->HashMaker = TestHashString;
        
        std::vector<std::string> nameArray = {
            "name53", "name70", "name87" // hashid -> 10
        };
        
        bool ok = true;
        for (auto name : nameArray) {
            ok &= pack.AddEntry(name, torch::Data(name.c_str()));
            TEST_TRUE(pack.IsEntryExist(name));
        }
        auto hashmap = pack.GetContxt()->hash->GetMetaHashMap();
        TEST_TRUE(hashmap->Size() == 4);
        TEST_TRUE(pack.GetContxt()->hash->GetById(10)->conflict_refc == 3);

        pack.RemoveEntry(nameArray[0]);
        TEST_TRUE(!pack.IsEntryExist(nameArray[0]));
        TEST_TRUE(hashmap->Size() == 3);
        TEST_TRUE(pack.GetContxt()->hash->GetById(10)->conflict_refc == 2);
        TEST_TRUE(pack.GetEntryStringByName(nameArray[1]) == nameArray[1]);
        TEST_TRUE(pack.GetEntryStringByName(nameArray[2]) == nameArray[2]);

        pack.RemoveEntry(nameArray[1]);
        TEST_TRUE(!pack.IsEntryExist(nameArray[1]));
        TEST_TRUE(hashmap->Size() == 2);
        TEST_TRUE(pack.GetContxt()->hash->GetById(10)->conflict_refc == 1);
        TEST_TRUE(pack.GetEntryStringByName(nameArray[2]) == nameArray[2]);
        
        pack.RemoveEntry(nameArray[2]);
        TEST_TRUE(!pack.IsEntryExist(nameArray[2]));
        TEST_TRUE(hashmap->Size() == 0);
        TEST_TRUE(pack.GetContxt()->hash->GetById(10) == nullptr);
        // InfoLog("%s\n", pack.GetContxt()->hash->DumpHash().c_str());

    }
    
    { // 2个冲突，其中一个跳转后同另一个冲突
        xpack::Package pack;
        std::string packpath = LoadNextPackage(pack);
        pack.GetContxt()->HashMaker = TestHashString;
        
        std::vector<std::string> nameArray = {
            "name69", "name34", // hashid -> 9, name34:hashid=7(seed=1)
            "name99" // 7
        };
        
        bool ok = true;
        for (auto name : nameArray) {
            ok &= pack.AddEntry(name, torch::Data(name.c_str()));
        }

        auto hashmap = pack.GetContxt()->hash->GetMetaHashMap();
        TEST_TRUE(hashmap->Size() == 5);
        TEST_TRUE(pack.GetContxt()->hash->GetById(9)->conflict_refc == 2);
        TEST_TRUE(pack.GetContxt()->hash->GetById(7)->conflict_refc == 2);

        pack.RemoveEntry(nameArray[0]);
        TEST_TRUE(!pack.IsEntryExist(nameArray[0]));
        TEST_TRUE(hashmap->Size() == 4);
        TEST_TRUE(pack.GetContxt()->hash->GetById(9)->conflict_refc == 1);
        TEST_TRUE(pack.GetContxt()->hash->GetById(7)->conflict_refc == 2);
        TEST_TRUE(pack.GetEntryStringByName(nameArray[1]) == nameArray[1]);
        TEST_TRUE(pack.GetEntryStringByName(nameArray[2]) == nameArray[2]);
        
        pack.RemoveEntry(nameArray[1]);
        TEST_TRUE(!pack.IsEntryExist(nameArray[1]));
        TEST_TRUE(hashmap->Size() == 2);
        TEST_TRUE(pack.GetContxt()->hash->GetById(9) == nullptr);
        TEST_TRUE(pack.GetContxt()->hash->GetById(7)->conflict_refc == 1);
        TEST_TRUE(pack.GetEntryStringByName(nameArray[2]) == nameArray[2]);

        pack.RemoveEntry(nameArray[2]);
        TEST_TRUE(!pack.IsEntryExist(nameArray[2]));
        TEST_TRUE(hashmap->Size() == 0);
        TEST_TRUE(pack.GetContxt()->hash->GetById(9) == nullptr);
        TEST_TRUE(pack.GetContxt()->hash->GetById(7) == nullptr);
        // InfoLog("%s\n", pack.GetContxt()->hash->DumpHash().c_str());
    }
    
}

int TestHashMain() {
    TestHash_ReadAndWrite();
    TestHash_AddNew();
    TestHash_RemoveByName();
    InfoLog("> test-hash ... ok\n");
    return 0;
}

