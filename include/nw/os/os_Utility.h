#pragma once

#include <nn/Assert.h>

#include <cstdarg>

#ifdef NW_RELEASE
#define nwosPrintf(...)  ((void)0)
#define nwosVPrintf(...) ((void)0)
#define nwosPanic(...)   ::nw::os::Halt()
#define nwosWarning(...) ((void)0)
#else
#define nwosPrintf       ::nw::os::internal::Printf
#define nwosVPrintf      ::nw::os::internal::VPrintf
#define nwosPanic(...)   ::nw::os::internal::Panic(NN_FILE_NAME, __LINE__, __VA_ARGS__)
#define nwosWarning(...) ::nw::os::internal::Warning(NN_FILE_NAME, __LINE__, __VA_ARGS__)
#endif

namespace nw {
namespace os {
namespace internal {

__weak void Printf(const char* fmt, ...);
__weak void VPrintf(const char* fmt, std::va_list vlist);
__weak void Warning(const char* fileName, int line, const char* fmt, ...);
__weak void Panic(const char* fileName, int line, const char* fmt, ...);
void Halt();

}
}
}