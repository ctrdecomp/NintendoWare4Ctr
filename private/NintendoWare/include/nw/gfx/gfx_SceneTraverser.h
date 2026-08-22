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

#ifndef NW_GFX_SCENETRAVERSER_H_
#define NW_GFX_SCENETRAVERSER_H_

#include <nw/gfx/gfx_ISceneVisitor.h>

namespace nw
{
namespace gfx
{

class SceneContext;

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class SceneTraverser  : public ISceneVisitor
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(SceneTraverser);

public:
    NW_UT_RUNTIME_TYPEINFO;

    //
    class Builder
    {
    public:
        //
        //
        //
        //
        //
        //
        SceneTraverser* Create(os::IAllocator* allocator);
    };

    //
    //
    //
    //
    //
    //
    void Begin(SceneContext* sceneContext);

    //
    void End();

    //----------------------------------------
    //
    //

    virtual void VisitSceneNode(SceneNode* node);
    virtual void VisitTransformNode(TransformNode* node);
    virtual void VisitUserRenderNode(UserRenderNode* node);
    virtual void VisitModel(Model* model);
    virtual void VisitSkeletalModel(SkeletalModel* model);
    virtual void VisitCamera(Camera* camera);
    virtual void VisitFog(Fog* fog);
    virtual void VisitLight(Light* light);
    virtual void VisitFragmentLight(FragmentLight* light);
#if defined(NW_GFX_VERTEX_LIGHT_ENABLED)
    virtual void VisitVertexLight(VertexLight* light);
#endif
    virtual void VisitAmbientLight(AmbientLight* light);
    virtual void VisitHemiSphereLight(HemiSphereLight* light);
    virtual void VisitParticleSet(ParticleSet* particleSet);
    virtual void VisitParticleEmitter(ParticleEmitter* particleEmitter);
    virtual void VisitParticleModel(ParticleModel* particleModel);

    //

private:
    SceneTraverser(os::IAllocator* allocator)
    : ISceneVisitor(allocator), m_SceneContext(NULL) {}
    virtual ~SceneTraverser() {}

    SceneContext* m_SceneContext;
};

} // namespace gfx
} // namespace nw

#endif // NW_GFX_SCENETRAVERSER_H_
