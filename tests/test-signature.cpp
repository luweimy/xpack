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

static std::string GetNextPath() {
    static int counter = 0;
    std::string homedir = torch::Path::GetHomeDir();
    std::string path = torch::String::Format("%s/.xpacktest/sign%03d", homedir.c_str(), counter++);
    return path;
}

int TestSignatureMain() {
    { // 测试寄生包，在已存在文件末尾(对齐字节后)追加包内容
        std::string packpath = GetNextPath();
        
        int fillsize = 1000;
        double alignedsize = (double)xpack::SIGNATURE_ALIGNED;
        uint32_t offset = ceil(fillsize / alignedsize) * alignedsize;

        torch::Data d = torch::Data().Alloc(fillsize).Memset(0);
        torch::File::WriteBytes(packpath, d);
        
        xpack::Package pack;
        bool ok = pack.OpenNew(packpath);
        ok &= pack.AddEntry("hello", torch::Data("hello"));
        ok &= pack.Flush();
        if (!ok) {
            ErrorLog("%s: opennew/addentry/flush xpack(%s) failed\n", __FUNCTION__, packpath.c_str());
        }

        TEST_TRUE(pack.GetContxt()->offset == offset);

        ok = pack.Open(packpath);
        if (!ok) {
            ErrorLog("%s: open exist xpack(%s) failed\n", __FUNCTION__, packpath.c_str());
        }
        
        TEST_TRUE(pack.GetContxt()->offset == offset);
        TEST_TRUE(pack.GetEntryStringByName("hello") == "hello");
    }
    
    InfoLog("> test-signature ... ok\n");
    return 0;
}
