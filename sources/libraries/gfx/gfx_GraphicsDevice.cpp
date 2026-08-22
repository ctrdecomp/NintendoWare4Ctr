#include <nw/gfx/gfx_GraphicsDevice.h>
#include <nw/gfx/gfx_CommandUtil.h>
#include <nw/gfx/res/gfx_ResMaterial.h>
#include <nw/gfx/res/gfx_ResLookupTable.h>

namespace nw{
namespace gfx{

enum { 
    REG_BUFFER_CACHE_CLEAR     = 0x110,
    REG_BUFFER_CACHE_TAG_CLEAR = 0x111,
    REG_BUFFER_MASK_BASE       = 0x112
};

uint GraphicsDevice::sLutIsAbs = 0;
uint GraphicsDevice::sLutInput = 0;
uint GraphicsDevice::sLutScale = 0;

u32  GraphicsDevice::sDepthFormat = RENDER_DEPTH_FORMAT_24_STENCIL8;
u32  GraphicsDevice::sWScale24 = 0;
f32  GraphicsDevice::sDepthRangeNear = 0.0f;
f32  GraphicsDevice::sDepthRangeFar  = 1.0f;
u32  GraphicsDevice::sDepthRange24   = nw::ut::Float24::Float32ToBits24( -1.0f );
bool GraphicsDevice::sPolygonOffsetEnabled = false;
f32  GraphicsDevice::sPolygonOffsetUnit = 0.0f;
res::ResImageLookupTable GraphicsDevice::mLutTargets[LUT_TARGET_COUNT];

u32  GraphicsDevice::sLightPositionW = 0;
u32  GraphicsDevice::sLightShadowed = 0xFF;
u32  GraphicsDevice::sLightSpotEnabled = 0xFF;
u32  GraphicsDevice::sLightDistanceAttnEnabled = 0xFF;

res::ResFragmentOperation::FragmentOperationMode GraphicsDevice::sFragOperationMode = ResFragmentOperation::FRAGMENT_OPERATION_MODE_GL;
bool GraphicsDevice::sIsFrameBufferUpdated = true;
bool GraphicsDevice::sBlendEnabled = true;
bool GraphicsDevice::sDepthTestEnabled = true;
bool GraphicsDevice::sStencilTestEnabled = false;
bool GraphicsDevice::sDepthMask = true;
u8  GraphicsDevice::sStencilMask = 0xFF;
u32 GraphicsDevice::sColorMask = 0xF;

u32 GraphicsDevice::sRenderBufferWidth = 400;
u32 GraphicsDevice::sRenderBufferHeight = 240;

u32 GraphicsDevice::sFrameBufferCommand[10] ={
    1,internal::MakeCommandHeader(REG_BUFFER_CACHE_TAG_CLEAR, 1, false, 0xF),
    1,internal::MakeCommandHeader(REG_BUFFER_CACHE_CLEAR, 1, false, 0xF),
    0,internal::MakeCommandHeader(REG_BUFFER_MASK_BASE, 4, true, 0xF),
    0,0,0,0
};

void GraphicsDevice::ActivateLookupTable(res::ResImageLookupTable lookupTable, LutTarget target){
    NW_ASSERT(lookupTable.IsValid());

    if (mLutTargets[target] != lookupTable){
        mLutTargets[target] =  lookupTable;
        ActivateLutLoadSetting(target);
        internal::NWUseCmdlist( lookupTable.GetCommandCache(), lookupTable.GetCommandCacheCount() );
    }
}

void GraphicsDevice::InvalidateLookupTable(res::ResImageLookupTable lookupTable){
    NW_ASSERT(lookupTable.IsValid());

    for (int target = 0; target < LUT_TARGET_COUNT; ++target){
        if (mLutTargets[target] == lookupTable){
            mLutTargets[target] = res::ResImageLookupTable(NULL);
        }
    }
}

void GraphicsDevice::InvalidateAllLookupTables(){
    for (int target = 0; target < LUT_TARGET_COUNT; ++target){
        mLutTargets[target] = res::ResImageLookupTable(NULL);
    }
}



}
}