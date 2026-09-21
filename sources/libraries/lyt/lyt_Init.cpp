// Filename: lyt_Init.cpp
//
// Project: NintendoWare4Ctr

#include <nw/lyt/lyt_Init.h>
#include <nw/lyt/lyt_Layout.h>
#include <nn/module.h>
#include <nw/version.h>

namespace nw{
namespace lyt{


void Initialize(nw::os::IAllocator* pAllocator,nw::os::IAllocator* pDeviceMemoryAllocator)
{
#if NW_VERSION_MAJOR > 3
    NN_MAKE_MODULE(moduleInfo, "NINTENDO", NW_CURRENT_VERSION_NUMBER "_lyt1");
    NN_REFER_MODULE(moduleInfo);
#endif
    Layout::SetAllocator(pAllocator);
    Layout::SetDeviceMemoryAllocator(pDeviceMemoryAllocator);
}

}
}