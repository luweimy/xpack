#!/bin/sh

## 1. copy code from src/xpack/
## 2. build android

sources_path_jni=./jni/xpack/
sources_path_xpack=../../src/xpack/

test ! -d $sources_path_jni && mkdir $sources_path_jni

rm -rf $sources_path_jni/* && \
cp -R -v $sources_path_xpack* $sources_path_jni/ && \
ndk-build NDK_PROJECT_PATH=./ $@

