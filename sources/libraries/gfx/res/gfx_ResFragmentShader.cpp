// Filename: gfx_ResFragmentShader.cpp
//
// Project: NintendoWare4Ctr

#include <nw/gfx/res/gfx_ResUtil.h>
#include <nw/gfx/res/gfx_ResFragmentShader.h>
#include <nw/gfx/res/gfx_ResGraphicsFile.h>

namespace nw {
namespace gfx {
namespace res {

Result ResFragmentShader::Setup(os::IAllocator* allocator, ResGraphicsFile graphicsFile){
    NW_ASSERT(this->IsValid());
    
    Result result = RESOURCE_RESULT_OK;

    ResFragmentLightingTable resFragmentLightingTable = this->GetFragmentLightingTable();
    result |= resFragmentLightingTable.Setup(allocator, graphicsFile);

    if (!this->CheckFragmentShader()){
        result |= RESOURCE_RESULT_IRRELEVANT_LOCATION_LUT;
    }

    return result;
}

Result ResFragmentLightingTable::Setup(os::IAllocator* allocator, ResGraphicsFile graphicsFile){
    NW_ASSERT(this->IsValid());
    
    Result result = RESOURCE_RESULT_OK;
    
    if (this->GetDistribution0Sampler().IsValid()){
        ResReferenceLookupTable resReferenceLut = ResDynamicCast<ResReferenceLookupTable>(this->GetDistribution0Sampler().GetSampler());

        Result referenceResult = SetupReferenceLut(resReferenceLut, graphicsFile);

        if (referenceResult.IsSuccess()){
            result |= this->GetDistribution0Sampler().GetSampler().Setup();
        }

        result |= referenceResult;
    }

    if (this->GetDistribution1Sampler().IsValid()){
        ResReferenceLookupTable resReferenceLut = ResDynamicCast<ResReferenceLookupTable>(this->GetDistribution1Sampler().GetSampler());

        Result referenceResult = SetupReferenceLut(resReferenceLut, graphicsFile);

        if (referenceResult.IsSuccess()){
            result |= this->GetDistribution1Sampler().GetSampler().Setup();
        }

        result |= referenceResult;
    }

    if (this->GetReflectanceRSampler().IsValid()){
        ResReferenceLookupTable resReferenceLut = ResDynamicCast<ResReferenceLookupTable>(this->GetReflectanceRSampler().GetSampler());

        Result referenceResult = SetupReferenceLut(resReferenceLut, graphicsFile);

        if (referenceResult.IsSuccess()){
            result |= this->GetReflectanceRSampler().GetSampler().Setup();
        }

        result |= referenceResult;
    }

    if (this->GetReflectanceGSampler().IsValid()){
        ResReferenceLookupTable resReferenceLut = ResDynamicCast<ResReferenceLookupTable>(this->GetReflectanceGSampler().GetSampler());

        Result referenceResult = SetupReferenceLut(resReferenceLut, graphicsFile);

        if (referenceResult.IsSuccess()){
            result |= this->GetReflectanceGSampler().GetSampler().Setup();
        }

        result |= referenceResult;
    }

    if (this->GetReflectanceBSampler().IsValid()){
        ResReferenceLookupTable resReferenceLut = ResDynamicCast<ResReferenceLookupTable>(this->GetReflectanceBSampler().GetSampler());

        Result referenceResult = SetupReferenceLut(resReferenceLut, graphicsFile);

        if (referenceResult.IsSuccess()){
            result |= this->GetReflectanceBSampler().GetSampler().Setup();
        }

        result |= referenceResult;
    }

    if (this->GetFresnelSampler().IsValid()){
        ResReferenceLookupTable resReferenceLut = ResDynamicCast<ResReferenceLookupTable>(this->GetFresnelSampler().GetSampler());

        Result referenceResult = SetupReferenceLut(resReferenceLut, graphicsFile);

        if (referenceResult.IsSuccess()){
            result |= this->GetFresnelSampler().GetSampler().Setup();
        }

        result |= referenceResult;
    }

    return result;
}

bool ResFragmentShader::CheckFragmentShader(){
    bool isExisted = true;

    ResFragmentLighting fragmentLighting = this->GetFragmentLighting();
    ResFragmentLightingTable fragmentLightingTable = this->GetFragmentLightingTable();

    s32 flags = fragmentLighting.GetFlags();
    if (ut::CheckFlag(flags, ResFragmentLightingData::FLAG_DISTRIBUTION0_ENABLED) && (!fragmentLightingTable.GetDistribution0Sampler().IsValid() || !fragmentLightingTable.GetDistribution0Sampler().GetSampler().IsValid())){
        flags = ut::DisableFlag(flags, ResFragmentLightingData::FLAG_DISTRIBUTION0_ENABLED);
        NW_WARNING(false, "Distribution0 is enable, but LUT is not found.");
        isExisted = false;
    }

    if (ut::CheckFlag(flags, ResFragmentLightingData::FLAG_DISTRIBUTION1_ENABLED) && (!fragmentLightingTable.GetDistribution1Sampler().IsValid() || !fragmentLightingTable.GetDistribution1Sampler().GetSampler().IsValid())){
        flags = ut::DisableFlag(flags, ResFragmentLightingData::FLAG_DISTRIBUTION1_ENABLED);
        NW_WARNING(false, "Distribution1 is enable, but LUT is not found.");
        isExisted = false;
    }

    if (fragmentLighting.GetFresnelConfig() != ResFragmentLighting::CONFIG_NO_FRESNEL && (!fragmentLightingTable.GetFresnelSampler().IsValid() || !fragmentLightingTable.GetFresnelSampler().GetSampler().IsValid())){
        fragmentLighting.SetFresnelConfig(ResFragmentLighting::CONFIG_NO_FRESNEL);
        NW_WARNING(false, "Fresnel is enable, but LUT is not found.");
        isExisted = false;
    }

    if (ut::CheckFlag(flags, ResFragmentLightingData::FLAG_REFLECTION_ENABLED)){
        ResFragmentLighting::ConfigDetail config = ResFragmentLighting::ToConfigDetail(fragmentLighting.GetLayerConfig());
        
        bool invalid = false;

        if (ut::CheckFlag(config, ResFragmentLighting::LUT_RR) && (!fragmentLightingTable.GetReflectanceRSampler().IsValid() || !fragmentLightingTable.GetReflectanceRSampler().GetSampler().IsValid())){
            invalid = true;
        }
        
        if (ut::CheckFlag(config, ResFragmentLighting::LUT_RG) && (!fragmentLightingTable.GetReflectanceGSampler().IsValid() || !fragmentLightingTable.GetReflectanceGSampler().GetSampler().IsValid())){
            invalid = true;
        }
        
        if (ut::CheckFlag(config, ResFragmentLighting::LUT_RB) && (!fragmentLightingTable.GetReflectanceBSampler().IsValid() || !fragmentLightingTable.GetReflectanceBSampler().GetSampler().IsValid())){
            invalid = true;
        }

        if (invalid){
            flags = ut::DisableFlag(flags, ResFragmentLightingData::FLAG_REFLECTION_ENABLED);
            NW_WARNING(false, "Relection is enable, but LUT is not found.\n");
            isExisted = false;
        }
    }
    fragmentLighting.SetFlags(flags);
    return isExisted;
}

void ResFragmentShader::Cleanup(){
    ut::SafeCleanup(this->GetFragmentLightingTable());
}

void ResFragmentLightingTable::Cleanup(){
    ut::SafeCleanup(this->GetReflectanceRSampler());
    ut::SafeCleanup(this->GetReflectanceGSampler());
    ut::SafeCleanup(this->GetReflectanceBSampler());
    ut::SafeCleanup(this->GetDistribution0Sampler());
    ut::SafeCleanup(this->GetDistribution1Sampler());
    ut::SafeCleanup(this->GetFresnelSampler());
}

}
}
}