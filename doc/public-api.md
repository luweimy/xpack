## 开放接口   
[文档主页](./index.md)     

#### 创建空包
```
bool ok = xpkg::PackageHelper::MakeNew(path);
```

#### 添加文件
```
Package pkg;
if (!pkg.Open(package, false)) {
	return false;
}
bool ok = package.AddEntry(name, torch::File::GetBytes(path));
```

#### 删除文件
```
xpack::Package pkg;
if (!pkg.Open(package, false)) {
    return false;
}
bool ok = pkg.RemoveEntry(name);
```

#### 遍历文件
```
xpack::Package pkg;
if (!pkg.Open(package)) {
    return false;
}
pkg.ForeachEntryNames([&](const std::string &name){
    return true;
});
```

#### 获得文件内容
```
xpack::Package pkg;
if (!pkg.Open(package)) {
    return false;
}
torch::Data content = pkg.GetEntryDataByName(name);
std::string text = pkg.GetEntryStringByName(name);
```

#### 查询文件是否存在
```
xpack::Package pkg;
if (!pkg.Open(package)) {
    return false;
}
bool exists = pkg.IsEntryExist(name);

```

#### 获得文件大小
```
xpack::Package pkg;
if (!pkg.Open(package)) {
    return false;
}
// 获得文件存在在包中的大小(数据可能经过压缩，加密等)
uint32_t size = pkg.GetEntrySizeByName(name);
// 获得文件原始大小
uint32_t unpacked_size = pkg.GetUnpackedEntrySizeByName(name);
```

#### 获得xpack包的大小
```
xpack::Package pkg;
if (!pkg.Open(package)) {
    return false;
}
// 获得xpack包大小，不一定等于文件大小
size_t size = pkg.GetPackageSize();

```
