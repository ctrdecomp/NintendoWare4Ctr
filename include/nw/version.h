#pragma once

#define VER_STR_(version) #version
#define VER_STR(version) VER_STR_(version)

#define NW_VERSION_MAJOR          2
#define NW_VERSION_MINOR          5
#define NW_VERSION_MICRO          1

#ifndef NW_VERSION_NUMBER
#define NW_VERSION_NUMBER(major, minor, micro, id) (((major) << 24) | ((minor) << 16) | ((micro) << 8) | ((id) << 0))

#define NW_CURRENT_VERSION_NUMBER "NW4C_"VER_STR(NW_VERSION_MAJOR)"_"VER_STR(NW_VERSION_MINOR)"_"VER_STR(NW_VERSION_MICRO)
#endif