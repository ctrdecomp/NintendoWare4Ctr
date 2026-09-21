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

class LightSet : public GfxObject
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(LightSet);

public:
    NW_UT_RUNTIME_TYPEINFO;

    enum
    {
        DEFAULT_MAX_VERTEX_LIGHTS = 4
    };

    struct Description
    {
        Description(): 
            isFixedSizeMemory(true),
            maxVertexLights(DEFAULT_MAX_VERTEX_LIGHTS) {}
        
        bool isFixedSizeMemory;
        s32  maxVertexLights;
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

        DynamicBuilder& MaxVertexLights(s32 maxVertexLights)
        {
            m_Description.maxVertexLights = maxVertexLights;
            return *this;
        }

        LightSet* Create(nw::os::IAllocator* allocator);

    private:
        LightSet::Description m_Description;
    };

    static LightSet* Create(ResLightSet resource,nw::os::IAllocator* allocator);
    static void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize,ResLightSet resource)
    {
        nw::os::MemorySizeCalculator& size = *pSize;

        size += sizeof(LightSet);

        size += sizeof(VertexLight*) * resource.GetLightsCount();
    }

    void SetAmbientLight(AmbientLight* light)
    {
        this->m_AmbientLight = light;
    }

    AmbientLight* GetAmbientLight()
    {
        return this->m_AmbientLight;
    }

    const AmbientLight* GetAmbientLight() const
    {
        return this->m_AmbientLight;
    }

    void SetHemiSphereLight(HemiSphereLight* light)
    {
        this->m_HemiSphereLight = light;
    }

    HemiSphereLight* GetHemiSphereLight()
    {
        return this->m_HemiSphereLight;
    }

    const HemiSphereLight* GetHemiSphereLight() const
    {
        return this->m_HemiSphereLight;
    }

    void SetVertexLight(VertexLight* light)
    {
        this->m_VertexLights.push_back(light);
    }

    VertexLightArray::iterator GetVertexLightBegin()
    {
        return this->m_VertexLights.begin();
    }

    VertexLightArray::const_iterator GetVertexLightBegin() const
    {
        return this->m_VertexLights.begin();
    }

    VertexLightArray::iterator GetVertexLightEnd()
    {
        return this->m_VertexLights.end();
    }

    VertexLightArray::const_iterator GetVertexLightEnd() const
    {
        return this->m_VertexLights.end();
    }

    s32 GetVertexLightCount() const
    {
        return this->m_VertexLights.size();
    }

    void SetFragmentLight(FragmentLight* light)
    {
        this->m_FragmentLights.push_back(light);
    }

    FixedFragmentLightArray::iterator GetFragmentLightBegin()
    {
        return this->m_FragmentLights.begin();
    }

    FixedFragmentLightArray::const_iterator GetFragmentLightBegin() const
    {
        return this->m_FragmentLights.begin();
    }

    FixedFragmentLightArray::iterator GetFragmentLightEnd()
    {
        return this->m_FragmentLights.end();
    }

    FixedFragmentLightArray::const_iterator GetFragmentLightEnd() const
    {
        return this->m_FragmentLights.end();
    }

    s32 GetFragmentLightCount() const
    {
        return this->m_FragmentLights.size();
    }

    void ClearAll()
    {
        this->m_AmbientLight = NULL;
        this->m_HemiSphereLight = NULL;
        this->m_VertexLights.clear();
        this->m_FragmentLights.clear();
    }

protected:
    LightSet(nw::os::IAllocator* allocator,ResLightSet resObj,const LightSet::Description& description): 
        GfxObject(allocator),
        m_Resource(resObj),
        m_AmbientLight(NULL),
        m_HemiSphereLight(NULL)
        {
        NW_UNUSED_VARIABLE(description);
    }

    virtual ~LightSet() {}
    
private:
    ResLightSet m_Resource;

    AmbientLight* m_AmbientLight;
    HemiSphereLight* m_HemiSphereLight;
    VertexLightArray m_VertexLights;
    FixedFragmentLightArray m_FragmentLights;
};

}
}