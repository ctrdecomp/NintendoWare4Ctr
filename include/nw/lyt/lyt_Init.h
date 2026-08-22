#pragma once

namespace nw{
namespace os{
    class IAllocator;
}

namespace lyt{
    void Initialize(os::IAllocator* pAllocator,os::IAllocator* pDeviceMemoryAllocator);
}
}