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
#ifndef NW_GFX_PARTICLE_UTIL_H_
#define NW_GFX_PARTICLE_UTIL_H_

#include <nw/types.h>

#include <nw/gfx/res/gfx_ResParticleModel.h>
#include <nw/gfx/res/gfx_ResParticleSet.h>

namespace nw {
namespace gfx {

class SceneNode;
class ParticleContext;

namespace res {
class ResParticleInitializer;
struct ResParticleInitializerData;
class ResParticleUpdater;
struct ResParticleUpdaterData;
} // namespace res

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ParticleUtil
{
public:
    //
    //
    //
    //
    static void SetupParticleObject(
        ut::MoveArray<SceneNode*>* sceneNodeArray,
        ParticleContext* context);

    //
    //
    //
    static void ResolveParticleObject(ut::MoveArray<SceneNode*>* sceneNodeArray);

    //
    static ResParticleInitializerData* DuplicateResParticleInitializer(
        const ResParticleInitializer* src,
        os::IAllocator* allocator);

    //
    static void GetMemorySizeForDuplicateResParticleInitializerInternal(
        os::MemorySizeCalculator* pSize,
        const ResParticleInitializer* src);

    //
    static ResParticleUpdaterData* DuplicateResParticleUpdater(
        const ResParticleUpdater* src,
        os::IAllocator* allocator);

    //
    static void GetMemorySizeForDuplicateResParticleUpdaterInternal(
        os::MemorySizeCalculator* pSize,
        const ResParticleUpdater* src);
};

//
struct ParticleSetCompare
    : public std::binary_function<ParticleSet*, ParticleSet*, bool>
{
    //
    //
    //
    //
    bool operator() (
        const ParticleSet* lhs,
        const ParticleSet* rhs);
};

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ParticleSetIsBufferFlushEnabledSetter
{
public:
    //
    //
    ParticleSetIsBufferFlushEnabledSetter(bool flag) : m_Flag(flag) {}

    //
    //
    void operator()(ResModel resModel) const
    {
        ResParticleModel resParticleModel = ResDynamicCast<ResParticleModel>(resModel);
        if (resParticleModel.IsValid())
        {
            resParticleModel.ForeachParticleSet(ParticleSetIsBufferFlushEnabledSetter(m_Flag));
        }
    }

    //
    //
    void operator()(ResParticleModel resParticleModel) const
    {
        if (resParticleModel.IsValid())
        {
            resParticleModel.ForeachParticleSet(ParticleSetIsBufferFlushEnabledSetter(m_Flag));
        }
    }

    //
    //
    void operator()(ResParticleSet resParticleSet) const
    {
        resParticleSet.SetIsBufferFlushEnabled(m_Flag);
    }

private:
    bool m_Flag;
};

} // namespace gfx
} // namespace nw

#endif // NW_GFX_PARTICLE_UTIL_H_
