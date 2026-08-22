#pragma once

#include <nw/ut/ut_Inlines.h>
#include <nw/ut/ut_ResTypes.h>
#include <nw/ut/ut_ResUtil.h>
#include <nw/math/math_ResTypes.h>
#include <nw/ut/ut_ResTypeInfo.h>

namespace nw {
namespace ut {

struct ResMetaDataData{
    nw::ut::ResTypeInfo typeInfo;
    nw::ut::BinString toKey;
    nw::ut::ResS32 mDataType;
};

struct ResIntArrayMetaDataData : public ResMetaDataData{
    nw::ut::ResS32  mValuesCount;
    nw::ut::ResS32  mValues[1];
};

struct ResFloatArrayMetaDataData : public ResMetaDataData{
    nw::ut::ResS32  mValuesCount;
    nw::ut::ResF32  mValues[1];
};

struct ResStringArrayMetaDataData : public ResMetaDataData{
    nw::ut::ResU32  mEncoding;
    nw::ut::ResS32  mValuesCount;
    nw::ut::BinString  mValues[1];
};

class ResMetaData : public nw::ut::ResCommon< ResMetaDataData >{
public:
    enum { TYPE_INFO = NW_UT_RES_TYPE_INFO(ResMetaData) };
    
    enum DataType{
        DATATYPE_FLOAT_ARRAY = 0,
        DATATYPE_INT_ARRAY,
        DATATYPE_STRING_ARRAY,
        DATATYPE_UNKOWN = -1
    };
    
    NW_RES_CTOR( ResMetaData )

    nw::ut::ResTypeInfo     GetTypeInfo() const { return ref().typeInfo; }
};

class ResIntArrayMetaData : public ResMetaData{
public:
    enum { TYPE_INFO = NW_UT_RES_TYPE_INFO(ResIntArrayMetaData) };
    
    NW_RES_CTOR_INHERIT( ResIntArrayMetaData, ResMetaData )
    
    s32 GetValuesCount() const { return ref().mValuesCount; }
    const s32* GetValues() const { return &(ref().mValues[0]); }
    s32* GetValues() { return &(ref().mValues[0]); }
    
    s32  GetValues(int idx) const { return ref().mValues[idx]; }
    void SetValues(int idx, s32 value) { ref().mValues[idx] = value; }

    nw::ut::ResTypeInfo     GetTypeInfo() const { return ref().typeInfo; }
};

class ResFloatArrayMetaData : public ResMetaData{
public:
    enum { TYPE_INFO = NW_UT_RES_TYPE_INFO(ResFloatArrayMetaData) };
    
    NW_RES_CTOR_INHERIT( ResFloatArrayMetaData, ResMetaData )

    s32 GetValuesCount() const { return ref().mValuesCount; }
    
    const f32* GetValues() const { return &(ref().mValues[0]); }
    f32* GetValues() { return &(ref().mValues[0]); }
    
    f32  GetValues(int idx) const { return ref().mValues[idx]; }
    void SetValues(int idx, f32 value) { ref().mValues[idx] = value; }

    nw::ut::ResTypeInfo     GetTypeInfo() const { return ref().typeInfo; }
};

class ResStringArrayMetaData : public ResMetaData{
public:
    enum { TYPE_INFO = NW_UT_RES_TYPE_INFO(ResStringArrayMetaData) };
    
    enum Encoding{
        ENCODING_ASCII   = 0,
        ENCODING_UTF8    = 1,
        ENCODING_UTF16LE = 2,
        ENCODING_UTF16BE = 3
    };
    
    NW_RES_CTOR_INHERIT( ResStringArrayMetaData, ResMetaData )
    
    s32 GetValuesCount() const { return ref().mValuesCount; }
    
    const char* GetValues(int idx) const { return ref().mValues[idx].to_ptr(); }
    const wchar_t* GetWValues(int idx) const { return reinterpret_cast<const wchar_t*>(ref().mValues[idx].to_ptr()); }

    nw::ut::ResTypeInfo     GetTypeInfo() const { return ref().typeInfo; }
};


} /* namespace ut */
} /* namespace nw */