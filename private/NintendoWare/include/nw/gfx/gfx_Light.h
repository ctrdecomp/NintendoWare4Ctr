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

#ifndef NW_GFX_LIGHT_H_
#define NW_GFX_LIGHT_H_

#include <nw/gfx/gfx_TransformNode.h>
#include <nw/gfx/res/gfx_ResLight.h>

namespace nw
{
namespace gfx
{

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class Light : public TransformNode
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(Light);

public:
    NW_UT_RUNTIME_TYPEINFO;

    //
    struct Description : public TransformNode::Description
    {
        //
        Description(){}
    };


    //----------------------------------------
    //
    //

    //
    //
    //
    //
    virtual void Accept(ISceneVisitor* visitor);

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    AnimGroup* GetAnimGroup() { return m_AnimGroup; }

    //
    //
    //
    //
    const AnimGroup* GetAnimGroup() const { return m_AnimGroup; }

    //
    AnimObject* GetAnimObject()
    {
        NW_NULL_ASSERT(m_AnimBinding);
        return m_AnimBinding->GetAnimObject(0);
    }

    //
    const AnimObject* GetAnimObject() const
    {
        NW_NULL_ASSERT(m_AnimBinding);
        return m_AnimBinding->GetAnimObject(0);
    }

    //
    //
    //
    void SetAnimObject(AnimObject* animObject)
    {
        NW_NULL_ASSERT(m_AnimBinding);
        NW_FAILSAFE_IF(!ValidateLightAnimType(animObject))
        {
            NW_LOG("type mismatch between Light and Animation. Animation did not set.\n");
            return;
        }
        m_AnimBinding->SetAnimObject(0, animObject);
    }

    //

protected:
    //----------------------------------------
    //
    //

    //
    Light(
        os::IAllocator* allocator,
        ResTransformNode resObj,
        const Light::Description& description)
    : TransformNode(
        allocator,
        resObj,
        description),
      m_OriginalValue(NULL),
      m_AnimGroup(NULL)
    {}

    //
    virtual ~Light()
    {
        ut::SafeDestroy(m_AnimGroup);
    }

    //

    //
    Result CreateAnimGroup(os::IAllocator* allocator);

    //
    void* GetAnimTargetObject(const anim::ResAnimGroupMember& anim);

    //
    void DestroyOriginalValue();

    // Used in ValidateLightAnimType.
    virtual u32 GetLightType() const = 0; //
    virtual u32 GetLightKind() const = 0; //

    ResLight m_OriginalValue;
    math::Transform3 m_OriginalTransform;

private:
    //
    //
    //
    bool ValidateLightAnimType(AnimObject* animObject);

    AnimGroup* m_AnimGroup;
};

} // namespace gfx
} // namespace nw

#endif // NW_GFX_LIGHT_H_
