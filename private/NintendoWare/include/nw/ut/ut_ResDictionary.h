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

#ifndef NW_UT_RESDICTIONARY_H_
#define NW_UT_RESDICTIONARY_H_

#include <nw/types.h>
#include <nw/ut/ut_ResTypes.h>
#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResArray.h>

namespace nw {
namespace ut {

#if 0
// Dictionary interface (this class is not inherited when implemented as a template)
class IResDic
{
public:
    // Returns whether dictionary data is enabled.
    virtual bool    IsValid() const = 0;

    // Number of entries registered in the dictionary.
    virtual s32     GetCount() const = 0;

    // Get elements from index or key string values.
    // Return NULL if specified element cannot be found in dictionary.
    virtual void*   operator[](int idx) const = 0;
    virtual void*   operator[](u32 idx) const = 0;
    virtual void*   operator[](const char* key) const   = 0;
    virtual void*   operator[](const ResName key) const = 0;

    // Search dictionary using key string value and get index.
    // Returns a negative number if specified key cannot be found.
    virtual s32     GetIndex(const char* key) const  = 0;
    virtual s32     GetIndex(const ResName n ) const = 0;

    // Get the key string for the nth element, where n = index number.
    virtual const ResName GetResName(u32 idx) const  = 0;
    virtual const char*   GetName(u32 idx) const     = 0;
};

#endif

#if 0
// When using a wrapper class
template <class TDic>
class ResDic
{
public:
    /* ctor */ ResDic(TDic& dic) : m_Dic( dic ) {}

    bool    IsValid() const { m_Dic.IsValid(); }
    s32     GetCount() const { return m_Dic.GetCount(); }
    void*   operator[](int idx) const { return m_Dic[ idx ]; }
    void*   operator[](u32 idx) const { return m_Dic[ idx ]; }
    void*   operator[](const char* key) const { return m_Dic[ key ]; }
    void*   operator[](const ResName key) const { return m_Dic[ key ]; }

    // Search dictionary using key string value and get index.
    // Returns a negative number if specified key cannot be found.
    s32     GetIndex(const char* key) const { return m_Dic.GetIndex( key ); }
    s32     GetIndex(const ResName n ) const { return m_Dic.GetIndex( n ); }

    // Get the key string for the nth element, where n = index number.
    const ResName GetResName(u32 idx) const { return m_Dic.GetResName( idx ); }
    const char*   GetName(u32 idx) const    { return m_Dic.GetName( idx ); }

private:
    TDic& m_Dic;
};
#endif


//
// Linear search dictionary.
struct ResDicLinearData : public DataBlockHeader
{
    Size   numData;

    //
    struct ResDicNodeData
    {
        BinString toName;
        Offset    ofsData;
    }
    data[1]; // There are numData of them.

    ResDicNodeData* GetBeginNode() { return data; }
    const ResDicNodeData* GetBeginNode() const { return data; }
};

//
class ResDicLinear : public ResCommon<ResDicLinearData>
{
public:
    NW_RES_CTOR( ResDicLinear )

    s32  GetCount() const { return ref().numData; }

    // Get elements from index or key string values.
    // Return NULL if specified element cannot be found in dictionary.
    void*   operator[](int idx) const
    {
        if ( ! this->IsValid() ) { return NULL; }
        if ( idx < 0 || s32(ref().numData) <= idx ) { return NULL; }
        return const_cast<void*>( ref().data[idx].ofsData.to_ptr() );
    }
    void*   operator[](u32 idx) const { return operator[](int(idx)); }
    void*   operator[](const char* key) const
    {
        s32 idx = this->GetIndex(key);
        if ( idx < 0 ) { return NULL; }
        return const_cast<void*>( ref().data[ idx ].ofsData.to_ptr() );
    }

    // Search dictionary using key string value and get index.
    // Returns a negative number if specified key cannot be found.
    s32     GetIndex(const char* key) const;

    // Get the key string for the nth element, where n = index number.
    const char*   GetName(u32 /*idx*/) const    { return NULL; }
};

//
struct ResDicPatriciaData : public DataBlockHeader
{
    ResU32  numData; //

    //
    struct ResDicNodeData
    {
        ResU32 ref;
        ResU16 idxLeft;
        ResU16 idxRight;
        Offset ofsString;
        Offset ofsData;
    }
    data[1]; // There are numData + 1 pieces of data

    //
    ResDicNodeData* GetBeginNode() { return &data[1]; }

    //
    const ResDicNodeData* GetBeginNode() const { return &data[1]; }
};

//
// Patricia dictionary accessor class.
class ResDicPatricia : public ResCommon< ResDicPatriciaData >
{
public:
    enum { NOT_FOUND = -1 };

    NW_RES_CTOR( ResDicPatricia );

    s32  GetCount() const { return ref().numData; }

    void* operator[](int idx) const
    {
        if (!this->IsValid()) { return NULL; }

        NW_MINMAX_ASSERT( idx, 0, static_cast<int>(ref().numData - 1) );
        // When it comes to dictionary lookup, it's probably no problem even if const correctness is not maintained.
        return const_cast<void*>(ref().data[idx + 1].ofsData.to_ptr());
    }

    void* operator[](u32 idx) const { return operator[](int(idx)); }

    void* operator[](const char* s) const
    {
        // s may be NULL
        if (this->IsValid() && s)
        {
            ResDicPatriciaData::ResDicNodeData* x = Get(s, std::strlen(s));

            // It is okay not to assume a destination for NULL in the dictionary.
            if (x)
            {
                return const_cast<void*>(x->ofsData.to_ptr());
            }
        }

        return NULL;
    }

    void* operator()(const char* s, size_t len) const
    {
        // Since len is set as the string length, the string cannot be NULL.
        NW_NULL_ASSERT(s);
        if (this->IsValid())
        {
            ResDicPatriciaData::ResDicNodeData* x = Get(s, len);

            // It is okay not to assume a destination for NULL in the dictionary.
            if (x)
            {
                return const_cast<void*>(x->ofsData.to_ptr());
            }
        }
        return NULL;
    }

    s32 GetIndex(const char* s) const
    {
        // s may be NULL
        if (IsValid() && s)
        {
            size_t len = std::strlen(s);
            ResDicPatriciaData::ResDicNodeData* x = Get(s, len);

            if (x)
            {
                return static_cast<s32>(x - &ptr()->data[1]);
                // Since Entry 0 is the root node, take its difference with 1.
            }
        }

        return -1;
    }

    const char* GetName(u32 idx) const
    {
        if (!IsValid()) { return NULL; }

        NW_MINMAX_ASSERT( idx, 0, ptr()->numData - 1 );
        return ptr()->data[idx + 1].ofsString.to_ptr<char>();
    }

    u32 GetNumData() const
    {
        if ( !IsValid() ) { return 0; }
        return ptr()->numData;
    }

    u32 GetLength() const { return ref().length; }

protected:
    ResDicPatriciaData::ResDicNodeData* Get(const char* s, size_t len) const;
};


namespace internal {

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    inline ResDicPatriciaData*
    InitializeResDicPatricia(ResDicPatriciaData* resData)
    {
        resData->signature = NW_RES_SIGNATURE32('DICT');
        resData->length = sizeof(ResDicPatriciaData);
        resData->numData = 0;
        resData->data[0].ref = 0xFFFFFFFF;
        resData->data[0].idxLeft = 0;
        resData->data[0].idxRight = 0;
        resData->data[0].ofsString.set_ptr(NULL);
        resData->data[0].ofsData.set_ptr(NULL);

        return resData;
    }


} /* namespace internal */

} /* namespace ut */
} /* namespace nw */

#endif /* NW_UT_RESDICTIONARY_H_ */
