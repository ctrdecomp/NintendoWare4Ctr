#pragma once

#include <nw/ut/ut_Inlines.h>
#include <nw/ut/ut_ResTypes.h>
#include <nw/ut/ut_ResUtil.h>
#include <nw/math/math_ResTypes.h>
#include <nw/ut/ut_ResTypeInfo.h>

namespace nw {
namespace ut {

struct ResMetaDataData
{
    nw::ut::ResTypeInfo typeInfo;
    nw::ut::BinString toKey;
    nw::ut::ResS32 m_DataType;
};

struct ResIntArrayMetaDataData : public ResMetaDataData
{
    nw::ut::ResS32  m_ValuesCount;
    nw::ut::ResS32  m_Values[1];
};

struct ResFloatArrayMetaDataData : public ResMetaDataData
{
    nw::ut::ResS32  m_ValuesCount;
    nw::ut::ResF32  m_Values[1];
};

struct ResStringArrayMetaDataData : public ResMetaDataData
{
    nw::ut::ResU32  m_Encoding;
    nw::ut::ResS32  m_ValuesCount;
    nw::ut::BinString  m_Values[1];
};

class ResMetaData : public nw::ut::ResCommon< ResMetaDataData >
{
public:
    enum{ TYPE_INFO = NW_UT_RES_TYPE_INFO(ResMetaData) };
    
    enum DataType
    {
        DATATYPE_FLOAT_ARRAY = 0,
        DATATYPE_INT_ARRAY,
        DATATYPE_STRING_ARRAY,
        DATATYPE_UNKOWN = -1
    };
    
    NW_RES_CTOR( ResMetaData )

    nw::ut::ResTypeInfo     GetTypeInfo() const { return ref().typeInfo; }
};

class ResIntArrayMetaData : public ResMetaData
{
public:
    enum{ TYPE_INFO = NW_UT_RES_TYPE_INFO(ResIntArrayMetaData) };
    
    NW_RES_CTOR_INHERIT( ResIntArrayMetaData, ResMetaData )
    
    s32 GetValuesCount() const { return ref().m_ValuesCount; }
    const s32* GetValues() const { return &(ref().m_Values[0]); }
    s32* GetValues() { return &(ref().m_Values[0]); }
    
    s32  GetValues(int idx) const { return ref().m_Values[idx]; }
    void SetValues(int idx, s32 value) { ref().m_Values[idx] = value; }

    nw::ut::ResTypeInfo     GetTypeInfo() const { return ref().typeInfo; }
};

class ResFloatArrayMetaData : public ResMetaData
{
public:
    enum{ TYPE_INFO = NW_UT_RES_TYPE_INFO(ResFloatArrayMetaData) };
    
    NW_RES_CTOR_INHERIT( ResFloatArrayMetaData, ResMetaData )

    s32 GetValuesCount() const { return ref().m_ValuesCount; }
    
    const f32* GetValues() const { return &(ref().m_Values[0]); }
    f32* GetValues() { return &(ref().m_Values[0]); }
    
    f32  GetValues(int idx) const { return ref().m_Values[idx]; }
    void SetValues(int idx, f32 value) { ref().m_Values[idx] = value; }

    nw::ut::ResTypeInfo     GetTypeInfo() const { return ref().typeInfo; }
};

class ResStringArrayMetaData : public ResMetaData
{
public:
    enum{ TYPE_INFO = NW_UT_RES_TYPE_INFO(ResStringArrayMetaData) };
    
    enum Encoding
    {
        ENCODING_ASCII   = 0,
        ENCODING_UTF8    = 1,
        ENCODING_UTF16LE = 2,
        ENCODING_UTF16BE = 3
    };
    
    NW_RES_CTOR_INHERIT( ResStringArrayMetaData, ResMetaData )
    
    s32 GetValuesCount() const { return ref().m_ValuesCount; }
    
    const char* GetValues(int idx) const { return ref().m_Values[idx].to_ptr(); }
    const wchar_t* GetWValues(int idx) const { return reinterpret_cast<const wchar_t*>(ref().m_Values[idx].to_ptr()); }

    nw::ut::ResTypeInfo     GetTypeInfo() const { return ref().typeInfo; }
};


} // namespace ut
} // namespace nw