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

#ifndef NW_GFX_RESMODEL_H_
#define NW_GFX_RESMODEL_H_

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/res/gfx_ResSceneObject.h>
#include <nw/gfx/res/gfx_ResShape.h>
#include <nw/gfx/res/gfx_ResMaterial.h>
#include <nw/gfx/res/gfx_ResSkeleton.h>
#include <nw/gfx/res/gfx_ResMesh.h>
#include <nw/gfx/res/gfx_ResRevision.h>
#include <nw/gfx/res/gfx_ResTypeInfo.h>

#ifdef NW_PLATFORM_CTR
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 2530 // padding added to end of struct.
#endif
#endif
namespace nw {
namespace gfx {
namespace res {

class ResGraphicsFile;

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//
struct ResMeshNodeVisibilityData
{
    nw::ut::BinString toName;       //
    nw::ut::ResBool m_IsVisible;    //
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//
struct ResModelData : public ResTransformNodeData
{
    nw::ut::ResS32 m_MeshesTableCount;              //
    nw::ut::Offset toMeshesTable;                   //
    nw::ut::ResS32 m_MaterialsDicCount;             //
    nw::ut::Offset toMaterialsDic;                  //
    nw::ut::ResS32 m_ShapesTableCount;              //
    nw::ut::Offset toShapesTable;                   //
    nw::ut::ResS32 m_MeshNodeVisibilitiesDicCount;  //
    nw::ut::Offset toMeshNodeVisibilitiesDic;       //
    nw::ut::ResBool m_IsVisible;                    //
    nw::ut::ResBool m_IsNonuniformScalable;         //
    u8 _padding_0[6];                               //
    nw::ut::ResU32 m_LayerId;                       //
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//
struct ResSkeletalModelData : public ResModelData
{
    nw::ut::Offset toSkeleton;  //
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResMeshNodeVisibility : public nw::ut::ResCommon< ResMeshNodeVisibilityData >
{
public:
    NW_RES_CTOR( ResMeshNodeVisibility )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_STRING_DECL( Name )            // GetName()
    NW_RES_FIELD_BOOL_PRIMITIVE_DECL( Visible ) // IsVisible(), SetVisible()
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResModel : public ResTransformNode
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResModel) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('CMDL') };
    enum { BINARY_REVISION = REVISION_RES_MODEL };

    NW_RES_CTOR_INHERIT( ResModel, ResTransformNode )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_LIST_DECL( ResMesh, Meshes )  // GetMeshes(int idx), GetMeshesCount()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_DIC_DECL( ResMaterial, Materials, nw::ut::ResDicPatricia ) // GetMaterials(int idx), GetMaterials(const char*), GetMaterialsIndex(const char*), GetMaterialsCount()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_LIST_DECL( ResShape, Shapes ) // GetShapes(int idx), GetShapesCount()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_DIC_DECL( ResMeshNodeVisibility, MeshNodeVisibilities, nw::ut::ResDicPatricia ) // GetMeshNodeVisibilities(int idx), GetMeshNodeVisibilities(const char*), GetMeshNodeVisibilitiesIndex(const char*), GetMeshNodeVisibilitiesCount()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_BOOL_PRIMITIVE_DECL( Visible )      // IsVisible(), SetVisible()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_BOOL_PRIMITIVE_DECL( NonuniformScalable ) // IsNonuniformScalable(), SetNonuniformScalable()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( u32, LayerId )      // GetLayerId(), SetLayerId()

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    u32 GetRevision() const { return this->GetHeader().revision; }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
     Result Setup(os::IAllocator* allocator, ResGraphicsFile graphicsFile);

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    void Cleanup();

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    void ForceSetupTexture(const char* targetName, ResTexture texture);

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    void ForceSetupShader(const char* targetName, ResShader shader);

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    void ForceSetupLookupTable(const char* targetName, ResLookupTable lut);
};

//
typedef nw::ut::ResArrayPatricia<ResModel>::type  ResModelArray;

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResSkeletalModel : public ResModel
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResSkeletalModel) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('CSMD') };

    NW_RES_CTOR_INHERIT( ResSkeletalModel, ResModel )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_DECL( ResSkeleton, Skeleton )             // GetSkeleton()
};

} // namespace res
} // namespace gfx
} // namespace nw
#ifdef NW_PLATFORM_CTR
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 2530 // padding added to end of struct.
#endif
#endif

#endif // NW_GFX_RESMODEL_H_
