#pragma once

// define __linux__ for android system
#ifdef __ANDROID__
#ifndef __linux__
#define __linux__
#endif // !__linux__
#endif // __ANDROID__

#include <lzham.h>

// add arch specific defines for android
#ifdef __ANDROID__

#undef LZHAM_PLATFORM_PC_X64
#define LZHAM_PLATFORM_PC_X64 0

#ifdef __arm__

#undef LZHAM_64BIT_POINTERS
#undef LZHAM_CPU_HAS_64BIT_REGISTERS
#define LZHAM_64BIT_POINTERS 0
#define LZHAM_CPU_HAS_64BIT_REGISTERS 0

#endif // __arm__

#ifdef __aarch64__

#undef LZHAM_64BIT_POINTERS
#undef LZHAM_CPU_HAS_64BIT_REGISTERS
#define LZHAM_64BIT_POINTERS 1
#define LZHAM_CPU_HAS_64BIT_REGISTERS 1

#endif // __aarch64__

#endif // __ANDROID__
