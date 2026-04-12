#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#ifndef NULL
#define NULL (void*)0
#endif

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

typedef unsigned short ushort;
typedef unsigned int uint;

typedef signed char        s8;
typedef signed short       s16;
typedef signed int         s32;
typedef signed long long   s64;

typedef u32 uptr;
typedef s32 ptr;

typedef float              f32;
typedef double             f64;

typedef volatile u8       vu8;
typedef volatile u16      vu16;
typedef volatile u32      vu32;
typedef volatile u64      vu64;

typedef volatile s8       vs8;
typedef volatile s16      vs16;
typedef volatile s32      vs32;
typedef volatile s64      vs64;

typedef volatile f32      vf32;
typedef volatile f64      vf64;

typedef unsigned char           bit8;
typedef unsigned short          bit16;
typedef unsigned int            bit32;
typedef unsigned long long int  bit64; 

#ifndef nullptr
#define nullptr NULL
#endif