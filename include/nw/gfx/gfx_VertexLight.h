#pragma once

#include <nw/gfx/gfx_Light.h>
#include <nw/gfx/res/gfx_ResLight.h>

#include <nw/ut/ut_MovePtr.h>
#include <functional>

namespace nw{
namespace gfx{

class VertexLight : public Light{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(VertexLight);

public:
    NW_UT_RUNTIME_TYPEINFO;

    const math::VEC3& Direction() const{
        return this->mDirection;
    }
    
    math::VEC3& Direction(){
        return this->mDirection;
    }

    struct Description : public Light::Description{
        Description(){}
    };

    class DynamicBuilder{
    public:
        DynamicBuilder() { }
        ~DynamicBuilder() { }

        DynamicBuilder& IsFixedSizeMemory(bool isFixedSizeMemory){
            mDescription.isFixedSizeMemory = isFixedSizeMemory;
            return *this;
        }

        DynamicBuilder& MaxChildren(int maxChildren){
            mDescription.maxChildren = maxChildren;
            return *this;
        }

        DynamicBuilder& MaxCallbacks(int maxCallbacks){
            mDescription.maxCallbacks = maxCallbacks;
            return *this;
        }

        VertexLight* Create(os::IAllocator* allocator);

        size_t GetMemorySize(size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT) const;

    private:
        VertexLight::Description mDescription;
    };

    static VertexLight* Create(SceneNode* parent,ResSceneObject resource,const VertexLight::Description& description,os::IAllocator* allocator);

    static void GetMemorySizeInternal(os::MemorySizeCalculator* pSize,ResVertexLight resource,Description description);

    virtual void UpdateDirection();
    virtual void Accept(ISceneVisitor* visitor);

    ResVertexLight GetResVertexLight() {
        return ResStaticCast<ResVertexLight>(this->GetResSceneObject());
    }

    const ResVertexLight GetResVertexLight() const {
        return ResStaticCast<ResVertexLight>(this->GetResSceneObject());
    }

protected:
    struct ResVertexLightDataDestroyer : public std::unary_function<ResVertexLightData*, void>{
        ResVertexLightDataDestroyer(os::IAllocator* allocator = 0) : 
            mAllocator(allocator)
        {}
        result_type operator()(argument_type data){
            DestroyResVertexLight(mAllocator, data);
        }

        nw::os::IAllocator* mAllocator;
    };

    typedef ut::MovePtr<ResVertexLightData, ResVertexLightDataDestroyer> ResPtr;

    VertexLight(os::IAllocator* allocator,ResVertexLight resObj,const VertexLight::Description& description): 
        Light(allocator,resObj,description),
        mDirection(resObj.GetDirection())
    {}

    VertexLight(os::IAllocator* allocator,ResPtr resource,const VertexLight::Description& description): 
        Light(allocator,ResVertexLight(resource.Get()),description),mResource(resource){
        mDirection = this->GetResVertexLight().GetDirection();
    }

    virtual ~VertexLight(){
        DestroyOriginalValue();
    }

private:
    virtual Result Initialize(os::IAllocator* allocator);
    static ResVertexLightData*    CreateResVertexLight(os::IAllocator* allocator, const char* name = NULL);

    static void DestroyResVertexLight(os::IAllocator* allocator, ResVertexLightData* resVertexLight);
    
    Result CreateOriginalValue(os::IAllocator* allocator);

    virtual u32 GetLightType() const{
        return anim::ResLightAnimData::LIGHT_TYPE_VERTEX;
    }

    virtual u32 GetLightKind() const{
        return GetResVertexLight().GetLightKind();
    }

    ResPtr mResource;
    nw::math::VEC3 mDirection;
};

}
}