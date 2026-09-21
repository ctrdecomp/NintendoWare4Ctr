#pragma once

#include "nn/Assert.h"
#include "nw/os/os_Utility.h"

#ifdef NW_RELEASE
  #define NW_WARNING(exp, ...) ((void)0)
  #define NW_ASSERTMSG(exp, ... ) ((void)0)
  #define NW_POINTER_ASSERT(exp)                                                                                            \
              NW_ASSERTMSG(                                                                                                 \
                    (((exp) & 0xFF000000) == 0x80000000)     \
                  || (((exp) & 0xFF800000) == 0x81000000)    \
                  || (((exp) & 0xF8000000) == 0x90000000)    \
                  || (((exp) & 0xFF000000) == 0xC0000000)    \
                  || (((exp) & 0xFF800000) == 0xC1000000)    \
                  || (((exp) & 0xF8000000) == 0xD0000000)    \
                  || (((exp) & 0xFFFFC000) == 0xE0000000),   \
                  "NW:Pointer Error\n"#exp"(=%p) is not valid pointer.", (exp) )

  #define NW_GL_ASSERT() ((void)0)
  #define NW_FATAL_ERROR    true ? 0:
#else
  #define NW_WARNING(exp, ...) (void) ((exp) || (nwosWarning(__VA_ARGS__), 0))
  #define NW_ASSERTMSG(exp, ... ) (void) ((exp) || (nwosPanic(__VA_ARGS__), 0))
  #define NW_POINTER_ASSERT(exp)                                                                                            \
              NW_ASSERTMSG(                                                                                                 \
                  exp != 0,                                                                            \
                  "NW:Pointer Error\n"#exp"(=%p) is not valid pointer.", (exp) )
  #define NW_FATAL_ERROR(...) \
              NW_ASSERTMSG( false, "NW:Fatal Error\n"__VA_ARGS__ )
  #define NW_GL_ASSERT()                                              \
    do {                                                            \
        GLuint err = glGetError();                                  \
        NW_ASSERTMSG( err == GL_NO_ERROR, "GL_ERROR : %s (0x%x)", nwGlErrorString( err ), err ); \
    } while (0)                                                     
#endif

// ASSERT

#ifndef NW_ASSERT
#define NW_ASSERT(exp) \
            NW_ASSERTMSG( (exp), "NW:Failed assertion " #exp )
#endif

// MIN VS MAX ASSERT

#ifndef NW_MINMAX_ASSERT
#define NW_MINMAX_ASSERT(exp, min, max) \
            NW_ASSERTMSG((exp) >= (min) && (exp) <= (max), #exp " is out of bounds(%d)\n%d <= "#exp" <= %d not satisfied.", static_cast<int>(exp), static_cast<int>(min), static_cast<int>(max))
#endif

#ifndef NW_MINMAXLT_ASSERT
#define NW_MINMAXLT_ASSERT(exp, min, max) \
            NW_ASSERTMSG((exp) >= (min) && (exp) < (max), #exp " is out of bounds(%d)\n%d <= "#exp" < %d not satisfied.", static_cast<int>(exp), static_cast<int>(min), static_cast<int>(max))
#endif

// NULL ASSERT

#ifndef NW_NULL_ASSERT
#define NW_NULL_ASSERT(exp) \
            NW_ASSERTMSG((exp) != NULL, "NW:Pointer must not be NULL ("#exp")")
#endif



// FAILSAFE ASSERT

#if !defined(NW_RELEASE)
#define NW_FAILSAFE_IF(exp) if (exp)
#else
#define NW_FAILSAFE_IF(exp) if (exp) 
{ NW_FATAL_ERROR(#exp); } if (false)
#endif