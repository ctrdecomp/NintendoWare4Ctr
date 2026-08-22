// Filename: os_Utility.cpp
//
// Project: NintendoWare4Ctr

#include <nw/types.h>
#include <nw/os/os_Utility.h>

namespace nw {
namespace os {
namespace internal {

void Printf(const char *fmt, ...){
    std::va_list vlist;
    va_start(vlist, fmt);
    
    VPrintf(fmt, vlist);
    
    va_end(vlist);
}

void Warning(const char* fileName, int line, const char* fmt, ...){
    va_list vlist;
    va_start(vlist, fmt);
    
    Printf("%s:%d Warning:", fileName, line);
    VPrintf(fmt, vlist);
    Printf("\n");
    
    va_end(vlist);
}

void Panic( const char* fileName, int line, const char* fmt, ...){
    std::va_list vlist;
    va_start(vlist, fmt);

    Printf("%s:%d Panic:", fileName, line);
    VPrintf(fmt, vlist);
    Printf("\n");
    
    va_end(vlist);
    
    Halt();
}

}
}
}