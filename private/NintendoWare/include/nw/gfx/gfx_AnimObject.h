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

#ifndef NW_GFX_ANIMOBJECT_H_
#define NW_GFX_ANIMOBJECT_H_

#include <nw/math.h>
#include <nw/anim/anim_AnimResult.h>
#include <nw/gfx/gfx_GfxObject.h>
#include <nw/ut/ut_RuntimeTypeInfo.h>

namespace nw {
namespace gfx {

class AnimGroup;

//---------------------------------------------------------------------------
//
//
//
//
//
//
//---------------------------------------------------------------------------
NW_INLINE bool
AnimWeightNearlyEqual(const float weight, const float value)
{
    // Values of less than 0.1% are ignored.
    const float Epsilon = 0.001f;
    return math::FAbs(weight - value) <= Epsilon;
}

//---------------------------------------------------------------------------
//
//
//
//
//
//---------------------------------------------------------------------------
NW_INLINE bool
AnimWeightNearlyEqualZero(const float weight)
{
    return AnimWeightNearlyEqual(weight, 0.0f);
}

//---------------------------------------------------------------------------
//
//
//
//
//
//---------------------------------------------------------------------------
NW_INLINE bool
AnimWeightNearlyEqualOne(const float weight)
{
    return AnimWeightNearlyEqual(weight, 1.0f);
}

//---------------------------------------------------------------------------
//
//
//
//
//
//---------------------------------------------------------------------------
NW_INLINE float
GetAnimWeightNormalizeScale(float weightSum)
{
    return
        AnimWeightNearlyEqualOne(weightSum) ||
        AnimWeightNearlyEqualZero(weightSum) ?
        1.0f : 1.0f / weightSum;
}

//---------------------------------------------------------------------------
//
//
//
//
//
//---------------------------------------------------------------------------
class AnimObject : public GfxObject
{
public:
    //
    NW_UT_RUNTIME_TYPEINFO;

    //----------------------------------------
    //
    //

    //
    AnimObject(os::IAllocator* allocator, u32 animType)
    : GfxObject(allocator),
      m_AnimGroup(NULL),
      m_AnimType(animType) {}

    //
    virtual ~AnimObject() {}

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    //
    //
    //
    bool Bind(AnimGroup* animGroup)
    {
        return TryBind(animGroup).IsSuccess();
    }

    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    virtual Result TryBind(AnimGroup* animGroup) = 0;

    //
    virtual void Release() = 0;

    //
    virtual void UpdateFrame() = 0;

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    //
    //
    //
    //
    virtual const anim::AnimResult* GetResult(
        void* target,
        int memberIdx) const = 0;

    //

    //----------------------------------------
    //
    //

    //
    const AnimGroup* GetAnimGroup() const { return m_AnimGroup; }

    //
    AnimGroup* GetAnimGroup() { return m_AnimGroup; }

    //
    void SetAnimGroup(AnimGroup* group) { m_AnimGroup = group; }

    //
    //
    //
    //
    //
    //
    virtual bool HasMemberAnim(int memberIdx) const = 0;

    //

    //----------------------------------------
    //
    //

    //
    virtual void UpdateCache() = 0;

    //
    enum
    {
        ANIMTYPE_SIMPLE,
        ANIMTYPE_TRANSFORM_SIMPLE,
        ANIMTYPE_BLENDER
    };

    //
    u32 GetAnimType() const { return m_AnimType; }

    //

protected:
    //
    AnimGroup* m_AnimGroup;
    //
    u32        m_AnimType;
};

} // namespace gfx
} // namespace nw

#endif // NW_GFX_ANIMOBJECT_H_
