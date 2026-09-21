#pragma once

#include <GLES2/gl2.h>
#include <GLES2/gl2extern.h>

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

struct ResLookupTableData
{
    nw::ut::ResTypeInfo typeInfo;
};

struct ResImageLookupTableData : public ResLookupTableData
{
    nw::ut::BinString toName;
    nw::ut::ResBool m_IsAbs;
    u8              _padding_0[3];
    nw::ut::ResS32 m_CommandCacheTableCount;
    nw::ut::Offset toCommandCacheTable;
};

struct ResReferenceLookupTableData : public ResLookupTableData
{
    nw::ut::BinString toPath;
    nw::ut::BinString toTableName;
    nw::ut::Offset toTargetLut;
};

class ResImageLookupTable;

class ResLookupTable : public nw::ut::ResCommon<ResLookupTableData>
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResLookupTable) };
    
    NW_RES_CTOR( ResLookupTable )

    nw::ut::ResTypeInfo GetTypeInfo() const { return ref().typeInfo; }

    inline ResImageLookupTable        Dereference();
    inline const ResImageLookupTable  Dereference() const;

    Result Setup();

    void Cleanup();
};

class ResImageLookupTable : public ResLookupTable
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResImageLookupTable) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('ILUT') };
    
    NW_RES_CTOR_INHERIT( ResImageLookupTable, ResLookupTable )

    NW_RES_FIELD_STRING_DECL( Name )
    NW_RES_FIELD_BOOL_PRIMITIVE_DECL( Abs )
    
    NW_RES_FIELD_PRIMITIVE_LIST_DECL( u32, CommandCache )
};

class ResReferenceLookupTable : public ResLookupTable
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResReferenceLookupTable) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('RLUT') };
    
    NW_RES_CTOR_INHERIT( ResReferenceLookupTable, ResLookupTable )

    NW_RES_FIELD_STRING_DECL( Path )
    NW_RES_FIELD_STRING_DECL( TableName )
    NW_RES_FIELD_CLASS_DECL( ResImageLookupTable, TargetLut )

    void ForceSetup(ResLookupTable lut);

    void ForceSetup(const char* targetName, ResLookupTable lut);
};

struct ResLookupTableSetData : public ResSceneObjectData
{
    nw::ut::ResS32 m_SamplersDicCount;
    nw::ut::Offset toSamplersDic;
};

class ResLookupTableSet : public ResSceneObject
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResLookupTableSet) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('LUTS') };
    enum
{ BINARY_REVISION = REVISION_RES_LUT_SET };
    
    NW_RES_CTOR_INHERIT(ResLookupTableSet, ResSceneObject)

    NW_RES_FIELD_CLASS_DIC_DECL( ResImageLookupTable, Samplers, nw::ut::ResDicPatricia )

    Result Setup(nw::os::IAllocator* allocator, ResGraphicsFile graphicsFile);

    void Cleanup();

    u32 GetRevision() const { return this->GetHeader().revision; }
};

inline ResImageLookupTable ResLookupTable::Dereference()
{
    NW_ASSERT( this->IsValid() );
    
    switch (this->ref().typeInfo)
    {

    case ResImageLookupTable::TYPE_INFO:{
            return ResStaticCast<ResImageLookupTable>(*this);
        }
    case ResReferenceLookupTable::TYPE_INFO:{
            ResReferenceLookupTable resRefLut = ResStaticCast<ResReferenceLookupTable>( *this );
            
            NW_ASSERT(resRefLut.GetTargetLut().IsValid());
            return resRefLut.GetTargetLut();
        }
    default:{
            NW_FATAL_ERROR("Unsupported lut type.");
            return ResImageLookupTable();
        }
    }
}

inline const ResImageLookupTable ResLookupTable::Dereference() const
{
    NW_ASSERT( this->IsValid() );
    
    switch (this->ref().typeInfo)
    {
    case ResImageLookupTable::TYPE_INFO:{
            return ResStaticCast<ResImageLookupTable>(*this);
        }
    case ResReferenceLookupTable::TYPE_INFO:{
            ResReferenceLookupTable resRefLut = ResStaticCast<ResReferenceLookupTable>( *this );
            
            NW_ASSERT( resRefLut.GetTargetLut().IsValid() );
            return resRefLut.GetTargetLut();
        }
    default:{
            NW_FATAL_ERROR("Unsupported lut type.");
            return ResImageLookupTable();
        }
    }
}

}
}
}