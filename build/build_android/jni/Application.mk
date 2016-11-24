APP_STL := gnustl_static
APP_CPPFLAGS := -frtti -DCC_ENABLE_CHIPMUNK_INTEGRATION=1 -std=c++11 -fsigned-char -D_FILE_OFFSET_BITS=64
APP_OPTIM := release
# APP_ABI := all # or armeabi
APP_ABI := armeabi armeabi-v7a x86
APP_MODULES := libxpack
NDK_TOOLCHAIN_VERSION=4.8
