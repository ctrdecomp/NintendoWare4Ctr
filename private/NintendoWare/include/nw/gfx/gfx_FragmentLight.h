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

#ifndef NW_GFX_FRAGMENTLIGHT_H_
#define NW_GFX_FRAGMENTLIGHT_H_

#include <nw/gfx/gfx_Light.h>
#include <nw/gfx/res/gfx_ResLight.h>
#include <nw/anim/res/anim_ResAnim.h>

#include <nw/ut/ut_MovePtr.h>
#include <functional>

namespace nw
{
namespace gfx
{

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class FragmentLight : public Light
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(FragmentLight);

public:
    NW_UT_RUNTIME_TYPEINFO;


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
    const math::VEC3& Direction() const
    {
        return this->m_Direction;
    }

    //
    //
    //
    //
    //
    //
    //
    //
    math::VEC3& Direction()
    {
        return this->m_Direction;
    }

    //

    //
    struct Description : public Light::Description
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
    class DynamicBuilder
    {
    public:
        //
        DynamicBuilder() {}

        //
        ~DynamicBuilder() {}

        //
        //
        //
        //
        //
        DynamicBuilder& IsFixedSizeMemory(bool isFixedSizeMemory)
        {
            m_Description.isFixedSizeMemory = isFixedSizeMemory;
            return *this;
        }

        //
        DynamicBuilder& MaxChildren(int maxChildren)
        {
            m_Description.maxChildren = maxChildren;
            return *this;
        }

        //
        DynamicBuilder& MaxCallbacks(int maxCallbacks)
        {
            m_Description.maxCallbacks = maxCallbacks;
            return *this;
        }

        //
        //
        //
        //
        //
        //
        FragmentLight* Create(os::IAllocator* allocator);

        //
        //
        //
        //
        //
        //
        size_t GetMemorySize(size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT) const;

    private:
        FragmentLight::Description m_Description;
    };

    //
    //
    //
    //
    //
    //
    //
    //
    //
    static FragmentLight* Create(
        SceneNode* parent,
        ResSceneObject resource,
        const FragmentLight::Description& description,
        os::IAllocator* allocator);

    //
    //
    //
    //
    //
    static size_t GetMemorySize(
        ResFragmentLight resource,
        Description description,
        size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT
    )
    {
        os::MemorySizeCalculator size(alignment);

        GetMemorySizeInternal(&size, resource, description);

        return size.GetSizeWithPadding(alignment);
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
    //
    //
    //
    //
    //
    //
    //
    static FragmentLight* CreateClone(
        SceneNode* parent,
        ResFragmentLight resource,
        const FragmentLight::Description& description,
        os::IAllocator* allocator);

    //
    //
    //
    //
    //
    static size_t GetMemorySizeForCreateClone(
        ResFragmentLight resource,
        Description description,
        size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT
    );

    //
    static void GetMemorySizeInternal(
        os::MemorySizeCalculator* pSize,
        ResFragmentLight resource,
        Description description);

    //

    //----------------------------------------
    //
    //

    //
    virtual void UpdateDirection();

    //

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
    ResFragmentLight GetResFragmentLight()
    {
        return ResStaticCast<ResFragmentLight>(this->GetResSceneObject());
    }

    //
    const ResFragmentLight GetResFragmentLight() const
    {
        return ResStaticCast<ResFragmentLight>(this->GetResSceneObject());
    }

    //

protected:
    struct ResFragmentLightDataDestroyer : public std::unary_function<ResFragmentLightData*, void>
    {
        ResFragmentLightDataDestroyer(
            os::IAllocator* allocator = NULL
        )
        : m_Allocator(allocator)
        {}
        result_type operator()(argument_type data)
        {
            DestroyResFragmentLight(m_Allocator, data);
        }

        os::IAllocator* m_Allocator;
    };

    //
    typedef ut::MovePtr<ResFragmentLightData, ResFragmentLightDataDestroyer> ResPtr;

    //----------------------------------------
    //
    //

    //
    FragmentLight(
        os::IAllocator* allocator,
        ResFragmentLight resObj,
        const FragmentLight::Description& description)
    : Light(
        allocator,
        resObj,
        description),
      m_Direction(resObj.GetDirection())
    {}

    //
    FragmentLight(
        os::IAllocator* allocator,
        ResPtr resource,
        const FragmentLight::Description& description)
    : Light(
        allocator,
        ResFragmentLight(resource.Get()),
        description),
      m_Resource(resource)
    {
        m_Direction = this->GetResFragmentLight().GetDirection();
    }

    //
    virtual ~FragmentLight()
    {
        DestroyOriginalValue();
    }

    //

protected:
    virtual Result Initialize(os::IAllocator* allocator);

private:
    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    static ResFragmentLightData*    CreateResFragmentLight(os::IAllocator* allocator, const char* name = NULL);

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    static void DestroyResFragmentLight(os::IAllocator* allocator, ResFragmentLightData* resFragmentLight);

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    static ResFragmentLightData*    CloneResFragmentLight(ResFragmentLight resource, os::IAllocator* allocator);

    //
    Result CreateOriginalValue(os::IAllocator* allocator);

        //
    virtual u32 GetLightType() const
    {
        return anim::ResLightAnimData::LIGHT_TYPE_FRAGMENT;
    }

    //
    virtual u32 GetLightKind() const
    {
        return GetResFragmentLight().GetLightKind();
    }

    ResPtr m_Resource;
    math::VEC3 m_Direction;
};

} // namespace gfx
} // namespace nw

#endif // NW_GFX_FRAGMENTLIGHT_H_
