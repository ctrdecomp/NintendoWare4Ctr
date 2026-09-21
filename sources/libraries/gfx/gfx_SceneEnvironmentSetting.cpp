// Filename: gfx_SceneEnvironmentSetting.cpp
//
// Project: NintendoWare4Ctr

#include <nw/os/os_Memory.h>

#include <nw/gfx/gfx_SceneEnvironmentSetting.h>
#include <nw/gfx/gfx_ISceneVisitor.h>
#include <nw/gfx/gfx_SceneContext.h>
#include <nw/gfx/gfx_Light.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(SceneEnvironmentSetting, SceneObject);

SceneEnvironmentSetting* SceneEnvironmentSetting::Create(ResSceneObject resource,const SceneEnvironmentSetting::Description& description,os::IAllocator* allocator)
{
    NW_NULL_ASSERT(allocator);
    
    ResSceneEnvironmentSetting resSetting = ResDynamicCast<ResSceneEnvironmentSetting>(resource);
    NW_ASSERT(resSetting.IsValid());
    NW_ASSERT(internal::ResCheckRevision(resSetting));
    
    void* memory = allocator->Alloc(sizeof(SceneEnvironmentSetting));
    NW_NULL_ASSERT(memory);

    SceneEnvironmentSetting* setting = new(memory) SceneEnvironmentSetting(allocator,resSetting,description);

    setting->CreateEnvironmentArray(allocator, resSetting);

    return setting;
}

void SceneEnvironmentSetting::CreateEnvironmentArray(os::IAllocator* allocator, ResSceneEnvironmentSetting resSetting)
{
    if (resSetting.GetLightSetsCount() != 0)
    {
        void* lightSetsMemory = allocator->Alloc(sizeof(LightSetBinder) * resSetting.GetLightSetsCount());
        m_LightSets = ut::MoveArray<LightSetBinder>(lightSetsMemory, resSetting.GetLightSetsCount(), allocator);
        this->m_LightSets.resize(resSetting.GetLightSetsCount());
        
        ResLightSetArray::iterator lightSetEnd = resSetting.GetLightSets().end();
        
        int lightSetIndex = 0;
        for (ResLightSetArray::iterator iter = resSetting.GetLightSets().begin();iter != lightSetEnd;++iter)
        {
            m_LightSets[lightSetIndex].lightSet = GfxPtr<LightSet>(LightSet::Create((*iter), allocator));
            m_LightSets[lightSetIndex].index = -1;
            ++lightSetIndex;
        }
    }

    if (resSetting.GetCamerasCount() != 0)
    {
        void* camerasMemory = allocator->Alloc(sizeof(CameraBinder) * resSetting.GetCamerasCount());
        NW_NULL_ASSERT(camerasMemory);
        m_Cameras = ut::MoveArray<CameraBinder>(camerasMemory, resSetting.GetCamerasCount(), allocator);
    }

    if (resSetting.GetFogsCount() != 0)
    {
        void* fogsMemory = allocator->Alloc(sizeof(FogBinder) * resSetting.GetFogsCount());
        NW_NULL_ASSERT(fogsMemory);
        m_Fogs = ut::MoveArray<FogBinder>(fogsMemory, resSetting.GetFogsCount(), allocator);
    }
}

void SceneEnvironmentSetting::ResolveReference(const SceneContext& sceneContext)
{
    ResSceneEnvironmentSetting resSetting = ResStaticCast<ResSceneEnvironmentSetting>(this->GetResSceneObject());
    NW_ASSERT(resSetting.IsValid());

    ResReferenceSceneObjectArray::iterator cameraEnd = resSetting.GetCameras().end();
    for (ResReferenceSceneObjectArray::iterator iter = resSetting.GetCameras().begin(); iter != cameraEnd; ++iter)
    {
        if ((*iter).IsValid())
        {

            CameraArray::const_iterator found = std::find_if(sceneContext.GetCameraBegin(), sceneContext.GetCameraEnd(), SceneObjectCompare<const Camera>((*iter)));

            if (found != sceneContext.GetCameraEnd())
            {
                CameraBinder cameraBinder;
                cameraBinder.index = (*iter).GetIndex();
                cameraBinder.camera = (*found);
                bool isSuccess = this->m_Cameras.push_back(cameraBinder);
                NW_ASSERT(isSuccess);
            }
        }
    }

    ResReferenceSceneObjectArray::iterator fogEnd = resSetting.GetFogs().end();
    for (ResReferenceSceneObjectArray::iterator iter = resSetting.GetFogs().begin(); iter != fogEnd; ++iter)
    {
        if ((*iter).IsValid())
        {
            FogArray::const_iterator found = std::find_if(sceneContext.GetFogBegin(), sceneContext.GetFogEnd(), SceneObjectCompare<const Fog>((*iter)));

            if (found != sceneContext.GetFogEnd())
            {
                FogBinder fogBinder;
                fogBinder.index = (*iter).GetIndex();
                fogBinder.fog = (*found);
                bool isSuccess = this->m_Fogs.push_back(fogBinder);
                NW_ASSERT(isSuccess);
            }
        }
    }

    int lightSetIndex = 0;
    ResLightSetArray::iterator lightSetEnd = resSetting.GetLightSets().end();
    for (ResLightSetArray::iterator iter = resSetting.GetLightSets().begin(); iter != lightSetEnd; ++iter)
    {
        NW_ASSERT(lightSetIndex < this->m_LightSets.size());
        LightSet* lightSet = this->m_LightSets[lightSetIndex].lightSet.Get();
        NW_ASSERT(lightSet != NULL);
        lightSet->ClearAll();

        ResReferenceSceneObjectArray::iterator lightEnd = (*iter).GetLights().end();
        for (ResReferenceSceneObjectArray::iterator lightIter = (*iter).GetLights().begin(); lightIter != lightEnd; ++lightIter)
        {

            if ((*lightIter).IsValid())
            {
                AmbientLightArray::const_iterator foundAmbient = std::find_if(sceneContext.GetAmbientLightsBegin(), sceneContext.GetAmbientLightsEnd(), SceneObjectCompare<const AmbientLight>((*lightIter)));

                if (foundAmbient != sceneContext.GetAmbientLightsEnd())
                {
                    lightSet->SetAmbientLight(*foundAmbient);
                }

                HemiSphereLightArray::const_iterator foundHemiSphere =
                    std::find_if(sceneContext.GetHemiSphereLightsBegin(), sceneContext.GetHemiSphereLightsEnd(), SceneObjectCompare<const HemiSphereLight>((*lightIter)));

                if (foundHemiSphere != sceneContext.GetHemiSphereLightsEnd())
                {
                    lightSet->SetHemiSphereLight(*foundHemiSphere);
                }
                VertexLightArray::const_iterator foundVertex = std::find_if(sceneContext.GetVertexLightsBegin(), sceneContext.GetVertexLightsEnd(), SceneObjectCompare<const VertexLight>((*lightIter)));

                if (foundVertex != sceneContext.GetVertexLightsEnd())
                {
                    lightSet->SetVertexLight(*foundVertex);
                }
                
                FragmentLightArray::const_iterator foundFragment = std::find_if(sceneContext.GetFragmentLightsBegin(), sceneContext.GetFragmentLightsEnd(), SceneObjectCompare<const FragmentLight>((*lightIter)));

                if (foundFragment != sceneContext.GetFragmentLightsEnd())
                {
                    lightSet->SetFragmentLight(*foundFragment);
                }
            }
        }
        
        this->m_LightSets[lightSetIndex].index = (*iter).GetIndex();
        ++lightSetIndex;
    }
}

void SceneEnvironmentSetting::Clear()
{
    this->m_Cameras.Clear();

    this->m_Fogs.Clear();

    LightSetBinderArray::iterator lightSetBinderEnd = this->m_LightSets.end();
    for (LightSetBinderArray::iterator iter = this->m_LightSets.begin(); iter != lightSetBinderEnd; ++iter)
    {
        LightSet* lightSet = iter->lightSet.Get();
        NW_ASSERT(lightSet != NULL);
        lightSet->ClearAll();
        iter->index = -1;
    }
}

void SceneEnvironmentSetting::GetMemorySizeInternal(os::MemorySizeCalculator* pSize,ResSceneEnvironmentSetting resource,Description description)
{
    NW_UNUSED_VARIABLE(description);

    os::MemorySizeCalculator& size = *pSize;

    size += sizeof(SceneEnvironmentSetting);

    size += sizeof(LightSetBinder) * resource.GetLightSetsCount();

    ResLightSetArray::iterator lightSetEnd = resource.GetLightSets().end();

    for (ResLightSetArray::iterator iter = resource.GetLightSets().begin(); iter != lightSetEnd; ++iter)
    {
        LightSet::GetMemorySizeInternal(&size, (*iter));
    }

    size += sizeof(CameraBinder) * resource.GetCamerasCount();
    size += sizeof(FogBinder) * resource.GetFogsCount();
}

}
}