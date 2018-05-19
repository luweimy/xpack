## shell命令   
[文档主页](./index.md)   

### xpack命令结构
xpack命令是主命令，其包含若干子命令，每个命令(包含主命令和子命令)都可以通过options: -h来打印文档。    
#####xpack命令本身有两个options 
```
-h  : 打印帮助文档
-v  : 显示版本信息
```

##### xpack 命令分为若干子命令
```
add      : 向包中添加文件(仅能添加一个)，可以指定添加文件在包中存储的名称
madd     : 同时向包中添加多个文件，不能指定添加文件在包中存储的名称。例如：xpack madd package file1 file2 file3 ...; xpack madd package *
rm       : 从包中删除文件，支持通配符(必须带引号)。例如：xpack rm package '*cc'
cat      : 打印包中文件的内容，支持通配符(必须带引号)，可以同时打印多个文件内容。
ls       : 列出文件中的文件名称，支持通配符(必须带引号)。
make     : 构造一个空的xpack包。
dump     : 打印内部元数据信息，用于调试。
unpack   : 解包到指定的目录。
check    : 检测文件是否是合法的xpack包。
merge    : 合并另一个xpack包中的内容到主xpack包中，只会改变主xpack包。
optimize : 优化xpack包。即重新构建一个新包，将数据重新写入，并替换原来的包。
diff     : 打印两个xpack包的对比分析数据。
```

##### 子命令用法
```
xpack add      <package> <file> [options]
xpack madd     <package> <file> [file ...] [options]
xpack rm       <package> <'wildcard'> [options]
xpack cat      <package> <'wildcard'> [options]
xpack ls       <package> ['wildcard'] [options]
xpack make     <package> [options]
xpack dump     <package> [options]
xpack unpack   <package> [pathto] [options]
xpack check    <package> [options]
xpack merge    <main-package> <other-package> [options]
xpack optimize <package> [options]
xpack diff     <main-package> <other-package> [options]
```

##### 帮助  
xpack -h    

```
localhost:bin luwei$ ./xpack
usage: xpack <subcommand> [options] [args]
xpack command line client, version 0.9.

options:
        -h  : show help document
        -v  : show version information

subcommands:
        add       : add file to package.
        madd      : add multi files to package.
        rm        : remove file from package(support 'wildcard').
        cat       : print entry content in package(support 'wildcard').
        ls        : list all entries in package(support 'wildcard').
        make      : make empty package.
        dump      : dump package information.
        unpack    : unpack all entries to target folder.
        check     : check package is valid.
        merge     : merge other package into main package.
        optimize  : rebuild to optimize package.
        diff      : show two package differences.
        benchmark : benchmark test.
```

