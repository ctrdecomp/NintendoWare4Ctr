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

#ifndef NW_GFX_AMBIENTLIGHT_H_
#define NW_GFX_AMBIENTLIGHT_H_

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
class AmbientLight : public Light
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(AmbientLight);

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
        AmbientLight* Create(os::IAllocator* allocator);

        //
        //
        //
        //
        //
        //
        size_t GetMemorySize(size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT) const;

    private:
        AmbientLight::Description m_Description;
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
    static AmbientLight* Create(
        SceneNode* parent,
        ResSceneObject resource,
        const AmbientLight::Description& description,
        os::IAllocator* allocator);

    //
    //
    //
    //
    //
    static size_t GetMemorySize(
        ResAmbientLight resource,
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
        ResAmbientLight resource,
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
    ResAmbientLight GetResAmbientLight()
    {
        return ResStaticCast<ResAmbientLight>(this->GetResSceneObject());
    }

    //
    const ResAmbientLight GetResAmbientLight() const
    {
        return ResStaticCast<ResAmbientLight>(this->GetResSceneObject());
    }

    //

protected:
    struct ResAmbientLightDataDestroyer : public std::unary_function<ResAmbientLightData*, void>
    {
        ResAmbientLightDataDestroyer(os::IAllocator* allocator = 0) : m_Allocator(allocator)
        {}
        result_type operator()(argument_type data)
        {
            DestroyResAmbientLight(m_Allocator, data);
        }

        os::IAllocator* m_Allocator;
    };

    //
    typedef ut::MovePtr<ResAmbientLightData, ResAmbientLightDataDestroyer> ResPtr;

    //----------------------------------------
    //
    //

    //
    AmbientLight(
        os::IAllocator* allocator,
        ResAmbientLight resObj,
        const AmbientLight::Description& description)
    : Light(
        allocator,
        resObj,
        description)
    {}

    //
    AmbientLight(
        os::IAllocator* allocator,
        ResPtr resource,
        const AmbientLight::Description& description)
    : Light(
        allocator,
        ResAmbientLight(resource.Get()),
        description),
      m_Resource(resource)
    {}

    //
    virtual ~AmbientLight()
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
    static ResAmbientLightData*    CreateResAmbientLight(os::IAllocator* allocator, const char* name = NULL);

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    static void DestroyResAmbientLight(os::IAllocator* allocator, ResAmbientLightData* resAmbientLight);

    //
    virtual u32 GetLightType() const
    {
        return anim::ResLightAnimData::LIGHT_TYPE_AMBIENT;
    }

    //
    virtual u32 GetLightKind() const
    {
        // For ambients, the type of light source is not considered
        return ResLight::KIND_UNUSED;
    }

    //
    Result CreateOriginalValue(os::IAllocator* allocator);

    ResPtr m_Resource;
};

} // namespace gfx
} // namespace nw

#endif // NW_GFX_AMBIENTLIGHT_H_
