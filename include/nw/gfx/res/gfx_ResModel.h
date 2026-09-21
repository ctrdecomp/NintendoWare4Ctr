#pragma once

#include <nw/gfx/res/gfx_ResSkeleton.h>
#include <nw/gfx/res/gfx_ResMesh.h>

namespace nw {
namespace gfx {
namespace res {

class ResGraphicsFile;

struct ResMeshNodeVisibilityData
{
    nw::ut::BinString toName;
    nw::ut::ResBool m_IsVisible;
};

struct ResModelData : public ResTransformNodeData
{
    nw::ut::ResS32 m_MeshesTableCount;
    nw::ut::Offset toMeshesTable;
    nw::ut::ResS32 m_MaterialsDicCount;
    nw::ut::Offset toMaterialsDic;
    nw::ut::ResS32 m_ShapesTableCount;
    nw::ut::Offset toShapesTable;
    nw::ut::ResS32 m_MeshNodeVisibilitiesDicCount;
    nw::ut::Offset toMeshNodeVisibilitiesDic;
    nw::ut::ResBool m_IsVisible;
    nw::ut::ResBool m_IsNonuniformScalable;
    u8 _padding_0[6];
    nw::ut::ResU32 m_LayerId;
};

struct ResSkeletalModelData : public ResModelData
{
    nw::ut::Offset toSkeleton;
};

class ResMeshNodeVisibility : public nw::ut::ResCommon<ResMeshNodeVisibilityData>
{
public:
    NW_RES_CTOR(ResMeshNodeVisibility)

    NW_RES_FIELD_STRING_DECL(Name)
    NW_RES_FIELD_BOOL_PRIMITIVE_DECL(Visible)
};

class ResModel : public ResTransformNode
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResModel) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('CMDL') };
    enum
{ BINARY_REVISION = REVISION_RES_MODEL };
    
    NW_RES_CTOR_INHERIT( ResModel, ResTransformNode )

    NW_RES_FIELD_CLASS_LIST_DECL( ResMesh, Meshes )
    NW_RES_FIELD_CLASS_DIC_DECL( ResMaterial, Materials, nw::ut::ResDicPatricia )
    NW_RES_FIELD_CLASS_LIST_DECL( ResShape, Shapes )
    NW_RES_FIELD_CLASS_DIC_DECL( ResMeshNodeVisibility, MeshNodeVisibilities, nw::ut::ResDicPatricia )
    NW_RES_FIELD_BOOL_PRIMITIVE_DECL( Visible )
    NW_RES_FIELD_BOOL_PRIMITIVE_DECL( NonuniformScalable )
    NW_RES_FIELD_PRIMITIVE_DECL( u32, LayerId )

    u32 GetRevision() const { return this->GetHeader().revision; }

    Result Setup(nw::os::IAllocator* allocator, ResGraphicsFile graphicsFile);

    void Cleanup();

    void ForceSetupTexture(const char* targetName, ResTexture texture);

    void ForceSetupShader(const char* targetName, ResShader shader);

    void ForceSetupLookupTable(const char* targetName, ResLookupTable lut);
};
typedef nw::ut::ResArrayPatricia<ResModel>::type  ResModelArray;

class ResSkeletalModel : public ResModel
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResSkeletalModel) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('CSMD') };
    
    NW_RES_CTOR_INHERIT(ResSkeletalModel, ResModel)
    NW_RES_FIELD_CLASS_DECL(ResSkeleton, Skeleton)
};

}
}
}