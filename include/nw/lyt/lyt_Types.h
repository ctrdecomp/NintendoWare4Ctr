#pragma once

#include <cstddef>
#include <nw/types.h>

#include <nw/math/math_Types.h>
#include <nw/ut/ut_BinaryFileFormat.h>
#include <nw/ut/ut_LinkList.h>
#include <nw/font/font_DrawerTypes.h>

namespace nw {
namespace lyt {

const int ResourceNameStrMax = 16;
const int MaterialNameStrMax = ResourceNameStrMax + 4;
const int UserDataStrMax = 8;
const int TexMapMax = 3;
const int TevStageMax = 6;
const int MatColorMax = 7;

namespace internal {

const int TexEnvUnitMax = 3;
const int TexUnitMax = 4;

using nw::ut::internal::TestBit;
using nw::ut::internal::GetBits;

template<typename T>
NW_FORCE_INLINE T SetBit(T bits, int pos, bool val)
{
    const T mask = T(~(1 << pos));
    bits &= mask;
    bits |= (val ? 1 : 0) << pos;
    return bits;
}

template<typename T>
NW_FORCE_INLINE void SetBit(T* pBits, int pos, bool val)
{
    const T mask = T(~(1 << pos));
    *pBits &= mask;
    *pBits |= (val ? 1 : 0) << pos;
}

template<typename T>
NW_FORCE_INLINE T SetBits(T bits, int pos, int len, T val)
{
    NW_ASSERT(len <= 32);
    const u32 MaxValue = 0xFFFFFFFFU >> (32 - len);
    NW_ASSERT(val <= MaxValue);
    const T mask = T(~(MaxValue << pos));
    bits &= mask;
    bits |= val << pos;
    return bits;
}

template<typename T>
NW_FORCE_INLINE void SetBits(T* pBits, int pos, int len, T val)
{
    NW_ASSERT(len <= 32);
    const u32 MaxValue = 0xFFFFFFFFU >> (32 - len);
    NW_ASSERT(val <= MaxValue);
    const T mask = T(~(MaxValue << pos));
    *pBits &= mask;
    *pBits |= val << pos;
}

template<typename T>
NW_FORCE_INLINE const T* ConvertOffsToPtr(const void* baseAddress, unsigned int offset)
{
    return reinterpret_cast<const T*>(static_cast<const u8*>(baseAddress) + offset);
}

template<typename T>
NW_FORCE_INLINE T* ConvertOffsToPtr(void* baseAddress, unsigned int offset)
{
    return reinterpret_cast<T*>(static_cast<u8*>(baseAddress) + offset);
}

typedef s16 ResS16;
typedef u16 ResU16;
typedef s32 ResS32;
typedef u32 ResU32;
typedef f32 ResF32;

} // namespace internal

enum PaneType
{
    PANETYPE_OTHER,
    PANETYPE_NULL,
    PANETYPE_PICTURE,
    PANETYPE_WINDOW,
    PANETYPE_TEXTBOX,
    PANETYPE_BOUNDING,
    PANETYPE_MAX
};

enum PaneFlag
{
    PANEFLAG_VISIBLE,
    PANEFLAG_INFLUENCEDALPHA,
    PANEFLAG_LOCATIONADJUST,
    PANEFLAG_USERALLOCATED,
    PANEFLAG_MTXCONDITION,
    PANEFLAG_MTXCONDITION_1,
    PANEFLAG_USERMTX,
    PANEFLAG_USERGLOBALMTX,
    PANEFLAG_MAX,
    PANEFLAG_MTXCONDITION_LENGTH = 2
};

enum MtxCondition
{
    MTXCONDITION_DIRTY,
    MTXCONDITION_CLEAN,
    MTXCONDITION_CLEAN_LOCATION_ADJUST,
    MTXCONDITION_MAX
};

enum HorizontalPosition
{
    HORIZONTALPOSITION_LEFT,
    HORIZONTALPOSITION_CENTER,
    HORIZONTALPOSITION_RIGHT,
    HORIZONTALPOSITION_MAX
};

enum VerticalPosition
{
    VERTICALPOSITION_TOP,
    VERTICALPOSITION_CENTER,
    VERTICALPOSITION_BOTTOM,
    VERTICALPOSITION_MAX
};

enum TexFormat
{
    TEXFORMAT_L8,
    TEXFORMAT_A8,
    TEXFORMAT_LA4,
    TEXFORMAT_LA8,
    TEXFORMAT_HILO8,
    TEXFORMAT_RGB565,
    TEXFORMAT_RGB8,
    TEXFORMAT_RGB5A1,
    TEXFORMAT_RGBA4,
    TEXFORMAT_RGBA8,
    TEXFORMAT_ETC1,
    TEXFORMAT_ETC1A4,
    TEXFORMAT_L4,
    TEXFORMAT_A4,
    TEXFORMAT_MAX
};

enum TexWrap
{
    TEXWRAP_CLAMP,
    TEXWRAP_REPEAT,
    TEXWRAP_MIRROR,
    TEXWRAP_MAX
};

enum TexFilter
{
    TEXFILTER_NEAR,
    TEXFILTER_LINEAR,
    TEXFILTER_MAX
};

enum TevMode
{
    TEVMODE_REPLACE,
    TEVMODE_MODULATE,
    TEVMODE_ADD,
    TEVMODE_ADD_SIGNED,
    TEVMODE_INTERPOLATE,
    TEVMODE_SUBTRACT,
    TEVMODE_ADD_MULT,
    TEVMODE_MULT_ADD,
    TEVMODE_MAX
};

enum TevSrc
{
    TEVSRC_TEXTURE0,
    TEVSRC_TEXTURE1,
    TEVSRC_TEXTURE2,
    TEVSRC_TEXTURE3,
    TEVSRC_CONSTANT,
    TEVSRC_PRIMARY,
    TEVSRC_PREVIOUS,
    TEVSRC_PREVIOUS_BUFFER,
    TEVSRC_MAX
};

enum TevOpRgb
{
    TEVOPRGB_RGB,
    TEVOPRGB_INV_RGB,
    TEVOPRGB_ALPHA,
    TEVOPRGB_INV_ALPHA,
    TEVOPRGB_RRR,
    TEVOPRGB_INV_RRR,
    TEVOPRGB_GGG,
    TEVOPRGB_INV_GGG,
    TEVOPRGB_BBB,
    TEVOPRGB_INV_BBB,
    TEVOPRGB_MAX
};

enum TevOpAlp
{
    TEVOPALP_ALPHA,
    TEVOPALP_INV_ALPHA,
    TEVOPALP_R,
    TEVOPALP_INV_R,
    TEVOPALP_G,
    TEVOPALP_INV_G,
    TEVOPALP_B,
    TEVOPALP_INV_B,
    TEVOPALP_MAX
};

enum TevScale
{
    TEVSCALE_1,
    TEVSCALE_2,
    TEVSCALE_4,
    TEVSCALE_MAX
};

enum TevKonstSel
{
    TEVKONSTSEL_BUFFER,
    TEVKONSTSEL_K0,
    TEVKONSTSEL_K1,
    TEVKONSTSEL_K2,
    TEVKONSTSEL_K3,
    TEVKONSTSEL_K4,
    TEVKONSTSEL_K5,
    TEVKONSTSEL_MAX
};

enum AlphaTest
{
    ALPHATEST_NEVER,
    ALPHATEST_LESS,
    ALPHATEST_LEQUAL,
    ALPHATEST_EQUAL,
    ALPHATEST_NEQUAL,
    ALPHATEST_GEQUAL,
    ALPHATEST_GREATER,
    ALPHATEST_ALWAYS,
    ALPHATEST_MAX
};

enum BlendFactorSrc
{
    BLENDFACTORSRC_0,
    BLENDFACTORSRC_1,
    BLENDFACTORSRC_DST_COLOR,
    BLENDFACTORSRC_INV_DST_COLOR,
    BLENDFACTORSRC_SRC_ALPHA,
    BLENDFACTORSRC_INV_SRC_ALPHA,
    BLENDFACTORSRC_DST_ALPHA,
    BLENDFACTORSRC_INV_DST_ALPHA,
    BLENDFACTORSRC_MAX
};

enum BlendFactorDst
{
    BLENDFACTORDST_0,
    BLENDFACTORDST_1,
    BLENDFACTORDST_SRC_COLOR,
    BLENDFACTORDST_INV_SRC_COLOR,
    BLENDFACTORDST_SRC_ALPHA,
    BLENDFACTORDST_INV_SRC_ALPHA,
    BLENDFACTORDST_DST_ALPHA,
    BLENDFACTORDST_INV_DST_ALPHA,
    BLENDFACTORDST_MAX
};

enum BlendOp
{
    BLENDOP_DISABLE,
    BLENDOP_ADD,
    BLENDOP_SUBTRACT,
    BLENDOP_REVERSE_SUBTRACT,
    BLENDOP_MAX
};

enum LogicOp
{
    LOGICOP_DISABLE,
    LOGICOP_NOOP,
    LOGICOP_CLEAR,
    LOGICOP_SET,
    LOGICOP_COPY,
    LOGICOP_INV_COPY,
    LOGICOP_INV,
    LOGICOP_AND,
    LOGICOP_NAND,
    LOGICOP_OR,
    LOGICOP_NOR,
    LOGICOP_XOR,
    LOGICOP_EQUIV,
    LOGICOP_REV_AND,
    LOGICOP_INV_AND,
    LOGICOP_REV_OR,
    LOGICOP_INV_OR,
    LOGICOP_MAX
};

enum TexGenType
{
    TEXGENTYPE_MTX2x4,
    TEXGENTYPE_MAX
};

enum TexGenSrc
{
    TEXGENSRC_TEX0,
    TEXGENSRC_TEX1,
    TEXGENSRC_TEX2,
    TEXGENSRC_MAX
};

enum TextureFlip
{
    TEXTUREFLIP_NONE,
    TEXTUREFLIP_FLIPH,
    TEXTUREFLIP_FLIPV,
    TEXTUREFLIP_ROTATE90,
    TEXTUREFLIP_ROTATE180,
    TEXTUREFLIP_ROTATE270,
    TEXTUREFLIP_MAX
};

enum ScreenOriginType
{
    SCREENORIGINTYPE_CLASSIC,
    SCREENORIGINTYPE_NORMAL,
    SCREENORIGINTYPE_MAX
};

enum TextAlignment
{
    TEXTALIGNMENT_SYNCHRONOUS,
    TEXTALIGNMENT_LEFT,
    TEXTALIGNMENT_CENTER,
    TEXTALIGNMENT_RIGHT,
    TEXTALIGNMENT_MAX
};

enum VertexAttr
{
    VERTEXATTR_POS,
    VERTEXATTR_COLOR,
    VERTEXATTR_TEXCOORD0,
    VERTEXATTR_TEXCOORD1,
    VERTEXATTR_TEXCOORD2,
    VERTEXATTR_VERTEX_INDEX = 0,
    VERTEXATTR_MAX
};

enum VertexAttrSize
{
    VERTEXATTRSIZE_POS      = 4,
    VERTEXATTRSIZE_COLOR    = 4,
    VERTEXATTRSIZE_TEXCOORD = 2,
    VERTEXATTRSIZE_INDEX    = 2
};

using nw::font::Vertex;
using nw::font::VERTEX_LT;
using nw::font::VERTEX_RT;
using nw::font::VERTEX_LB;
using nw::font::VERTEX_RB;
using nw::font::VERTEX_MAX;

enum VertexColor
{
    VERTEXCOLOR_LT,
    VERTEXCOLOR_RT,
    VERTEXCOLOR_LB,
    VERTEXCOLOR_RB,
    VERTEXCOLOR_MAX
};

enum WindowFrame
{
    WINDOWFRAME_LT,
    WINDOWFRAME_RT,
    WINDOWFRAME_LB,
    WINDOWFRAME_RB,
    WINDOWFRAME_L,
    WINDOWFRAME_R,
    WINDOWFRAME_T,
    WINDOWFRAME_B,
    WINDOWFRAME_MAX
};

enum TextColor
{
    TEXTCOLOR_TOP,
    TEXTCOLOR_BOTTOM,
    TEXTCOLOR_MAX
};

enum InterpolateColor
{
    INTERPOLATECOLOR_BLACK = 0,
    INTERPOLATECOLOR_WHITE = 1,
    INTERPOLATECOLOR_MAX
};

enum MemArea
{
    MEMAREA_FCRAM = 0,
    MEMAREA_VRAMA = 1,
    MEMAREA_VRAMB = 2,
    MEMAREA_MAX
};

enum ImageFlag
{
    IMAGEFLAG_AREA_POS = 0,
    IMAGEFLAG_AREA_LEN = 2
};

enum ExtUserDataType
{
    EXTUSERDATATYPE_STRING,
    EXTUSERDATATYPE_INT,
    EXTUSERDATATYPE_FLOAT,
    EXTUSERDATATYPE_MAX
};

enum AnimContentType
{
    ANIMCONTENTTYPE_PANE,
    ANIMCONTENTTYPE_MATERIAL,
    ANIMCONTENTTYPE_MAX
};

enum AnimTagFlag
{
    ANIMTAGFLAG_DESCENDINGBIND,
    ANIMTAGFLAG_MAX
};

enum AnimTargetPane
{
    ANIMTARGET_PANE_TRANSLATEX = 0,
    ANIMTARGET_PANE_TRANSLATEY,
    ANIMTARGET_PANE_TRANSLATEZ,
    ANIMTARGET_PANE_ROTATEX,
    ANIMTARGET_PANE_ROTATEY,
    ANIMTARGET_PANE_ROTATEZ,
    ANIMTARGET_PANE_SCALEX,
    ANIMTARGET_PANE_SCALEY,
    ANIMTARGET_PANE_SIZEW,
    ANIMTARGET_PANE_SIZEH,
    ANIMTARGET_PANE_MAX
};

enum AnimTargetVisibility
{
    ANIMTARGET_VISIBILITY_VISIBILITY = 0,
    ANIMTARGET_VISIBILITY_MAX
};

enum AnimTargetPaneColor
{
    ANIMTARGET_VERTEXCOLOR_LT_R = 0,
    ANIMTARGET_VERTEXCOLOR_LT_G,
    ANIMTARGET_VERTEXCOLOR_LT_B,
    ANIMTARGET_VERTEXCOLOR_LT_A,
    ANIMTARGET_VERTEXCOLOR_RT_R,
    ANIMTARGET_VERTEXCOLOR_RT_G,
    ANIMTARGET_VERTEXCOLOR_RT_B,
    ANIMTARGET_VERTEXCOLOR_RT_A,
    ANIMTARGET_VERTEXCOLOR_LB_R,
    ANIMTARGET_VERTEXCOLOR_LB_G,
    ANIMTARGET_VERTEXCOLOR_LB_B,
    ANIMTARGET_VERTEXCOLOR_LB_A,
    ANIMTARGET_VERTEXCOLOR_RB_R,
    ANIMTARGET_VERTEXCOLOR_RB_G,
    ANIMTARGET_VERTEXCOLOR_RB_B,
    ANIMTARGET_VERTEXCOLOR_RB_A,
    ANIMTARGET_VERTEXCOLOR_MAX,
    ANIMTARGET_PANE_ALPHA = ANIMTARGET_VERTEXCOLOR_MAX,
    ANIMTARGET_PANE_COLOR_MAX
};

enum AnimTargetMatColor
{
    ANIMTARGET_MATCOLOR_BUFFER_R = 0,
    ANIMTARGET_MATCOLOR_BUFFER_G,
    ANIMTARGET_MATCOLOR_BUFFER_B,
    ANIMTARGET_MATCOLOR_BUFFER_A,
    ANIMTARGET_MATCOLOR_KONST0_R,
    ANIMTARGET_MATCOLOR_KONST0_G,
    ANIMTARGET_MATCOLOR_KONST0_B,
    ANIMTARGET_MATCOLOR_KONST0_A,
    ANIMTARGET_MATCOLOR_KONST1_R,
    ANIMTARGET_MATCOLOR_KONST1_G,
    ANIMTARGET_MATCOLOR_KONST1_B,
    ANIMTARGET_MATCOLOR_KONST1_A,
    ANIMTARGET_MATCOLOR_KONST2_R,
    ANIMTARGET_MATCOLOR_KONST2_G,
    ANIMTARGET_MATCOLOR_KONST2_B,
    ANIMTARGET_MATCOLOR_KONST2_A,
    ANIMTARGET_MATCOLOR_KONST3_R,
    ANIMTARGET_MATCOLOR_KONST3_G,
    ANIMTARGET_MATCOLOR_KONST3_B,
    ANIMTARGET_MATCOLOR_KONST3_A,
    ANIMTARGET_MATCOLOR_KONST4_R,
    ANIMTARGET_MATCOLOR_KONST4_G,
    ANIMTARGET_MATCOLOR_KONST4_B,
    ANIMTARGET_MATCOLOR_KONST4_A,
    ANIMTARGET_MATCOLOR_KONST5_R,
    ANIMTARGET_MATCOLOR_KONST5_G,
    ANIMTARGET_MATCOLOR_KONST5_B,
    ANIMTARGET_MATCOLOR_KONST5_A,
    ANIMTARGET_MATCOLOR_MAX
};

enum AnimTargetTexSRT
{
    ANIMTARGET_TEXSRT_TRANSLATES = 0,
    ANIMTARGET_TEXSRT_TRANSLATET,
    ANIMTARGET_TEXSRT_ROTATE,
    ANIMTARGET_TEXSRT_SCALES,
    ANIMTARGET_TEXSRT_SCALET,
    ANIMTARGET_TEXSRT_MAX
};

enum AnimTargetTexPattern
{
    ANIMTARGET_TEXPATTURN_IMAGE = 0,
    ANIMTARGET_TEXPATTURN_MAX
};

enum AnimCurve
{
    ANIMCURVE_CONSTANT,
    ANIMCURVE_STEP,
    ANIMCURVE_HERMITE,
    ANIMCURVE_MAX
};

enum AnimateOpt
{
    ANIMATEOPT_NOANIMATEINVISIBLE = 0x0001
};

namespace res {

struct Vec2
{
    Vec2() {}
    Vec2(f32 x, f32 y) { this->x = x; this->y = y; }

    operator const nn::math::VEC2() const { return nn::math::VEC2(x, y); }

    ut::ResF32 x;
    ut::ResF32 y;
};

struct Vec3
{
    Vec3() {}
    Vec3(f32 x, f32 y, f32 z) { this->x = x; this->y = y; this->z = z; }

    operator const math::VEC3() const { return math::VEC3(x, y, z); }

    ut::ResF32 x;
    ut::ResF32 y;
    ut::ResF32 z;
};

} // namespace res

struct Size
{
    Size(): width(0), height(0) {}
    Size(f32 aWidth, f32 aHeight): width(aWidth), height(aHeight) {}
    Size(const Size& other): width(other.width), height(other.height) {}

    ut::ResF32 width;
    ut::ResF32 height;
};

inline bool operator==(const Size& a, const Size& b)
{
    return a.width == b.width && a.height == b.height;
}

struct TexSize
{
    TexSize(): width(0), height(0) {}
    TexSize(u16 aWidth, u16 aHeight): width(aWidth), height(aHeight) {}
    TexSize(const TexSize& other): width(other.width), height(other.height) {}

    operator const Size() const { return Size(this->width, this->height); }

    u16 width;
    u16 height;
};

struct TexSRT
{
    res::Vec2  translate;
    ut::ResF32 rotate;
    res::Vec2  scale;
};

struct TexCoordGen
{
    TexCoordGen() { Set(TEXGENTYPE_MTX2x4, TEXGENSRC_TEX0); }
    TexCoordGen(TexGenType aTexGenType, TexGenSrc aTexGenSrc) { Set(aTexGenType, aTexGenSrc); }

    void Set(TexGenType aTexGenType, TexGenSrc aTexGenSrc)
    {
        texGenType = aTexGenType;
        texGenSrc  = aTexGenSrc;
    }

    TexGenType GetTexGenType() const { return TexGenType(texGenType); }
    TexGenSrc  GetTexGenSrc()  const { return TexGenSrc(texGenSrc); }

protected:
    ut::ResU8 texGenType;
    ut::ResU8 texGenSrc;
    ut::ResU8 reserve[2];
};

struct TevStage
{
    TevStage()
    {
        Set(
            TEVMODE_REPLACE, TEVMODE_REPLACE,
            TEVSRC_PRIMARY, TEVSRC_PRIMARY, TEVSRC_PRIMARY,
            TEVSRC_PRIMARY, TEVSRC_PRIMARY, TEVSRC_PRIMARY,
            TEVOPRGB_RGB, TEVOPRGB_RGB, TEVOPRGB_RGB,
            TEVOPALP_ALPHA, TEVOPALP_ALPHA, TEVOPALP_ALPHA,
            TEVSCALE_1, TEVSCALE_1,
            TEVKONSTSEL_K0, TEVKONSTSEL_K0,
            false, false);
    }

    void Set(TevMode combineRgb, TevMode combineAlpha,
             TevSrc srcRgb0, TevSrc srcRgb1, TevSrc srcRgb2,
             TevSrc srcAlpha0, TevSrc srcAlpha1, TevSrc srcAlpha2,
             TevOpRgb operandRgb0, TevOpRgb operandRgb1, TevOpRgb operandRgb2,
             TevOpAlp operandAlpha0, TevOpAlp operandAlpha1, TevOpAlp operandAlpha2,
             TevScale scaleRgb, TevScale scaleAlpha,
             TevKonstSel konstSelRgb, TevKonstSel konstSelAlpha,
             bool savePrevRgb, bool savePrevAlpha)
    {
        SetCombineRgb(combineRgb);       SetCombineAlpha(combineAlpha);
        SetSrcRgb0(srcRgb0);             SetSrcRgb1(srcRgb1);           SetSrcRgb2(srcRgb2);
        SetSrcAlpha0(srcAlpha0);         SetSrcAlpha1(srcAlpha1);       SetSrcAlpha2(srcAlpha2);
        SetOperandRgb0(operandRgb0);     SetOperandRgb1(operandRgb1);   SetOperandRgb2(operandRgb2);
        SetOperandAlpha0(operandAlpha0); SetOperandAlpha1(operandAlpha1); SetOperandAlpha2(operandAlpha2);
        SetScaleRgb(scaleRgb);           SetScaleAlpha(scaleAlpha);
        SetKonstSelRgb(konstSelRgb);     SetKonstSelAlpha(konstSelAlpha);
        SetSavePrevRgb(savePrevRgb);     SetSavePrevAlpha(savePrevAlpha);
    }

    void SetCombineRgb(TevMode value) { bits0 = internal::SetBits(u32(bits0), POS0_COMBINERGB,    BITS_COMBINERGB,   u32(value)); }
    void SetCombineAlpha(TevMode value) { bits1 = internal::SetBits(u32(bits1), POS1_COMBINEALPHA,  BITS_COMBINEALPHA, u32(value)); }
    void SetSrcRgb0(TevSrc value) { bits0 = internal::SetBits(u32(bits0), POS0_SRCRGB0,       BITS_SRCRGB,       u32(value)); }
    void SetSrcRgb1(TevSrc value) { bits0 = internal::SetBits(u32(bits0), POS0_SRCRGB1,       BITS_SRCRGB,       u32(value)); }
    void SetSrcRgb2(TevSrc value) { bits0 = internal::SetBits(u32(bits0), POS0_SRCRGB2,       BITS_SRCRGB,       u32(value)); }
    void SetSrcAlpha0(TevSrc value) { bits1 = internal::SetBits(u32(bits1), POS1_SRCALPHA0,     BITS_SRCALPHA,     u32(value)); }
    void SetSrcAlpha1(TevSrc value) { bits1 = internal::SetBits(u32(bits1), POS1_SRCALPHA1,     BITS_SRCALPHA,     u32(value)); }
    void SetSrcAlpha2(TevSrc value) { bits1 = internal::SetBits(u32(bits1), POS1_SRCALPHA2,     BITS_SRCALPHA,     u32(value)); }
    void SetOperandRgb0(TevOpRgb value) { bits0 = internal::SetBits(u32(bits0), POS0_OPERANDRGB0,   BITS_OPERANDRGB,   u32(value)); }
    void SetOperandRgb1(TevOpRgb value) { bits0 = internal::SetBits(u32(bits0), POS0_OPERANDRGB1,   BITS_OPERANDRGB,   u32(value)); }
    void SetOperandRgb2(TevOpRgb value) { bits0 = internal::SetBits(u32(bits0), POS0_OPERANDRGB2,   BITS_OPERANDRGB,   u32(value)); }
    void SetOperandAlpha0(TevOpAlp value) { bits1 = internal::SetBits(u32(bits1), POS1_OPERANDALPHA0, BITS_OPERANDALPHA, u32(value)); }
    void SetOperandAlpha1(TevOpAlp value) { bits1 = internal::SetBits(u32(bits1), POS1_OPERANDALPHA1, BITS_OPERANDALPHA, u32(value)); }
    void SetOperandAlpha2(TevOpAlp value) { bits1 = internal::SetBits(u32(bits1), POS1_OPERANDALPHA2, BITS_OPERANDALPHA, u32(value)); }
    void SetScaleRgb(TevScale value) { bits0 = internal::SetBits(u32(bits0), POS0_SCALERGB,      BITS_SCALE,        u32(value)); }
    void SetScaleAlpha(TevScale value) { bits1 = internal::SetBits(u32(bits1), POS1_SCALEALPHA,    BITS_SCALE,        u32(value)); }
    void SetKonstSelRgb(TevKonstSel value) { bits2 = internal::SetBits(u32(bits2), POS2_KONSTSELRGB,   BITS_KONSTSEL,     u32(value)); }
    void SetKonstSelAlpha(TevKonstSel value) { bits2 = internal::SetBits(u32(bits2), POS2_KONSTSELALPHA, BITS_KONSTSEL,     u32(value)); }
    void SetSavePrevRgb(bool value) { bits0 = internal::SetBit(u32(bits0), POS0_SAVEPREVRGB,   value); }
    void SetSavePrevAlpha(bool value) { bits1 = internal::SetBit(u32(bits1), POS1_SAVEPREVALPHA, value); }

    TevMode     GetCombineRgb()    const { return TevMode(internal::GetBits(u32(bits0), POS0_COMBINERGB,    BITS_COMBINERGB)); }
    TevMode     GetCombineAlpha()  const { return TevMode(internal::GetBits(u32(bits1), POS1_COMBINEALPHA,  BITS_COMBINEALPHA)); }
    TevSrc      GetSrcRgb0()       const { return TevSrc(internal::GetBits(u32(bits0),  POS0_SRCRGB0,       BITS_SRCRGB)); }
    TevSrc      GetSrcRgb1()       const { return TevSrc(internal::GetBits(u32(bits0),  POS0_SRCRGB1,       BITS_SRCRGB)); }
    TevSrc      GetSrcRgb2()       const { return TevSrc(internal::GetBits(u32(bits0),  POS0_SRCRGB2,       BITS_SRCRGB)); }
    TevSrc      GetSrcAlpha0()     const { return TevSrc(internal::GetBits(u32(bits1),  POS1_SRCALPHA0,     BITS_SRCALPHA)); }
    TevSrc      GetSrcAlpha1()     const { return TevSrc(internal::GetBits(u32(bits1),  POS1_SRCALPHA1,     BITS_SRCALPHA)); }
    TevSrc      GetSrcAlpha2()     const { return TevSrc(internal::GetBits(u32(bits1),  POS1_SRCALPHA2,     BITS_SRCALPHA)); }
    TevOpRgb    GetOperandRgb0()   const { return TevOpRgb(internal::GetBits(u32(bits0), POS0_OPERANDRGB0,  BITS_OPERANDRGB)); }
    TevOpRgb    GetOperandRgb1()   const { return TevOpRgb(internal::GetBits(u32(bits0), POS0_OPERANDRGB1,  BITS_OPERANDRGB)); }
    TevOpRgb    GetOperandRgb2()   const { return TevOpRgb(internal::GetBits(u32(bits0), POS0_OPERANDRGB2,  BITS_OPERANDRGB)); }
    TevOpAlp    GetOperandAlpha0() const { return TevOpAlp(internal::GetBits(u32(bits1), POS1_OPERANDALPHA0, BITS_OPERANDALPHA)); }
    TevOpAlp    GetOperandAlpha1() const { return TevOpAlp(internal::GetBits(u32(bits1), POS1_OPERANDALPHA1, BITS_OPERANDALPHA)); }
    TevOpAlp    GetOperandAlpha2() const { return TevOpAlp(internal::GetBits(u32(bits1), POS1_OPERANDALPHA2, BITS_OPERANDALPHA)); }
    TevScale    GetScaleRgb()      const { return TevScale(internal::GetBits(u32(bits0),  POS0_SCALERGB,    BITS_SCALE)); }
    TevScale    GetScaleAlpha()    const { return TevScale(internal::GetBits(u32(bits1),  POS1_SCALEALPHA,  BITS_SCALE)); }
    TevKonstSel GetKonstSelRgb()   const { return TevKonstSel(internal::GetBits(u32(bits2), POS2_KONSTSELRGB,   BITS_KONSTSEL)); }
    TevKonstSel GetKonstSelAlpha() const { return TevKonstSel(internal::GetBits(u32(bits2), POS2_KONSTSELALPHA, BITS_KONSTSEL)); }
    bool        GetSavePrevRgb()   const { return internal::TestBit(u32(bits0), POS0_SAVEPREVRGB); }
    bool        GetSavePrevAlpha() const { return internal::TestBit(u32(bits1), POS1_SAVEPREVALPHA); }

protected:
    ut::ResU32 bits0;
    ut::ResU32 bits1;
    ut::ResU32 bits2;

    enum Bits
    {
        BITS_COMBINERGB   = 4,
        BITS_COMBINEALPHA = 4,
        BITS_SRCRGB       = 4,
        BITS_SRCALPHA     = 4,
        BITS_OPERANDRGB   = 4,
        BITS_OPERANDALPHA = 4,
        BITS_SCALE        = 2,
        BITS_KONSTSEL     = 4,
        BITS_SAVEPREV     = 1
    };

    enum Pos0
    {
        POS0_SRCRGB0     = 0,
        POS0_SRCRGB1     = 4,
        POS0_SRCRGB2     = 8,
        POS0_OPERANDRGB0 = 12,
        POS0_OPERANDRGB1 = 16,
        POS0_OPERANDRGB2 = 20,
        POS0_COMBINERGB  = 24,
        POS0_SCALERGB    = 28,
        POS0_SAVEPREVRGB = 30,
        FIELD0_SIZE      = 31
    };

    enum Pos1
    {
        POS1_SRCALPHA0      = 0,
        POS1_SRCALPHA1      = 4,
        POS1_SRCALPHA2      = 8,
        POS1_OPERANDALPHA0  = 12,
        POS1_OPERANDALPHA1  = 16,
        POS1_OPERANDALPHA2  = 20,
        POS1_COMBINEALPHA   = 24,
        POS1_SCALEALPHA     = 28,
        POS1_SAVEPREVALPHA  = 30,
        FIELD1_SIZE         = 31
    };

    enum Pos2
    {
        POS2_KONSTSELRGB   = 0,
        POS2_KONSTSELALPHA = 4,
        FIELD2_SIZE        = 8
    };

    static void CompileCheck();
};

struct AlphaCompare
{
    AlphaCompare() { Set(ALPHATEST_ALWAYS, 0.f); }
    AlphaCompare(AlphaTest aFunc, f32 aRef) { Set(aFunc, aRef); }

    void Set(AlphaTest aFunc, f32 aRef)
    {
        func = u8(aFunc);
        ref  = aRef;
    }

    AlphaTest GetFunc() const { return AlphaTest(func); }
    f32       GetRef()  const { return ref; }

protected:
    u8         func;
    ut::ResF32 ref;
};

struct BlendMode
{
    BlendMode() { Set(BLENDOP_DISABLE, BLENDFACTORSRC_SRC_ALPHA, BLENDFACTORDST_INV_SRC_ALPHA, LOGICOP_DISABLE); }
    BlendMode(BlendOp aBlendOp, BlendFactorSrc aSrcFactor, BlendFactorDst aDstFactor, LogicOp aLogicOp)
    {
        Set(aBlendOp, aSrcFactor, aDstFactor, aLogicOp);
    }

    void Set(BlendOp aBlendOp, BlendFactorSrc aSrcFactor, BlendFactorDst aDstFactor, LogicOp aLogicOp)
    {
        blendOp   = u8(aBlendOp);
        srcFactor = u8(aSrcFactor);
        dstFactor = u8(aDstFactor);
        logicOp   = u8(aLogicOp);
    }

    BlendOp        GetBlendOp()   const { return BlendOp(blendOp); }
    BlendFactorSrc GetSrcFactor() const { return BlendFactorSrc(srcFactor); }
    BlendFactorDst GetDstFactor() const { return BlendFactorDst(dstFactor); }
    LogicOp        GetLogicOp()   const { return LogicOp(logicOp); }

protected:
    ut::ResU8 blendOp;
    ut::ResU8 srcFactor;
    ut::ResU8 dstFactor;
    ut::ResU8 logicOp;
};

struct InflationLRTB
{
    ut::ResF32 l;
    ut::ResF32 r;
    ut::ResF32 t;
    ut::ResF32 b;
};

struct WindowFrameSize
{
    ut::ResF32 l;
    ut::ResF32 r;
    ut::ResF32 t;
    ut::ResF32 b;
};

class ExtUserData
{
public:
    ExtUserData(u32 nameStrOffset, u32 dataOffset, u16 num, u8 type):
        m_NameStrOffset(nameStrOffset),
        m_DataOffset(dataOffset),
        m_Num(num),
        m_Type(type),
        m_Padding(0) {}

    const char*       GetName()       const { return m_NameStrOffset ? internal::ConvertOffsToPtr<const char>(this, m_NameStrOffset) : 0; }
    ExtUserDataType   GetType()       const { return ExtUserDataType(m_Type); }
    u16               GetNum()        const { return m_Num; }

    const char* GetString() const
    {
        return internal::ConvertOffsToPtr<const char>(this, m_DataOffset);
    }

    const ut::ResS32* GetIntArray() const
    {
        return internal::ConvertOffsToPtr<const ut::ResS32>(this, m_DataOffset);
    }

    const ut::ResF32* GetFloatArray() const
    {
        return internal::ConvertOffsToPtr<const ut::ResF32>(this, m_DataOffset);
    }

protected:
    ut::ResU32 m_NameStrOffset;
    ut::ResU32 m_DataOffset;
    ut::ResU16 m_Num;
    ut::ResU8  m_Type;
    ut::ResU8  m_Padding;
};

struct AnimationGroupRef
{
    AnimationGroupRef(): flag(0)
    {
        std::memset(name,    0, sizeof(name));
        std::memset(padding, 0, sizeof(padding));
    }

    const char* GetName() const { return name; }

    char      name[ResourceNameStrMax + 1];
    ut::ResU8 flag;
    ut::ResU8 padding[2];
};

struct AnimationShareInfo
{
    AnimationShareInfo()
    {
        std::memset(srcPaneName,     0, sizeof(srcPaneName));
        std::memset(targetGroupName, 0, sizeof(targetGroupName));
        std::memset(padding,         0, sizeof(padding));
    }

    const char* GetSrcPaneName()      const { return srcPaneName; }
    const char* GetTargetGroupName()  const { return targetGroupName; }

    char      srcPaneName[ResourceNameStrMax + 1];
    char      targetGroupName[ResourceNameStrMax + 1];
    ut::ResU8 padding[2];
};

class AnimTransform;

class AnimationLink
{
public:
    AnimationLink() { Reset(); }

    void Reset() { Set(0, 0, false); }

    void Set(AnimTransform* animTrans, u16 idx, bool bDisable)
    {
        m_AnimTrans = animTrans;
        m_Idx       = idx;
        m_Disable   = bDisable;
    }

    void SetAnimTransform(AnimTransform* animTrans, u16 idx)
    {
        m_AnimTrans = animTrans;
        m_Idx       = idx;
    }

    AnimTransform* GetAnimTransform() const { return m_AnimTrans; }
    u16            GetIndex()         const { return m_Idx; }

    bool IsEnable()          const { return !m_Disable; }
    void SetEnable(bool bEnable) { m_Disable = !bEnable; }

    ut::LinkListNode m_Link;

protected:
    AnimTransform* m_AnimTrans;
    u16            m_Idx;
    bool           m_Disable;
};

typedef ut::LinkList<AnimationLink, offsetof(AnimationLink, m_Link)> AnimationList;

class TextureInfo
{
public:
    static const u32 INVALID = 0;

    TextureInfo(): m_TexObject(INVALID), m_PhysicalAddress(NULL) {}
    TextureInfo(const TextureInfo& src) { this->Set(src); }
    TextureInfo(u32 texObject, uptr physicalAddress, const TexSize& size, const TexSize& realSize, TexFormat format)
    {
        this->Set(texObject, physicalAddress, size, realSize, format);
    }

    void Set(const TextureInfo& src) { *this = src; }

    void Set(u32 texObject, uptr physicalAddress, const TexSize& size, const TexSize& realSize, TexFormat format)
    {
        m_TexObject       = texObject;
        m_PhysicalAddress = physicalAddress;
        m_Size            = size;
        m_RealSize        = realSize;
        m_Format          = format;
    }

    u32            GetTextureObject()   const { return m_TexObject; }
    const TexSize& GetSize()            const { return m_Size; }
    const TexSize& GetRealSize()        const { return m_RealSize; }
    uptr           GetPhysicalAddress() const { return m_PhysicalAddress; }
    TexFormat      GetFormat()          const { return TexFormat(m_Format); }

    bool IsValid() const
    {
        return m_PhysicalAddress != 0;
    }

protected:
    static bool IsPowerOfTwo(u32 value) { return (value & ~(value - 1)) == value; }

protected:
    u32     m_TexObject;
    uptr    m_PhysicalAddress;
    TexSize m_Size;
    TexSize m_RealSize;
    u8      m_Format;
};

typedef nn::math::VEC2 TexCoordQuad[VERTEX_MAX];

typedef const void* FontKey;
typedef const void* TextureKey;

}
}