// Filename: gfx_ResLight.cpp
//
// Project: NintendoWare4Ctr

#include <nw/gfx/res/gfx_ResUtil.h>
#include <nw/gfx/res/gfx_ResLight.h>
#include <nw/gfx/res/gfx_ResGraphicsFile.h>

namespace nw {
namespace gfx {
namespace res {

Result ResLight::Setup(os::IAllocator* allocator, ResGraphicsFile graphicsFile){
    Result result = RESOURCE_RESULT_OK;

    ResFragmentLight fragment = ResDynamicCast<ResFragmentLight>(*this);

    if (fragment.IsValid()){
        ResReferenceLookupTable referenceDistanceSampler = ResDynamicCast<ResReferenceLookupTable>(fragment.GetDistanceSampler());

        Result distanceResult = SetupReferenceLut(referenceDistanceSampler, graphicsFile);
        
        if (fragment.GetDistanceSampler().IsValid()){
            if (distanceResult.IsSuccess()){
                result |= fragment.GetDistanceSampler().Setup();
            }
        }

        result |= distanceResult;

        if (fragment.GetAngleSampler().IsValid()){
            ResLookupTable resLut = fragment.GetAngleSampler().GetSampler();

            if (resLut.IsValid()){
                ResReferenceLookupTable referenceAngleSampler = ResDynamicCast<ResReferenceLookupTable>(resLut);
                
                Result angleResult = SetupReferenceLut(referenceAngleSampler, graphicsFile);

                if (angleResult.IsSuccess()){
                    result |= resLut.Setup();
                }

                result |= angleResult;
            }
            else{
                result |= RESOURCE_RESULT_IRRELEVANT_LOCATION_LUT;
            }
        }
    }

    return result;
}

void ResLight::Cleanup(){
    ResFragmentLight fragment = ResDynamicCast<ResFragmentLight>(*this);

    if (fragment.IsValid()){
        ut::SafeCleanup(fragment.GetDistanceSampler());
        ut::SafeCleanup(fragment.GetAngleSampler());
    }
}

}
}
}