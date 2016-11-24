//
//  xpack
//
//  Created by Luwei.
//  Copyright (c) 2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy
//

#include "xpack-base.h"
#include "xpack-def.h"
#include "torch/torch.h"

// prime number(must>=256)
static const uint16_t __xpack_primenumber_array[] = {
    1193,1201,1213,1217,1223,1229,1231,1237,1249,1259,1277,1279,1283,1289,1291,
    2089,2099,2111,2113,2129,2131,2137,2141,2143,2153,2161,2179,2203,2207,2213,
    2579,2591,2593,2609,2617,2621,2633,2647,2657,2659,2663,2671,2677,2683,2687,
    3041,3049,3061,3067,3079,3083,3089,3109,3119,3121,3137,3163,3167,3169,3181,
    3187,3191,3203,3209,3217,3221,3229,3251,3253,3257,3259,3271,3299,3301,3307,
    3433,3449,3457,3461,3463,3467,3469,3491,3499,3511,3517,3527,3529,3533,3539,
    3659,3671,3673,3677,3691,3697,3701,3709,3719,3727,3733,3739,3761,3767,3769,
    3911,3917,3919,3923,3929,3931,3943,3947,3967,3989,4001,4003,4007,4013,4019,
    6101,6113,6121,6131,6133,6143,6151,6163,6173,6197,6199,6203,6211,6217,6221,
    6229,6247,6257,6263,6269,6271,6277,6287,6299,6301,6311,6317,6323,6329,6337,
    6343,6353,6359,6361,6367,6373,6379,6389,6397,6421,6427,6449,6451,6469,6473,
    6481,6491,6521,6529,6547,6551,6553,6563,6569,6571,6577,6581,6599,6607,6619,
    6637,6653,6659,6661,6673,6679,6689,6691,6701,6703,6709,6719,6733,6737,6761,
    6763,6779,6781,6791,6793,6803,6823,6827,6829,6833,6841,6857,6863,6869,6871,
    6883,6899,6907,6911,6917,6947,6949,6959,6961,6967,6971,6977,6983,6991,6997,
    8117,8123,8147,8161,8167,8171,8179,8191,8209,8219,8221,8231,8233,8237,8243,
    8263,8269,8273,8287,8291,8293,8297,8311,8317,8329,8353,8363,8369,8377,8387,
    8389,8419,8423,8429,8431,8443,8447,8461,8467,8501,8513,8521,8527,8537,8539,
};

static uint16_t GetPrimeNumber(uint8_t index) {
    return __xpack_primenumber_array[index];
}

// Hash

uint32_t xpack::HashString(const std::string &s, uint8_t seed) {
    return torch::Hash::XXHash32(s.c_str(), s.length(), GetPrimeNumber(seed));
}

// LastError

static int __xpack_error_code = 0;

void xpack::SetLastError(int errcode) {
    __xpack_error_code = errcode;
}

void xpack::SetLastError(int errcode, const char *where) {
    __xpack_error_code = errcode;
    //   printf("Error # (%s)%d | %s\n", xpack::GetLastErrorMessage(), errcode, where);
}

int xpack::GetLastError() {
    return __xpack_error_code;
}

const char* xpack::GetLastErrorMessage()
{
    switch (__xpack_error_code) {
        case (int)xpack::Error::NoErr:
            return "no error.";
        case (int)xpack::Error::IO:
            return "io error.";
        case (int)xpack::Error::Memory:
            return "memory error.";
        case (int)xpack::Error::Format:
            return "package is invalid.";
        case (int)xpack::Error::Version:
            return "version not match.";
        case (int)xpack::Error::CRC:
            return "crc check failed.";
        case (int)xpack::Error::AlreadyExists:
            return "file already exists.";
        case (int)xpack::Error::NotExists:
            return "file not exists.";
        case (int)xpack::Error::Compress:
            return "compress error.";
            
        case (int)xpack::Error::Unknow:
        default:
            return "unknow error.";
    }
}
