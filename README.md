# xpack pack/unpack tools

### 概述
+ xpack是一个文件资源打包工具及类库，可以对多文件进行打包解包。   
+ 其使用文件名的hash作为索引，建立hash索引表以加速文件查找。  
+ 基于torch工具库

### 特性 
+ 支持hashid自动解冲突，即使添加的多个文件名hashid发生冲突也可以正常存取，不用担心文件名hash一样导致的添加失败问题
+ 支持存储文件的原文件名，可以解包复原文件目录结构
+ 支持文件分块存储，可以重复利用被删除文件的剩下的空闲块，提高资源包的空间利用率，提高资源删除的性能  
+ 支持寄生资源包，可以把资源包追加到其他文件中，使用xpack类库仍可以正常打开并进行读写操作
+ 可选对文件进行数据加密/压缩/CRC校验等
+ 支持资源主要区段的信息(元数据)加密
+ 提供配套资源包操作命令行工具

### 性能

`不加密，不压缩，读取2300个文件(40M~0K)`   

资源包读取(关闭crc) | 资源包读取(开启crc) | 直接读取
----------|---------|---------
0.108492|1.019783|0.384069
0.106661|0.964988|0.311151
0.081684|0.991808|0.238214

### 工程目录
```
xpack/-|
       |-src   // 源码目录(包含xpack源码+shell工具源码)
       |-build // 编辑构建目录，包含Makefile生成工具，安卓编译
       |-doc   // 文档目录
       |-tests // 测试代码
```

### 文档
[详细文档入口](./doc/index.md)    

### 编译运行xpack工具
```
cd build
python automake.py && make
cd bin/ && ./xpack
```

### 编译运行tests
```
cd build/build_test
sh build.sh
cd bin/ && ./test
```

### 编译Android静态库
```
cd build/build_android
sh build.sh
cd obj/local
```

### shell演示
![xpack-shell](https://raw.githubusercontent.com/Luweimy/luweimy.github.io/master/res/xpack-shell-3.gif)
![xpack-shell](https://raw.githubusercontent.com/Luweimy/luweimy.github.io/master/res/xpack-shell-6.gif)
![xpack-shell](https://raw.githubusercontent.com/Luweimy/luweimy.github.io/master/res/xpack-shell-5.gif)
![xpack-shell](https://raw.githubusercontent.com/Luweimy/luweimy.github.io/master/res/xpack-shell-4.gif)
![xpack-shell](https://raw.githubusercontent.com/Luweimy/luweimy.github.io/master/res/xpack-shell-1.gif)
![xpack-shell](https://raw.githubusercontent.com/Luweimy/luweimy.github.io/master/res/xpack-shell-2.gif)
