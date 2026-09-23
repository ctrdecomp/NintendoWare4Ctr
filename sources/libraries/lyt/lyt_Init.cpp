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
    NW_PUT_MODULE_SYMBOL(lyt2);
    Layout::SetAllocator(pAllocator);
    Layout::SetDeviceMemoryAllocator(pDeviceMemoryAllocator);
}

}
}