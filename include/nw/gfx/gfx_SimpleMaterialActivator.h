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

class SimpleMaterialActivator : public IMaterialActivator{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(SimpleMaterialActivator);

public:
    NW_UT_RUNTIME_TYPEINFO;

    static SimpleMaterialActivator* Create(nw::os::IAllocator* allocator);

    static size_t GetMemorySize(size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT){
        nw::os::MemorySizeCalculator size(alignment);
        
        GetMemorySizeInternal(&size);

        return size.GetSizeWithPadding(alignment);
    }

    static void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize){
        nw::os::MemorySizeCalculator& size = *pSize;
        
        size += sizeof(SimpleMaterialActivator);
    }

    virtual void Activate(RenderContext* renderContext, const Material* material);

private:

    template<typename TRes, typename URes>
    inline bool EqualHash(const TRes lhs, const URes rhs){
        return (lhs.GetHash() == rhs.GetHash());
    }

    SimpleMaterialActivator(nw::os::IAllocator* allocator);
    virtual ~SimpleMaterialActivator();
};

}
}