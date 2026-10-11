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


    const char* GetName() const { return ref().toName.to_ptr(); }

    bool IsVisible() const { return static_cast<bool>(ref().m_IsVisible); }
    void SetVisible(bool value) { ref().m_IsVisible = value; }
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


    s32 GetMeshesCount() const
        { return ref().m_MeshesTableCount; }
    ResMesh GetMeshes(int idx)
        {
            nw::ut::Offset* tbl_ptr = static_cast<nw::ut::Offset*>(ref().toMeshesTable.to_table_ptr());
            return ResMesh(tbl_ptr[idx].to_ptr());
        }
    const ResMesh GetMeshes(int idx) const
        {
            const nw::ut::Offset* tbl_ptr = static_cast<const nw::ut::Offset*>(ref().toMeshesTable.to_table_ptr());
            return ResMesh(tbl_ptr[idx].to_ptr());
        }
    nw::ut::internal::ResArray<ResMesh, nw::ut::internal::ResArrayClassTraits> GetMeshes()
        {
            return nw::ut::internal::ResArray<ResMesh, nw::ut::internal::ResArrayClassTraits>(ref().toMeshesTable.to_table_ptr(), GetMeshesCount());
        }
    const nw::ut::internal::ResArray<const ResMesh, nw::ut::internal::ResArrayClassTraits> GetMeshes() const
        {
            return nw::ut::internal::ResArray<const ResMesh, nw::ut::internal::ResArrayClassTraits>(ref().toMeshesTable.to_table_ptr(), GetMeshesCount());
        }

    s32 GetMaterialsCount() const
        { return ref().m_MaterialsDicCount; }
    ResMaterial GetMaterials(int idx)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toMaterialsDic.to_ptr());
            return ResMaterial(dic[idx]);
        }
    ResMaterial GetMaterials(const char* key)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toMaterialsDic.to_ptr());
            return ResMaterial(dic[key]);
        }
    const ResMaterial GetMaterials(int idx) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toMaterialsDic.to_ptr());
            return ResMaterial(dic[idx]);
        }
    const ResMaterial GetMaterials(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toMaterialsDic.to_ptr());
            return ResMaterial(dic[key]);
        }
    s32 GetMaterialsIndex(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toMaterialsDic.to_ptr());
            return dic.GetIndex(key);
        }
    nw::ut::internal::ResArray<ResMaterial, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetMaterials()
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toMaterialsDic.to_ptr());
            return nw::ut::internal::ResArray<ResMaterial, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetMaterialsCount());
        }
    const nw::ut::internal::ResArray<const ResMaterial, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetMaterials() const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toMaterialsDic.to_ptr());
            return nw::ut::internal::ResArray<const ResMaterial, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetMaterialsCount());
        }

    s32 GetShapesCount() const
        { return ref().m_ShapesTableCount; }
    ResShape GetShapes(int idx)
        {
            nw::ut::Offset* tbl_ptr = static_cast<nw::ut::Offset*>(ref().toShapesTable.to_table_ptr());
            return ResShape(tbl_ptr[idx].to_ptr());
        }
    const ResShape GetShapes(int idx) const
        {
            const nw::ut::Offset* tbl_ptr = static_cast<const nw::ut::Offset*>(ref().toShapesTable.to_table_ptr());
            return ResShape(tbl_ptr[idx].to_ptr());
        }
    nw::ut::internal::ResArray<ResShape, nw::ut::internal::ResArrayClassTraits> GetShapes()
        {
            return nw::ut::internal::ResArray<ResShape, nw::ut::internal::ResArrayClassTraits>(ref().toShapesTable.to_table_ptr(), GetShapesCount());
        }
    const nw::ut::internal::ResArray<const ResShape, nw::ut::internal::ResArrayClassTraits> GetShapes() const
        {
            return nw::ut::internal::ResArray<const ResShape, nw::ut::internal::ResArrayClassTraits>(ref().toShapesTable.to_table_ptr(), GetShapesCount());
        }

    s32 GetMeshNodeVisibilitiesCount() const
        { return ref().m_MeshNodeVisibilitiesDicCount; }
    ResMeshNodeVisibility GetMeshNodeVisibilities(int idx)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toMeshNodeVisibilitiesDic.to_ptr());
            return ResMeshNodeVisibility(dic[idx]);
        }
    ResMeshNodeVisibility GetMeshNodeVisibilities(const char* key)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toMeshNodeVisibilitiesDic.to_ptr());
            return ResMeshNodeVisibility(dic[key]);
        }
    const ResMeshNodeVisibility GetMeshNodeVisibilities(int idx) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toMeshNodeVisibilitiesDic.to_ptr());
            return ResMeshNodeVisibility(dic[idx]);
        }
    const ResMeshNodeVisibility GetMeshNodeVisibilities(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toMeshNodeVisibilitiesDic.to_ptr());
            return ResMeshNodeVisibility(dic[key]);
        }
    s32 GetMeshNodeVisibilitiesIndex(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toMeshNodeVisibilitiesDic.to_ptr());
            return dic.GetIndex(key);
        }
    nw::ut::internal::ResArray<ResMeshNodeVisibility, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetMeshNodeVisibilities()
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toMeshNodeVisibilitiesDic.to_ptr());
            return nw::ut::internal::ResArray<ResMeshNodeVisibility, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetMeshNodeVisibilitiesCount());
        }
    const nw::ut::internal::ResArray<const ResMeshNodeVisibility, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetMeshNodeVisibilities() const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toMeshNodeVisibilitiesDic.to_ptr());
            return nw::ut::internal::ResArray<const ResMeshNodeVisibility, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetMeshNodeVisibilitiesCount());
        }

    bool IsVisible() const { return static_cast<bool>(ref().m_IsVisible); }
    void SetVisible(bool value) { ref().m_IsVisible = value; }

    bool IsNonuniformScalable() const { return static_cast<bool>(ref().m_IsNonuniformScalable); }
    void SetNonuniformScalable(bool value) { ref().m_IsNonuniformScalable = value; }

    u32 GetLayerId() const { return static_cast<u32>(ref().m_LayerId); }
    void SetLayerId(u32 value) { ref().m_LayerId = value; }

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

    ResSkeleton GetSkeleton() { return ResSkeleton(ref().toSkeleton.to_ptr()); }
    const ResSkeleton GetSkeleton() const { return ResSkeleton(ref().toSkeleton.to_ptr()); }
};

}
}
}

