#pragma once

#include <nw/gfx/gfx_Light.h>
#include <nw/gfx/res/gfx_ResLight.h>

#include <nw/ut/ut_MovePtr.h>
#include <functional>

namespace nw{
namespace gfx{

class VertexLight : public Light
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(VertexLight);

public:
    NW_UT_RUNTIME_TYPEINFO;

    const math::VEC3& Direction() const
    {
        return this->m_Direction;
    }
    
    math::VEC3& Direction()
    {
        return this->m_Direction;
    }

    struct Description : public Light::Description
    {
        Description() {}
    };

    class DynamicBuilder
    {
    public:
        DynamicBuilder() { }
        ~DynamicBuilder() { }

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

        VertexLight* Create(os::IAllocator* allocator);

        size_t GetMemorySize(size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT) const;

    private:
        VertexLight::Description m_Description;
    };

    static VertexLight* Create(SceneNode* parent,ResSceneObject resource,const VertexLight::Description& description,os::IAllocator* allocator);

    static void GetMemorySizeInternal(os::MemorySizeCalculator* pSize,ResVertexLight resource,Description description);

    virtual void UpdateDirection();
    virtual void Accept(ISceneVisitor* visitor);

    ResVertexLight GetResVertexLight() 
    {
        return ResStaticCast<ResVertexLight>(this->GetResSceneObject());
    }

    const ResVertexLight GetResVertexLight() const 
    {
        return ResStaticCast<ResVertexLight>(this->GetResSceneObject());
    }

protected:
    struct ResVertexLightDataDestroyer : public std::unary_function<ResVertexLightData*, void>
    {
        ResVertexLightDataDestroyer(os::IAllocator* allocator = 0) : 
            m_Allocator(allocator) {}
        
        result_type operator()(argument_type data)
        {
            DestroyResVertexLight(m_Allocator, data);
        }

        nw::os::IAllocator* m_Allocator;
    };

    typedef ut::MovePtr<ResVertexLightData, ResVertexLightDataDestroyer> ResPtr;

    VertexLight(os::IAllocator* allocator,ResVertexLight resObj,const VertexLight::Description& description): 
        Light(allocator,resObj,description),
        m_Direction(resObj.GetDirection()) {}

    VertexLight(os::IAllocator* allocator,ResPtr resource,const VertexLight::Description& description): 
        Light(allocator,ResVertexLight(resource.Get()),description),m_Resource(resource)
    {
        m_Direction = this->GetResVertexLight().GetDirection();
    }

    virtual ~VertexLight()
    {
        DestroyOriginalValue();
    }

private:
    virtual Result Initialize(os::IAllocator* allocator);
    static ResVertexLightData*    CreateResVertexLight(os::IAllocator* allocator, const char* name = NULL);

    static void DestroyResVertexLight(os::IAllocator* allocator, ResVertexLightData* resVertexLight);
    
    Result CreateOriginalValue(os::IAllocator* allocator);

    virtual u32 GetLightType() const
    {
        return anim::ResLightAnimData::LIGHT_TYPE_VERTEX;
    }

    virtual u32 GetLightKind() const
    {
        return GetResVertexLight().GetLightKind();
    }

    ResPtr m_Resource;
    nw::math::VEC3 m_Direction;
};

}
}