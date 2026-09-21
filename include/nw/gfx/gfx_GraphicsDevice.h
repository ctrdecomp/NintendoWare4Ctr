#pragma once

#include <GLES2/gl2.h>
#include <GLES2/gl2extern.h>

#include <nw/types.h>
#include <nw/gfx/res/gfx_ResMaterial.h>
#include <nw/gfx/res/gfx_ResLookupTable.h>
#include <nw/ut/ut_Color.h>
#include <nw/ut/ut_Rect.h>
#include <nw/ut/ut_Float24.h>
#include <nw/math/math_Types.h>
#include <nw/gfx/gfx_Viewport.h>

namespace nw{
namespace os{
    class IAllocator;
}
namespace gfx{

class GraphicsDevice
{
public:
    static inline void SetPolygonOffsetEnabled(bool enabled)
    {
        s_PolygonOffsetEnabled = enabled;
    }

    static inline void SetPolygonOffsetUnit(f32 polygonOffsetUnit)
    {
        s_PolygonOffsetUnit = polygonOffsetUnit;
    }

    static inline void ActivatePolygonOffset();

    static inline void SetBlendEnabled(bool enabled);

    static inline void SetFragmentOperationMode(const ResFragmentOperation::FragmentOperationMode mode)
    {
        if (s_FragOperationMode != mode)
        {
            s_FragOperationMode = mode;
            s_IsFrameBufferUpdated = true;
        }
    }

    static inline void ActivateFragmentOperationMode(ResFragmentOperation::FragmentOperationMode mode)
    {
        s_FragOperationMode = mode;

        enum
{ REG_FRAGMENT_OPERATION = 0x100 };

        const u32 HEADER = internal::MakeCommandHeader(REG_FRAGMENT_OPERATION, 1, false, 0x1);

        const u32 COMMAND[] =
        {
            mode,
            HEADER
        };

        s_IsFrameBufferUpdated = true;
        ActivateFrameBuffer();
    }

    static inline void SetDepthTestEnabled(bool enabled);

    static inline void SetStencilTestEnabled(bool enabled);

    static void SetDepthRange(f32 near, f32 far)
    {
        s_DepthRangeNear = nw::ut::Clamp(near, 0.0f, 1.0f);
        s_DepthRangeFar = nw::ut::Clamp(far, 0.0f, 1.0f);
        s_DepthRange24 = nw::ut::Float24::Float32ToBits24(s_DepthRangeNear - s_DepthRangeFar);
    }

    static void SetWScale(f32 wscale)
    {
        if (wscale == 0.0f)
        {
            s_WScale24 = 0;
        }
        else
        {
            s_WScale24 = nw::ut::Float24::Float32ToBits24(-wscale);
        }
    }

    static void SetDepthFormat(u32 format)
    {
        s_DepthFormat = format;
    }

    static inline void SetColorMask(bool red, bool green, bool blue, bool alpha);

    static inline void SetDepthMaskEnabled(bool enabled);

    static inline void SetStencilMask(u8 mask);

    static inline void ActivateFrameBuffer();

    static inline void ActivateMask();

    enum LutTarget
    {
        LUT_TARGET_D0,
        LUT_TARGET_D1,
        LUT_TARGET_SP,
        LUT_TARGET_FR,
        LUT_TARGET_RB,
        LUT_TARGET_RG,
        LUT_TARGET_RR,
        LUT_TARGET_SP0,
        LUT_TARGET_SP1,
        LUT_TARGET_SP2,
        LUT_TARGET_SP3,
        LUT_TARGET_SP4,
        LUT_TARGET_SP5,
        LUT_TARGET_SP6,
        LUT_TARGET_SP7,
        LUT_TARGET_DA0,
        LUT_TARGET_DA1,
        LUT_TARGET_DA2,
        LUT_TARGET_DA3,
        LUT_TARGET_DA4,
        LUT_TARGET_DA5,
        LUT_TARGET_DA6,
        LUT_TARGET_DA7,
        LUT_TARGET_FOG,
        LUT_TARGET_COUNT
    };

    static void SetLutIsAbs(LutTarget target, bool isAbs)
    {
        NW_ASSERT(target < LUT_TARGET_COUNT);

        if (LUT_TARGET_SP0 <= target && target <= LUT_TARGET_SP7)
        {
            target = LUT_TARGET_SP;
        }

        if (isAbs)
        {
            s_LutIsAbs &= ~(0x1 << (1 + 4 * target));
        }
        else
        {
            s_LutIsAbs |= 0x1 << (1 + 4 * target);
        }
    }

    static void SetLutInput(LutTarget target, ResLightingLookupTable::Input input)
    {
        NW_ASSERT(target < LUT_TARGET_COUNT);
        NW_ASSERT(input < ResLightingLookupTable::INPUT_NUM);

        if (LUT_TARGET_SP0 <= target && target <= LUT_TARGET_SP7)
        {
            target = LUT_TARGET_SP;
        }

        s_LutInput &= ~(0x7 << (4 * target));
        s_LutInput |= input << (4 * target);
    }

    static void SetLutScale(LutTarget target, ResLightingLookupTable::Scale scale)
    {
        NW_ASSERT(target < LUT_TARGET_COUNT);
        NW_ASSERT(scale <= ResLightingLookupTable::SCALE_MAX);

        if (LUT_TARGET_SP0 <= target && target <= LUT_TARGET_SP7)
        {
            target = LUT_TARGET_SP;
        }

        s_LutScale &= ~(0x7 << (4 * target));
        s_LutScale |= scale << (4 * target);
    }

    static void ActivateLutParameters()
    {
        enum
{ REG_LUT_ABS = 0x1d0 };

        const u32 HEADER = internal::MakeCommandHeader(REG_LUT_ABS, 3, true, 0xf);

        const u32 COMMAND[] =
        {
            s_LutIsAbs,
            HEADER,
            s_LutInput,
            s_LutScale
        };

        internal::NWUseCmdlist<sizeof(COMMAND)>(&COMMAND[0]);
    }

    static void ResetLutParameters()
    {
        s_LutIsAbs = 0;
        s_LutInput = 0;
        s_LutScale = 0;
    }

    static void ActivateLookupTable(ResImageLookupTable lookupTable, LutTarget target);

    static void InvalidateLookupTable(ResImageLookupTable lookupTable);

    static void InvalidateAllLookupTables();

    static void ResetFragmentLightEnabled()
    {
        s_LightPositionW = 0x0;
        s_LightShadowed = 0xFF;
        s_LightSpotEnabled = 0xFF;
        s_LightDistanceAttnEnabled = 0xFF;
    }

    static inline bool GetFragmentLightPositionW(int index)
    {
        return ((s_LightPositionW >> index) & 0x1);
    }

    static inline void SetFragmentLightPositionW(int index, bool isWZero)
    {
        if (isWZero)
        {
            s_LightPositionW |= (0x1 << index);
        }
        else
        {
            s_LightPositionW &= ~(0x1 << index);
        }
    }

    static void SetFragmentLightShadowed(int index, bool shadowed)
    {
        if (shadowed)
        {
            s_LightShadowed &= ~(0x1 << index);
        }
        else
        {
            s_LightShadowed |= 0x1 << index;
        }
    }

    static void SetFragmentLightSpotEnabled(int index, bool enabled)
    {
        if (enabled)
        {
            s_LightSpotEnabled &= ~(0x1 << index);
        }
        else
        {
            s_LightSpotEnabled |= 0x1 << index;
        }
    }

    static void SetFragmentLightDistanceAttnEnabled(int index, bool enabled)
    {
        if (enabled)
        {
            s_LightDistanceAttnEnabled &= ~(0x1 << index);
        }
        else
        {
            s_LightDistanceAttnEnabled |= 0x1 << index;
        }
    }

    static inline void ActivateFragmentLightDistanceAttnTable(int index, const ResImageLookupTable lookupTable)
    {
        GraphicsDevice::LutTarget tableTarget = static_cast<GraphicsDevice::LutTarget>(GraphicsDevice::LUT_TARGET_DA0 + index);

        GraphicsDevice::ActivateLookupTable(lookupTable, tableTarget);
    }

    static inline void ActivateFragmentLightSpotTable(int index, const ResImageLookupTable lookupTable)
    {
        GraphicsDevice::LutTarget tableTarget = static_cast<GraphicsDevice::LutTarget>(GraphicsDevice::LUT_TARGET_SP0 + index);

        GraphicsDevice::ActivateLookupTable(lookupTable, tableTarget);
    }

    static void ActivateFragmentLightDistanceAttnScaleBias(int index, u32 scalef20, u32 biasf20)
    {
        enum
        {
            REG_DISTANCE_ATTN_BIAS = 0x14a,
            REG_DISTANCE_ATTN_SCALE = 0x14b
        };

        const u32 HEADER = internal::MakeCommandHeader(REG_DISTANCE_ATTN_BIAS, 2, true, 0x7);

        u32 COMMAND[] =
        {
            biasf20,
            HEADER + 0x10 * index,
            scalef20,
            0
        };

        internal::NWUseCmdlist<sizeof(COMMAND)>(&COMMAND[0]);
    }

    static inline void ActivateFragmentLightPosition(s32 index, const nw::math::VEC4& pos)
    {
        enum
{ REG_FRAGMENT_LIGHT_POS_BASE = 0x144 };

        const u32 POS_HEADER = internal::MakeCommandHeader(REG_FRAGMENT_LIGHT_POS_BASE, 2, true, 0xF);

        u32 LIGHT_COMMAND[4] =
        {
            nw::ut::Float16(pos.y).GetFloat16Value() << 16 | nw::ut::Float16(pos.x).GetFloat16Value(),
            POS_HEADER + 0x10 * index,
            nw::ut::Float16(pos.z).GetFloat16Value(),
            0
        };

        internal::NWUseCmdlist<sizeof(LIGHT_COMMAND)>(&LIGHT_COMMAND[0]);
    }

    static inline void ActivateFragmentLightPosition(s32 index, const nw::math::VEC4& pos, const nw::math::VEC3& spotDir)
    {
        enum
{ REG_FRAGMENT_LIGHT_POS_BASE = 0x144 };

        const u32 POS_HEADER = internal::MakeCommandHeader(REG_FRAGMENT_LIGHT_POS_BASE, 4, true, 0xF);

        u32 LIGHT_COMMAND[6] =
        {
            nw::ut::Float16(pos.y).GetFloat16Value() << 16 | nw::ut::Float16(pos.x).GetFloat16Value(),
            POS_HEADER + 0x10 * index,
            nw::ut::Float16(pos.z).GetFloat16Value(),
            nw::ut::Fixed13(-spotDir.y).GetFixed13Value() << 16 | nw::ut::Fixed13(-spotDir.x).GetFixed13Value(),
            nw::ut::Fixed13(-spotDir.z).GetFixed13Value(),
            0
        };

        internal::NWUseCmdlist<sizeof(LIGHT_COMMAND)>(&LIGHT_COMMAND[0]);
    }

    static void ActivateFragmentLightEnabled()
    {
        enum
{ REG_FRAGMENT_LIGHT_KIND = 0x1c4 };

        const u32 HEADER = internal::MakeCommandHeader(REG_FRAGMENT_LIGHT_KIND, 1, false, 0xb);

        const u32 COMMAND[] =
        {
            s_LightShadowed | (s_LightSpotEnabled << 8) | (s_LightDistanceAttnEnabled << 24),
            HEADER
        };

        internal::NWUseCmdlist<sizeof(COMMAND)>(&COMMAND[0]);
    }

    static void ActivateViewport(const Viewport& viewport)
    {
        enum
        {
            REG_VIEWPORT_WIDTH_HALF = 0x41,
            REG_VIEWPORT_WIDTH_HALF_S = 0x42,
            REG_VIEWPORT_HEIGHT_HALF = 0x43,
            REG_VIEWPORT_HEIGHT_HALF_S = 0x44,
            REG_VIEWPORT_XY = 0x68
        };

        u16 x16 = static_cast<u16>(viewport.GetBound().GetX());
        u16 y16 = static_cast<u16>(viewport.GetBound().GetY());

        const u32 HEADER_VIEWPORT_WIDTH_HALF = gfx::internal::MakeCommandHeader(REG_VIEWPORT_WIDTH_HALF, 4, true, 0xF);
        const u32 HEADER_VIEWPORT_XY = gfx::internal::MakeCommandHeader(REG_VIEWPORT_XY, 1, false, 0xF);

        u32 VIEWPORT_COMMAND[] =
        {
            nw::ut::Float24::Float32ToBits24(viewport.GetBound().GetWidth() / 2.f),
            HEADER_VIEWPORT_WIDTH_HALF,
            (nw::ut::Float31::Float32ToBits31(2.f / viewport.GetBound().GetWidth()) << 1),
            nw::ut::Float24::Float32ToBits24(viewport.GetBound().GetHeight() / 2.f),
            (nw::ut::Float31::Float32ToBits31(2.f / viewport.GetBound().GetHeight()) << 1),
            0,
            (x16 | (y16 << 16)),
            HEADER_VIEWPORT_XY
        };

        gfx::internal::NWUseCmdlist<sizeof(VIEWPORT_COMMAND)>(&VIEWPORT_COMMAND[0]);
    }

    static void SetRenderBufferSize(s32 width, s32 height)
    {
        s_RenderBufferWidth = width;
        s_RenderBufferHeight = height;
    }

    static void ActivateScissor(bool enabled, const nw::ut::Rect& scissor)
    {
        enum
        {
            REG_SCISSOR_ENABLE = 0x65,
            REG_SCISSOR_XY = 0x66,
            REG_SCISSOR_WH = 0x67,

            REG_SCISSOR_DISABLED = 0,
            REG_SCISSOR_ENABLED = 3
        };

        NW_MINMAX_ASSERT(scissor.GetX(), 0, static_cast<s32>(s_RenderBufferWidth));
        NW_MINMAX_ASSERT(scissor.GetY(), 0, static_cast<s32>(s_RenderBufferHeight));
        NW_MINMAX_ASSERT(scissor.GetX() + scissor.GetWidth(), 0, static_cast<s32>(s_RenderBufferWidth));
        NW_MINMAX_ASSERT(scissor.GetY() + scissor.GetHeight(), 0, static_cast<s32>(s_RenderBufferHeight));

        u16 x16 = static_cast<u16>(scissor.GetX());
        u16 w16 = static_cast<u16>(scissor.GetWidth());
        u16 y16 = static_cast<u16>(scissor.GetY());
        u16 h16 = static_cast<u16>(scissor.GetHeight());

        const u32 HEADER_SCISSOR = gfx::internal::MakeCommandHeader(REG_SCISSOR_ENABLE, 3, true, 0xF);

        u32 SCISSOR_COMMAND[] =
        {
            enabled ? REG_SCISSOR_ENABLED : REG_SCISSOR_DISABLED,
            HEADER_SCISSOR,
            enabled ? (x16 | (y16 << 16)) : 0,
            enabled ? ((x16 + w16 - 1) | ((y16 + h16 - 1) << 16)) : ((s_RenderBufferWidth - 1) | ((s_RenderBufferHeight - 1) << 16)),
        };

        gfx::internal::NWUseCmdlist<sizeof(SCISSOR_COMMAND)>(&SCISSOR_COMMAND[0]);
    }

    static void Report();

private:
    static inline void ActivateLutLoadSetting(LutTarget target);

    static inline void UpdateFrameBufferCommand();

    static ResFragmentOperation::FragmentOperationMode s_FragOperationMode;
    static bool s_IsFrameBufferUpdated;
    static bool s_BlendEnabled;
    static bool s_DepthTestEnabled;
    static bool s_StencilTestEnabled;
    static bool s_DepthMask;
    static bool s_PolygonOffsetEnabled;
    static u8   s_StencilMask;

    static u32 s_ColorMask;

    static u32 s_RenderBufferWidth;
    static u32 s_RenderBufferHeight;

    enum
    {
        COLOR_READ_MASK_INDEX = 4,
        COLOR_WRITE_MASK_INDEX = 6,
        DEPTH_READ_MASK_INDEX = 7,
        DEPTH_WRITE_MASK_INDEX = 8,
        FRAME_BUFFER_COMMAND_COUNT = 10
    };

    static u32 s_FrameBufferCommand[FRAME_BUFFER_COMMAND_COUNT];

    static u32 s_LutIsAbs;
    static u32 s_LutInput;
    static u32 s_LutScale;

    static u32 s_DepthFormat;
    static u32 s_WScale24;
    static f32 s_DepthRangeNear;
    static f32 s_DepthRangeFar;
    static u32 s_DepthRange24;
    static f32 s_PolygonOffsetUnit;

    static u32 s_LightPositionW;
    static u32 s_LightShadowed;
    static u32 s_LightSpotEnabled;
    static u32 s_LightDistanceAttnEnabled;

    static ResImageLookupTable m_LutTargets[LUT_TARGET_COUNT];
};

inline void GraphicsDevice::SetBlendEnabled(bool enabled)
{
    s_IsFrameBufferUpdated = s_IsFrameBufferUpdated || (s_BlendEnabled != enabled);
    s_BlendEnabled = enabled;
}

inline void GraphicsDevice::SetStencilTestEnabled(bool enabled)
{
    s_IsFrameBufferUpdated = s_IsFrameBufferUpdated || (s_StencilTestEnabled != enabled);
    s_StencilTestEnabled = enabled;
}

inline void GraphicsDevice::SetDepthTestEnabled(bool enabled)
{
    s_IsFrameBufferUpdated = s_IsFrameBufferUpdated || (s_DepthTestEnabled != enabled);
    s_DepthTestEnabled = enabled;
}

inline void GraphicsDevice::ActivateMask()
{
    enum RegColorMask
    {
        REG_COLOR_MASK_ADDR = 0x107,
        REG_COLOR_MASK_SHIFT = 8,
        REG_DEPTH_MASK_SHIFT = 12,
        REG_STENCIL_MASK_ADDR = 0x105,
        REG_STENCIL_MASK_SHIFT = 8
    };

    const u32 COLOR_MASK_HEADER = internal::MakeCommandHeader(REG_COLOR_MASK_ADDR, 1, false, 0x2);
    const u32 STENCIL_MASK_HEADER = internal::MakeCommandHeader(REG_STENCIL_MASK_ADDR, 1, false, 0x2);

    const u32 COMMAND[] =
    {
        s_ColorMask << REG_COLOR_MASK_SHIFT | s_DepthMask << REG_DEPTH_MASK_SHIFT,
        COLOR_MASK_HEADER,
        s_StencilMask << REG_STENCIL_MASK_SHIFT,
        STENCIL_MASK_HEADER
    };

    internal::NWUseCmdlist<sizeof(COMMAND)>(&COMMAND[0]);
}

inline void GraphicsDevice::ActivateFrameBuffer()
{
    if (s_IsFrameBufferUpdated)
    {
        s_IsFrameBufferUpdated = false;
        UpdateFrameBufferCommand();
    }

    internal::NWUseCmdlist(&s_FrameBufferCommand[0], sizeof(s_FrameBufferCommand));
}

inline void GraphicsDevice::UpdateFrameBufferCommand()
{
    enum
    {
        COLOR_MASK_ALL = 0xF,
        COLOR_ACCESS_ENABLE = 0xF,
        DEPTH_ACCESS_ENABLE = 0x2,
        STENCIL_ACCESS_ENABLE = 0x1
    };

    s_FrameBufferCommand[COLOR_READ_MASK_INDEX] = 0;
    s_FrameBufferCommand[COLOR_WRITE_MASK_INDEX] = 0;

    if (s_FragOperationMode != ResFragmentOperation::FRAGMENT_OPERATION_MODE_GL)
    {
        s_FrameBufferCommand[COLOR_READ_MASK_INDEX] = COLOR_ACCESS_ENABLE;
        s_FrameBufferCommand[COLOR_WRITE_MASK_INDEX] = COLOR_ACCESS_ENABLE;
    }
    else if (s_ColorMask)
    {
        if (s_BlendEnabled || (s_ColorMask != COLOR_MASK_ALL))
        {
            s_FrameBufferCommand[COLOR_READ_MASK_INDEX] = COLOR_ACCESS_ENABLE;
        }

        s_FrameBufferCommand[COLOR_WRITE_MASK_INDEX] = COLOR_ACCESS_ENABLE;
    }

    s_FrameBufferCommand[DEPTH_READ_MASK_INDEX] = 0;
    s_FrameBufferCommand[DEPTH_WRITE_MASK_INDEX] = 0;

    if (s_FragOperationMode == ResFragmentOperation::FRAGMENT_OPERATION_MODE_GAS)
    {
        s_FrameBufferCommand[DEPTH_READ_MASK_INDEX] = DEPTH_ACCESS_ENABLE | STENCIL_ACCESS_ENABLE;
    }
    else if (s_FragOperationMode == ResFragmentOperation::FRAGMENT_OPERATION_MODE_GL)
    {
        if (s_DepthTestEnabled)
        {
            if (s_DepthMask)
            {
                s_FrameBufferCommand[DEPTH_READ_MASK_INDEX] = DEPTH_ACCESS_ENABLE;
                s_FrameBufferCommand[DEPTH_WRITE_MASK_INDEX] = DEPTH_ACCESS_ENABLE;
            }
            else if (s_ColorMask)
            {
                s_FrameBufferCommand[DEPTH_READ_MASK_INDEX] = DEPTH_ACCESS_ENABLE;
            }
        }

        if (s_StencilTestEnabled)
        {
            if (s_StencilMask)
            {
                s_FrameBufferCommand[DEPTH_READ_MASK_INDEX] |= STENCIL_ACCESS_ENABLE;
                s_FrameBufferCommand[DEPTH_WRITE_MASK_INDEX] |= STENCIL_ACCESS_ENABLE;
            }
            else if (s_ColorMask)
            {
                s_FrameBufferCommand[DEPTH_READ_MASK_INDEX] |= STENCIL_ACCESS_ENABLE;
            }
        }
    }
}

inline void GraphicsDevice::ActivateLutLoadSetting(LutTarget target)
{
    enum
    {
        REG_PICA_LIGHTING_LUT_SETTING = 0x1C5,
        REG_PICA_FOG_LUT_SETTING = 0x0e6
    };

    const u32 LIGHTING_HEADER = internal::MakeCommandHeader(REG_PICA_LIGHTING_LUT_SETTING, 1, false, 0xF);
    const u32 FOG_HEADER = internal::MakeCommandHeader(REG_PICA_FOG_LUT_SETTING, 1, false, 0xF);

    if (target == LUT_TARGET_FOG)
    {
        u32 LUT_COMMAND[2] =
        {
            0, FOG_HEADER
        };

        internal::NWUseCmdlist<sizeof(LUT_COMMAND)>(&LUT_COMMAND[0]);
        return;
    }

    int targetReg = 0;

    if (target < LUT_TARGET_SP0)
    {
        targetReg = static_cast<int>(target);
    }
    else
    {
        targetReg = static_cast<int>(target) + 1;
    }

    u32 LUT_COMMAND[2] =
    {
        targetReg << 8,
        LIGHTING_HEADER
    };

    internal::NWUseCmdlist<sizeof(LUT_COMMAND)>(&LUT_COMMAND[0]);
}

inline void GraphicsDevice::ActivatePolygonOffset()
{
    enum
    {
        REG_POLYGON_OFFSET = 0x4d,
        REG_WSCALE_ENABLE = 0x6d
    };

    const u32 OFFSET_HEADER = internal::MakeCommandHeader(REG_POLYGON_OFFSET, 2, true, 0x7);
    const u32 WSCALE_HEADER = internal::MakeCommandHeader(REG_WSCALE_ENABLE, 1, false, 0x1);

    static u32 COMMAND[] =
    {
        0,WSCALE_HEADER,0,
        OFFSET_HEADER,0,0
    };

    f32 zBias;
    f32 offset;

    if (s_WScale24 == 0)
    {
        COMMAND[0] = 1;
        COMMAND[2] = s_DepthRange24;

        zBias = s_DepthRangeNear;
        offset = (s_DepthRangeNear - s_DepthRangeFar) * s_PolygonOffsetUnit;
    }
    else
    {
        COMMAND[0] = 0;
        COMMAND[2] = s_WScale24;

        zBias = 0.0f;
        offset = -s_PolygonOffsetUnit;
    }

    if (s_PolygonOffsetEnabled)
    {
        if (s_DepthFormat == RENDER_DEPTH_FORMAT_16)
        {
            zBias -= offset / 65535.0f;
        }
        else
        {
            zBias -= offset * 128.0f / 16777215.0f;
        }
    }

    COMMAND[4] = nw::ut::Float24::Float32ToBits24(zBias);

    internal::NWUseCmdlist<sizeof(COMMAND)>(&COMMAND[0]);
}

inline void GraphicsDevice::SetColorMask(bool red, bool green, bool blue, bool alpha)
{
    u32 colorMask = u32(red) | (u32(green) << 1) | (u32(blue) << 2) | (u32(alpha) << 3);

    s_IsFrameBufferUpdated = s_IsFrameBufferUpdated || (s_ColorMask != colorMask);
    s_ColorMask = colorMask;
}

inline void GraphicsDevice::SetDepthMaskEnabled(bool enabled)
{
    s_IsFrameBufferUpdated = s_IsFrameBufferUpdated || (s_DepthMask != enabled);
    s_DepthMask = enabled;
}

inline void GraphicsDevice::SetStencilMask(u8 mask)
{
    s_IsFrameBufferUpdated = s_IsFrameBufferUpdated || (s_StencilMask != mask);
    s_StencilMask = mask;
}

}
}