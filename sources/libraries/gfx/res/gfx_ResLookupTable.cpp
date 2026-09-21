// Filename: gfx_ResLookupTable.cpp
//
// Project: NintendoWare4Ctr

#include <nw/gfx/res/gfx_ResLookupTable.h>
#include <nw/gfx/gfx_RenderContext.h>
#include <nw/gfx/gfx_GraphicsDevice.h>

namespace nw {
namespace gfx {
namespace res {

typedef void (*ActivateFunc)(ResTexture resTex, RenderContext& context);
typedef void (*CleanupFunc)(ResLookupTable resLut);
typedef Result (*SetupFunc)(ResLookupTable resLut);

static void ResImageLut_Cleanup(ResLookupTable resLut);
static void ResReferenceLut_Cleanup(ResLookupTable resLut);

static Result ResImageLut_Setup(ResLookupTable resLut);
static Result ResReferenceLut_Setup(ResLookupTable resLut);

static SetupFunc s_LutSetupTable[] = {
    ResImageLut_Setup,
    ResReferenceLut_Setup
};

static CleanupFunc s_LutCleanupTable[] = {
    ResImageLut_Cleanup,
    ResReferenceLut_Cleanup
};

static Result ResImageLut_Setup(ResLookupTable resLut)
{
    Result result = RESOURCE_RESULT_OK;

    return result;
}

static Result ResReferenceLut_Setup(ResLookupTable resLut)
{
    Result result = RESOURCE_RESULT_OK;

    result |= ResImageLut_Setup(resLut.Dereference());

    return result;
}

static void ResImageLut_Cleanup(ResLookupTable resLut)
{
    ResImageLookupTable imageLut = ResStaticCast<ResImageLookupTable>( resLut );
    
    GraphicsDevice::InvalidateLookupTable(imageLut);
}

static void ResReferenceLut_Cleanup(ResLookupTable resLut)
{
    ResReferenceLookupTable referLut = ResStaticCast<ResReferenceLookupTable>( resLut );
    NW_ASSERT(resLut.IsValid());

    referLut.ref().toTargetLut.set_ptr(NULL);
}

Result ResLookupTable::Setup()
{
    NW_ASSERT(this->IsValid());
    
    Result result = RESOURCE_RESULT_OK;

    switch (this->ref().typeInfo)
    {
    case ResImageLookupTable::TYPE_INFO:{
            result |= s_LutSetupTable[0]( *this );
        }
        break;
    case ResReferenceLookupTable::TYPE_INFO:{
            result |= s_LutSetupTable[1]( *this );
        }
        break;
    default:{
            NW_FATAL_ERROR("Unsupported lut type.");
        }
    }

    return result;
}

void ResLookupTable::Cleanup()
{
    switch (this->ref().typeInfo)
    {
    case ResImageLookupTable::TYPE_INFO:{
            s_LutCleanupTable[0](*this);
        }
        break;
    case ResReferenceLookupTable::TYPE_INFO:{
            s_LutCleanupTable[1](*this);
        }
        break;
    default:{
            NW_FATAL_ERROR("Unsupported lut type.");
        }
    }
}

void ResReferenceLookupTable::ForceSetup(ResLookupTable lut)
{
    NW_ASSERT(lut.IsValid());
    ref().toTargetLut.set_ptr(lut.ptr());

    ResReferenceLut_Setup(*this);
}

void ResReferenceLookupTable::ForceSetup(const char* targetName, ResLookupTable lut)
{
    NW_NULL_ASSERT(targetName);
    if (std::strcmp(targetName, this->GetPath()) == 0)
    {
        this->ForceSetup(lut);
    }
}

Result ResLookupTableSet::Setup(os::IAllocator* allocator, ResGraphicsFile graphicsFile)
{
    NW_UNUSED_VARIABLE(allocator);
    NW_UNUSED_VARIABLE(graphicsFile);
    NW_ASSERT(internal::ResCheckRevision(*this));

    Result result = RESOURCE_RESULT_OK;

    s32 samplerNum = this->GetSamplersCount();
    for (int i = 0; i < samplerNum; ++i)
    {
        result |= this->GetSamplers(i).Setup();
    }

    return result;
}

void ResLookupTableSet::Cleanup()
{
    ut::SafeCleanupAll(this->GetSamplers());
}

}
}
}