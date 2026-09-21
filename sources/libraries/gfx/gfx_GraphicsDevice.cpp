#include <nw/gfx/gfx_GraphicsDevice.h>
#include <nw/gfx/gfx_CommandUtil.h>
#include <nw/gfx/res/gfx_ResMaterial.h>
#include <nw/gfx/res/gfx_ResLookupTable.h>

namespace nw{
namespace gfx{

enum
{ 
    REG_BUFFER_CACHE_CLEAR     = 0x110,
    REG_BUFFER_CACHE_TAG_CLEAR = 0x111,
    REG_BUFFER_MASK_BASE       = 0x112
};

uint GraphicsDevice::s_LutIsAbs = 0;
uint GraphicsDevice::s_LutInput = 0;
uint GraphicsDevice::s_LutScale = 0;

u32  GraphicsDevice::s_DepthFormat = RENDER_DEPTH_FORMAT_24_STENCIL8;
u32  GraphicsDevice::s_WScale24 = 0;
f32  GraphicsDevice::s_DepthRangeNear = 0.0f;
f32  GraphicsDevice::s_DepthRangeFar  = 1.0f;
u32  GraphicsDevice::s_DepthRange24   = nw::ut::Float24::Float32ToBits24( -1.0f );
bool GraphicsDevice::s_PolygonOffsetEnabled = false;
f32  GraphicsDevice::s_PolygonOffsetUnit = 0.0f;
res::ResImageLookupTable GraphicsDevice::m_LutTargets[LUT_TARGET_COUNT];

u32  GraphicsDevice::s_LightPositionW = 0;
u32  GraphicsDevice::s_LightShadowed = 0xFF;
u32  GraphicsDevice::s_LightSpotEnabled = 0xFF;
u32  GraphicsDevice::s_LightDistanceAttnEnabled = 0xFF;

res::ResFragmentOperation::FragmentOperationMode GraphicsDevice::s_FragOperationMode = ResFragmentOperation::FRAGMENT_OPERATION_MODE_GL;
bool GraphicsDevice::s_IsFrameBufferUpdated = true;
bool GraphicsDevice::s_BlendEnabled = true;
bool GraphicsDevice::s_DepthTestEnabled = true;
bool GraphicsDevice::s_StencilTestEnabled = false;
bool GraphicsDevice::s_DepthMask = true;
u8  GraphicsDevice::s_StencilMask = 0xFF;
u32 GraphicsDevice::s_ColorMask = 0xF;

u32 GraphicsDevice::s_RenderBufferWidth = 400;
u32 GraphicsDevice::s_RenderBufferHeight = 240;

u32 GraphicsDevice::s_FrameBufferCommand[10] ={
    1,internal::MakeCommandHeader(REG_BUFFER_CACHE_TAG_CLEAR, 1, false, 0xF),
    1,internal::MakeCommandHeader(REG_BUFFER_CACHE_CLEAR, 1, false, 0xF),
    0,internal::MakeCommandHeader(REG_BUFFER_MASK_BASE, 4, true, 0xF),
    0,0,0,0
};

void GraphicsDevice::ActivateLookupTable(res::ResImageLookupTable lookupTable, LutTarget target)
{
    NW_ASSERT(lookupTable.IsValid());

    if (m_LutTargets[target] != lookupTable)
    {
        m_LutTargets[target] =  lookupTable;
        ActivateLutLoadSetting(target);
        internal::NWUseCmdlist( lookupTable.GetCommandCache(), lookupTable.GetCommandCacheCount() );
    }
}

void GraphicsDevice::InvalidateLookupTable(res::ResImageLookupTable lookupTable)
{
    NW_ASSERT(lookupTable.IsValid());

    for (int target = 0; target < LUT_TARGET_COUNT; ++target)
    {
        if (m_LutTargets[target] == lookupTable)
        {
            m_LutTargets[target] = res::ResImageLookupTable(NULL);
        }
    }
}

void GraphicsDevice::InvalidateAllLookupTables()
{
    for (int target = 0; target < LUT_TARGET_COUNT; ++target)
    {
        m_LutTargets[target] = res::ResImageLookupTable(NULL);
    }
}



}
}