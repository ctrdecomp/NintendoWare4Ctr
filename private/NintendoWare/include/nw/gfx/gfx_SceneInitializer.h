/*---------------------------------------------------------------------------*
  Project:  NintendoWare

  Copyright (C)Nintendo/HAL Laboratory, Inc.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary
  information of Nintendo and/or its licensed developers and are protected by
  national and international copyright laws. They may not be disclosed to third
  parties or copied or duplicated in any form, in whole or in part, without the
  prior written consent of Nintendo.

  The content herein is highly confidential and should be handled accordingly.
 *---------------------------------------------------------------------------*/

#ifndef NW_GFX_SCENEINITIALIZER_H_
#define NW_GFX_SCENEINITIALIZER_H_

#include <nw/gfx/gfx_ISceneVisitor.h>

#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 2530 // padding added to end of struct.
#endif
namespace nw
{
namespace gfx
{

class IMaterialIdGenerator;

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class SceneInitializer : public ISceneVisitor
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(SceneInitializer);

public:
    NW_UT_RUNTIME_TYPEINFO;

    //
    struct Description
    {
        IMaterialIdGenerator* materialIdGenerator; //

        //
        Description()
         : materialIdGenerator(NULL)
        {}
    };

    //
    class Builder
    {
    public:
        //
        //
        Builder& MaterialIdGenerator(IMaterialIdGenerator* materialIdGenerator)
        {
            m_Description.materialIdGenerator = materialIdGenerator;
            return *this;
        }

        //
        //
        //
        //
        //
        //
        SceneInitializer* Create(os::IAllocator* allocator);

    private:
        Description m_Description;
    };

    //
    //
    void Begin();

    //
    //
    //
    //
    void End();

    //----------------------------------------
    //
    //

    virtual void VisitSceneNode(SceneNode*) {}
    virtual void VisitTransformNode(TransformNode*) {}
    virtual void VisitUserRenderNode(UserRenderNode*) {}
    virtual void VisitModel(Model* model);
    virtual void VisitSkeletalModel(SkeletalModel* model);
    virtual void VisitCamera(Camera*) {}
    virtual void VisitFog(Fog*) {}
    virtual void VisitLight(Light*) {}
    virtual void VisitFragmentLight(FragmentLight*) {}
#if defined(NW_GFX_VERTEX_LIGHT_ENABLED)
    virtual void VisitVertexLight(VertexLight*) {}
#endif
    virtual void VisitAmbientLight(AmbientLight*) {}
    virtual void VisitHemiSphereLight(HemiSphereLight*) {}
    virtual void VisitParticleSet(ParticleSet*) {}
    virtual void VisitParticleEmitter(ParticleEmitter*) {}
    virtual void VisitParticleModel(ParticleModel*);

    //

private:

    SceneInitializer(os::IAllocator* allocator, Description description)
        : ISceneVisitor(allocator),
          m_MaterialIdGenerator(description.materialIdGenerator)
    {}

    virtual ~SceneInitializer() {}

    GfxPtr<IMaterialIdGenerator> m_MaterialIdGenerator;
};

} // namespace gfx
} // namespace nw
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 2530 // padding added to end of struct.
#endif

#endif // NW_GFX_SCENEINITIALIZER_H_
