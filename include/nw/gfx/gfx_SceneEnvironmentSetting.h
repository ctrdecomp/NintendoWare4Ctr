#pragma once

#include <nw/gfx/res/gfx_ResSceneEnvironmentSetting.h>
#include <nw/ut/ut_MoveArray.h>
#include <nw/gfx/gfx_GfxObject.h>
#include <nw/gfx/gfx_SceneObject.h>
#include <nw/gfx/gfx_Fog.h>
#include <nw/gfx/gfx_LightSet.h>
#include <nw/gfx/gfx_Camera.h>
#include <nw/gfx/gfx_SceneContext.h>
#include <functional>

namespace nw{
namespace gfx{

class SceneEnvironmentSetting : public SceneObject{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(SceneEnvironmentSetting);

public:
    NW_UT_RUNTIME_TYPEINFO;

    struct LightSetBinder{
        LightSetBinder(): 
            index(-1)
        {}

        s32 index;
        GfxPtr<LightSet> lightSet;
    };

    struct CameraBinder{
        CameraBinder(): 
            index(-1),
            camera(NULL)
        {}

        s32 index;
        Camera* camera;
    };

    struct FogBinder{
        FogBinder(): 
            index(-1),
            fog(NULL)
        {}

        s32 index;
        Fog* fog;
    };

    typedef nw::ut::MoveArray<LightSetBinder> LightSetBinderArray;
    typedef nw::ut::MoveArray<CameraBinder> CameraBinderArray;
    typedef nw::ut::MoveArray<FogBinder> FogBinderArray;

    struct Description{
        Description(){}
    };

    static SceneEnvironmentSetting* Create(ResSceneObject resource,const SceneEnvironmentSetting::Description& description,nw::os::IAllocator* allocator);

    static size_t GetMemorySize(ResSceneEnvironmentSetting resource,Description description,size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT){
        nw::os::MemorySizeCalculator size(alignment);

        GetMemorySizeInternal(&size, resource, description);

        return size.GetSizeWithPadding(alignment);
    }

    static void GetMemorySizeInternal( nw::os::MemorySizeCalculator* pSize,ResSceneEnvironmentSetting resource,Description description);

    void ResolveReference(const SceneContext& sceneContext);

    void Clear();

    ResSceneEnvironmentSetting GetResSceneEnvironmentSetting() {
        return nw::ut::ResDynamicCast<ResSceneEnvironmentSetting>(this->GetResSceneObject());
    }

    const ResSceneEnvironmentSetting GetResSceneEnvironmentSetting() const {
        return nw::ut::ResDynamicCast<ResSceneEnvironmentSetting>(this->GetResSceneObject());
    }

    CameraBinderArray::iterator GetCameraBegin(){
        return this->mCameras.begin();
    }

    CameraBinderArray::const_iterator GetCameraBegin() const{
        return this->mCameras.begin();
    }

    CameraBinderArray::iterator GetCameraEnd(){
        return this->mCameras.end();
    }

    CameraBinderArray::const_iterator GetCameraEnd() const{
        return this->mCameras.end();
    }

    FogBinderArray::iterator GetFogBegin(){
        return this->mFogs.begin();
    }

    FogBinderArray::const_iterator GetFogBegin() const{
        return this->mFogs.begin();
    }

    FogBinderArray::iterator GetFogEnd(){
        return this->mFogs.end();
    }

    FogBinderArray::const_iterator GetFogEnd() const{
        return this->mFogs.end();
    }

    LightSetBinderArray::iterator GetLightSetBegin(){
        return this->mLightSets.begin();
    }

    LightSetBinderArray::const_iterator GetLightSetBegin() const{
        return this->mLightSets.begin();
    }

    LightSetBinderArray::iterator GetLightSetEnd(){
        return this->mLightSets.end();
    }

    LightSetBinderArray::const_iterator GetLightSetEnd() const{
        return this->mLightSets.end();
    }
protected:
    SceneEnvironmentSetting(nw::os::IAllocator* allocator,ResSceneEnvironmentSetting resSetting,const SceneEnvironmentSetting::Description& description): 
    SceneObject(allocator, resSetting){
        NW_UNUSED_VARIABLE(description);
    }

    virtual ~SceneEnvironmentSetting() {}
private:
    void CreateEnvironmentArray(nw::os::IAllocator* allocator, ResSceneEnvironmentSetting );

    template<typename TObject>
    struct SceneObjectCompare: public std::unary_function<TObject, bool>{
        SceneObjectCompare(ResReferenceSceneObject referenceSceneObject): mObject(referenceSceneObject){}

        ResReferenceSceneObject mObject;
        bool operator()(TObject* lhs) const{
            if (lhs->GetName() != NULL &&
                mObject.GetPath() != NULL &&
                std::strcmp(lhs->GetName(), mObject.GetPath()) == 0){
                return true;
            }
            return false;
        }
    };

    LightSetBinderArray mLightSets;
    CameraBinderArray mCameras;
    FogBinderArray mFogs;
};

}
}