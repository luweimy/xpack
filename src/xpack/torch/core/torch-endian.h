//
//  Torch
//
//  Created by Luwei.
//  Copyright (c) 2014-2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy/Torch
//

#ifndef __TORCH__ENDIAN__
#define __TORCH__ENDIAN__

#include <arpa/inet.h>
#include <stdlib.h>


namespace torch {
    
    class Endian
    {
    public:
        inline static bool IsBig()                      {
            unsigned int test = 0xff000000;
            return (*(unsigned char *)&test == 0xff);
        }
        inline static bool IsLittle()                   {
            return !IsBig();
        }
        
        /** unsigned 8/16/32/64 bit */
        inline static uint8_t ToNet(uint8_t host)       {
            return host;
        }
        inline static uint16_t ToNet(uint16_t host)     {
            return htons((uint16_t)host);
        }
        inline static uint32_t ToNet(uint32_t host)     {
            return htonl((uint32_t)host);
        }
        inline static uint64_t ToNet(uint64_t host)     {
#if defined(__linux__)
#   if __BYTE_ORDER == __LITTLE_ENDIAN
            return Reverse(host);
#   else
            return host;
#   endif
#else
            return htonll((uint64_t)host);
#endif
        }
        
        /** signed 8/16/32/64 bit */
        inline static int8_t ToNet(int8_t host)         {
            return host;
        }
        inline static int16_t  ToNet(int16_t host)      {
            return htons((int16_t)host);
        }
        inline static int32_t  ToNet(int32_t host)      {
            return htonl((int32_t)host);
        }
        inline static int64_t  ToNet(int64_t host)      {
            return ToNet((uint64_t)host);
        }
        
        /** unsigned 8/16/32/64 bit */
        inline static uint8_t ToHost(uint8_t net)       {
            return net;
        }
        inline static uint16_t ToHost(uint16_t net)     {
            return ntohs((uint16_t)net);
        }
        inline static uint32_t ToHost(uint32_t net)     {
            return ntohl((uint32_t)net);
        }
        inline static uint64_t ToHost(uint64_t net)     {
#if defined(__linux__)
#   if __BYTE_ORDER == __LITTLE_ENDIAN
            return Reverse(net);
#   else
            return net;
#   endif
#else
            return ntohll((uint64_t)net);
#endif
        }
        
        /** signed 8/16/32/64 bit */
        inline static int8_t ToHost(int8_t net)         {
            return net;
        }
        inline static int16_t  ToHost(int16_t net)      {
            return ntohs((int16_t)net);
        }
        inline static int32_t  ToHost(int32_t net)      {
            return ntohl((int32_t)net);
        }
        inline static int64_t  ToHost(int64_t net)      {
            return ToHost((int64_t)net);
        }
        
        /** unsigned 16/32/64 bit */
        
        inline static uint16_t Reverse(uint16_t x)      {
            x = ((x & 0x00FF) << 8) | ((x & 0xFF00) >> 8);
            return x;
        }
        inline static uint32_t Reverse(uint32_t x)      {
            x = ((x & 0x000000FF) << 24) |
                ((x & 0x0000FF00) <<  8) |
                ((x & 0x00FF0000) >>  8) |
                ((x & 0xFF000000) >> 24);
            return x;
        }
        inline static uint64_t Reverse(uint64_t x)      {
            x = ((x & 0x00000000000000FFULL) << 56) |
                ((x & 0x000000000000FF00ULL) << 40) |
                ((x & 0x0000000000FF0000ULL) << 24) |
                ((x & 0x00000000FF000000ULL) <<  8) |
                ((x & 0x000000FF00000000ULL) >>  8) |
                ((x & 0x0000FF0000000000ULL) >> 24) |
                ((x & 0x00FF000000000000ULL) >> 40) |
                ((x & 0xFF00000000000000ULL) >> 56);
            return x;
        }
        
        /** signed 16/32/64 bit */
        
        inline static int16_t Reverse(int16_t x)        {
            return Reverse((uint16_t)x);
        }
        inline static int32_t Reverse(int32_t x)        {
            return Reverse((uint32_t)x);
        }
        inline static int64_t Reverse(int64_t x)        {
            return Reverse((uint64_t)x);
        }
        
    };
    
}

#endif /* __TORCH__ENDIAN__ */
