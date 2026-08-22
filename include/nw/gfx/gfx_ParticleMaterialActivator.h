#pragma once

#include <nw/gfx/gfx_GfxObject.h>
#include <nw/gfx/gfx_IMaterialActivator.h>

namespace nw{
namespace os{
    class IAllocator;
}
namespace gfx{

class RenderContext;
class Material;

class ParticleMaterialActivator : public IMaterialActivator{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(ParticleMaterialActivator);

public:
    NW_UT_RUNTIME_TYPEINFO;

    static ParticleMaterialActivator* Create(nw::os::IAllocator* allocator);
    virtual void Activate(RenderContext* renderContext, const Material* material);

private:
    ParticleMaterialActivator(nw::os::IAllocator* allocator);
    virtual ~ParticleMaterialActivator();
};

}
}