//
//  Torch
//
//  Created by Luwei.
//  Copyright (c) 2014-2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy/Torch
//

#ifndef __TORCH__DEF__
#define __TORCH__DEF__

#include <assert.h>

// Platform headers for feature detection below.
#if defined(__ANDROID__)
# include <sys/cdefs.h>
#elif defined(__APPLE__)
# include <TargetConditionals.h>
#elif defined(__linux__)
# include <features.h>
#endif

// -----------------------------------------------------------------------------
// Operating system detection
//
//  TORCH_OS_ANDROID       - Android
//  TORCH_OS_LINUX         - Linux
//  TORCH_OS_APPLE         - Apple OS(iOS, Mac OS X)
//  TORCH_OS_WIN           - Microsoft Windows

#if defined(__ANDROID__)
# define TORCH_OS_ANDROID 	1
# define TORCH_OS_LINUX 	1
#elif defined(__APPLE__)
# define TORCH_OS_APPLE 	1
#elif defined(__linux__)
# define TORCH_OS_LINUX 	1
#elif defined(_WIN32)
# define TORCH_OS_WIN 		1
#endif



// -----------------------------------------------------------------------------
// Operating system detection
//
//  TORCH_OS_ANDROID       - Android
//  TORCH_OS_BSD           - BSDish (Mac OS X, Net/Free/Open/DragonFlyBSD)
//  TORCH_OS_CYGWIN        - Cygwin
//  TORCH_OS_FREEBSD       - FreeBSD
//  TORCH_OS_LINUX         - Linux
//  TORCH_OS_MACOSX        - Mac OS X
//  TORCH_OS_NETBSD        - NetBSD
//  TORCH_OS_OPENBSD       - OpenBSD
//  TORCH_OS_POSIX         - POSIX compatible (mostly everything except Windows)
//  TORCH_OS_WIN           - Microsoft Windows

#if defined(__ANDROID__)
# define TORCH_OS_ANDROID 1
# define TORCH_OS_LINUX 1
# define TORCH_OS_POSIX 1
#elif defined(__APPLE__)
# define TORCH_OS_BSD 1
# define TORCH_OS_MACOSX 1
# define TORCH_OS_POSIX 1
#elif defined(__CYGWIN__)
# define TORCH_OS_POSIX 1
#elif defined(__linux__)
# define TORCH_OS_LINUX 1
# define TORCH_OS_POSIX 1
#elif defined(__sun)
# define TORCH_OS_POSIX 1
#elif defined(__FreeBSD__)
# define TORCH_OS_BSD 1
# define TORCH_OS_FREEBSD 1
# define TORCH_OS_POSIX 1
#elif defined(__NetBSD__)
# define TORCH_OS_BSD 1
# define TORCH_OS_POSIX 1
#elif defined(__OpenBSD__)
# define TORCH_OS_BSD 1
# define TORCH_OS_OPENBSD 1
# define TORCH_OS_POSIX 1
#elif defined(_WIN32)
# define TORCH_OS_WIN 1
#endif


#define TORCH_TOSTRING(__v) #__v

#endif
