#pragma once

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/res/gfx_ResShape.h>
#include <nw/gfx/res/gfx_ResMaterial.h>

namespace nw {
namespace gfx {
namespace res {

struct ResMeshData : public ResSceneObjectData
{
    enum
{ IR_SCALE_COMMAND_SIZE = 2 + 1 + 2 * 4 + 1 };
    
    nw::ut::ResS32 m_ShapeIndex;
    nw::ut::ResS32 m_MaterialIndex;
    nw::ut::Offset toOwnerModel;
    nw::ut::ResBool m_IsVisible;
    nw::ut::ResU8 m_RenderPriority;
    nw::ut::ResS16 m_MeshNodeVisibilityIndex;
    nw::ut::ResS32 m_CurrentPrimitiveIndex;
    nw::ut::ResU32 m_Flags;
    nw::ut::ResU32 m_IrScaleCommand[ IR_SCALE_COMMAND_SIZE ];
    void*          m_ActivateCommandCache;
    s32            m_ActivateCommandCacheSize;
    void*          m_DeactivateCommandCache;
    s32            m_DeactivateCommandCacheSize;
    nw::ut::BinString toMeshNodeName;
    u64            m_RenderKeyCache;
    nw::os::IAllocator* m_CommandAllocator;
};

class ResMesh : public ResSceneObject
{
public:
    enum Flag
    {
        FLAG_HAS_VERTEX_ALPHA  = 0x1,
        FLAG_HAS_BONE_WEIGHT_W = 0x1 << 1,
        FLAG_VALID_RENDER_KEY_CACHE = 0x1 << 2,
        FLAG_HAS_BEEN_SETUP = 0x1 << 3
    };
    
    NW_RES_CTOR_INHERIT( ResMesh, ResSceneObject )


    s32 GetShapeIndex() const { return static_cast<s32>(ref().m_ShapeIndex); }
    void SetShapeIndex(s32 value) { ref().m_ShapeIndex = value; }

    s32 GetMaterialIndex() const { return static_cast<s32>(ref().m_MaterialIndex); }
    void SetMaterialIndex(s32 value) { ref().m_MaterialIndex = value; }

    ResTransformNode GetOwnerModel() { return ResTransformNode(ref().toOwnerModel.to_ptr()); }
    const ResTransformNode GetOwnerModel() const { return ResTransformNode(ref().toOwnerModel.to_ptr()); }

    bool IsVisible() const { return static_cast<bool>(ref().m_IsVisible); }
    void SetVisible(bool value) { ref().m_IsVisible = value; }

    u8 GetRenderPriority() const { return static_cast<u8>(ref().m_RenderPriority); }
    void SetRenderPriority(u8 value) { ref().m_RenderPriority = value; }

    s32 GetCurrentPrimitiveIndex() const { return static_cast<s32>(ref().m_CurrentPrimitiveIndex); }
    void SetCurrentPrimitiveIndex(s32 value) { ref().m_CurrentPrimitiveIndex = value; }
    u32 GetFlags() const { return static_cast<u32>(ref().m_Flags); }
    void SetFlags(u32 value) { ref().m_Flags = value; }
    void EnableFlags(u32 value) { ref().m_Flags = nw::ut::EnableFlag(ref().m_Flags, value); }
    void DisableFlags(u32 value) { ref().m_Flags = nw::ut::DisableFlag(ref().m_Flags, value); }


    s32 GetMeshNodeVisibilityIndex() const { return static_cast<s32>(ref().m_MeshNodeVisibilityIndex); }
    void SetMeshNodeVisibilityIndex(s32 value) { ref().m_MeshNodeVisibilityIndex = value; }
    
    u64 GetRenderKeyCache() const { return ref().m_RenderKeyCache; }

    void SetRenderKeyCache(u64 renderKey) { ref().m_RenderKeyCache = renderKey; }

    Result Setup(ResModel owner, nw::os::IAllocator* allocator, ResGraphicsFile graphicsFile);

    void Cleanup();
};

typedef nw::ut::ResArrayClass<ResMesh>::type  ResMeshArray;

}
}
}

