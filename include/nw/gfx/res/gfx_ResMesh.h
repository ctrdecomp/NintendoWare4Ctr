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

    NW_RES_FIELD_PRIMITIVE_DECL( s32, ShapeIndex )
    NW_RES_FIELD_PRIMITIVE_DECL( s32, MaterialIndex )
    NW_RES_FIELD_CLASS_DECL( ResTransformNode, OwnerModel )
    NW_RES_FIELD_BOOL_PRIMITIVE_DECL( Visible )
    NW_RES_FIELD_PRIMITIVE_DECL( u8, RenderPriority )
    NW_RES_FIELD_PRIMITIVE_DECL( s32, CurrentPrimitiveIndex )
    NW_RES_FIELD_FLAGS_DECL( u32, Flags )

    NW_RES_FIELD_PRIMITIVE_DECL( s32, MeshNodeVisibilityIndex )
    
    u64 GetRenderKeyCache() const { return ref().m_RenderKeyCache; }

    void SetRenderKeyCache(u64 renderKey) { ref().m_RenderKeyCache = renderKey; }

    Result Setup(ResModel owner, nw::os::IAllocator* allocator, ResGraphicsFile graphicsFile);

    void Cleanup();
};

typedef nw::ut::ResArrayClass<ResMesh>::type  ResMeshArray;

}
}
}