#pragma once

#include <nw/math/math_types.h>
#include <nw/ut/ut_Color.h>
#include <nw/lyt/lyt_Types.h>

namespace nw {
namespace lyt {
namespace res {

const u8  MajorVersion  = 2;
const u8  MinorVersion  = 2;
const u8  MicroVersion  = 0;
const u8  BugfixVersion = 0;

const u32 BinaryFileFormatVersion = NW_UT_MAKE_VERSION(MajorVersion, MinorVersion, MicroVersion, BugfixVersion);

/* BCLYT */

const u32 FILESIGNATURE_CLYT          = NW_UT_MAKE_SIGWORD('C', 'L', 'Y', 'T');
const u32 DATABLOCKKIND_LAYOUT        = NW_UT_MAKE_SIGWORD('l', 'y', 't', '1');
const u32 DATABLOCKKIND_PANEBEGIN     = NW_UT_MAKE_SIGWORD('p', 'a', 's', '1');
const u32 DATABLOCKKIND_PANEEND       = NW_UT_MAKE_SIGWORD('p', 'a', 'e', '1');
const u32 DATABLOCKKIND_PANE          = NW_UT_MAKE_SIGWORD('p', 'a', 'n', '1');
const u32 DATABLOCKKIND_PICTURE       = NW_UT_MAKE_SIGWORD('p', 'i', 'c', '1');
const u32 DATABLOCKKIND_TEXTBOX       = NW_UT_MAKE_SIGWORD('t', 'x', 't', '1');
const u32 DATABLOCKKIND_WINDOW        = NW_UT_MAKE_SIGWORD('w', 'n', 'd', '1');
const u32 DATABLOCKKIND_BOUNDING      = NW_UT_MAKE_SIGWORD('b', 'n', 'd', '1');
const u32 DATABLOCKKIND_GROUPBEGIN    = NW_UT_MAKE_SIGWORD('g', 'r', 's', '1');
const u32 DATABLOCKKIND_GROUPEND      = NW_UT_MAKE_SIGWORD('g', 'r', 'e', '1');
const u32 DATABLOCKKIND_GROUP         = NW_UT_MAKE_SIGWORD('g', 'r', 'p', '1');
const u32 DATABLOCKKIND_FONTLIST      = NW_UT_MAKE_SIGWORD('f', 'n', 'l', '1');
const u32 DATABLOCKKIND_TEXTURELIST   = NW_UT_MAKE_SIGWORD('t', 'x', 'l', '1');
const u32 DATABLOCKKIND_MATERIALLIST  = NW_UT_MAKE_SIGWORD('m', 'a', 't', '1');
const u32 DATABLOCKKIND_USERDATALIST  = NW_UT_MAKE_SIGWORD('u', 's', 'd', '1');

/* BCLAN */

const u32 FILESIGNATURE_CLAN          = NW_UT_MAKE_SIGWORD('C', 'L', 'A', 'N');
const u32 DATABLOCKKIND_PANEANIMTAG   = NW_UT_MAKE_SIGWORD('p', 'a', 't', '1');
const u32 DATABLOCKKIND_PANEANIMSHARE = NW_UT_MAKE_SIGWORD('p', 'a', 'h', '1');
const u32 DATABLOCKKIND_PANEANIMINFO  = NW_UT_MAKE_SIGWORD('p', 'a', 'i', '1');

/* BCLPA, ANIMATION */

const u32 ANIMATIONTYPE_PANESRT       = NW_UT_MAKE_SIGWORD('C', 'L', 'P', 'A');
const u32 ANIMATIONTYPE_VISIBILITY    = NW_UT_MAKE_SIGWORD('C', 'L', 'V', 'I');
const u32 ANIMATIONTYPE_VTXCOLOR      = NW_UT_MAKE_SIGWORD('C', 'L', 'V', 'C');
const u32 ANIMATIONTYPE_MATCOLOR      = NW_UT_MAKE_SIGWORD('C', 'L', 'M', 'C');
const u32 ANIMATIONTYPE_TEXSRT        = NW_UT_MAKE_SIGWORD('C', 'L', 'T', 'S');
const u32 ANIMATIONTYPE_TEXPATTERN    = NW_UT_MAKE_SIGWORD('C', 'L', 'T', 'P');

/* BCLIM */

const u32 FILESIGNATURE_CLIM          = NW_UT_MAKE_SIGWORD('C', 'L', 'I', 'M');
const u32 DATABLOCKKIND_IMAGE         = NW_UT_MAKE_SIGWORD('i', 'm', 'a', 'g');

const u32 RESOURCETYPE_LAYOUT         = 'blyt';
const u32 RESOURCETYPE_ANIMATION      = 'anim';
const u32 RESOURCETYPE_TEXTURE        = 'timg';
const u32 RESOURCETYPE_FONT           = 'font';
const u32 RESOURCETYPE_ARCHIVEFONT    = 'fnta';

struct Lyt{
    ut::BinaryFileHeader fileHeader;
};

struct Layout{
    ut::BinaryBlockHeader blockHeader;
    ut::ResU8 originType;
    ut::ResU8 padding[3];
    Size layoutSize;
};

struct Font{
    internal::ResU32 nameStrOffset;
};

struct FontList{
    ut::BinaryBlockHeader blockHeader;
    ut::ResU16 fontNum;
    ut::ResU8 padding[2];
};

struct Texture{
    internal::ResU32 nameStrOffset;
};

struct TextureList{
    ut::BinaryBlockHeader blockHeader;
    ut::ResU16 texNum;
    ut::ResU8 padding[2];
};

struct TexMap{
    TexMap():   
        texIdx(0),
        wrapSflt(0),
        wrapTflt(0)
    {}

    TexWrap GetWarpModeS() const{
        return TexWrap(internal::GetBits(wrapSflt,  0, 2));
    }

    TexWrap GetWarpModeT() const{
        return TexWrap(internal::GetBits(wrapTflt,  0, 2));
    }

    TexFilter GetMinFilter() const{
        return TexFilter(internal::GetBits(wrapSflt,  2, 2));
    }

    TexFilter GetMagFilter() const{
        return TexFilter(internal::GetBits(wrapTflt,  2, 2));
    }

    void SetWarpModeS(u8 value){
        internal::SetBits(&wrapSflt,  0, 2, value);
    }

    void SetWarpModeT(u8 value){
        internal::SetBits(&wrapTflt,  0, 2, value);
    }

    void SetMinFilter(u8 value){
        internal::SetBits(&wrapSflt,  2, 2, value);
    }

    void SetMagFilter(u8 value){
        internal::SetBits(&wrapTflt,  2, 2, value);
    }

    ut::ResU16 texIdx;
    ut::ResU8 wrapSflt;                       // TexWrap, TexFilter
    ut::ResU8 wrapTflt;                       // TexWrap, TexFilter
};

struct MaterialResourceNum{
    MaterialResourceNum(): bits(0) {}

    u8   GetTexMapNum()      const { return u8(internal::GetBits(bits, 0,  2)); }
    u8   GetTexSRTNum()      const { return u8(internal::GetBits(bits, 2,  2)); }
    u8   GetTexCoordGenNum() const { return u8(internal::GetBits(bits, 4,  2)); }
    u8   GetTevStageNum()    const { return u8(internal::GetBits(bits, 6,  3)); }
    bool HasAlphaCompare()   const { return internal::TestBit(bits, 9); }
    bool HasBlendMode()      const { return internal::TestBit(bits, 10); }
    bool IsTextureOnly()     const { return internal::TestBit(bits, 11); }

    void SetTexMapNum(u32 value)      { SetBits(0, 2, value); }
    void SetTexSRTNum(u32 value)      { SetBits(2, 2, value); }
    void SetTexCoordGenNum(u32 value) { SetBits(4, 2, value); }
    void SetTevStageNum(u32 value)    { SetBits(6, 3, value); }
    void SetAlphaCompare(bool b)      { SetBit(9,  b); }
    void SetBlendMode(bool b)         { SetBit(10, b); }
    void SetTextureOnly(bool b)       { SetBit(11, b); }

    ut::ResU32 bits;

protected:
    void SetBits(int pos, int len, u32 value){
        u32 work = bits;
        internal::SetBits(&work, pos, len, value);
        bits = work;
    }
    void SetBit(int pos, bool value){
        u32 work = bits;
        internal::SetBit(&work, pos, value);
        bits = work;
    }
};

struct Color{
    operator ut::Color8() const { return ut::Color8(r, g, b, a); }

    ut::ResU8 r;
    ut::ResU8 g;
    ut::ResU8 b;
    ut::ResU8 a;
};

struct Material{
    char                name[MaterialNameStrMax];
    Color               colors[MatColorMax];
    MaterialResourceNum resNum;
};

struct MaterialList{
    ut::BinaryBlockHeader blockHeader;
    ut::ResU16 materialNum;
    ut::ResU8  padding[2];
};

struct PaneBegin{
    ut::BinaryBlockHeader blockHeader;
};

struct PaneEnd{
    ut::BinaryBlockHeader blockHeader;
};

struct Pane{
    ut::BinaryBlockHeader blockHeader;
    ut::ResU8 flag;
    ut::ResU8 basePosition;
    ut::ResU8 alpha;
    ut::ResU8 padding[1];
    char      name[ResourceNameStrMax];
    char      userData[UserDataStrMax];
    Vec3      translate;
    Vec3      rotate;
    Vec2      scale;
    Size      size;
};

struct Picture : public Pane{
    Color      vtxCols[VERTEXCOLOR_MAX];
    ut::ResU16 materialIdx;
    ut::ResU8  texCoordNum;
    ut::ResU8  padding[1];
};

struct TextBox : public Pane{
    ut::ResU16 textBufBytes;
    ut::ResU16 textStrBytes;
    ut::ResU16 materialIdx;
    ut::ResU16 fontIdx;
    ut::ResU8  textPosition;
    ut::ResU8  textAlignment;
    ut::ResU8  padding[2];
    ut::ResU32 textStrOffset;
    Color      textCols[TEXTCOLOR_MAX];
    Size       fontSize;
    ut::ResF32 charSpace;
    ut::ResF32 lineSpace;
};

struct WindowFrame{
    ut::ResU16 materialIdx;
    ut::ResU8  textureFlip;
    ut::ResU8  padding[1];
};

struct WindowContent{
    Color      vtxCols[VERTEXCOLOR_MAX];
    ut::ResU16 materialIdx;
    ut::ResU8  texCoordNum;
    ut::ResU8  padding[1];
};

struct Window : public Pane{
    InflationLRTB inflation;
    ut::ResU8     frameNum;
    ut::ResU8     padding[3];
    ut::ResU32    contentOffset;
    ut::ResU32    frameOffsetTableOffset;
};

struct Bounding : public Pane{};

struct ExtUserDataList{
    ut::BinaryBlockHeader blockHeader;
    ut::ResU16 num;
    ut::ResU8  padding[2];
};

struct GroupBegin{
    ut::BinaryBlockHeader blockHeader;
};

struct GroupEnd{
    ut::BinaryBlockHeader blockHeader;
};

struct Group{
    ut::BinaryBlockHeader blockHeader;
    char       name[ResourceNameStrMax];
    ut::ResU16 paneNum;
    ut::ResU8  padding[2];
};

struct Lan{
    ut::BinaryFileHeader fileHeader;
};

struct AnimationTagBlock{
    ut::BinaryBlockHeader blockHeader;
    ut::ResU16 tagOrder;
    ut::ResU16 groupNum;
    ut::ResU32 nameOffset;
    ut::ResU32 groupsOffset;
    ut::ResS16 startFrame;
    ut::ResS16 endFrame;
    ut::ResU8  flag;
    ut::ResU8  padding[3];
};

struct AnimationShareBlock{
    ut::BinaryBlockHeader blockHeader;
    ut::ResU32 animShareInfoOffset;
    ut::ResU16 shareNum;
    ut::ResU8  padding[2];
};

struct AnimationBlock{
    ut::BinaryBlockHeader blockHeader;
    ut::ResU16 frameSize;
    ut::ResU8  loop;
    ut::ResU8  padding[1];
    ut::ResU16 fileNum;
    ut::ResU16 animContNum;
    ut::ResU32 animContOffsetsOffset;
};

struct AnimationContent{
    char      name[MaterialNameStrMax];
    ut::ResU8 num;
    ut::ResU8 type;
    ut::ResU8 padding[2];
};

struct AnimationInfo{
    ut::ResU32 kind;
    ut::ResU8  num;
    ut::ResU8  padding[3];
};

struct AnimationTarget{
    ut::ResU8  id;
    ut::ResU8  target;
    ut::ResU8  curveType;
    ut::ResU8  padding[1];
    ut::ResU16 keyNum;
    ut::ResU8  padding2[2];
    ut::ResU32 keysOffset;
};

struct HermiteKey{
    ut::ResF32 frame;
    ut::ResF32 value;
    ut::ResF32 slope;
};

struct StepKey{
    ut::ResF32 frame;
    ut::ResU16 value;
    ut::ResU16 padding[1];
};

struct Lim{
    ut::BinaryFileHeader fileHeader;
};

struct Image{
    ut::BinaryBlockHeader blockHeader;
    ut::ResU16 width;
    ut::ResU16 height;
    ut::ResU8  format;
    ut::ResU8  flag;
    ut::ResU8  padding[2];
};

struct ImageSize{
    ut::ResU32 imageSize;
};

} // namespace res
} // namespace lyt
} // namespace nw