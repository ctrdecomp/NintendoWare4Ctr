#pragma once

#include <nw/gfx/gfx_Light.h>
#include <nw/gfx/res/gfx_ResLight.h>
#include <nw/anim/res/anim_ResAnim.h>

#include <nw/ut/ut_MovePtr.h>
#include <functional>

namespace nw{
namespace gfx{

class FragmentLight : public Light{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(FragmentLight);

public:
    NW_UT_RUNTIME_TYPEINFO;

    const nw::math::VEC3& Direction() const{
        return this->mDirection;
    }

    nw::math::VEC3& Direction(){
        return this->mDirection;
    }

    struct Description : public Light::Description{
        Description(){}
    };

    class DynamicBuilder{
    public:
        DynamicBuilder() {}
        ~DynamicBuilder() {}

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

        FragmentLight* Create(nw::os::IAllocator* allocator);

        size_t GetMemorySize(size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT) const;

    private:
        FragmentLight::Description mDescription;
    };

    static FragmentLight* Create(SceneNode* parent,ResSceneObject resource,const FragmentLight::Description& description,nw::os::IAllocator* allocator);

    static size_t GetMemorySize(ResFragmentLight resource,Description description,size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT){
        nw::os::MemorySizeCalculator size(alignment);

        GetMemorySizeInternal(&size, resource, description);

        return size.GetSizeWithPadding(alignment);
    }

    static void GetMemorySizeInternal( nw::os::MemorySizeCalculator* pSize,ResFragmentLight resource,Description description);

    virtual void UpdateDirection();
    virtual void Accept(ISceneVisitor* visitor);

    ResFragmentLight GetResFragmentLight() {
        return ResStaticCast<ResFragmentLight>(this->GetResSceneObject());
    }

    const ResFragmentLight GetResFragmentLight() const {
        return ResStaticCast<ResFragmentLight>(this->GetResSceneObject());
    }

protected:
    struct ResFragmentLightDataDestroyer : public std::unary_function<ResFragmentLightData*, void>{
        ResFragmentLightDataDestroyer(nw::os::IAllocator* allocator = NULL): 
            mAllocator(allocator)
        {}
        result_type operator()(argument_type data){
            DestroyResFragmentLight(mAllocator, data);
        }

        nw::os::IAllocator* mAllocator;
    };

    typedef nw::ut::MovePtr<ResFragmentLightData, ResFragmentLightDataDestroyer> ResPtr;

    FragmentLight(nw::os::IAllocator* allocator,ResFragmentLight resObj,const FragmentLight::Description& description): 
        Light(allocator,resObj,description),
        mDirection(resObj.GetDirection())
    {}

    FragmentLight(nw::os::IAllocator* allocator,ResPtr resource,const FragmentLight::Description& description): 
        Light(allocator,ResFragmentLight(resource.Get()),description),mResource(resource){
        mDirection = this->GetResFragmentLight().GetDirection();
    }

    virtual ~FragmentLight(){this->DestroyOriginalValue();}
    
private:
    virtual Result Initialize(nw::os::IAllocator* allocator);

    static ResFragmentLightData*    CreateResFragmentLight(os::IAllocator* allocator, const char* name = NULL);
    static void DestroyResFragmentLight(nw::os::IAllocator* allocator, ResFragmentLightData* resFragmentLight);
    static ResFragmentLightData*    CloneResFragmentLight(ResFragmentLight resource, nw::os::IAllocator* allocator);

    Result CreateOriginalValue(nw::os::IAllocator* allocator);

    virtual u32 GetLightType() const{return anim::ResLightAnimData::LIGHT_TYPE_FRAGMENT;}
    virtual u32 GetLightKind() const{return this->GetResFragmentLight().GetLightKind();}

    ResPtr mResource;
    nw::math::VEC3 mDirection;
};

}
}