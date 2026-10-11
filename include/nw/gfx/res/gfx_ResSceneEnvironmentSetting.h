#pragma once

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>

#include <nw/gfx/res/gfx_ResSceneObject.h>
#include <nw/gfx/res/gfx_ResRevision.h>
#include <nw/gfx/res/gfx_ResFog.h>

namespace nw{
namespace gfx{
namespace res{

class ResCamera;
class ResLight;

struct ResReferenceSceneObjectData
{
    nw::ut::ResS32 m_Index;
    nw::ut::BinString toPath;
    nw::ut::Offset toTarget;
};

struct ResLightSetData
{
    nw::ut::ResS32 m_Index;
    nw::ut::ResS32 m_LightsTableCount;
    nw::ut::Offset toLightsTable;
};

struct ResSceneEnvironmentSettingData : public ResSceneObjectData
{
    nw::ut::ResS32 m_CamerasTableCount;
    nw::ut::Offset toCamerasTable;
    nw::ut::ResS32 m_LightSetsTableCount;
    nw::ut::Offset toLightSetsTable;
    nw::ut::ResS32 m_FogsTableCount;
    nw::ut::Offset toFogsTable;
};

class ResReferenceSceneObject : public nw::ut::ResCommon<ResReferenceSceneObjectData>
{
public:
    NW_RES_CTOR( ResReferenceSceneObject )
    

    s32 GetIndex() const { return static_cast<s32>(ref().m_Index); }
    void SetIndex(s32 value) { ref().m_Index = value; }


    const char* GetPath() const { return ref().toPath.to_ptr(); }


    ResSceneObject GetTarget() { return ResSceneObject(ref().toTarget.to_ptr()); }
    const ResSceneObject GetTarget() const { return ResSceneObject(ref().toTarget.to_ptr()); }
};

class ResLightSet : public nw::ut::ResCommon<ResLightSetData>
{
public:
    NW_RES_CTOR(ResLightSet);
    


    s32 GetIndex() const { return static_cast<s32>(ref().m_Index); }
    void SetIndex(s32 value) { ref().m_Index = value; }

    s32 GetLightsCount() const
        { return ref().m_LightsTableCount; }
    ResReferenceSceneObject GetLights(int idx)
        {
            nw::ut::Offset* tbl_ptr = static_cast<nw::ut::Offset*>(ref().toLightsTable.to_table_ptr());
            return ResReferenceSceneObject(tbl_ptr[idx].to_ptr());
        }
    const ResReferenceSceneObject GetLights(int idx) const
        {
            const nw::ut::Offset* tbl_ptr = static_cast<const nw::ut::Offset*>(ref().toLightsTable.to_table_ptr());
            return ResReferenceSceneObject(tbl_ptr[idx].to_ptr());
        }
    nw::ut::internal::ResArray<ResReferenceSceneObject, nw::ut::internal::ResArrayClassTraits> GetLights()
        {
            return nw::ut::internal::ResArray<ResReferenceSceneObject, nw::ut::internal::ResArrayClassTraits>(ref().toLightsTable.to_table_ptr(), GetLightsCount());
        }
    const nw::ut::internal::ResArray<const ResReferenceSceneObject, nw::ut::internal::ResArrayClassTraits> GetLights() const
        {
            return nw::ut::internal::ResArray<const ResReferenceSceneObject, nw::ut::internal::ResArrayClassTraits>(ref().toLightsTable.to_table_ptr(), GetLightsCount());
        }
};

class ResSceneEnvironmentSetting : public ResSceneObject
{
public:
    enum
    {
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResSceneEnvironmentSetting)
    };
    enum
    {
        SIGNATURE = NW_RES_SIGNATURE32('CENV')
    };
    enum
    {
        BINARY_REVISION = REVISION_RES_RENDER_ENV_SETTING
    };
    
    NW_RES_CTOR_INHERIT( ResSceneEnvironmentSetting, ResSceneObject )

    u32 GetRevision() const { return this->GetHeader().revision; }


    s32 GetCamerasCount() const
        { return ref().m_CamerasTableCount; }
    ResReferenceSceneObject GetCameras(int idx)
        {
            nw::ut::Offset* tbl_ptr = static_cast<nw::ut::Offset*>(ref().toCamerasTable.to_table_ptr());
            return ResReferenceSceneObject(tbl_ptr[idx].to_ptr());
        }
    const ResReferenceSceneObject GetCameras(int idx) const
        {
            const nw::ut::Offset* tbl_ptr = static_cast<const nw::ut::Offset*>(ref().toCamerasTable.to_table_ptr());
            return ResReferenceSceneObject(tbl_ptr[idx].to_ptr());
        }
    nw::ut::internal::ResArray<ResReferenceSceneObject, nw::ut::internal::ResArrayClassTraits> GetCameras()
        {
            return nw::ut::internal::ResArray<ResReferenceSceneObject, nw::ut::internal::ResArrayClassTraits>(ref().toCamerasTable.to_table_ptr(), GetCamerasCount());
        }
    const nw::ut::internal::ResArray<const ResReferenceSceneObject, nw::ut::internal::ResArrayClassTraits> GetCameras() const
        {
            return nw::ut::internal::ResArray<const ResReferenceSceneObject, nw::ut::internal::ResArrayClassTraits>(ref().toCamerasTable.to_table_ptr(), GetCamerasCount());
        }

    s32 GetLightSetsCount() const
        { return ref().m_LightSetsTableCount; }
    ResLightSet GetLightSets(int idx)
        {
            nw::ut::Offset* tbl_ptr = static_cast<nw::ut::Offset*>(ref().toLightSetsTable.to_table_ptr());
            return ResLightSet(tbl_ptr[idx].to_ptr());
        }
    const ResLightSet GetLightSets(int idx) const
        {
            const nw::ut::Offset* tbl_ptr = static_cast<const nw::ut::Offset*>(ref().toLightSetsTable.to_table_ptr());
            return ResLightSet(tbl_ptr[idx].to_ptr());
        }
    nw::ut::internal::ResArray<ResLightSet, nw::ut::internal::ResArrayClassTraits> GetLightSets()
        {
            return nw::ut::internal::ResArray<ResLightSet, nw::ut::internal::ResArrayClassTraits>(ref().toLightSetsTable.to_table_ptr(), GetLightSetsCount());
        }
    const nw::ut::internal::ResArray<const ResLightSet, nw::ut::internal::ResArrayClassTraits> GetLightSets() const
        {
            return nw::ut::internal::ResArray<const ResLightSet, nw::ut::internal::ResArrayClassTraits>(ref().toLightSetsTable.to_table_ptr(), GetLightSetsCount());
        }

    s32 GetFogsCount() const
        { return ref().m_FogsTableCount; }
    ResReferenceSceneObject GetFogs(int idx)
        {
            nw::ut::Offset* tbl_ptr = static_cast<nw::ut::Offset*>(ref().toFogsTable.to_table_ptr());
            return ResReferenceSceneObject(tbl_ptr[idx].to_ptr());
        }
    const ResReferenceSceneObject GetFogs(int idx) const
        {
            const nw::ut::Offset* tbl_ptr = static_cast<const nw::ut::Offset*>(ref().toFogsTable.to_table_ptr());
            return ResReferenceSceneObject(tbl_ptr[idx].to_ptr());
        }
    nw::ut::internal::ResArray<ResReferenceSceneObject, nw::ut::internal::ResArrayClassTraits> GetFogs()
        {
            return nw::ut::internal::ResArray<ResReferenceSceneObject, nw::ut::internal::ResArrayClassTraits>(ref().toFogsTable.to_table_ptr(), GetFogsCount());
        }
    const nw::ut::internal::ResArray<const ResReferenceSceneObject, nw::ut::internal::ResArrayClassTraits> GetFogs() const
        {
            return nw::ut::internal::ResArray<const ResReferenceSceneObject, nw::ut::internal::ResArrayClassTraits>(ref().toFogsTable.to_table_ptr(), GetFogsCount());
        }
};

typedef nw::ut::ResArrayPatricia<ResSceneEnvironmentSetting>::type  ResSceneEnvironmentSettingArray;
typedef nw::ut::ResArrayClass<ResReferenceSceneObject>::type ResReferenceSceneObjectArray;
typedef nw::ut::ResArrayClass<ResLightSet>::type ResLightSetArray;

}
}
}

