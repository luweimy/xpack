//
//  Torch
//
//  Created by Luwei.
//  Copyright (c) 2014-2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy/Torch
//

#include "torch-wildcard.h"
#include <assert.h>
extern "C" {
#include "deps/wildcard/wildcard.h"
}

using namespace torch;

int WildcardMatcher::Match(const std::string &wildcard, const std::string &target)
{
    return wc_match(wildcard.c_str(), target.c_str());
}

const char* WildcardMatcher::FormatError(int error)
{
    return wc_error(error);
}
