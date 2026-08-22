#pragma once

#include <nw/types.h>
#include <nw/ut/ut_Flag.h>
#include <nw/gfx/gfx_CalculatedTransform.h>

namespace nw   {
namespace anim {

class AnimResult{
public:
    enum Flags{
        FLAG_VALID_COMPONENT0_SHIFT = 0,
        FLAG_VALID_COMPONENT1_SHIFT = 1,
        FLAG_VALID_COMPONENT2_SHIFT = 2,
        FLAG_VALID_COMPONENT3_SHIFT = 3,
        FLAG_CONVERTED_SHIFT = 30,
        FLAG_VALID_COMPONENT0 = 0x1 << FLAG_VALID_COMPONENT0_SHIFT,

        FLAG_VALID_COMPONENT1 = 0x1 << FLAG_VALID_COMPONENT1_SHIFT,

        FLAG_VALID_COMPONENT2 = 0x1 << FLAG_VALID_COMPONENT2_SHIFT,

        FLAG_VALID_COMPONENT3 = 0x1 << FLAG_VALID_COMPONENT3_SHIFT,

        FLAG_CONVERTED = 0x1 << FLAG_CONVERTED_SHIFT
    };

    enum { MAX_COMPONENTS = 4 };

    AnimResult(): 
        mFlags(0) 
    {}

    virtual ~AnimResult() {}

    bit32 GetFlags() const { return mFlags; }

    void SetFlags(bit32 flags) { mFlags = flags; }

    bool IsEnabledFlags(bit32 flags) const { return nw::ut::CheckFlag(mFlags, flags); } 

    void EnableFlags(bit32 flags) { mFlags = nw::ut::EnableFlag(mFlags, flags); }

    void DisableFlags(bit32 flags) { mFlags = nw::ut::DisableFlag(mFlags, flags); }

    void EnableFlags(bit32 flags, bool enable){
        if (enable){
            mFlags |= flags;
        }
        else{
            mFlags &= ~flags;
        }
    }

    void ResetFlags() { mFlags = 0; }

    const void* GetValueBuffer() const { return mValueBuf; }

    void* GetValueBuffer() { return mValueBuf; }

    int GetOffsetToValueBuffer() const{
        return reinterpret_cast<const u8*>(GetValueBuffer()) -
            reinterpret_cast<const u8*>(this);
    }

private:
    bit32 mFlags;

    float mValueBuf[(sizeof(gfx::CalculatedTransform) + sizeof(float) - 1) / sizeof(float)];
};

}
}