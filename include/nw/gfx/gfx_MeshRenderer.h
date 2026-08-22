#pragma once

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/res/gfx_ResModel.h>
#include <nw/gfx/res/gfx_ResShape.h>
#include <nw/gfx/res/gfx_ResParticleShape.h>
#include <nw/gfx/res/gfx_ResMaterial.h>
#include <nw/gfx/res/gfx_ResMesh.h>

namespace nw{
namespace os{
    class IAllocator;
}
namespace gfx{

class Model;
class SkeletalModel;
class Mesh;
class RenderContext;
class PrimitiveSet;
class Primitive;

class MeshRenderer : public GfxObject{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(MeshRenderer);

public:
    static MeshRenderer* Create(nw::os::IAllocator* pAllocator);

    void SetRenderContext(RenderContext* renderContext){
        mRenderContext = renderContext;
    }
    
    void RenderMesh(ResMesh mesh, Model* model);

private:    
    MeshRenderer(nw::os::IAllocator* allocator): 
        GfxObject(allocator),
        mRenderContext(NULL) 
    {}

    virtual ~MeshRenderer() {}

    void RenderSeparateDataShape(Model* model,ResSeparateDataShape shape,s32 currentPrimitiveIndex);

    void RenderParticleShape(Model* model,ResParticleShape shape,int index);

    void SetMatrixPalette(SkeletalModel* skeletalModel,ResPrimitiveSet primitiveSet,s32 boneIndexCount);

    RenderContext*      mRenderContext;
};

}
}