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

#ifndef NW_GFX_SCENEENVIRONMENTSETTING_H_
#define NW_GFX_SCENEENVIRONMENTSETTING_H_

#include <nw/gfx/res/gfx_ResSceneEnvironmentSetting.h>
#include <nw/ut/ut_MoveArray.h>
#include <nw/gfx/gfx_GfxObject.h>
#include <functional>

#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 2530 // padding added to end of struct.
#endif
namespace nw
{
namespace gfx
{

class Fog;
class Camera;
class LightSet;

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class SceneEnvironmentSetting : public SceneObject
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(SceneEnvironmentSetting);

public:
    NW_UT_RUNTIME_TYPEINFO;

    //
    struct LightSetBinder
    {
        LightSetBinder()
            : index(-1)
        {}

        s32 index;
        GfxPtr<LightSet> lightSet;
    };

    //
    struct CameraBinder
    {
        CameraBinder()
            : index(-1),
              camera(NULL)
        {}

        s32 index;
        Camera* camera;
    };

    //
    struct FogBinder
    {
        FogBinder()
            : index(-1),
              fog(NULL)
        {}

        s32 index;
        Fog* fog;
    };

    typedef ut::MoveArray<LightSetBinder> LightSetBinderArray;
    typedef ut::MoveArray<CameraBinder> CameraBinderArray;
    typedef ut::MoveArray<FogBinder> FogBinderArray;

    //
    struct Description
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
    //
    //
    //
    //
    //
    static SceneEnvironmentSetting* Create(
        ResSceneObject resource,
        const SceneEnvironmentSetting::Description& description,
        os::IAllocator* allocator);

    //
    //
    //
    //
    //
    static size_t GetMemorySize(
        ResSceneEnvironmentSetting resource,
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
        ResSceneEnvironmentSetting resource,
        Description description);

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    void ResolveReference(const SceneContext& sceneContext);

    //
    //
    //
    void Clear();

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    ResSceneEnvironmentSetting GetResSceneEnvironmentSetting()
    {
        return ResDynamicCast<ResSceneEnvironmentSetting>(this->GetResSceneObject());
    }

    //
    //
    //
    //
    const ResSceneEnvironmentSetting GetResSceneEnvironmentSetting() const
    {
        return ResDynamicCast<ResSceneEnvironmentSetting>(this->GetResSceneObject());
    }

    //
    //
    //
    //
    CameraBinderArray::iterator GetCameraBegin()
    {
        return m_Cameras.begin();
    }

    //
    //
    //
    //
    CameraBinderArray::const_iterator GetCameraBegin() const
    {
        return m_Cameras.begin();
    }

    //
    //
    //
    //
    CameraBinderArray::iterator GetCameraEnd()
    {
        return m_Cameras.end();
    }

    //
    //
    //
    //
    CameraBinderArray::const_iterator GetCameraEnd() const
    {
        return m_Cameras.end();
    }

    //
    //
    //
    //
    FogBinderArray::iterator GetFogBegin()
    {
        return m_Fogs.begin();
    }

    //
    //
    //
    //
    FogBinderArray::const_iterator GetFogBegin() const
    {
        return m_Fogs.begin();
    }

    //
    //
    //
    //
    FogBinderArray::iterator GetFogEnd()
    {
        return m_Fogs.end();
    }

    //
    //
    //
    //
    FogBinderArray::const_iterator GetFogEnd() const
    {
        return m_Fogs.end();
    }

    //
    //
    //
    //
    LightSetBinderArray::iterator GetLightSetBegin()
    {
        return m_LightSets.begin();
    }

    //
    //
    //
    //
    LightSetBinderArray::const_iterator GetLightSetBegin() const
    {
        return m_LightSets.begin();
    }

    //
    //
    //
    //
    LightSetBinderArray::iterator GetLightSetEnd()
    {
        return m_LightSets.end();
    }

    //
    //
    //
    //
    LightSetBinderArray::const_iterator GetLightSetEnd() const
    {
        return m_LightSets.end();
    }

    //

protected:
    //----------------------------------------
    //
    //

    //
    SceneEnvironmentSetting(
        os::IAllocator* allocator,
        ResSceneEnvironmentSetting resSetting,
        const SceneEnvironmentSetting::Description& description
    )
    : SceneObject(allocator, resSetting)
    {
        NW_UNUSED_VARIABLE(description);
    }

    //
    virtual ~SceneEnvironmentSetting() {}

    //

private:
    //
    void CreateEnvironmentArray(os::IAllocator* allocator, ResSceneEnvironmentSetting );

    //
    template<typename TObject>
    struct SceneObjectCompare: public std::unary_function<TObject, bool>
    {
        SceneObjectCompare(ResReferenceSceneObject referenceSceneObject): m_Object(referenceSceneObject){}
        ResReferenceSceneObject m_Object;
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

} // namespace gfx
} // namespace nw
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 2530 // padding added to end of struct.
#endif

#endif // NW_GFX_SCENEENVIRONMENTSETTING_H_
