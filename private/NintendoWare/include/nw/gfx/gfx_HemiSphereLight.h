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

#ifndef NW_GFX_HEMISPHERELIGHT_H_
#define NW_GFX_HEMISPHERELIGHT_H_

#include <nw/gfx/gfx_Light.h>
#include <nw/gfx/res/gfx_ResLight.h>

#include <nw/ut/ut_MovePtr.h>
#include <functional>

namespace nw
{
namespace gfx
{

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class HemiSphereLight : public Light
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(HemiSphereLight);

public:
    NW_UT_RUNTIME_TYPEINFO;

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
        HemiSphereLight* Create(os::IAllocator* allocator);

        //
        //
        //
        //
        //
        //
       size_t GetMemorySize(size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT) const;

    private:
        HemiSphereLight::Description m_Description;
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
    static HemiSphereLight* Create(
        SceneNode* parent,
        ResSceneObject resource,
        const HemiSphereLight::Description& description,
        os::IAllocator* allocator);

    //
    //
    //
    //
    //
    static size_t GetMemorySize(
        ResHemiSphereLight resource,
        Description description,
        size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT
    )
    {
        os::MemorySizeCalculator size(alignment);

        GetMemorySizeInternal(&size, resource, description);

        return size.GetSizeWithPadding(alignment);
    }

    //
    static void GetMemorySizeInternal(
        os::MemorySizeCalculator* pSize,
        ResHemiSphereLight resource,
        Description description);

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
    ResHemiSphereLight GetResHemiSphereLight()
    {
        return ResStaticCast<ResHemiSphereLight>(this->GetResSceneObject());
    }

    //
    const ResHemiSphereLight GetResHemiSphereLight() const
    {
        return ResStaticCast<ResHemiSphereLight>(this->GetResSceneObject());
    }

    //

protected:
    struct ResHemiSphereLightDataDestroyer : public std::unary_function<ResHemiSphereLightData*, void>
    {
        ResHemiSphereLightDataDestroyer(os::IAllocator* allocator = 0) : m_Allocator(allocator)
        {}
        result_type operator()(argument_type data)
        {
            DestroyResHemiSphereLight(m_Allocator, data);
        }

        os::IAllocator* m_Allocator;
    };

    //
    typedef ut::MovePtr<ResHemiSphereLightData, ResHemiSphereLightDataDestroyer> ResPtr;

    //----------------------------------------
    //
    //

    //
    HemiSphereLight(
        os::IAllocator* allocator,
        ResHemiSphereLight resObj,
        const HemiSphereLight::Description& description)
    : Light(
        allocator,
        resObj,
        description)
    {}

    //
    HemiSphereLight(
        os::IAllocator* allocator,
        ResPtr resource,
        const HemiSphereLight::Description& description)
    : Light(
        allocator,
        ResHemiSphereLight(resource.Get()),
        description),
      m_Resource(resource)
    {}

    //
    virtual ~HemiSphereLight()
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
    static ResHemiSphereLightData*    CreateResHemiSphereLight(os::IAllocator* allocator, const char* name = NULL);

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    static void DestroyResHemiSphereLight(os::IAllocator* allocator, ResHemiSphereLightData* resHemiSphereLight);

    //
    Result CreateOriginalValue(os::IAllocator* allocator);

    //
    virtual u32 GetLightType() const
    {
        return anim::ResLightAnimData::LIGHT_TYPE_HEMISPHERE;
    }

    //
    virtual u32 GetLightKind() const
    {
        // For hemispheres, the type of light source is not considered
        return ResLight::KIND_UNUSED;
    }

    ResPtr m_Resource;
};

} // namespace gfx
} // namespace nw

#endif // NW_GFX_HEMISPHERELIGHT_H_
