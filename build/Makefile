TARGET     = bin/xpack
TARGET_DIR = $(dir ${TARGET})
OBJECT_DIR = obj/
LINK       = g++
CXX        = g++
CC         = gcc
CCFLAGS    = -g -Wall
CXXFLAGS   = -g -Wall -std=c++11 -D_FILE_OFFSET_BITS=64
LINKFLAGS  = -g -Wall -std=c++11 -D_FILE_OFFSET_BITS=64
OBJECT     = \
	$(OBJECT_DIR)shell.o\
	$(OBJECT_DIR)torch-collection-hashmap.o\
	$(OBJECT_DIR)torch-collection-iterator.o\
	$(OBJECT_DIR)torch-collection-sortedset.o\
	$(OBJECT_DIR)torch-collection-util.o\
	$(OBJECT_DIR)torch-compress-zip.o\
	$(OBJECT_DIR)torch-allocator.o\
	$(OBJECT_DIR)torch-base.o\
	$(OBJECT_DIR)torch-endian.o\
	$(OBJECT_DIR)torch-hash.o\
	$(OBJECT_DIR)torch-crypto-crc32.o\
	$(OBJECT_DIR)torch-crypto-rc4.o\
	$(OBJECT_DIR)lookup3.o\
	$(OBJECT_DIR)ioapi.o\
	$(OBJECT_DIR)ioapi_buf.o\
	$(OBJECT_DIR)ioapi_mem.o\
	$(OBJECT_DIR)iowin32.o\
	$(OBJECT_DIR)unzip.o\
	$(OBJECT_DIR)zip.o\
	$(OBJECT_DIR)wildcard.o\
	$(OBJECT_DIR)xxhash.o\
	$(OBJECT_DIR)aes.o\
	$(OBJECT_DIR)arc4.o\
	$(OBJECT_DIR)base64.o\
	$(OBJECT_DIR)bignum.o\
	$(OBJECT_DIR)certs.o\
	$(OBJECT_DIR)debug.o\
	$(OBJECT_DIR)des.o\
	$(OBJECT_DIR)dhm.o\
	$(OBJECT_DIR)havege.o\
	$(OBJECT_DIR)md2.o\
	$(OBJECT_DIR)md4.o\
	$(OBJECT_DIR)md5.o\
	$(OBJECT_DIR)net.o\
	$(OBJECT_DIR)padlock.o\
	$(OBJECT_DIR)rsa.o\
	$(OBJECT_DIR)sha1.o\
	$(OBJECT_DIR)sha2.o\
	$(OBJECT_DIR)sha4.o\
	$(OBJECT_DIR)ssl_cli.o\
	$(OBJECT_DIR)ssl_srv.o\
	$(OBJECT_DIR)ssl_tls.o\
	$(OBJECT_DIR)timing.o\
	$(OBJECT_DIR)x509parse.o\
	$(OBJECT_DIR)adler32.o\
	$(OBJECT_DIR)deflate.o\
	$(OBJECT_DIR)gzclose.o\
	$(OBJECT_DIR)gzlib.o\
	$(OBJECT_DIR)gzread.o\
	$(OBJECT_DIR)gzwrite.o\
	$(OBJECT_DIR)infback.o\
	$(OBJECT_DIR)inffast.o\
	$(OBJECT_DIR)inflate.o\
	$(OBJECT_DIR)inftrees.o\
	$(OBJECT_DIR)trees.o\
	$(OBJECT_DIR)uncompr.o\
	$(OBJECT_DIR)zcompress.o\
	$(OBJECT_DIR)zcrc32.o\
	$(OBJECT_DIR)zutil.o\
	$(OBJECT_DIR)torch-arguments.o\
	$(OBJECT_DIR)torch-console.o\
	$(OBJECT_DIR)torch-data.o\
	$(OBJECT_DIR)torch-file.o\
	$(OBJECT_DIR)torch-memorypool.o\
	$(OBJECT_DIR)torch-path.o\
	$(OBJECT_DIR)torch-string.o\
	$(OBJECT_DIR)torch-util.o\
	$(OBJECT_DIR)torch-wildcard.o\
	$(OBJECT_DIR)xpack-base.o\
	$(OBJECT_DIR)xpack-block.o\
	$(OBJECT_DIR)xpack-content.o\
	$(OBJECT_DIR)xpack-context.o\
	$(OBJECT_DIR)xpack-hash.o\
	$(OBJECT_DIR)xpack-header.o\
	$(OBJECT_DIR)xpack-name.o\
	$(OBJECT_DIR)xpack-signature.o\
	$(OBJECT_DIR)xpack-stream.o\
	$(OBJECT_DIR)xpack-util.o\
	$(OBJECT_DIR)xpack.o

$(shell test -d $(OBJECT_DIR) || mkdir -p $(OBJECT_DIR))

$(TARGET):$(OBJECT)
	test -d $(TARGET_DIR) || mkdir -p $(TARGET_DIR)
	$(LINK) $(LINKFLAGS) -o $(TARGET) $(OBJECT)

$(OBJECT_DIR)shell.o:../src/shell.cpp
	$(CXX) $(CXXFLAGS) -c -o $(OBJECT_DIR)shell.o ../src/shell.cpp
$(OBJECT_DIR)torch-collection-hashmap.o:../src/xpack/torch/collection/torch-collection-hashmap.cpp
	$(CXX) $(CXXFLAGS) -c -o $(OBJECT_DIR)torch-collection-hashmap.o ../src/xpack/torch/collection/torch-collection-hashmap.cpp
$(OBJECT_DIR)torch-collection-iterator.o:../src/xpack/torch/collection/torch-collection-iterator.cpp
	$(CXX) $(CXXFLAGS) -c -o $(OBJECT_DIR)torch-collection-iterator.o ../src/xpack/torch/collection/torch-collection-iterator.cpp
$(OBJECT_DIR)torch-collection-sortedset.o:../src/xpack/torch/collection/torch-collection-sortedset.cpp
	$(CXX) $(CXXFLAGS) -c -o $(OBJECT_DIR)torch-collection-sortedset.o ../src/xpack/torch/collection/torch-collection-sortedset.cpp
$(OBJECT_DIR)torch-collection-util.o:../src/xpack/torch/collection/torch-collection-util.cpp
	$(CXX) $(CXXFLAGS) -c -o $(OBJECT_DIR)torch-collection-util.o ../src/xpack/torch/collection/torch-collection-util.cpp
$(OBJECT_DIR)torch-compress-zip.o:../src/xpack/torch/compress/torch-compress-zip.cpp
	$(CXX) $(CXXFLAGS) -c -o $(OBJECT_DIR)torch-compress-zip.o ../src/xpack/torch/compress/torch-compress-zip.cpp
$(OBJECT_DIR)torch-allocator.o:../src/xpack/torch/core/torch-allocator.cpp
	$(CXX) $(CXXFLAGS) -c -o $(OBJECT_DIR)torch-allocator.o ../src/xpack/torch/core/torch-allocator.cpp
$(OBJECT_DIR)torch-base.o:../src/xpack/torch/core/torch-base.cpp
	$(CXX) $(CXXFLAGS) -c -o $(OBJECT_DIR)torch-base.o ../src/xpack/torch/core/torch-base.cpp
$(OBJECT_DIR)torch-endian.o:../src/xpack/torch/core/torch-endian.cpp
	$(CXX) $(CXXFLAGS) -c -o $(OBJECT_DIR)torch-endian.o ../src/xpack/torch/core/torch-endian.cpp
$(OBJECT_DIR)torch-hash.o:../src/xpack/torch/core/torch-hash.cpp
	$(CXX) $(CXXFLAGS) -c -o $(OBJECT_DIR)torch-hash.o ../src/xpack/torch/core/torch-hash.cpp
$(OBJECT_DIR)torch-crypto-crc32.o:../src/xpack/torch/crypto/torch-crypto-crc32.cpp
	$(CXX) $(CXXFLAGS) -c -o $(OBJECT_DIR)torch-crypto-crc32.o ../src/xpack/torch/crypto/torch-crypto-crc32.cpp
$(OBJECT_DIR)torch-crypto-rc4.o:../src/xpack/torch/crypto/torch-crypto-rc4.cpp
	$(CXX) $(CXXFLAGS) -c -o $(OBJECT_DIR)torch-crypto-rc4.o ../src/xpack/torch/crypto/torch-crypto-rc4.cpp
$(OBJECT_DIR)lookup3.o:../src/xpack/torch/deps/jenkins/lookup3.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)lookup3.o ../src/xpack/torch/deps/jenkins/lookup3.c
$(OBJECT_DIR)ioapi.o:../src/xpack/torch/deps/minizip/ioapi.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)ioapi.o ../src/xpack/torch/deps/minizip/ioapi.c
$(OBJECT_DIR)ioapi_buf.o:../src/xpack/torch/deps/minizip/ioapi_buf.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)ioapi_buf.o ../src/xpack/torch/deps/minizip/ioapi_buf.c
$(OBJECT_DIR)ioapi_mem.o:../src/xpack/torch/deps/minizip/ioapi_mem.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)ioapi_mem.o ../src/xpack/torch/deps/minizip/ioapi_mem.c
$(OBJECT_DIR)iowin32.o:../src/xpack/torch/deps/minizip/iowin32.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)iowin32.o ../src/xpack/torch/deps/minizip/iowin32.c
$(OBJECT_DIR)unzip.o:../src/xpack/torch/deps/minizip/unzip.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)unzip.o ../src/xpack/torch/deps/minizip/unzip.c
$(OBJECT_DIR)zip.o:../src/xpack/torch/deps/minizip/zip.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)zip.o ../src/xpack/torch/deps/minizip/zip.c
$(OBJECT_DIR)wildcard.o:../src/xpack/torch/deps/wildcard/wildcard.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)wildcard.o ../src/xpack/torch/deps/wildcard/wildcard.c
$(OBJECT_DIR)xxhash.o:../src/xpack/torch/deps/xxhash/xxhash.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)xxhash.o ../src/xpack/torch/deps/xxhash/xxhash.c
$(OBJECT_DIR)aes.o:../src/xpack/torch/deps/xyssl/src/aes.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)aes.o ../src/xpack/torch/deps/xyssl/src/aes.c
$(OBJECT_DIR)arc4.o:../src/xpack/torch/deps/xyssl/src/arc4.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)arc4.o ../src/xpack/torch/deps/xyssl/src/arc4.c
$(OBJECT_DIR)base64.o:../src/xpack/torch/deps/xyssl/src/base64.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)base64.o ../src/xpack/torch/deps/xyssl/src/base64.c
$(OBJECT_DIR)bignum.o:../src/xpack/torch/deps/xyssl/src/bignum.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)bignum.o ../src/xpack/torch/deps/xyssl/src/bignum.c
$(OBJECT_DIR)certs.o:../src/xpack/torch/deps/xyssl/src/certs.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)certs.o ../src/xpack/torch/deps/xyssl/src/certs.c
$(OBJECT_DIR)debug.o:../src/xpack/torch/deps/xyssl/src/debug.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)debug.o ../src/xpack/torch/deps/xyssl/src/debug.c
$(OBJECT_DIR)des.o:../src/xpack/torch/deps/xyssl/src/des.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)des.o ../src/xpack/torch/deps/xyssl/src/des.c
$(OBJECT_DIR)dhm.o:../src/xpack/torch/deps/xyssl/src/dhm.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)dhm.o ../src/xpack/torch/deps/xyssl/src/dhm.c
$(OBJECT_DIR)havege.o:../src/xpack/torch/deps/xyssl/src/havege.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)havege.o ../src/xpack/torch/deps/xyssl/src/havege.c
$(OBJECT_DIR)md2.o:../src/xpack/torch/deps/xyssl/src/md2.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)md2.o ../src/xpack/torch/deps/xyssl/src/md2.c
$(OBJECT_DIR)md4.o:../src/xpack/torch/deps/xyssl/src/md4.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)md4.o ../src/xpack/torch/deps/xyssl/src/md4.c
$(OBJECT_DIR)md5.o:../src/xpack/torch/deps/xyssl/src/md5.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)md5.o ../src/xpack/torch/deps/xyssl/src/md5.c
$(OBJECT_DIR)net.o:../src/xpack/torch/deps/xyssl/src/net.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)net.o ../src/xpack/torch/deps/xyssl/src/net.c
$(OBJECT_DIR)padlock.o:../src/xpack/torch/deps/xyssl/src/padlock.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)padlock.o ../src/xpack/torch/deps/xyssl/src/padlock.c
$(OBJECT_DIR)rsa.o:../src/xpack/torch/deps/xyssl/src/rsa.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)rsa.o ../src/xpack/torch/deps/xyssl/src/rsa.c
$(OBJECT_DIR)sha1.o:../src/xpack/torch/deps/xyssl/src/sha1.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)sha1.o ../src/xpack/torch/deps/xyssl/src/sha1.c
$(OBJECT_DIR)sha2.o:../src/xpack/torch/deps/xyssl/src/sha2.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)sha2.o ../src/xpack/torch/deps/xyssl/src/sha2.c
$(OBJECT_DIR)sha4.o:../src/xpack/torch/deps/xyssl/src/sha4.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)sha4.o ../src/xpack/torch/deps/xyssl/src/sha4.c
$(OBJECT_DIR)ssl_cli.o:../src/xpack/torch/deps/xyssl/src/ssl_cli.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)ssl_cli.o ../src/xpack/torch/deps/xyssl/src/ssl_cli.c
$(OBJECT_DIR)ssl_srv.o:../src/xpack/torch/deps/xyssl/src/ssl_srv.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)ssl_srv.o ../src/xpack/torch/deps/xyssl/src/ssl_srv.c
$(OBJECT_DIR)ssl_tls.o:../src/xpack/torch/deps/xyssl/src/ssl_tls.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)ssl_tls.o ../src/xpack/torch/deps/xyssl/src/ssl_tls.c
$(OBJECT_DIR)timing.o:../src/xpack/torch/deps/xyssl/src/timing.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)timing.o ../src/xpack/torch/deps/xyssl/src/timing.c
$(OBJECT_DIR)x509parse.o:../src/xpack/torch/deps/xyssl/src/x509parse.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)x509parse.o ../src/xpack/torch/deps/xyssl/src/x509parse.c
$(OBJECT_DIR)adler32.o:../src/xpack/torch/deps/zlib/adler32.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)adler32.o ../src/xpack/torch/deps/zlib/adler32.c
$(OBJECT_DIR)deflate.o:../src/xpack/torch/deps/zlib/deflate.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)deflate.o ../src/xpack/torch/deps/zlib/deflate.c
$(OBJECT_DIR)gzclose.o:../src/xpack/torch/deps/zlib/gzclose.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)gzclose.o ../src/xpack/torch/deps/zlib/gzclose.c
$(OBJECT_DIR)gzlib.o:../src/xpack/torch/deps/zlib/gzlib.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)gzlib.o ../src/xpack/torch/deps/zlib/gzlib.c
$(OBJECT_DIR)gzread.o:../src/xpack/torch/deps/zlib/gzread.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)gzread.o ../src/xpack/torch/deps/zlib/gzread.c
$(OBJECT_DIR)gzwrite.o:../src/xpack/torch/deps/zlib/gzwrite.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)gzwrite.o ../src/xpack/torch/deps/zlib/gzwrite.c
$(OBJECT_DIR)infback.o:../src/xpack/torch/deps/zlib/infback.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)infback.o ../src/xpack/torch/deps/zlib/infback.c
$(OBJECT_DIR)inffast.o:../src/xpack/torch/deps/zlib/inffast.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)inffast.o ../src/xpack/torch/deps/zlib/inffast.c
$(OBJECT_DIR)inflate.o:../src/xpack/torch/deps/zlib/inflate.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)inflate.o ../src/xpack/torch/deps/zlib/inflate.c
$(OBJECT_DIR)inftrees.o:../src/xpack/torch/deps/zlib/inftrees.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)inftrees.o ../src/xpack/torch/deps/zlib/inftrees.c
$(OBJECT_DIR)trees.o:../src/xpack/torch/deps/zlib/trees.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)trees.o ../src/xpack/torch/deps/zlib/trees.c
$(OBJECT_DIR)uncompr.o:../src/xpack/torch/deps/zlib/uncompr.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)uncompr.o ../src/xpack/torch/deps/zlib/uncompr.c
$(OBJECT_DIR)zcompress.o:../src/xpack/torch/deps/zlib/zcompress.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)zcompress.o ../src/xpack/torch/deps/zlib/zcompress.c
$(OBJECT_DIR)zcrc32.o:../src/xpack/torch/deps/zlib/zcrc32.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)zcrc32.o ../src/xpack/torch/deps/zlib/zcrc32.c
$(OBJECT_DIR)zutil.o:../src/xpack/torch/deps/zlib/zutil.c
	$(CC) $(CCFLAGS) -c -o $(OBJECT_DIR)zutil.o ../src/xpack/torch/deps/zlib/zutil.c
$(OBJECT_DIR)torch-arguments.o:../src/xpack/torch/torch-arguments.cpp
	$(CXX) $(CXXFLAGS) -c -o $(OBJECT_DIR)torch-arguments.o ../src/xpack/torch/torch-arguments.cpp
$(OBJECT_DIR)torch-console.o:../src/xpack/torch/torch-console.cpp
	$(CXX) $(CXXFLAGS) -c -o $(OBJECT_DIR)torch-console.o ../src/xpack/torch/torch-console.cpp
$(OBJECT_DIR)torch-data.o:../src/xpack/torch/torch-data.cpp
	$(CXX) $(CXXFLAGS) -c -o $(OBJECT_DIR)torch-data.o ../src/xpack/torch/torch-data.cpp
$(OBJECT_DIR)torch-file.o:../src/xpack/torch/torch-file.cpp
	$(CXX) $(CXXFLAGS) -c -o $(OBJECT_DIR)torch-file.o ../src/xpack/torch/torch-file.cpp
$(OBJECT_DIR)torch-memorypool.o:../src/xpack/torch/torch-memorypool.cpp
	$(CXX) $(CXXFLAGS) -c -o $(OBJECT_DIR)torch-memorypool.o ../src/xpack/torch/torch-memorypool.cpp
$(OBJECT_DIR)torch-path.o:../src/xpack/torch/torch-path.cpp
	$(CXX) $(CXXFLAGS) -c -o $(OBJECT_DIR)torch-path.o ../src/xpack/torch/torch-path.cpp
$(OBJECT_DIR)torch-string.o:../src/xpack/torch/torch-string.cpp
	$(CXX) $(CXXFLAGS) -c -o $(OBJECT_DIR)torch-string.o ../src/xpack/torch/torch-string.cpp
$(OBJECT_DIR)torch-util.o:../src/xpack/torch/torch-util.cpp
	$(CXX) $(CXXFLAGS) -c -o $(OBJECT_DIR)torch-util.o ../src/xpack/torch/torch-util.cpp
$(OBJECT_DIR)torch-wildcard.o:../src/xpack/torch/torch-wildcard.cpp
	$(CXX) $(CXXFLAGS) -c -o $(OBJECT_DIR)torch-wildcard.o ../src/xpack/torch/torch-wildcard.cpp
$(OBJECT_DIR)xpack-base.o:../src/xpack/xpack-base.cpp
	$(CXX) $(CXXFLAGS) -c -o $(OBJECT_DIR)xpack-base.o ../src/xpack/xpack-base.cpp
$(OBJECT_DIR)xpack-block.o:../src/xpack/xpack-block.cpp
	$(CXX) $(CXXFLAGS) -c -o $(OBJECT_DIR)xpack-block.o ../src/xpack/xpack-block.cpp
$(OBJECT_DIR)xpack-content.o:../src/xpack/xpack-content.cpp
	$(CXX) $(CXXFLAGS) -c -o $(OBJECT_DIR)xpack-content.o ../src/xpack/xpack-content.cpp
$(OBJECT_DIR)xpack-context.o:../src/xpack/xpack-context.cpp
	$(CXX) $(CXXFLAGS) -c -o $(OBJECT_DIR)xpack-context.o ../src/xpack/xpack-context.cpp
$(OBJECT_DIR)xpack-hash.o:../src/xpack/xpack-hash.cpp
	$(CXX) $(CXXFLAGS) -c -o $(OBJECT_DIR)xpack-hash.o ../src/xpack/xpack-hash.cpp
$(OBJECT_DIR)xpack-header.o:../src/xpack/xpack-header.cpp
	$(CXX) $(CXXFLAGS) -c -o $(OBJECT_DIR)xpack-header.o ../src/xpack/xpack-header.cpp
$(OBJECT_DIR)xpack-name.o:../src/xpack/xpack-name.cpp
	$(CXX) $(CXXFLAGS) -c -o $(OBJECT_DIR)xpack-name.o ../src/xpack/xpack-name.cpp
$(OBJECT_DIR)xpack-signature.o:../src/xpack/xpack-signature.cpp
	$(CXX) $(CXXFLAGS) -c -o $(OBJECT_DIR)xpack-signature.o ../src/xpack/xpack-signature.cpp
$(OBJECT_DIR)xpack-stream.o:../src/xpack/xpack-stream.cpp
	$(CXX) $(CXXFLAGS) -c -o $(OBJECT_DIR)xpack-stream.o ../src/xpack/xpack-stream.cpp
$(OBJECT_DIR)xpack-util.o:../src/xpack/xpack-util.cpp
	$(CXX) $(CXXFLAGS) -c -o $(OBJECT_DIR)xpack-util.o ../src/xpack/xpack-util.cpp
$(OBJECT_DIR)xpack.o:../src/xpack/xpack.cpp
	$(CXX) $(CXXFLAGS) -c -o $(OBJECT_DIR)xpack.o ../src/xpack/xpack.cpp

clean:
	rm $(OBJECT)
