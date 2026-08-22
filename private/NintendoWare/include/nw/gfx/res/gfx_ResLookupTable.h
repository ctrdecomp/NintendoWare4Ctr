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

#ifndef NW_GFX_RESLOOKUPTABLE_H_
#define NW_GFX_RESLOOKUPTABLE_H_

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <nw/ut/ut_Color.h>
#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/res/gfx_ResSceneObject.h>
#include <nw/gfx/res/gfx_ResTypeInfo.h>
#include <nw/gfx/res/gfx_ResRevision.h>

namespace nw {
namespace gfx {
namespace res {

class ResGraphicsFile;

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//
struct ResLookupTableData
{
    nw::ut::ResTypeInfo typeInfo;   //
};

//
//
// The command format of CommandCacheTable is as follows.
// 0 : The settings value [0:11]value, [23:12]diff for the LUT settings register (0x1c8, 0x132), etc.
// 1 : Write header to the register.
// 2～ : LUT settings in the amount of the LUT size.
//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResImageLookupTableData : public ResLookupTableData
{
    nw::ut::BinString toName;                   //
    nw::ut::ResBool m_IsAbs;                    //
    u8              _padding_0[3];              //
    nw::ut::ResS32 m_CommandCacheTableCount;    //
    nw::ut::Offset toCommandCacheTable;         //
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//
struct ResReferenceLookupTableData : public ResLookupTableData
{
    nw::ut::BinString toPath;       //
    nw::ut::BinString toTableName;  //
    nw::ut::Offset toTargetLut;     //
};

class ResImageLookupTable;

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResLookupTable : public nw::ut::ResCommon<ResLookupTableData>
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResLookupTable) };

    NW_RES_CTOR( ResLookupTable )

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    nw::ut::ResTypeInfo GetTypeInfo() const { return ref().typeInfo; }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    NW_INLINE ResImageLookupTable        Dereference();
    NW_INLINE const ResImageLookupTable  Dereference() const;

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    Result Setup();

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    void Cleanup();
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResImageLookupTable : public ResLookupTable
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResImageLookupTable) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('ILUT') };

    NW_RES_CTOR_INHERIT( ResImageLookupTable, ResLookupTable )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_STRING_DECL( Name )                    // GetName()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_BOOL_PRIMITIVE_DECL( Abs )             // IsAbs(), SetAbs()

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
    NW_RES_FIELD_PRIMITIVE_LIST_DECL( u32, CommandCache ) // GetCommandCache(), GetCommandCacheCount(), GetCommandCache(int idx)
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResReferenceLookupTable : public ResLookupTable
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResReferenceLookupTable) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('RLUT') };

    NW_RES_CTOR_INHERIT( ResReferenceLookupTable, ResLookupTable )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_STRING_DECL( Path )        // GetPath()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_STRING_DECL( TableName )   // GetTableName()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_DECL( ResImageLookupTable, TargetLut ) // GetTargetLut()

    //
    void ForceSetup(ResLookupTable lut);

    //
    void ForceSetup(const char* targetName, ResLookupTable lut);
};


//
struct ResLookupTableSetData : public ResSceneObjectData
{
    nw::ut::ResS32 m_SamplersDicCount;
    nw::ut::Offset toSamplersDic;
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResLookupTableSet : public ResSceneObject
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResLookupTableSet) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('LUTS') };
    enum { BINARY_REVISION = REVISION_RES_LUT_SET };

    NW_RES_CTOR_INHERIT( ResLookupTableSet, ResSceneObject )

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
    NW_RES_FIELD_CLASS_DIC_DECL( ResImageLookupTable, Samplers, nw::ut::ResDicPatricia )

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
    //
    //
    //---------------------------------------------------------------------------
    u32 GetRevision() const { return this->GetHeader().revision; }
};

//----------------------------------------
NW_INLINE ResImageLookupTable
ResLookupTable::Dereference()
{
    NW_ASSERT( this->IsValid() );

    switch ( this->ref().typeInfo )
    {
    case ResImageLookupTable::TYPE_INFO:
        {
            return ResStaticCast<ResImageLookupTable>(*this);
        }
    case ResReferenceLookupTable::TYPE_INFO:
        {
            ResReferenceLookupTable resRefLut = ResStaticCast<ResReferenceLookupTable>( *this );

            NW_ASSERT( resRefLut.GetTargetLut().IsValid() );
            return resRefLut.GetTargetLut();
        }
    default:
        {
            NW_FATAL_ERROR("Unsupported lut type.");
            return ResImageLookupTable();
        }
    }
}


//----------------------------------------
NW_INLINE const ResImageLookupTable
ResLookupTable::Dereference() const
{
    NW_ASSERT( this->IsValid() );

    switch ( this->ref().typeInfo )
    {
    case ResImageLookupTable::TYPE_INFO:
        {
            return ResStaticCast<ResImageLookupTable>(*this);
        }
    case ResReferenceLookupTable::TYPE_INFO:
        {
            ResReferenceLookupTable resRefLut = ResStaticCast<ResReferenceLookupTable>( *this );

            NW_ASSERT( resRefLut.GetTargetLut().IsValid() );
            return resRefLut.GetTargetLut();
        }
    default:
        {
            NW_FATAL_ERROR("Unsupported lut type.");
            return ResImageLookupTable();
        }
    }
}

} // namespace res
} // namespace gfx
} // namespace nw

#endif // NW_GFX_RESLOOKUPTABLE_H_
