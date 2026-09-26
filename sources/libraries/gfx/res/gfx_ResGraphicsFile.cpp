// Filename: gfx_ResGraphicsFile.cpp
//
// Project: NintendoWare4Ctr

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/gfx_SceneObject.h>
#include <nw/gfx/res/gfx_ResShape.h>
#include <nw/gfx/res/gfx_ResModel.h>
#include <nw/gfx/res/gfx_ResMaterial.h>
#include <nw/gfx/res/gfx_ResGraphicsFile.h>
#include <nw/ut/ut_MiddlewareString.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2extern.h>

namespace nw {
namespace gfx {
namespace res {

Result ResGraphicsFile::Setup(os::IAllocator* allocator, ResGraphicsFile graphicsFile)
{
    Result result = RESOURCE_RESULT_OK;
    NW_ASSERT(this->IsValid());
    NW_ASSERT(internal::ResCheckRevision(*this));
    NW_ASSERT(internal::ResCheckRevision(graphicsFile));
    
    // Luts
    s32 lutSetsNum = this->GetLutSetsCount();
    for (int i = 0; i < lutSetsNum; ++i)
    {
        result |= this->GetLutSets(i).Setup(allocator, graphicsFile);
    }

    // Lights
    s32 lightNum = this->GetLightsCount();
    for (int i = 0; i < lightNum; ++i)
    {
        result |= this->GetLights(i).Setup(allocator, graphicsFile);
    }

    // Fog
    s32 fogNum = this->GetFogsCount();
    for (int i = 0; i < fogNum; ++i)
    {
        result |= this->GetFogs(i).Setup(allocator, graphicsFile);
    }

    // Shaders
    s32 shaderNum = this->GetShadersCount();
    for (int i = 0; i < shaderNum; ++i)
    {
        result |= this->GetShaders(i).Setup(allocator, graphicsFile);
    }
    
    // Textures
    s32 textureNum = this->GetTexturesCount();
    for (int i = 0; i < textureNum; ++i)
    {
        result |= this->GetTextures(i).Setup(allocator, graphicsFile);
    }

    // Skeleton Animations
    s32 skeletalAnimNum = this->GetSkeletalAnimsCount();
    for (int i = 0; i < skeletalAnimNum; ++i)
    {
        result |= this->GetSkeletalAnims(i).Setup(allocator, graphicsFile);
    }

    // Material Animations
    s32 materialAnimNum = this->GetMaterialAnimsCount();
    for (int i = 0; i < materialAnimNum; ++i)
    {
        result |= this->GetMaterialAnims(i).Setup(allocator, graphicsFile);
    }

    // Visib Animations
    s32 visibilityAnimNum = this->GetVisibilityAnimsCount();
    for (int i = 0; i < visibilityAnimNum; ++i)
    {
        result |= this->GetVisibilityAnims(i).Setup(allocator, graphicsFile);
    }

    // Camera Animations
    s32 cameraAnimNum = this->GetCameraAnimsCount();
    for (int i = 0; i < cameraAnimNum; ++i)
    {
        result |= this->GetCameraAnims(i).Setup(allocator, graphicsFile);
    }

    // Light Animations
    s32 lightAnimNum = this->GetLightAnimsCount();
    for (int i = 0; i < lightAnimNum; ++i)
    {
        result |= this->GetLightAnims(i).Setup(allocator, graphicsFile);
    }

    // Models
    s32 modelNum = this->GetModelsCount();
    for (int i = 0; i < modelNum; ++i)
    {
        result |= this->GetModels(i).Setup(allocator, graphicsFile);
    }
    
    return result;
}

void ResGraphicsFile::Cleanup()
{
    NW_ASSERT(this->IsValid());

    ut::SafeCleanupAll(this->GetModels());
    ut::SafeCleanupAll(this->GetLightAnims());
    ut::SafeCleanupAll(this->GetCameraAnims());
    ut::SafeCleanupAll(this->GetVisibilityAnims());
    ut::SafeCleanupAll(this->GetMaterialAnims());
    ut::SafeCleanupAll(this->GetSkeletalAnims());
    ut::SafeCleanupAll(this->GetTextures());
    ut::SafeCleanupAll(this->GetShaders());
    ut::SafeCleanupAll(this->GetFogs());
    ut::SafeCleanupAll(this->GetLights());
    ut::SafeCleanupAll(this->GetLutSets());
}

}
}
}