// Filename: gfx_ResFog.cpp
//
// Project: NintendoWare4Ctr

#include <nw/gfx/res/gfx_ResUtil.h>
#include <nw/gfx/res/gfx_ResLight.h>
#include <nw/gfx/res/gfx_ResGraphicsFile.h>

namespace nw {
namespace gfx {
namespace res {

Result ResFog::Setup(os::IAllocator* allocator, ResGraphicsFile graphicsFile)
{
    Result result = RESOURCE_RESULT_OK;

    ResReferenceLookupTable refLut = ResDynamicCast<ResReferenceLookupTable>(this->GetFogSampler());

    Result referenceResult = SetupReferenceLut(refLut, graphicsFile);

    if (referenceResult.IsSuccess())
    {
        result |= this->GetFogSampler().Setup();
    }

    result |= referenceResult;

    return result;
}

void ResFog::Cleanup()
{
    ut::SafeCleanup(this->GetFogSampler());
}

}
}
}