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

class SceneEnvironmentSetting : public SceneObject
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(SceneEnvironmentSetting);

public:
    NW_UT_RUNTIME_TYPEINFO;

    struct LightSetBinder
    {
        LightSetBinder(): 
            index(-1) {}

        s32 index;
        GfxPtr<LightSet> lightSet;
    };

    struct CameraBinder
    {
        CameraBinder(): 
            index(-1),
            camera(NULL) {}

        s32 index;
        Camera* camera;
    };

    struct FogBinder
    {
        FogBinder(): 
            index(-1),
            fog(NULL) {}

        s32 index;
        Fog* fog;
    };

    typedef nw::ut::MoveArray<LightSetBinder> LightSetBinderArray;
    typedef nw::ut::MoveArray<CameraBinder> CameraBinderArray;
    typedef nw::ut::MoveArray<FogBinder> FogBinderArray;

    struct Description
    {
        Description() {}
    };

    static SceneEnvironmentSetting* Create(ResSceneObject resource,const SceneEnvironmentSetting::Description& description,nw::os::IAllocator* allocator);

    static size_t GetMemorySize(ResSceneEnvironmentSetting resource,Description description,size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT)
    {
        nw::os::MemorySizeCalculator size(alignment);

        GetMemorySizeInternal(&size, resource, description);

        return size.GetSizeWithPadding(alignment);
    }

    static void GetMemorySizeInternal( nw::os::MemorySizeCalculator* pSize,ResSceneEnvironmentSetting resource,Description description);

    void ResolveReference(const SceneContext& sceneContext);

    void Clear();

    ResSceneEnvironmentSetting GetResSceneEnvironmentSetting() 
    {
        return nw::ut::ResDynamicCast<ResSceneEnvironmentSetting>(this->GetResSceneObject());
    }

    const ResSceneEnvironmentSetting GetResSceneEnvironmentSetting() const 
    {
        return nw::ut::ResDynamicCast<ResSceneEnvironmentSetting>(this->GetResSceneObject());
    }

    CameraBinderArray::iterator GetCameraBegin()
    {
        return this->m_Cameras.begin();
    }

    CameraBinderArray::const_iterator GetCameraBegin() const
    {
        return this->m_Cameras.begin();
    }

    CameraBinderArray::iterator GetCameraEnd()
    {
        return this->m_Cameras.end();
    }

    CameraBinderArray::const_iterator GetCameraEnd() const
    {
        return this->m_Cameras.end();
    }

    FogBinderArray::iterator GetFogBegin()
    {
        return this->m_Fogs.begin();
    }

    FogBinderArray::const_iterator GetFogBegin() const
    {
        return this->m_Fogs.begin();
    }

    FogBinderArray::iterator GetFogEnd()
    {
        return this->m_Fogs.end();
    }

    FogBinderArray::const_iterator GetFogEnd() const
    {
        return this->m_Fogs.end();
    }

    LightSetBinderArray::iterator GetLightSetBegin()
    {
        return this->m_LightSets.begin();
    }

    LightSetBinderArray::const_iterator GetLightSetBegin() const
    {
        return this->m_LightSets.begin();
    }

    LightSetBinderArray::iterator GetLightSetEnd()
    {
        return this->m_LightSets.end();
    }

    LightSetBinderArray::const_iterator GetLightSetEnd() const
    {
        return this->m_LightSets.end();
    }
protected:
    SceneEnvironmentSetting(nw::os::IAllocator* allocator, ResSceneEnvironmentSetting resSetting,const SceneEnvironmentSetting::Description& description): 
    SceneObject(allocator, resSetting)
    {
        NW_UNUSED_VARIABLE(description);
    }

    virtual ~SceneEnvironmentSetting() {}
private:
    void CreateEnvironmentArray(nw::os::IAllocator* allocator, ResSceneEnvironmentSetting );

    template<typename TObject>
    struct SceneObjectCompare: public std::unary_function<TObject, bool>
    {
        SceneObjectCompare(nw::gfx::res::ResReferenceSceneObject referenceSceneObject): 
            m_Object(referenceSceneObject) 
        {
        }

        nw::gfx::res::ResReferenceSceneObject m_Object;
        bool operator()(TObject* lhs) const
        {
            if (lhs->GetName() != NULL &&
                m_Object.GetPath() != NULL &&
                std::strcmp(lhs->GetName(), m_Object.GetPath()) == 0)
            {
                return true;
            }
            return false;
        }
    };

    LightSetBinderArray m_LightSets;
    CameraBinderArray m_Cameras;
    FogBinderArray m_Fogs;
};

}
}