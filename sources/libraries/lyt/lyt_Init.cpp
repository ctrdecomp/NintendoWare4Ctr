// Filename: lyt_Init.cpp
//
// Project: NintendoWare4Ctr

#include <nw/lyt/lyt_Init.h>
#include <nw/lyt/lyt_Layout.h>

namespace nw{
namespace lyt{

void Initialize(nw::os::IAllocator* pAllocator,nw::os::IAllocator* pDeviceMemoryAllocator)
{
    Layout::SetAllocator(pAllocator);
    Layout::SetDeviceMemoryAllocator(pDeviceMemoryAllocator);
}

}
}