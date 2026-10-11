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
    
    NW_RES_CTOR(ResLookupTable)

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
    { 
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResImageLookupTable) 
    };
    enum
    { 
        SIGNATURE = NW_RES_SIGNATURE32('ILUT') 
    };
    
    NW_RES_CTOR_INHERIT(ResImageLookupTable, ResLookupTable)


    const char* GetName() const { return ref().toName.to_ptr(); }

    bool IsAbs() const { return static_cast<bool>(ref().m_IsAbs); }
    void SetAbs(bool value) { ref().m_IsAbs = value; }
    

    s32 GetCommandCacheCount() const
        { return ref().m_CommandCacheTableCount; }
    u32 GetCommandCache(int idx) const
        {
            const u32* tbl_ptr = static_cast<const u32*>(ref().toCommandCacheTable.to_table_ptr());
            return tbl_ptr[idx];
        }
    void SetCommandCache(int idx, u32 value)
        {
            u32* tbl_ptr = GetCommandCache();
            tbl_ptr[idx] = value;
        }
    u32* GetCommandCache()
        { return static_cast<u32*>(ref().toCommandCacheTable.to_table_ptr()); }
    const u32* GetCommandCache() const
        { return static_cast<const u32*>(ref().toCommandCacheTable.to_table_ptr()); }
};

class ResReferenceLookupTable : public ResLookupTable
{
public:
    enum
    { 
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResReferenceLookupTable) 
    };
    enum
    { 
        SIGNATURE = NW_RES_SIGNATURE32('RLUT') 
    };
    
    NW_RES_CTOR_INHERIT(ResReferenceLookupTable, ResLookupTable)


    const char* GetPath() const { return ref().toPath.to_ptr(); }

    const char* GetTableName() const { return ref().toTableName.to_ptr(); }

    ResImageLookupTable GetTargetLut() { return ResImageLookupTable(ref().toTargetLut.to_ptr()); }
    const ResImageLookupTable GetTargetLut() const { return ResImageLookupTable(ref().toTargetLut.to_ptr()); }

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
    { 
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResLookupTableSet) 
    };
    enum
    { 
        SIGNATURE = NW_RES_SIGNATURE32('LUTS') 
    };
    enum
    { 
        BINARY_REVISION = REVISION_RES_LUT_SET 
    };
    
    NW_RES_CTOR_INHERIT(ResLookupTableSet, ResSceneObject)


    s32 GetSamplersCount() const
        { return ref().m_SamplersDicCount; }
    ResImageLookupTable GetSamplers(int idx)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toSamplersDic.to_ptr());
            return ResImageLookupTable(dic[idx]);
        }
    ResImageLookupTable GetSamplers(const char* key)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toSamplersDic.to_ptr());
            return ResImageLookupTable(dic[key]);
        }
    const ResImageLookupTable GetSamplers(int idx) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toSamplersDic.to_ptr());
            return ResImageLookupTable(dic[idx]);
        }
    const ResImageLookupTable GetSamplers(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toSamplersDic.to_ptr());
            return ResImageLookupTable(dic[key]);
        }
    s32 GetSamplersIndex(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toSamplersDic.to_ptr());
            return dic.GetIndex(key);
        }
    nw::ut::internal::ResArray<ResImageLookupTable, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetSamplers()
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toSamplersDic.to_ptr());
            return nw::ut::internal::ResArray<ResImageLookupTable, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetSamplersCount());
        }
    const nw::ut::internal::ResArray<const ResImageLookupTable, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetSamplers() const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toSamplersDic.to_ptr());
            return nw::ut::internal::ResArray<const ResImageLookupTable, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetSamplersCount());
        }

    Result Setup(nw::os::IAllocator* allocator, ResGraphicsFile graphicsFile);

    void Cleanup();

    u32 GetRevision() const { return this->GetHeader().revision; }
};

inline ResImageLookupTable ResLookupTable::Dereference()
{
    NW_ASSERT(IsValid());
    
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
    NW_ASSERT(IsValid());
    
    switch (this->ref().typeInfo)
    {
    case ResImageLookupTable::TYPE_INFO:{
            return ResStaticCast<ResImageLookupTable>(*this);
        }
    case ResReferenceLookupTable::TYPE_INFO:{
            ResReferenceLookupTable resRefLut = ResStaticCast<ResReferenceLookupTable>(*this);
            
            NW_ASSERT(resRefLut.GetTargetLut().IsValid());
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

