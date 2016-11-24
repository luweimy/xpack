//
//  xpack
//
//  Created by Luwei.
//  Copyright (c) 2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy
//

#ifndef __XPACK__DEF__
#define __XPACK__DEF__

#include <stdint.h>

#define XPACK_WHERE torch::String::Format("%s:%s:%d", __FILE__, __FUNCTION__, __LINE__).c_str()
#define XPACK_ERROR(__errco) xpack::SetLastError(int32_t(__errco), XPACK_WHERE)

namespace xpack {
    
    typedef int32_t xpack_index_t; 
    typedef uint32_t xpack_hashid_t;
    typedef uint32_t xpack_size_t;
    
    enum {
        SIGNATURE   = 0x1A4B434150585E1A,   /* the 0x1A4B434150585E1A ('\x1A^XPACK\x1A') signature */
        VERSION     = 0x0009,               /* 0x0009 for now */
        SIGNATURE_ALIGNED = 0x200,          /* signature has to start at an offset aligned to 512 (0x200) bytes. */
    };
    
    enum class Error {
        NoErr   =  0,
        IO      = -1,    /* io error on file. */
        Memory  = -2,    /* memory error. */
        Format  = -3,    /* format error. */
        Version = -4,    /* version not support. */
        CRC     = -5,    /* crc32 check error. */

        AlreadyExists   = -11,
        NotExists       = -12,
        Compress        = -13,
        
        Unknow          = -99
    };

    enum class HashFlags {
        Unused       = 1 << 0,          /* mark unused item */
        Conflict     = 1 << 1,          /* mark conflict item */
        CryptoRC4    = 1 << 2,
        Compressed   = 1 << 3,
    };
    
    enum class BlockFlags {
        UnusedContent  = 1 << 0,      /* mark item's content unused */
        UnusedBlock    = 1 << 1,      /* mark unused item */
        NotStart       = 1 << 2,      /* mark the item is not start item */
    };
    
    
#pragma pack(push, 1)
    
    
    typedef struct {
        uint64_t	signature;		/* xpack::SIGNATURE */
        uint16_t	version;		/* xpack::VERSION */
    } MetaSignature;
    
    
    typedef struct {
        uint32_t	archive_size;	/* size of xpack archive. */
        uint32_t	content_offset;	/* file position of file content segment. */
        uint32_t	content_size;   /* size of file content segment. */
        uint32_t	block_offset;	/* file position of blocks segment. */
        uint32_t	block_count;	/* number of entries in the block table. */
        uint32_t	hash_offset;	/* file position of hashs segment. */
        uint32_t	hash_count;     /* number of entries in hash table. */
        uint32_t	name_size;      /* name size in bytes. */
        uint8_t     reserved[16];   /* 16 bytes reserved.  */
    } MetaHeader;
    
    
    typedef struct {
        uint32_t    hash;           /* hashid */
        uint32_t    crc;
        int32_t     block_index;    /* block chain first index */
        uint32_t    unpacked_size;  /* original size */
        uint32_t    name_offset;    /* relative offset in name segment */
        uint16_t    name_size;
        uint8_t     conflict_refc;  /* conflict references counting */
        uint8_t     salt;           /* use for hash(name, salt) */
        uint8_t     flags;          /* unused, conflict, rc4, compressed */
    } MetaHash;
    
    
    typedef struct {
        uint32_t	offset;
        uint32_t	size;
        int32_t     next_index;     /* block chain next index */
        uint8_t     flags;          /* unused-content, unused-block, not-start */
    } MetaBlock;
    
    
#pragma pack(pop)
    
    
}

#endif
