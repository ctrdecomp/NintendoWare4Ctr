#pragma once

#include <nw/gfx/gfx_GfxObject.h>
#include <nw/gfx/gfx_Skeleton.h>
#include <nw/gfx/gfx_CalculatedTransform.h>

namespace nw{
namespace os{
    class IAllocator;
}
namespace gfx{
class Skeleton;
class WorldMatrixUpdater;
class BillboardUpdater;
class Camera;

class SkeletonUpdater : public GfxObject
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(SkeletonUpdater);

public:
    class Builder
    {
    public:
        Builder() {}

        SkeletonUpdater* Create(nw::os::IAllocator* allocator);
    };

    void UpdateWorld(Skeleton* skeleton,const WorldMatrixUpdater& worldMatrixUpdater) const;

    void UpdateView(Skeleton* skeleton,const BillboardUpdater& billboardUpdater,const Camera& camera) const;

private:
    SkeletonUpdater(nw::os::IAllocator* allocator);
    virtual ~SkeletonUpdater();
};
}
}