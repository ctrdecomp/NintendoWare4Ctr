#pragma once

#include <nw/gfx/gfx_Light.h>
#include <nw/gfx/res/gfx_ResLight.h>

#include <nw/ut/ut_MovePtr.h>
#include <functional>

namespace nw{
namespace gfx{

class AmbientLight : public Light
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(AmbientLight);

public:
    NW_UT_RUNTIME_TYPEINFO;

    struct Description : public Light::Description
    {
        Description() {}
    };

    class DynamicBuilder
    {
    public:
        DynamicBuilder() {}
        ~DynamicBuilder() {}

        DynamicBuilder& IsFixedSizeMemory(bool isFixedSizeMemory)
        {
            m_Description.isFixedSizeMemory = isFixedSizeMemory;
            return *this;
        }

        DynamicBuilder& MaxChildren(int maxChildren)
        {
            m_Description.maxChildren = maxChildren;
            return *this;
        }

        DynamicBuilder& MaxCallbacks(int maxCallbacks)
        {
            m_Description.maxCallbacks = maxCallbacks;
            return *this;
        }

        AmbientLight* Create(nw::os::IAllocator* allocator);

        size_t GetMemorySize(size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT) const;

    private:
        AmbientLight::Description m_Description;
    };

    static AmbientLight* Create(SceneNode* parent,ResSceneObject resource, const AmbientLight::Description& description, os::IAllocator* allocator);

    static size_t GetMemorySize(ResAmbientLight resource, Description description, size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT)
    {
        os::MemorySizeCalculator size(alignment);

        GetMemorySizeInternal(&size, resource, description);

        return size.GetSizeWithPadding(alignment);
    }

    static void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize, ResAmbientLight resource, Description description);

    virtual void Accept(ISceneVisitor* visitor);

    ResAmbientLight GetResAmbientLight()
    {
        return ResStaticCast<ResAmbientLight>(this->GetResSceneObject());
    }

    const ResAmbientLight GetResAmbientLight() const
    {
        return ResStaticCast<ResAmbientLight>(this->GetResSceneObject());
    }

protected:
    struct ResAmbientLightDataDestroyer : public std::unary_function<ResAmbientLightData*, void>
{
        ResAmbientLightDataDestroyer(os::IAllocator* allocator = 0): 
            m_Allocator(allocator) {}

        result_type operator()(argument_type data)
        {
            DestroyResAmbientLight(m_Allocator, data);
        }

        nw::os::IAllocator* m_Allocator;
    };

    typedef nw::ut::MovePtr<ResAmbientLightData, ResAmbientLightDataDestroyer> ResPtr;

    AmbientLight(nw::os::IAllocator* allocator,ResAmbientLight resObj,const AmbientLight::Description& description): 
        Light(allocator,resObj,description) {}

    AmbientLight(nw::os::IAllocator* allocator,ResPtr resource,const AmbientLight::Description& description): 
        Light(allocator,ResAmbientLight(resource.Get()),description),
        m_Resource(resource) {}

    virtual ~AmbientLight()
    {
        DestroyOriginalValue();
    }
    
private:
    virtual Result Initialize(nw::os::IAllocator* allocator);
    static ResAmbientLightData*  CreateResAmbientLight(os::IAllocator* allocator, const char* name = NULL);

    static void DestroyResAmbientLight(nw::os::IAllocator* allocator, ResAmbientLightData* resAmbientLight);

    virtual u32 GetLightType() const
    {
        return anim::ResLightAnimData::LIGHT_TYPE_AMBIENT;
    }

    virtual u32 GetLightKind() const
    {
        return ResLight::KIND_UNUSED;
    }
    Result CreateOriginalValue(nw::os::IAllocator* allocator);

    ResPtr m_Resource;
};

}
}