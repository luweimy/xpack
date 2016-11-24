#!/bin/bash
## 更新xpack中引用的torch，并精简部分无用代码
## 更新automake.py工具
## 不会更新torch.h

current_dir=$(cd `dirname ${0}`; pwd)

path_storage=$current_dir/.storage/
path_storage_torch=$path_storage/Torch/
path_storage_torch_src=$path_storage_torch/src/
giturl_torch=https://github.com/Luweimy/Torch.git  

test ! -d $path_storage_torch && mkdir $path_storage
cd $path_storage && test ! -d $path_storage_torch && git clone $giturl_torch


cd $path_storage_torch_src && \
git pull && \

cd $current_dir/src/xpack/torch/ && \
cp -v torch.h $path_storage/torch.h && \
rm -rf * && \

cp -R $path_storage_torch_src/* ./  && \
cp -v $path_storage_torch/build/automake.py $current_dir/build/automake.py  && \

rm -rf thread/ && \
rm compress/torch-compress-bz2* && \
rm core/torch-locallife* && \
rm crypto/torch-crypto-base64* && \
rm crypto/torch-crypto-md5* && \
rm crypto/torch-crypto-aes* && \
rm crypto/torch-crypto-sha1* && \
rm crypto/torch-crypto-xxtea* && \
rm -rf deps/xxtea/ && \
rm -rf deps/bzip2/ && \
rm -rf deps/base64/ && \
rm -rf deps/arcemu/ && \
rm torch-scanner* && \
rm torch-time* && \
rm torch-date* && \
rm torch-random* && \
rm torch-directory* && \
rm torch-system* && \
rm torch-regex* && \

cp -v $path_storage/torch.h torch.h && \
ls && \
echo "done."

