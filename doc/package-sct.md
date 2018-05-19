## 文件结构
[文档主页](./index.md)   

### xpack包文件结构   
```
+---------------------+
|                     |
| MetaSignature       | SignatureSegment(签名数据区段)
|                     |
+---------------------+
|                     |
| MetaHeader          | HeaderSegment(头信息元数据区段)
|                     |
+---------------------+
| Data Block(数据块)   |
| ··················· | 
| ...                 | ContentSegment(文件内容存放区段)
| ··················· |
| Data Block          |
+---------------------+
| MetaBlock           |
| ··················· |
| ...                 | BlockSegment(Block元数据区段)
| ··················· |
| MetaBlock           |
+---------------------+
| MetaHash            |
| ··················· |
| ...                 | HashSegment(Hash元数据区段)
| ··················· |
| MetaHash            |
+---------------------+
| Name                |
| ··················· | 
| ...                 | NameSegment(文件名数据区段)
| ··················· |
| Name                |
+---------------------+

```


### 元数据结构定义
```
#pragma pack(push, 1)
    

typedef struct {
    uint64_t    signature;      /* xpack::SIGNATURE */
    uint16_t    version;        /* xpack::VERSION */
} MetaSignature;


typedef struct {
    uint32_t    archive_size;   /* size of xpack archive. */
    uint32_t    content_offset; /* file position of file content segment. */
    uint32_t    content_size;   /* size of file content segment. */
    uint32_t    block_offset;   /* file position of blocks segment. */
    uint32_t    block_count;    /* number of entries in the block table. */
    uint32_t    hash_offset;    /* file position of hashs segment. */
    uint32_t    hash_count;     /* number of entries in hash table. */
    uint32_t    name_size;      /* name size in bytes. */
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
    uint32_t    offset;
    uint32_t    size;
    int32_t     next_index;     /* block chain next index */
    uint8_t     flags;          /* unused-content, unused-block, not-start */
} MetaBlock;
    
    
#pragma pack(pop)

```
