#pragma once

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/res/gfx_ResParticleUpdater.h>

namespace nw {
namespace gfx {
namespace res {

struct ResParticleAttributeData
{
    nw::ut::ResTypeInfo typeInfo;
    nw::ut::ResS32 m_Usage;
};

class ResParticleAttribute : public nw::ut::ResCommon< ResParticleAttributeData >
{
public:
    enum
    {
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleAttribute)
    };
    enum
    {
        SIGNATURE = NW_RES_SIGNATURE32('PSAT')
    };

    NW_RES_CTOR( ResParticleAttribute )


    s32 GetUsage() const { return static_cast<s32>(ref().m_Usage); }
    void SetUsage(s32 value) { ref().m_Usage = value; }
    nw::ut::ResTypeInfo     GetTypeInfo() const { return ref().typeInfo; }
};

typedef nw::ut::ResArrayClass<ResParticleAttribute>::type  ResParticleAttributeArray;

struct ResParticleStreamAttributeData : public ResParticleAttributeData
{
};

class ResParticleStreamAttribute : public ResParticleAttribute
{
public:
    enum
    {
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleStreamAttribute)
    };
    enum
    {
        SIGNATURE = NW_RES_SIGNATURE32('PSST')
    };

    NW_RES_CTOR_INHERIT( ResParticleStreamAttribute, ResParticleAttribute )
};

struct ResParticleParameterAttributeData : public ResParticleAttributeData
{
    nw::ut::ResS32 m_Dimension;
    nw::ut::ResS32 m_DataTableCount;
    nw::ut::Offset toDataTable;
};

class ResParticleParameterAttribute : public ResParticleAttribute
{
public:
    enum
    {
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleParameterAttribute)
    };
    enum
    {
        SIGNATURE = NW_RES_SIGNATURE32('PSPA')
    };

    NW_RES_CTOR_INHERIT( ResParticleParameterAttribute, ResParticleAttribute )


    s32 GetDimension() const { return static_cast<s32>(ref().m_Dimension); }
    void SetDimension(s32 value) { ref().m_Dimension = value; }

    s32 GetDataCount() const
        { return ref().m_DataTableCount; }
    f32 GetData(int idx) const
        {
            const f32* tbl_ptr = static_cast<const f32*>(ref().toDataTable.to_table_ptr());
            return tbl_ptr[idx];
        }
    void SetData(int idx, f32 value)
        {
            f32* tbl_ptr = GetData();
            tbl_ptr[idx] = value;
        }
    f32* GetData()
        { return static_cast<f32*>(ref().toDataTable.to_table_ptr()); }
    const f32* GetData() const
        { return static_cast<const f32*>(ref().toDataTable.to_table_ptr()); }

    nw::ut::ResTypeInfo     GetTypeInfo() const { return ref().typeInfo; }
};

struct ResParticleCollectionData
{
    nw::ut::ResS32 m_Capacity;
    nw::ut::ResS32 m_AttributesTableCount;
    nw::ut::Offset toAttributesTable;
};

class ResParticleCollection : public nw::ut::ResCommon< ResParticleCollectionData >
{
public:
    NW_RES_CTOR( ResParticleCollection )


    s32 GetCapacity() const { return static_cast<s32>(ref().m_Capacity); }
    void SetCapacity(s32 value) { ref().m_Capacity = value; }

    s32 GetAttributesCount() const
        { return ref().m_AttributesTableCount; }
    ResParticleAttribute GetAttributes(int idx)
        {
            nw::ut::Offset* tbl_ptr = static_cast<nw::ut::Offset*>(ref().toAttributesTable.to_table_ptr());
            return ResParticleAttribute(tbl_ptr[idx].to_ptr());
        }
    const ResParticleAttribute GetAttributes(int idx) const
        {
            const nw::ut::Offset* tbl_ptr = static_cast<const nw::ut::Offset*>(ref().toAttributesTable.to_table_ptr());
            return ResParticleAttribute(tbl_ptr[idx].to_ptr());
        }
    nw::ut::internal::ResArray<ResParticleAttribute, nw::ut::internal::ResArrayClassTraits> GetAttributes()
        {
            return nw::ut::internal::ResArray<ResParticleAttribute, nw::ut::internal::ResArrayClassTraits>(ref().toAttributesTable.to_table_ptr(), GetAttributesCount());
        }
    const nw::ut::internal::ResArray<const ResParticleAttribute, nw::ut::internal::ResArrayClassTraits> GetAttributes() const
        {
            return nw::ut::internal::ResArray<const ResParticleAttribute, nw::ut::internal::ResArrayClassTraits>(ref().toAttributesTable.to_table_ptr(), GetAttributesCount());
        }
};

}
}
}

