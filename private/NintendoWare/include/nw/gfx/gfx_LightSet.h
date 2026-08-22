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

#ifndef NW_GFX_LIGHTSET_H_
#define NW_GFX_LIGHTSET_H_

#include <nw/gfx/gfx_Common.h>
#include <nw/ut/ut_MoveArray.h>
#include <nw/gfx/res/gfx_ResSceneEnvironmentSetting.h>
#include <nw/gfx/gfx_SceneContext.h>

#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 1301 // padding inserted in struct.
#endif
namespace nw
{
namespace gfx
{

class AmbientLight;
class VertexLight;
class FragmentLight;
class HemiSphereLight;

//
typedef ut::FixedSizeArray<FragmentLight*, LIGHT_COUNT> FixedFragmentLightArray;

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
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

    //
    struct Description
    {
        //
        Description()
            : isFixedSizeMemory(true),
            maxVertexLights(DEFAULT_MAX_VERTEX_LIGHTS)
        {}

        bool isFixedSizeMemory;
        s32  maxVertexLights;
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
        DynamicBuilder& MaxVertexLights(s32 maxVertexLights)
        {
            m_Description.maxVertexLights = maxVertexLights;
            return *this;
        }

        //
        //
        //
        //
        //
        //
        LightSet* Create(os::IAllocator* allocator);

    private:
        LightSet::Description m_Description;
    };

    //
    //
    //
    //
    //
    //
    //
    static LightSet* Create(
        ResLightSet resource,
        os::IAllocator* allocator);

    //
    static void GetMemorySizeInternal(
        os::MemorySizeCalculator* pSize,
        ResLightSet resource)
    {
        os::MemorySizeCalculator& size = *pSize;

        // LightSet::Create
        size += sizeof(LightSet);

        // CreateEnvironmentArray
        size += sizeof(VertexLight*) * resource.GetLightsCount();
    }

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    void SetAmbientLight(AmbientLight* light)
    {
        this->m_AmbientLight = light;
    }

    //
    //
    //
    //
    AmbientLight* GetAmbientLight()
    {
        return this->m_AmbientLight;
    }

    //
    //
    //
    //
    const AmbientLight* GetAmbientLight() const
    {
        return this->m_AmbientLight;
    }

    //
    //
    //
    //
    void SetHemiSphereLight(HemiSphereLight* light)
    {
        this->m_HemiSphereLight = light;
    }

    //
    //
    //
    //
    HemiSphereLight* GetHemiSphereLight()
    {
        return this->m_HemiSphereLight;
    }

    //
    //
    //
    //
    const HemiSphereLight* GetHemiSphereLight() const
    {
        return this->m_HemiSphereLight;
    }

    //
    //
    //
    //
    //
    //
    bool PushVertexLight(VertexLight* light)
    {
        return this->m_VertexLights.push_back(light);
    }

    //
    //
    //
    //
    //
    VertexLightArray::iterator EraseVertexLight(VertexLightArray::iterator begin, VertexLightArray::iterator end)
    {
        return this->m_VertexLights.erase(begin, end);
    }

    //
    //
    //
    //
    //
    //
    bool EraseFindVertexLight(VertexLight* light)
    {
        return this->m_VertexLights.erase_find(light);
    }

    //
    //
    //
    //
    VertexLightArray::iterator GetVertexLightBegin()
    {
        return m_VertexLights.begin();
    }

    //
    //
    //
    //
    VertexLightArray::const_iterator GetVertexLightBegin() const
    {
        return m_VertexLights.begin();
    }

    //
    //
    //
    //
    VertexLightArray::iterator GetVertexLightEnd()
    {
        return m_VertexLights.end();
    }

    //
    //
    //
    //
    VertexLightArray::const_iterator GetVertexLightEnd() const
    {
        return m_VertexLights.end();
    }

    //
    //
    //
    //
    s32 GetVertexLightCount() const
    {
        return m_VertexLights.size();
    }

    //
    //
    //
    //
    //
    //
    bool PushFragmentLight(FragmentLight* light)
    {
        return this->m_FragmentLights.push_back(light);
    }

    //
    //
    //
    //
    //
    FixedFragmentLightArray::iterator EraseFragmentLight(FixedFragmentLightArray::iterator begin, FixedFragmentLightArray::iterator end)
    {
        return this->m_FragmentLights.erase(begin, end);
    }

    //
    //
    //
    //
    //
    //
    bool EraseFindFragmentLight(FragmentLight* light)
    {
        return this->m_FragmentLights.erase_find(light);
    }

    //
    //
    //
    //
    FixedFragmentLightArray::iterator GetFragmentLightBegin()
    {
        return m_FragmentLights.begin();
    }

    //
    //
    //
    //
    FixedFragmentLightArray::const_iterator GetFragmentLightBegin() const
    {
        return m_FragmentLights.begin();
    }

    //
    //
    //
    //
    FixedFragmentLightArray::iterator GetFragmentLightEnd()
    {
        return m_FragmentLights.end();
    }

    //
    //
    //
    //
    FixedFragmentLightArray::const_iterator GetFragmentLightEnd() const
    {
        return m_FragmentLights.end();
    }

    //
    //
    //
    //
    s32 GetFragmentLightCount() const
    {
        return m_FragmentLights.size();
    }

    //
    void ClearAll()
    {
        this->m_AmbientLight = NULL;
        this->m_HemiSphereLight = NULL;
        this->m_VertexLights.clear();
        this->m_FragmentLights.clear();
    }

    //

protected:

    //----------------------------------------
    //
    //

    //
    LightSet(
        os::IAllocator* allocator,
        ResLightSet resObj,
        const LightSet::Description& description)
    : GfxObject(allocator),
      m_Resource(resObj),
      m_AmbientLight(NULL),
      m_HemiSphereLight(NULL)
    {
        NW_UNUSED_VARIABLE(description);
    }

    //
    virtual ~LightSet() {}

    //

private:
    ResLightSet m_Resource;

    AmbientLight* m_AmbientLight;
    HemiSphereLight* m_HemiSphereLight;
    VertexLightArray m_VertexLights;
    FixedFragmentLightArray m_FragmentLights;
};

} // namespace gfx
} // namespace nw
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 1301 // padding inserted in struct.
#endif

#endif // NW_GFX_LIGHTSET_H_
