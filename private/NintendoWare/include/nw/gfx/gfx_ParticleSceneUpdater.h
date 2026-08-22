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

#ifndef NW_GFX_PARTICLESCENEUPDATER_H_
#define NW_GFX_PARTICLESCENEUPDATER_H_

#include <nw/gfx/gfx_ISceneUpdater.h>
#include <nw/gfx/gfx_SkeletonUpdater.h>
#include <nw/gfx/gfx_WorldMatrixUpdater.h>
#include <nw/gfx/gfx_BillboardUpdater.h>

namespace nw
{
namespace gfx
{

class SceneContext;
class ParticleContext;
class ParticleSet;

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ParticleSceneUpdater  : public GfxObject
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(ParticleSceneUpdater);

public:
    NW_UT_RUNTIME_TYPEINFO;

    //
    typedef ut::MoveArray<SceneNode*> SceneNodeArray;

    //
    typedef ut::MoveArray<ParticleSet*> ParticleSetArray;

    //----------------------------------------
    //
    //

    //
    class Builder
    {
    public:
        //
        Builder() {}

        //
        //
        //
        //
        //
        //
        ParticleSceneUpdater* Create(os::IAllocator* allocator);
    };

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    //
    void SetStepFrame(
        SceneContext* sceneContext,
        f32 stepFrame);

    //
    //
    //
    //
    //
    //
    virtual void UpdateNode(
        SceneContext* sceneContext,
        ParticleContext* particleContext,
        bool enableSwapBuffer = true);

    //

protected:
    //----------------------------------------
    //
    //

    //
    ParticleSceneUpdater(os::IAllocator* allocator)
    : GfxObject(allocator)
    {}

    //
    virtual ~ParticleSceneUpdater() {}

    //
};

} // namespace gfx
} // namespace nw

#endif // NW_GFX_SCENEUPDATER_H_
