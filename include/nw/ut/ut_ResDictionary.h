#pragma once

#include <nw/types.h>
#include <nw/Assert.h>
#include <nw/ut/ut_ResTypes.h>
#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResArray.h>

namespace nw {
namespace ut {

struct ResDicLinearData : public DataBlockHeader
{
    Size   numData;
    struct ResDicNodeData
    {
        BinString toName;
        Offset    ofsData;
    }
    data[1];
    
    ResDicNodeData* GetBeginNode() { return data; }
    const ResDicNodeData* GetBeginNode() const { return data; }
};

struct ResDicPatriciaData : public DataBlockHeader
{
    ResU32 numData;
    struct ResDicNodeData
    {
        ResU32 ref;
        ResU16 idxLeft;
        ResU16 idxRight;
        Offset ofsString;
        Offset ofsData;
    }
    data[1];

    ResDicNodeData* GetBeginNode() { return &data[1]; }
    const ResDicNodeData* GetBeginNode() const { return &data[1]; }
};

class ResDicPatricia : public ResCommon<ResDicPatriciaData>
{
public:
    NW_RES_CTOR(ResDicPatricia)
    enum{ NOT_FOUND = -1 };

    s32 GetCount() const { return ref().numData; }

    void* operator[](int idx) const
    {
        if (!this->IsValid()) { return NULL; }
        return const_cast<void*>(ref().data[idx + 1].ofsData.to_ptr());
    }
    void* operator[](u32 idx) const { return operator[](int(idx)); }
    
    void* operator[](const char* s) const
    {
        if (this->IsValid() && s)
        {
            ResDicPatriciaData::ResDicNodeData* x = Get(s, std::strlen(s));

            if (x)
            {
                return const_cast<void*>(x->ofsData.to_ptr());
            }
        }
        return NULL;
    }

    s32 GetIndex(const char* s) const
    {
        if (IsValid() && s)
        {
            size_t len = std::strlen(s);
            ResDicPatriciaData::ResDicNodeData* x = Get(s, len);

            if (x)
            {
                return static_cast<s32>(x - &ptr()->data[1]);
            }
        }
        return -1;
    }
    
    s32 GetIndex(const ResName n) const
    {
        if (IsValid() && n.IsValid())
        {
            ResDicPatriciaData::ResDicNodeData* x = Get(n);

            if (x)
            {
                return static_cast<s32>(x - &ptr()->data[1]);
            }
        }
        return -1;
    }

    const ResName GetResName(u32 idx) const
    {
        if (!IsValid()) { return ResName(NULL); }
        ptr()->data[idx + 1].ofsString - s32(sizeof(u32));
    }

    const char* GetName(u32 idx) const
    {
        if (!IsValid()) { return NULL; }
        return GetResName(idx).GetName();
    }

    u32 GetLength() const { return ref().length; }

protected:
    ResDicPatriciaData::ResDicNodeData* Get(const char* s, size_t len) const;
    ResDicPatriciaData::ResDicNodeData* Get(const ResName rhs) const;
};

namespace internal {

inline ResDicPatriciaData* InitializeResDicPatricia(ResDicPatriciaData* resData)
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

} // namespace internal

} // namespace ut
} // namespace nw