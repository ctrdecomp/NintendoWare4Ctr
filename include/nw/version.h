#pragma once

#define VER_STR_(version) #version
#define VER_STR(version) VER_STR_(version)

#define NW_VERSION_NONE        0
#define NW_VERSION_MILLI4C     1
#define NW_VERSION_REDPEPPER   2
#define NW_VERSION_CTRDASH     3 /* Is not defined. Multi-Game versions. */
#define NW_VERSION_GARDEN      4
#define NW_VERSION_STICKSTR    5

#define NW_VERSION NW_VERSION_REDPEPPER

#if NW_VERSION == NW_VERSION_MILLI4C

#define NW_VERSION_MAJOR 2
#define NW_VERSION_MINOR 5
#define NW_VERSION_MICRO 1

#elif NW_VERSION == NW_VERSION_REDPEPPER

/* GUESS */

#define NW_VERSION_MAJOR 2
#define NW_VERSION_MINOR 0
#define NW_VERSION_MICRO 4

#elif NW_VERSION == NW_VERSION_CTRDASH

#define NW_VERSION_MAJOR
#define NW_VERSION_MINOR
#define NW_VERSION_MICRO

#elif NW_VERSION == NW_VERSION_GARDEN

#define NW_VERSION_MAJOR 3
#define NW_VERSION_MINOR 7
#define NW_VERSION_MICRO 4

#elif NW_VERSION == NW_VERSION_STICKSTR

#define NW_VERSION_MAJOR 2
#define NW_VERSION_MINOR
#define NW_VERSION_MICRO

#elif NW_VERSION == NW_VERSION_NONE

#define NW_VERSION_MAJOR 0
#define NW_VERSION_MINOR 0
#define NW_VERSION_MICRO 0

#endif

#ifndef NW_VERSION_NUMBER
#define NW_VERSION_NUMBER(major, minor, micro, id) (((major) << 24) | ((minor) << 16) | ((micro) << 8) | ((id) << 0))

#define NW_CURRENT_VERSION_NUMBER "NW4C_"VER_STR(NW_VERSION_MAJOR)"_"VER_STR(NW_VERSION_MINOR)"_"VER_STR(NW_VERSION_MICRO)
#endif