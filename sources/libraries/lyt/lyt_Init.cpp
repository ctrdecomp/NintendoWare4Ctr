// Filename: lyt_Init.cpp
//
// Project: NintendoWare4Ctr

#include <nw/lyt/lyt_Init.h>
#include <nw/lyt/lyt_Layout.h>
#include <nw/ut/ut_MiddlewareString.h>
#include <nn/module.h>
#include <nw/version.h>

namespace nw{
namespace lyt{


void Initialize(nw::os::IAllocator* pAllocator,nw::os::IAllocator* pDeviceMemoryAllocator)
{
#if defined(NW_VERSION_USE_LYT1)
    NW_PUT_MODULE_SYMBOL(lyt1)
#elif defined(NW_VERSION_USE_LYT2)
    NW_PUT_MODULE_SYMBOL(lyt2)
#endif
    Layout::SetAllocator(pAllocator);
    Layout::SetDeviceMemoryAllocator(pDeviceMemoryAllocator);
}

}
}