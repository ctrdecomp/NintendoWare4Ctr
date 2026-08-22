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

#ifndef NW_GFX_RESMESH_H_
#define NW_GFX_RESMESH_H_

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/res/gfx_ResSceneObject.h>
#include <nw/gfx/res/gfx_ResShape.h>
#include <nw/gfx/res/gfx_ResMaterial.h>

#ifdef NW_PLATFORM_CTR
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 1301 // padding inserted in struct.
#pragma diag_suppress 2530 // padding added to end of struct.
#endif
#endif
namespace nw {
namespace gfx {
namespace res {

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//
struct ResMeshData : public ResSceneObjectData
{
    //
    enum { IR_SCALE_COMMAND_SIZE = 2 + 1 + 2 * 4 + 1 };

    nw::ut::ResS32 m_ShapeIndex;                    //
    nw::ut::ResS32 m_MaterialIndex;                 //
    nw::ut::Offset toOwnerModel;                    //
    nw::ut::ResBool m_IsVisible;                    //
    nw::ut::ResU8 m_RenderPriority;                 //
    nw::ut::ResS16 m_MeshNodeVisibilityIndex;       //
    nw::ut::ResS32 m_CurrentPrimitiveIndex;         //
    nw::ut::ResU32 m_Flags;                         //
    nw::ut::ResU32 m_IrScaleCommand[ IR_SCALE_COMMAND_SIZE ];   //
    void*          m_ActivateCommandCache;          //
    s32            m_ActivateCommandCacheSize;      //
    void*          m_DeactivateCommandCache;        //
    s32            m_DeactivateCommandCacheSize;    //
    nw::ut::BinString toMeshNodeName;               //
    u64            m_RenderKeyCache;                //
    nw::os::IAllocator* m_CommandAllocator;         //
};

//--------------------------------------------------------------------------
//
//
//
//
//
//---------------------------------------------------------------------------
class ResMesh : public ResSceneObject
{
public:

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    enum Flag
    {
        FLAG_HAS_VERTEX_ALPHA       = 0x1,      //
        FLAG_HAS_BONE_WEIGHT_W      = 0x1 << 1, //
        FLAG_VALID_RENDER_KEY_CACHE = 0x1 << 2, //
        FLAG_HAS_BEEN_SETUP         = 0x1 << 3  //
    };

    NW_RES_CTOR_INHERIT( ResMesh, ResSceneObject )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( s32, ShapeIndex )            // GetShapeIndex(), SetShapeIndex()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( s32, MaterialIndex )         // GetMaterialIndex(), SetMaterialIndex()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_DECL( ResTransformNode, OwnerModel )   // GetOwnerModel()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_BOOL_PRIMITIVE_DECL( Visible )               // IsVisible(), SetVisible()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( u8, RenderPriority )         // GetRenderPriority(), SetRenderPriority()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( s32, CurrentPrimitiveIndex ) // GetCurrentPrimitiveIndex(), SetCurrentPrimitiveIndex()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_FLAGS_DECL( u32, Flags )                     // GetFlags(), SetFlags(), EnableFlags(), DisableFlags()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    // TODO: Fix the fact that Name cannot be taken.
    // NW_RES_FIELD_STRING_DECL( MeshNodeName )              // GetMeshNodeName()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( s32, MeshNodeVisibilityIndex )   // GetMeshNodeVisibilityIndex(), SetMeshNodeVisibilityIndex()

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    u64 GetRenderKeyCache() const { return ref().m_RenderKeyCache; }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetRenderKeyCache(u64 renderKey) { ref().m_RenderKeyCache = renderKey; }


    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    Result Setup(ResModel owner, os::IAllocator* allocator, ResGraphicsFile graphicsFile);

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    void Cleanup();
};

//
typedef nw::ut::ResArrayClass<ResMesh>::type  ResMeshArray;

} // namespace res
} // namespace gfx
} // namespace nw
#ifdef NW_PLATFORM_CTR
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 2530 // padding added to end of struct.
#pragma diag_default 1301 // padding inserted in struct.
#endif
#endif

#endif // NW_GFX_RESMESH_H_
