#pragma once

#include <nw/gfx/gfx_Common.h>
#include <nw/ut/ut_MoveArray.h>
#include <nw/gfx/gfx_AmbientLight.h>
#include <nw/gfx/gfx_HemiSphereLight.h>
#include <nw/gfx/gfx_FragmentLight.h>
#include <nw/gfx/gfx_VertexLight.h>
#include <nw/gfx/res/gfx_ResSceneEnvironmentSetting.h>
#include <nw/gfx/gfx_SceneContext.h>
#include <nw/gfx/gfx_GfxObject.h>

namespace nw{
namespace gfx{

typedef nw::ut::FixedSizeArray<FragmentLight*, LIGHT_COUNT> FixedFragmentLightArray;

class LightSet : public GfxObject{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(LightSet);

public:
    NW_UT_RUNTIME_TYPEINFO;

    enum{
        DEFAULT_MAX_VERTEX_LIGHTS = 4
    };

    struct Description{
        Description(): 
            isFixedSizeMemory(true),
            maxVertexLights(DEFAULT_MAX_VERTEX_LIGHTS)
        {}
        
        bool isFixedSizeMemory;
        s32  maxVertexLights;
    };

    class DynamicBuilder{
    public:
        DynamicBuilder() {}
        ~DynamicBuilder() {}

        DynamicBuilder& IsFixedSizeMemory(bool isFixedSizeMemory){
            mDescription.isFixedSizeMemory = isFixedSizeMemory;
            return *this;
        }

        DynamicBuilder& MaxVertexLights(s32 maxVertexLights){
            mDescription.maxVertexLights = maxVertexLights;
            return *this;
        }

        LightSet* Create(nw::os::IAllocator* allocator);

    private:
        LightSet::Description mDescription;
    };

    static LightSet* Create(ResLightSet resource,nw::os::IAllocator* allocator);
    static void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize,ResLightSet resource){
        nw::os::MemorySizeCalculator& size = *pSize;

        size += sizeof(LightSet);

        size += sizeof(VertexLight*) * resource.GetLightsCount();
    }

    void SetAmbientLight(AmbientLight* light){
        this->mAmbientLight = light;
    }

    AmbientLight* GetAmbientLight(){
        return this->mAmbientLight;
    }

    const AmbientLight* GetAmbientLight() const{
        return this->mAmbientLight;
    }

    void SetHemiSphereLight(HemiSphereLight* light){
        this->mHemiSphereLight = light;
    }

    HemiSphereLight* GetHemiSphereLight(){
        return this->mHemiSphereLight;
    }

    const HemiSphereLight* GetHemiSphereLight() const{
        return this->mHemiSphereLight;
    }

    void SetVertexLight(VertexLight* light){
        this->mVertexLights.push_back(light);
    }

    VertexLightArray::iterator GetVertexLightBegin(){
        return this->mVertexLights.begin();
    }

    VertexLightArray::const_iterator GetVertexLightBegin() const{
        return this->mVertexLights.begin();
    }

    VertexLightArray::iterator GetVertexLightEnd(){
        return this->mVertexLights.end();
    }

    VertexLightArray::const_iterator GetVertexLightEnd() const{
        return this->mVertexLights.end();
    }

    s32 GetVertexLightCount() const{
        return this->mVertexLights.size();
    }

    void SetFragmentLight(FragmentLight* light){
        this->mFragmentLights.push_back(light);
    }

    FixedFragmentLightArray::iterator GetFragmentLightBegin(){
        return this->mFragmentLights.begin();
    }

    FixedFragmentLightArray::const_iterator GetFragmentLightBegin() const{
        return this->mFragmentLights.begin();
    }

    FixedFragmentLightArray::iterator GetFragmentLightEnd(){
        return this->mFragmentLights.end();
    }

    FixedFragmentLightArray::const_iterator GetFragmentLightEnd() const{
        return this->mFragmentLights.end();
    }

    s32 GetFragmentLightCount() const{
        return this->mFragmentLights.size();
    }

    void ClearAll(){
        this->mAmbientLight = NULL;
        this->mHemiSphereLight = NULL;
        this->mVertexLights.clear();
        this->mFragmentLights.clear();
    }

protected:
    LightSet(nw::os::IAllocator* allocator,ResLightSet resObj,const LightSet::Description& description): 
        GfxObject(allocator),
        mResource(resObj),
        mAmbientLight(NULL),
        mHemiSphereLight(NULL){
        NW_UNUSED_VARIABLE(description);
    }

    virtual ~LightSet() {}
    
private:
    ResLightSet mResource;

    AmbientLight* mAmbientLight;
    HemiSphereLight* mHemiSphereLight;
    VertexLightArray mVertexLights;
    FixedFragmentLightArray mFragmentLights;
};

}
}