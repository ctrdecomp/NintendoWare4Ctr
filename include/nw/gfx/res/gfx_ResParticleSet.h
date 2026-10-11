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


    ResParticleCollection GetParticleCollection() { return ResParticleCollection(ref().toParticleCollection.to_ptr()); }
    const ResParticleCollection GetParticleCollection() const { return ResParticleCollection(ref().toParticleCollection.to_ptr()); }

    s32 GetParticleInitializersCount() const
        { return ref().m_ParticleInitializersTableCount; }
    ResParticleInitializer GetParticleInitializers(int idx)
        {
            nw::ut::Offset* tbl_ptr = static_cast<nw::ut::Offset*>(ref().toParticleInitializersTable.to_table_ptr());
            return ResParticleInitializer(tbl_ptr[idx].to_ptr());
        }
    const ResParticleInitializer GetParticleInitializers(int idx) const
        {
            const nw::ut::Offset* tbl_ptr = static_cast<const nw::ut::Offset*>(ref().toParticleInitializersTable.to_table_ptr());
            return ResParticleInitializer(tbl_ptr[idx].to_ptr());
        }
    nw::ut::internal::ResArray<ResParticleInitializer, nw::ut::internal::ResArrayClassTraits> GetParticleInitializers()
        {
            return nw::ut::internal::ResArray<ResParticleInitializer, nw::ut::internal::ResArrayClassTraits>(ref().toParticleInitializersTable.to_table_ptr(), GetParticleInitializersCount());
        }
    const nw::ut::internal::ResArray<const ResParticleInitializer, nw::ut::internal::ResArrayClassTraits> GetParticleInitializers() const
        {
            return nw::ut::internal::ResArray<const ResParticleInitializer, nw::ut::internal::ResArrayClassTraits>(ref().toParticleInitializersTable.to_table_ptr(), GetParticleInitializersCount());
        }

    s32 GetParticleUpdatersCount() const
        { return ref().m_ParticleUpdatersTableCount; }
    ResParticleUpdater GetParticleUpdaters(int idx)
        {
            nw::ut::Offset* tbl_ptr = static_cast<nw::ut::Offset*>(ref().toParticleUpdatersTable.to_table_ptr());
            return ResParticleUpdater(tbl_ptr[idx].to_ptr());
        }
    const ResParticleUpdater GetParticleUpdaters(int idx) const
        {
            const nw::ut::Offset* tbl_ptr = static_cast<const nw::ut::Offset*>(ref().toParticleUpdatersTable.to_table_ptr());
            return ResParticleUpdater(tbl_ptr[idx].to_ptr());
        }
    nw::ut::internal::ResArray<ResParticleUpdater, nw::ut::internal::ResArrayClassTraits> GetParticleUpdaters()
        {
            return nw::ut::internal::ResArray<ResParticleUpdater, nw::ut::internal::ResArrayClassTraits>(ref().toParticleUpdatersTable.to_table_ptr(), GetParticleUpdatersCount());
        }
    const nw::ut::internal::ResArray<const ResParticleUpdater, nw::ut::internal::ResArrayClassTraits> GetParticleUpdaters() const
        {
            return nw::ut::internal::ResArray<const ResParticleUpdater, nw::ut::internal::ResArrayClassTraits>(ref().toParticleUpdatersTable.to_table_ptr(), GetParticleUpdatersCount());
        }

    ResParticleShapeBuilder GetParticleShapeBuilder() { return ResParticleShapeBuilder(ref().toParticleShapeBuilder.to_ptr()); }
    const ResParticleShapeBuilder GetParticleShapeBuilder() const { return ResParticleShapeBuilder(ref().toParticleShapeBuilder.to_ptr()); }

    s32 GetParticleShapeIndex() const { return static_cast<s32>(ref().m_ParticleShapeIndex); }
    void SetParticleShapeIndex(s32 value) { ref().m_ParticleShapeIndex = value; }

    u32 GetParticleSetPriority() const { return static_cast<u32>(ref().m_ParticleSetPriority); }
    void SetParticleSetPriority(u32 value) { ref().m_ParticleSetPriority = value; }

    bool GetIsForceWorld() const { return static_cast<bool>(ref().m_IsForceWorld); }
    void SetIsForceWorld(bool value) { ref().m_IsForceWorld = value; }

    bool GetIsBufferFlushEnabled() const { return static_cast<bool>(ref().m_IsBufferFlushEnabled); }
    void SetIsBufferFlushEnabled(bool value) { ref().m_IsBufferFlushEnabled = value; }

    bool GetIsInheritParentVelocity() const { return static_cast<bool>(ref().m_IsInheritParentVelocity); }
    void SetIsInheritParentVelocity(bool value) { ref().m_IsInheritParentVelocity = value; }

    void    Setup();
};
typedef nw::ut::ResArrayPatricia<ResParticleSet>::type  ResParticleSetArray;

}
}
}

