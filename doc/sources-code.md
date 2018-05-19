## 源码说明
[文档主页](./index.md)   

### 文件
```
torch/              - 基础工具库(文件读取、Data封装、Hash、加密、压缩等)
shell.cpp           - shell工具实现

xpack.cpp           - 对其他模块(name,signature,header,hash,content,block)的封装，提供简单的接口供外部使用
xpack-base.cpp      - Hash封装，错误处理
xpack-util.cpp      - 提供获取内部数据的封装，主要用于调试

xpack-context.cpp   - 对各个模块的统一持有，没有特殊功能
xpack-stream.cpp    - 对数据流读写的抽象，提供替换Stream功能

xpack-block.cpp     - Block模块，提供对Block区段的数据操作接口
xpack-content.cpp   - Content模块，提供对Content区段的数据操作接口
xpack-hash.cpp      - Hash模块，提供对Hash区段的数据操作接口
xpack-header.cpp    - Header模块，提供对Header区段的数据操作接口
xpack-name.cpp      - Name模块，提供对Name区段的数据操作接口
xpack-signature.cpp - Signature模块，提供对Signature区段的数据操作接口

```

### 概述
```

			  | class FileStream
class Stream -| ...


			  |-stream
			  |-signature
class Context-|-header
			  |-content
	          |-block
			  |-hash
			  |-name


			  |-stream
class Package-|-context

```

### 存储结构：
[文件结构](./package-sct.md)   
```
|- Signature // 签名区段，保存签名等信息，用于在文件内定位资源包的偏移量
|- Header	 // 头信息区段，保存其他区段偏移量和尺寸等
|- Content	 // 存储文件内容的区段
|- Blocks	 // 存储所有block结构列表数据存储区段
|- Hashs	 // 存储所有hash结构列表数据存储区段
|- Names	 // 存储所有的文件名

````

以上为资源包的文件结构，以及每个区段的作用。  

在`xpack-def.h`中定义了每个区段的主要数据结构：`(注意数据对齐为1Byte)`   

+ MetaSignature
+ MetaHeader
+ MetaHash
+ MetaBlock

每个区段也都对应一个处理的类：

+ SignatureSegment - 处理资源包定位逻辑(在宿主文件中搜索资源包数据)
+ HeaderSegment    - 只是简单的读写header区段数据，以及数据更新
+ ContentSegment   - 对content区段读写
+ BlockSegment     - 申请和管理文件分块信息`(较复杂：文件分块存储管理)`
+ HashSegment      - 申请和管理文件的Hash记录`(较复杂：Hash解冲突)`
+ NameSegment      - 管理文件名信息的存取和添加删除等


### 数据存储
```
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
```
每个存储的文件，都仅对应着一个MetaHash，此MetaHash记录这文件的所有信息及其查找凭据，xpack通过hash来查找一个文件的MetaHash    
找到MetaHash后：   
通过name_offset/name_size在NameSegment模块中找到文件名   
通过block_index在BlockSegment模块中找到MetaBlock   

```
typedef struct {
    uint32_t    offset;
    uint32_t    size;
    int32_t     next_index;     /* block chain next index */
    uint8_t     flags;          /* unused-content, unused-block, not-start */
} MetaBlock;
```
一个文件可能对应多个MetaBlock，由于支持分块存储，一个文件可能被分拆成n个块，分别存储在包内的不同位置，而每个MetaBlock则可以记录文件的一个存储位置。   
通过offset/size就可以知道本个文件块存储在包中的那个位置及其大小。      

一个MetaHash如何记录文件对应的多个MetaBlock呢？就是使用next_index，类似于链表，如果文件真分块存在不同的地方则next_index指向下一个MetaBlock，直到最终next_index为-1，就知道没有下一块了，将之前的所有块拼接起来就是文件的所有内容。  

### 数据流程

`xpack.h`定义的是对外提供的api接口，基本的增删改查都可以在这里找到  

##### 加载
1. 通过signature模块，在文件中搜索签名标记，找到后记录当前签名位置的offset（签名要求对齐到512Byte）
2. 读取Header信息，校验Header合法性
3. 读取blocks信息，加载文件存储的分块信息数据
4. 读取hashs信息，加载文件索引信息数据
5. 读取names信息，加载文件名信息 

##### 添加
1. 向hash模块申请一个hash索引记录`(MetaHash)`，注意重名失败
2. 向block模块申请一个合适尺寸的文件块记录链(记录结构MetaBlock是链表；其优先重用废弃块)
3. content模块根据申请的文件块记录链，将文件数据写入到content区段
4. 将文件名加入到names区域
5. 将block链表头index和name的存储位置，放入对应hash索引记录中存储

##### 删除
1. 通过hash模块，查找要删除的文件索引记录(MetaHash)
2. 通过block模块，根据索引记录(MetaHash)将其分块全部删除或者加入重用集合
3. 通过name模块，根据索引记录(MetaHash)将文件名删除
4. 最后将索引记录(MetaHash)删除

### 关键实现
##### 文件解冲突
**实现见HashSegment::AddNew**    

思路：通过递归实现，若文件冲突则将其在冲突的索引记录处标记`CONFLICT`，并且生成一个种子存放于冲突的索引记录salt处，重新使用新种子对文件名进行hash，递归进行之前的步骤，直到没有冲突为止。    
 
测试：表明多级冲突或者同位置多个冲突也可以正常解决，理论上当文件极多导致hashid(uint32_t)不足时会造成无限冲突跳转(实际应该不会出现)。  

注意：删除时也要一层一层都删除干净，为避免多个冲突到同一位置造成多引用问题，使用MetaHash.conflict_refc记录冲突引用数量，只有当conflict_refc为0时才能删除。  

##### 文件分块存储
**实现见BlockSegment::AllocLinkedBlock**    

注意：这里的重用分两种：

+ 一种是`MetaBlock`存储结构本身的重用(由于`MetaBlock`列表连续存储，并且是用index标记其唯一性，所以不能随便删除)
+ 一种是`MetaBlock`结构指向的Content区段的区域重用(由于移动数据性能很低，而不移动的话空间利用率较低，所以为了中和两者使用数据块可重用机制)

思路：
优先从Content重用池`(ContentReuser)`中获取小尺寸的block记录，分配直到正好够用。(优先使用小尺寸有利于避免数据块碎片化) 
但是若使用重用池中的数据块添加后，大于申请的尺寸，则将最后添加的一个数据块Content区域拆分成两块，使其刚好够用。  
若重用池中记录的Content区域尺寸不足，则需另外在Content区段末尾追加新区域，使其刚好满足申请尺寸。

由于文件Content区域删除效率极低，所以使用假性删除，即将其标记为可重用，但是并不会删除，但这样会造成空间利用率较低，虽然加入了重用机制，但为了使效果更好还加入了几种优化手段：  

+ 将Content区域末尾的可重用区域全部删除(此处效率较高，不用移动数据)
+ 将Block记录的小尺寸Content抛弃(待议，若删除数据块，会导致content区域删除受阻)
+ 将Block区域末尾的可重用的Block记录块删除，以减少Block记录。(Block只能从末尾往前删除无效块，否则导致index改变，会造成数据错乱)

