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
#include "../src/xpack/xpack-util.h"
#include "test.h"

using namespace xpack;

static std::string LoadNextPackage(xpack::Package &pkg) {
    static int counter = 0;
    std::string homedir = torch::Path::GetHomeDir();
    std::string path = torch::String::Format("%s/.xpacktest/block%03d", homedir.c_str(), counter++);
    if (torch::FileSystem::IsPathExist(path)) {
        torch::FileSystem::Remove(path);
    }
    bool ok = pkg.OpenNew(path);
    if (!ok) {
        ErrorLog("%s: open new xpack(%s) failed\n", __FUNCTION__, path.c_str());
    }
    return path;
}

int TestBlockMain() {
    { // 测试最基础的申请block
        xpack::Package pack;
        std::string packpath = LoadNextPackage(pack);
        
        bool ok = true;
        std::string name = "hello";
        ok &= pack.AddEntry(name, torch::Data(name.c_str()));
        xpack::MetaHash *metahash = pack.GetContxt()->hash->QueryByName(name);
        xpack::MetaBlock *metablock = pack.GetContxt()->block->GetByIndex(metahash->block_index);
        TEST_TRUE(metahash->block_index == 0);
        TEST_TRUE(metablock->size == name.size());
        TEST_TRUE(metablock->flags == 0);
        TEST_TRUE(metablock->next_index == -1);
        TEST_TRUE(pack.GetContxt()->block->GetBlockNumber() == 1);
        TEST_TRUE(pack.GetContxt()->block->GetBlockReuser()->Size() == 0);
        TEST_TRUE(pack.GetContxt()->block->GetContentReuser()->Size() == 0);
    }
    
    
    {
        // 测试申请->删除->再次申请
        // 本次删除时，内部会直接删除掉不会进行缓存(符合删除条件)
        xpack::Package pack;
        std::string packpath = LoadNextPackage(pack);
        
        bool ok = true;
        std::string name = "hello";
        ok &= pack.AddEntry(name, torch::Data(name.c_str()));
        ok &= pack.RemoveEntry(name);
        TEST_TRUE(pack.GetContxt()->block->GetBlockNumber() == 0);
        TEST_TRUE(pack.GetContxt()->block->GetBlockReuser()->Size() == 0);
        TEST_TRUE(pack.GetContxt()->block->GetContentReuser()->Size() == 0);
        
        ok &= pack.AddEntry(name, torch::Data(name.c_str()));
        xpack::MetaHash *metahash = pack.GetContxt()->hash->QueryByName(name);
        xpack::MetaBlock *metablock = pack.GetContxt()->block->GetByIndex(metahash->block_index);
        TEST_TRUE(metahash->block_index == 0);
        TEST_TRUE(metablock->size == name.size());
        TEST_TRUE(metablock->flags == 0);
        TEST_TRUE(metablock->next_index == -1);
        TEST_TRUE(pack.GetContxt()->block->GetBlockNumber() == 1);
        TEST_TRUE(pack.GetContxt()->block->GetBlockReuser()->Size() == 0);
        TEST_TRUE(pack.GetContxt()->block->GetContentReuser()->Size() == 0);
    }
    
    {
        // 1.添加entry1,添加entry2,删除entry1,添加entry3，删除entry3，删除entry2
        // 2.添加entry1,添加entry2,添加entry3, 删除entry1,删除entry2，删除entry3
        // 测试unused-content重用
        xpack::Package pack;
        std::string packpath = LoadNextPackage(pack);
        
        bool ok = true;
        std::vector<std::string> names = {
            "first", "second", "third_longlonglonglonglong"
        };
        ok &= pack.AddEntry(names[0], torch::Data(names[0].c_str()));
        ok &= pack.AddEntry(names[1], torch::Data(names[1].c_str()));
        TEST_TRUE(pack.GetContxt()->block->GetBlockNumber() == 2);
        TEST_TRUE(pack.GetContxt()->block->GetBlockReuser()->Size() == 0);
        TEST_TRUE(pack.GetContxt()->block->GetContentReuser()->Size() == 0);
        
        ok &= pack.RemoveEntry(names[0]);
        TEST_TRUE(pack.GetContxt()->block->GetBlockNumber() == 2);
        TEST_TRUE(pack.GetContxt()->block->GetBlockReuser()->Size() == 0);
        TEST_TRUE(pack.GetContxt()->block->GetContentReuser()->Size() == 1);
        
        
        ok &= pack.AddEntry(names[2], torch::Data(names[2].c_str()));
        TEST_TRUE(pack.GetContxt()->block->GetBlockNumber() == 3);
        TEST_TRUE(pack.GetContxt()->block->GetBlockReuser()->Size() == 0);
        TEST_TRUE(pack.GetContxt()->block->GetContentReuser()->Size() == 0);
        
        
        xpack::MetaHash *metahash = pack.GetContxt()->hash->QueryByName(names[2]);
        xpack::MetaBlock *metablock = pack.GetContxt()->block->GetByIndex(metahash->block_index);
        auto bichain = xpack::Utils::GetBlockIndexChainByMetablock(pack.GetContxt(), metablock);
        TEST_TRUE(metahash->block_index == 0);
        TEST_TRUE(metablock->size < names[2].size());
        TEST_TRUE(metablock->flags == 0);
        TEST_TRUE(metablock->next_index == 2);
        TEST_TRUE(bichain.size() == 2);
        size_t size = 0;
        for (auto i :bichain) { size += pack.GetContxt()->block->GetByIndex(i)->size; }
        TEST_TRUE(size == names[2].size());
        
        ok &= pack.RemoveEntry(names[2]);
        TEST_TRUE(pack.GetContxt()->block->GetBlockNumber() == 2);
        TEST_TRUE(pack.GetContxt()->block->GetBlockReuser()->Size() == 0);
        TEST_TRUE(pack.GetContxt()->block->GetContentReuser()->Size() == 1);
        TEST_TRUE(pack.GetContxt()->block->GetContentReuser()->Contains(bichain[0]));
        
        ok &= pack.RemoveEntry(names[1]);
        TEST_TRUE(pack.GetContxt()->block->GetBlockNumber() == 0);
        TEST_TRUE(pack.GetContxt()->block->GetBlockReuser()->Size() == 0);
        TEST_TRUE(pack.GetContxt()->block->GetContentReuser()->Size() == 0);
        
        ok &= pack.AddEntry(names[0], torch::Data(names[0].c_str()));
        ok &= pack.AddEntry(names[1], torch::Data(names[1].c_str()));
        ok &= pack.AddEntry(names[2], torch::Data(names[2].c_str()));
        TEST_TRUE(pack.GetContxt()->block->GetBlockNumber() == 3);
        TEST_TRUE(pack.GetContxt()->block->GetBlockReuser()->Size() == 0);
        TEST_TRUE(pack.GetContxt()->block->GetContentReuser()->Size() == 0);

        ok &= pack.RemoveEntry(names[0]);
        TEST_TRUE(pack.GetContxt()->block->GetBlockNumber() == 3);
        TEST_TRUE(pack.GetContxt()->block->GetBlockReuser()->Size() == 0);
        TEST_TRUE(pack.GetContxt()->block->GetContentReuser()->Size() == 1);

        ok &= pack.RemoveEntry(names[1]);
        TEST_TRUE(pack.GetContxt()->block->GetBlockNumber() == 3);
        TEST_TRUE(pack.GetContxt()->block->GetBlockReuser()->Size() == 0);
        TEST_TRUE(pack.GetContxt()->block->GetContentReuser()->Size() == 2);

        ok &= pack.RemoveEntry(names[2]);
        TEST_TRUE(pack.GetContxt()->block->GetBlockNumber() == 0);
        TEST_TRUE(pack.GetContxt()->block->GetBlockReuser()->Size() == 0);
        TEST_TRUE(pack.GetContxt()->block->GetContentReuser()->Size() == 0);

    }
    
    
    {
        // 1.添加entry1,添加entry2,删除entry1,添加entry3，删除entry3，删除entry2
        // 2.添加entry1,添加entry2,添加entry3, 删除entry1,删除entry2，删除entry3
        // 测试unused-block重用
        xpack::Package pack;
        std::string packpath = LoadNextPackage(pack);
        
        bool ok = true;
        std::vector<std::string> names = {
            "first", "second", "third"
        };
        ok &= pack.AddEntry(names[0], torch::Data::Null);
        ok &= pack.AddEntry(names[1], torch::Data::Null);
        TEST_TRUE(pack.GetContxt()->block->GetBlockNumber() == 2);
        TEST_TRUE(pack.GetContxt()->block->GetBlockReuser()->Size() == 0);
        TEST_TRUE(pack.GetContxt()->block->GetContentReuser()->Size() == 0);
        
        ok &= pack.RemoveEntry(names[0]);
        TEST_TRUE(pack.GetContxt()->block->GetBlockNumber() == 2);
        TEST_TRUE(pack.GetContxt()->block->GetBlockReuser()->Size() == 1);
        TEST_TRUE(pack.GetContxt()->block->GetContentReuser()->Size() == 0);
        
        
        ok &= pack.AddEntry(names[2], torch::Data::Null);
        TEST_TRUE(pack.GetContxt()->block->GetBlockNumber() == 2);
        TEST_TRUE(pack.GetContxt()->block->GetBlockReuser()->Size() == 0);
        TEST_TRUE(pack.GetContxt()->block->GetContentReuser()->Size() == 0);
        
        xpack::MetaHash *metahash = pack.GetContxt()->hash->QueryByName(names[2]);
        xpack::MetaBlock *metablock = pack.GetContxt()->block->GetByIndex(metahash->block_index);
        TEST_TRUE(metahash->block_index == 0);
        TEST_TRUE(metablock->size == 0);
        TEST_TRUE(metablock->flags == 0);
        TEST_TRUE(metablock->next_index == -1);
        
        ok &= pack.RemoveEntry(names[2]);
        TEST_TRUE(pack.GetContxt()->block->GetBlockNumber() == 2);
        TEST_TRUE(pack.GetContxt()->block->GetBlockReuser()->Size() == 1);
        TEST_TRUE(pack.GetContxt()->block->GetContentReuser()->Size() == 0);
        
        ok &= pack.RemoveEntry(names[1]);
        TEST_TRUE(pack.GetContxt()->block->GetBlockNumber() == 0);
        TEST_TRUE(pack.GetContxt()->block->GetBlockReuser()->Size() == 0);
        TEST_TRUE(pack.GetContxt()->block->GetContentReuser()->Size() == 0);
        
        
        
        
        ok &= pack.AddEntry(names[0], torch::Data::Null);
        ok &= pack.AddEntry(names[1], torch::Data::Null);
        ok &= pack.AddEntry(names[2], torch::Data::Null);
        TEST_TRUE(pack.GetContxt()->block->GetBlockNumber() == 3);
        TEST_TRUE(pack.GetContxt()->block->GetBlockReuser()->Size() == 0);
        TEST_TRUE(pack.GetContxt()->block->GetContentReuser()->Size() == 0);
        
        ok &= pack.RemoveEntry(names[0]);
        TEST_TRUE(pack.GetContxt()->block->GetBlockNumber() == 3);
        TEST_TRUE(pack.GetContxt()->block->GetBlockReuser()->Size() == 1);
        TEST_TRUE(pack.GetContxt()->block->GetContentReuser()->Size() == 0);
        
        ok &= pack.RemoveEntry(names[1]);
        TEST_TRUE(pack.GetContxt()->block->GetBlockNumber() == 3);
        TEST_TRUE(pack.GetContxt()->block->GetBlockReuser()->Size() == 2);
        TEST_TRUE(pack.GetContxt()->block->GetContentReuser()->Size() == 0);
        
        ok &= pack.RemoveEntry(names[2]);
        TEST_TRUE(pack.GetContxt()->block->GetBlockNumber() == 0);
        TEST_TRUE(pack.GetContxt()->block->GetBlockReuser()->Size() == 0);
        TEST_TRUE(pack.GetContxt()->block->GetContentReuser()->Size() == 0);
    }
    
    {
        // 测试4个entry,包含两个block-chain的正确性，及其删除的正确性
        // 测试unused-content重用
        xpack::Package pack;
        std::string packpath = LoadNextPackage(pack);
        bool ok = true;

        struct Entry {
            std::string name;
            std::string content;
        };
        std::vector<Entry> entries = {
            {"first", "first"},
            {"second", "second"},
            {"third", "third_longlonglong"},
            {"fourth", "fourth_longlonglong"},
        };
        ok &= pack.AddEntry(entries[0].name, entries[0].content.c_str());
        ok &= pack.AddEntry(entries[1].name, entries[1].content.c_str());
        TEST_TRUE(pack.GetContxt()->block->GetBlockNumber() == 2);
        TEST_TRUE(pack.GetContxt()->block->GetBlockReuser()->Size() == 0);
        TEST_TRUE(pack.GetContxt()->block->GetContentReuser()->Size() == 0);

        ok &= pack.RemoveEntry(entries[0].name);
        TEST_TRUE(pack.GetContxt()->block->GetBlockNumber() == 2);
        TEST_TRUE(pack.GetContxt()->block->GetBlockReuser()->Size() == 0);
        TEST_TRUE(pack.GetContxt()->block->GetContentReuser()->Size() == 1);
        ok &= pack.AddEntry(entries[2].name, entries[2].content.c_str());
        TEST_TRUE(pack.GetContxt()->block->GetBlockNumber() == 3);
        TEST_TRUE(pack.GetContxt()->block->GetBlockReuser()->Size() == 0);
        TEST_TRUE(pack.GetContxt()->block->GetContentReuser()->Size() == 0);

        ok &= pack.RemoveEntry(entries[1].name);
        TEST_TRUE(pack.GetContxt()->block->GetBlockNumber() == 3);
        TEST_TRUE(pack.GetContxt()->block->GetBlockReuser()->Size() == 0);
        TEST_TRUE(pack.GetContxt()->block->GetContentReuser()->Size() == 1);
        ok &= pack.AddEntry(entries[3].name, entries[3].content.c_str());
        TEST_TRUE(pack.GetContxt()->block->GetBlockNumber() == 4);
        TEST_TRUE(pack.GetContxt()->block->GetBlockReuser()->Size() == 0);
        TEST_TRUE(pack.GetContxt()->block->GetContentReuser()->Size() == 0);

        ok &= pack.AddEntry(entries[0].name, entries[0].content.c_str());
        ok &= pack.AddEntry(entries[1].name, entries[1].content.c_str());
        TEST_TRUE(pack.GetContxt()->block->GetBlockNumber() == 6);
        TEST_TRUE(pack.GetContxt()->block->GetBlockReuser()->Size() == 0);
        TEST_TRUE(pack.GetContxt()->block->GetContentReuser()->Size() == 0);

        std::vector<std::vector<int32_t>> result = {
            {4}, //0
            {5}, //1
            {0,2},//2
            {1,3},//3
        };
        for (int i = 0; i < entries.size(); i++) {
            auto e = entries[i];
            auto metahash = pack.GetContxt()->hash->QueryByName(e.name);
            auto metablock = pack.GetContxt()->block->GetByIndex(metahash->block_index);
            TEST_TRUE(xpack::Utils::GetBlockIndexChainByMetablock(pack.GetContxt(), metablock) == result[i]);
        }
    }
    
    {
        // 测试block-offset和block-size计算的正确性
        xpack::Package pack;
        std::string packpath = LoadNextPackage(pack);
        bool ok = true;
        
        struct Entry {
            std::string name;
            std::string content;
        };
        std::vector<Entry> entries = {
            {"first", "first_longlonglong"},
            {"second", "second"},
            {"third", "third"}
        };
        ok &= pack.AddEntry(entries[0].name, entries[0].content.c_str());
        ok &= pack.AddEntry(entries[1].name, entries[1].content.c_str());
        TEST_TRUE(pack.GetContxt()->block->GetBlockNumber() == 2);
        TEST_TRUE(pack.GetContxt()->block->GetBlockReuser()->Size() == 0);
        TEST_TRUE(pack.GetContxt()->block->GetContentReuser()->Size() == 0);
        
        ok &= pack.RemoveEntry(entries[0].name);
        TEST_TRUE(pack.GetContxt()->block->GetBlockNumber() == 2);
        TEST_TRUE(pack.GetContxt()->block->GetBlockReuser()->Size() == 0);
        TEST_TRUE(pack.GetContxt()->block->GetContentReuser()->Size() == 1);
        ok &= pack.AddEntry(entries[2].name, entries[2].content.c_str());
        TEST_TRUE(pack.GetContxt()->block->GetBlockNumber() == 3);
        TEST_TRUE(pack.GetContxt()->block->GetBlockReuser()->Size() == 0);
        TEST_TRUE(pack.GetContxt()->block->GetContentReuser()->Size() == 1);
        int32_t bindex1 = pack.GetContxt()->block->GetContentReuser()->GetMinimumSizeEntry();
        int32_t bindex2 = pack.GetContxt()->hash->QueryByName(entries[2].name)->block_index;
        MetaBlock *block1 = pack.GetContxt()->block->GetByIndex(bindex1); // splitout block
        MetaBlock *block2 = pack.GetContxt()->block->GetByIndex(bindex2);
        TEST_TRUE(pack.GetContxt()->block->GetByIndex(bindex1)->size == 13);
        TEST_TRUE(block2->offset+block2->size == block1->offset);
    }

    InfoLog("> test-block ... ok\n");
    return 0;
}


