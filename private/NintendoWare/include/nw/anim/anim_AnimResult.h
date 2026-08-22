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

#ifndef NW_ANIM_ANIMRESULT_H_
#define NW_ANIM_ANIMRESULT_H_

#include <nw/types.h>
#include <nw/ut/ut_Flag.h>
#include <nw/gfx/gfx_CalculatedTransform.h>

namespace nw {
namespace anim {

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class AnimResult
{
public:
    //
    enum Flags
    {
        FLAG_VALID_COMPONENT0_SHIFT = 0, //
        FLAG_VALID_COMPONENT1_SHIFT = 1, //
        FLAG_VALID_COMPONENT2_SHIFT = 2, //
        FLAG_VALID_COMPONENT3_SHIFT = 3, //
        FLAG_CONVERTED_SHIFT = 30, //

        //
        FLAG_VALID_COMPONENT0 = 0x1 << FLAG_VALID_COMPONENT0_SHIFT,

        //
        FLAG_VALID_COMPONENT1 = 0x1 << FLAG_VALID_COMPONENT1_SHIFT,

        //
        FLAG_VALID_COMPONENT2 = 0x1 << FLAG_VALID_COMPONENT2_SHIFT,

        //
        FLAG_VALID_COMPONENT3 = 0x1 << FLAG_VALID_COMPONENT3_SHIFT,

        //
        FLAG_CONVERTED = 0x1 << FLAG_CONVERTED_SHIFT
    };

    //
    enum { MAX_COMPONENTS = 4 };

    //----------------------------------------
    //
    //

    //
    AnimResult()
    : m_Flags(0) {}

    //
    virtual ~AnimResult() {}

    //

    //----------------------------------------
    //
    //

    //
    bit32 GetFlags() const { return m_Flags; }

    //
    void SetFlags(bit32 flags) { m_Flags = flags; }

    //
    bool IsEnabledFlags(bit32 flags) const { return ut::CheckFlag(m_Flags, flags); }

    //
    void EnableFlags(bit32 flags) { m_Flags = ut::EnableFlag(m_Flags, flags); }

    //
    void DisableFlags(bit32 flags) { m_Flags = ut::DisableFlag(m_Flags, flags); }

    //
    void EnableFlags(bit32 flags, bool enable)
    {
        if (enable)
        {
            m_Flags |= flags;
        }
        else
        {
            m_Flags &= ~flags;
        }
    }

    //
    void ResetFlags() { m_Flags = 0; }

    //
    const void* GetValueBuffer() const { return m_ValueBuf; }

    //
    void* GetValueBuffer() { return m_ValueBuf; }

    //
    int GetOffsetToValueBuffer() const
    {
        return reinterpret_cast<const u8*>(GetValueBuffer()) -
            reinterpret_cast<const u8*>(this);
    }

    //

private:
    bit32 m_Flags;

    // The size of the region beyond the maximum value for ResMemberAnim::GetPrimitiveSize()
    float m_ValueBuf[(sizeof(gfx::CalculatedTransform) + sizeof(float) - 1) / sizeof(float)];
};

} /* namespace anim */
} /* namespace nw */

#endif /* NW_ANIM_ANIMRESULT_H_ */
