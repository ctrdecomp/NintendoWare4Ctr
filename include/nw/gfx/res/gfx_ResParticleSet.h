#pragma once

#include <nw/ut/ut_ResUtil.h>
#include <nw/gfx/res/gfx_ResSceneObject.h>
#include <nw/gfx/res/gfx_ResParticleCollection.h>
#include <nw/gfx/res/gfx_ResParticleShape.h>
#include <nw/gfx/res/gfx_ResParticleInitializer.h>
#include <nw/gfx/res/gfx_ResParticleUpdater.h>
#include <nw/gfx/res/gfx_ResParticleShapeBuilder.h>
#include <nw/gfx/res/gfx_ResTypeInfo.h>

namespace nw {
namespace gfx {
namespace res {

typedef nw::ut::ResArrayClass<ResParticleInitializer>::type::iterator ResParticleInitializerArrayIterator;
typedef nw::ut::ResArrayClass<const ResParticleInitializer>::type::const_iterator ResParticleInitializerArrayConstIterator;
typedef nw::ut::ResArrayClass<ResParticleInitializer>::type ResParticleInitializerArray;
typedef nw::ut::ResArrayClass<const ResParticleInitializer>::type ResParticleInitializerArrayConst;

typedef nw::ut::ResArrayClass<ResParticleUpdater>::type::iterator ResParticleUpdaterArrayIterator;
typedef nw::ut::ResArrayClass<const ResParticleUpdater>::type::const_iterator ResParticleUpdaterArrayConstIterator;
typedef nw::ut::ResArrayClass<ResParticleUpdater>::type ResParticleUpdaterArray;
typedef nw::ut::ResArrayClass<const ResParticleUpdater>::type ResParticleUpdaterArrayConst;
struct ResParticleSetData : public ResSceneNodeData
{
    nw::ut::Offset toParticleCollection;
    nw::ut::ResS32 m_ParticleInitializersTableCount;
    nw::ut::Offset toParticleInitializersTable;
    nw::ut::ResS32 m_ParticleUpdatersTableCount;
    nw::ut::Offset toParticleUpdatersTable;
    nw::ut::Offset toParticleShapeBuilder;
    nw::ut::ResS32 m_ParticleShapeIndex;
    nw::ut::ResU32 m_ParticleSetPriority;
    nw::ut::ResBool m_IsForceWorld;
    nw::ut::ResBool m_IsBufferFlushEnabled;
    nw::ut::ResBool m_IsInheritParentVelocity;
};

class ResParticleSet : public ResSceneNode
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleSet) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('PSET') };

    NW_RES_CTOR_INHERIT( ResParticleSet, ResSceneNode )

    NW_RES_FIELD_CLASS_DECL(ResParticleCollection, ParticleCollection)
    NW_RES_FIELD_CLASS_LIST_DECL(ResParticleInitializer, ParticleInitializers)
    NW_RES_FIELD_CLASS_LIST_DECL(ResParticleUpdater, ParticleUpdaters )
    NW_RES_FIELD_CLASS_DECL(ResParticleShapeBuilder, ParticleShapeBuilder)
    NW_RES_FIELD_PRIMITIVE_DECL(s32, ParticleShapeIndex)
    NW_RES_FIELD_PRIMITIVE_DECL(u32, ParticleSetPriority)
    NW_RES_FIELD_PRIMITIVE_DECL(bool, IsForceWorld)
    NW_RES_FIELD_PRIMITIVE_DECL(bool, IsBufferFlushEnabled)
    NW_RES_FIELD_PRIMITIVE_DECL(bool, IsInheritParentVelocity)

    void    Setup();
};
typedef nw::ut::ResArrayPatricia<ResParticleSet>::type  ResParticleSetArray;

}
}
}