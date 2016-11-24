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


extern int TestSignatureMain();
extern int TestNameMain();
extern int TestHashMain();
extern int TestBlockMain();

#ifdef XPACK_TEST

int main(int argc, const char * argv[]) {
    TestSignatureMain();
    TestNameMain();
    TestHashMain();
    TestBlockMain();
    InfoLog("* tests pass.\n");
    return 0;
}

#endif

