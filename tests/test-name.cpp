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
    std::string path = torch::String::Format("%s/.xpacktest/name%03d", homedir.c_str(), counter++);
    if (torch::FileSystem::IsPathExist(path)) {
        torch::FileSystem::Remove(path);
    }
    bool ok = pkg.OpenNew(path);
    if (!ok) {
        ErrorLog("%s: open new xpack(%s) failed\n", __FUNCTION__, path.c_str());
    }
    return path;
}

int TestNameMain() {
    // 测试name模块的增加和安全删除名称的正确性
    xpack::Package pack;
    std::string packpath = LoadNextPackage(pack);
    std::vector<std::string> names = {
        "first", "second", "third", "fourth"
    };

    bool ok = true;
    for (auto n : names) {
        ok &= pack.AddEntry(n, torch::Data::Null);
    }
    if (!ok) {
        ErrorLog("%s: addentry xpack(%s) failed\n", __FUNCTION__, packpath.c_str());
    }
    
    const torch::Data & rawnames = pack.GetContxt()->name->GetRawNames();
    std::string n = names[0]+names[1]+names[2]+names[3];
    std::string hexall = torch::ToHex(n.c_str(), n.size());
    TEST_TRUE(rawnames.ToHex() == hexall);
    
    {
        MetaHash *metahash = pack.GetContxt()->hash->QueryByName(names[0]);
        pack.GetContxt()->name->RemoveNameSafely(metahash);
        TEST_TRUE(rawnames.ToHex() == "00000000007365636F6E647468697264666F75727468");
    }
    {
        MetaHash *metahash = pack.GetContxt()->hash->QueryByName(names[2]);
        pack.GetContxt()->name->RemoveNameSafely(metahash);
        TEST_TRUE(rawnames.ToHex() == "00000000007365636F6E640000000000666F75727468");
    }
    {
        MetaHash *metahash = pack.GetContxt()->hash->QueryByName(names.back());
        pack.GetContxt()->name->RemoveNameSafely(metahash);
        TEST_TRUE(rawnames.ToHex() == "00000000007365636F6E64");
    }
    {
        MetaHash *metahash = pack.GetContxt()->hash->QueryByName(names[1]);
        pack.GetContxt()->name->RemoveNameSafely(metahash);
        TEST_TRUE(rawnames.ToHex() == "");
    }

    
    InfoLog("> test-name ... ok\n");
    return 0;
}

