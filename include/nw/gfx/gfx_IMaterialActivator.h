#pragma once

#include <nw/gfx/gfx_GfxObject.h>
#include <nw/gfx/gfx_IMaterialIdGenerator.h>

namespace nw{
namespace os{
    class IAllocator;
}
namespace gfx{

class RenderContext;
class Material;

class IMaterialActivator : public GfxObject
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(IMaterialActivator);

public:
    NW_UT_RUNTIME_TYPEINFO;

    virtual void Activate(RenderContext* renderContext, const Material* material) = 0;
protected:
    IMaterialActivator(nw::os::IAllocator* allocator) : GfxObject(allocator) {}

    virtual ~IMaterialActivator() = 0;
};

}
}