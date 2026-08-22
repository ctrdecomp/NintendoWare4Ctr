#pragma once

#define NW_VERSION_MAJOR          2
#define NW_VERSION_MINOR          5
#define NW_VERSION_MICRO          1
#define NW_VERSION_ID             0

#ifndef NW_VERSION_NUMBER
#define NW_VERSION_NUMBER(major, minor, micro, id) (((major) << 24) | ((minor) << 16) | ((micro) << 8) | ((id) << 0))

#define NW_CURRENT_VERSION_NUMBER NW_VERSION_NUMBER(NW_VERSION_MAJOR, NW_VERSION_MINOR, NW_VERSION_MICRO, NW_VERSION_ID)
#endif