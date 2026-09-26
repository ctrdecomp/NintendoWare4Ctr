// Filename: os_UtilityCTR.cpp
//
// Project: NintendoWare4Ctr

#include <nw/types.h>
#include <nn/dbg/dbg_Break.h>
#include <nn/dbg/dbg_DebugString.h>

#include <cstdio>
#include <cstdarg>
#include <cstring>

namespace nw {
namespace os {
namespace internal {

__weak void VPrintf(const char *fmt, std::va_list vlist)
{
    nndbgDetailVPrintf(fmt, vlist);
}

void Halt()
{
    (void)nn::dbg::Break();
}

}
}
}