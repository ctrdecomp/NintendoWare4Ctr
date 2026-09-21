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
    
    NW_RES_FIELD_PRIMITIVE_DECL(s32, Index)

    NW_RES_FIELD_STRING_DECL(Path)

    NW_RES_FIELD_CLASS_DECL(ResSceneObject, Target)
};

class ResLightSet : public nw::ut::ResCommon<ResLightSetData>
{
public:
    NW_RES_CTOR(ResLightSet);
    

    NW_RES_FIELD_PRIMITIVE_DECL(s32, Index)
    NW_RES_FIELD_CLASS_LIST_DECL(ResReferenceSceneObject, Lights)
};

class ResSceneEnvironmentSetting : public ResSceneObject
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResSceneEnvironmentSetting) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('CENV') };
    enum { BINARY_REVISION = REVISION_RES_RENDER_ENV_SETTING };
    
    NW_RES_CTOR_INHERIT( ResSceneEnvironmentSetting, ResSceneObject )

    u32 GetRevision() const { return this->GetHeader().revision; }

    NW_RES_FIELD_CLASS_LIST_DECL(ResReferenceSceneObject, Cameras)
    NW_RES_FIELD_CLASS_LIST_DECL(ResLightSet, LightSets)
    NW_RES_FIELD_CLASS_LIST_DECL(ResReferenceSceneObject, Fogs)
};

typedef nw::ut::ResArrayPatricia<ResSceneEnvironmentSetting>::type  ResSceneEnvironmentSettingArray;
typedef nw::ut::ResArrayClass<ResReferenceSceneObject>::type ResReferenceSceneObjectArray;
typedef nw::ut::ResArrayClass<ResLightSet>::type ResLightSetArray;

}
}
}