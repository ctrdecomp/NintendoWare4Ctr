#pragma once

#include <nw/lyt/lyt_Material.h>
#include <nw/lyt/lyt_Resources.h>
#include <nw/lyt/lyt_Types.h>

#define NW_LYT_PRINT ::nw::os::internal::Printf

namespace nw{
namespace lyt{

class Material;
struct Size;
class DrawInfo;
class GraphicsResource;
class Layout;
class ResourceAccessor;

struct ResBlockSet{
    const res::TextureList* pTextureList;
    const res::FontList* pFontList;
    const res::MaterialList* pMaterialList;
    ResourceAccessor* pResAccessor;
};

namespace internal{

enum FrameSpecFlag{
    FRAMESPECFLAG_CONST_VERTEX  = 1 << 0,
    FRAMESPECFLAG_FRAME         = 1 << 1,
    FRAMESPECFLAG_SWAP          = 1 << 2,
    FRAMESPECFLAG_RIGHT         = 1 << 3,
    FRAMESPECFLAG_BOTTOM        = 1 << 4,
    FRAMESPECFLAG_HFLIP         = 1 << 5,
    FRAMESPECFLAG_VFLIP         = 1 << 6,

    FRAMESPECFLAG_FRAME_LT = FRAMESPECFLAG_CONST_VERTEX | FRAMESPECFLAG_FRAME,
    FRAMESPECFLAG_FRAME_RT = FRAMESPECFLAG_CONST_VERTEX | FRAMESPECFLAG_FRAME | FRAMESPECFLAG_RIGHT,
    FRAMESPECFLAG_FRAME_LB = FRAMESPECFLAG_CONST_VERTEX | FRAMESPECFLAG_FRAME | FRAMESPECFLAG_BOTTOM,
    FRAMESPECFLAG_FRAME_RB = FRAMESPECFLAG_CONST_VERTEX | FRAMESPECFLAG_FRAME | FRAMESPECFLAG_RIGHT | FRAMESPECFLAG_BOTTOM,
    FRAMESPECFLAG_FLIP_HFLIP = FRAMESPECFLAG_HFLIP,
    FRAMESPECFLAG_FLIP_VFLIP = FRAMESPECFLAG_VFLIP,
    FRAMESPECFLAG_FLIP_R90 = FRAMESPECFLAG_HFLIP | FRAMESPECFLAG_SWAP,
    FRAMESPECFLAG_FLIP_R180 = FRAMESPECFLAG_HFLIP | FRAMESPECFLAG_VFLIP,
    FRAMESPECFLAG_FLIP_R270 = FRAMESPECFLAG_VFLIP | FRAMESPECFLAG_SWAP,
    FRAMESPECFLAG_NORMAL = 0
};

bool EqualsResName(const char* name1, const char* name2);
bool EqualsMaterialName(const char* name1, const char* name2);

inline const char* GetStrTableStr(const void* pStrTable, int index){
    const u32* offsets = static_cast<const u32*>(pStrTable);
    const char* stringPool = static_cast<const char*>(pStrTable);

    return &stringPool[offsets[index]];
}
class TexCoordAry{
public:
    TexCoordAry();
    bool IsEmpty() const{return mCap == 0;}
    void Free();
    void Reserve(u8 num);
    u8 GetSize() const{return mNum;}
    void SetSize(u8 num);
    const TexCoordQuad* GetArray() const {return mpData;}
    void GetCoord(u32 idx,TexCoordQuad coord) const;
    void SetCoord(u32 idx,const TexCoordQuad coord);
    void Copy(const void* pResTexCoord,u8 texCoordNum);

protected:
    u8 mCap;
    u8 mNum;
    math::VEC2 (*mpData)[VERTEX_MAX];
};

const ut::Color8 MultipleAlpha(const ut::Color8 col,u8 alpha);
void  DrawQuad(const DrawInfo& drawInfo,const nn::math::VEC2& basePt,const Size& size,u8 texCoordNum = 0,
    const math::VEC2 (*texCoords)[VERTEX_MAX] = NULL, const ut::Color8* vtxColors = NULL);

void DrawQuad_Repeat(const DrawInfo& drawInfo,const math::VEC2& basePt,const Size& size);
void DrawLine(const DrawInfo& drawInfo,const math::VEC2& pos,const Size& size,ut::Color8 color);

void            FinalizeGraphics();

inline u8 GetVtxColorElement(const ut::Color8 cols[],u32 idx){
    return reinterpret_cast<const u8*>(&cols[idx / sizeof(ut::Color8)])[idx % sizeof(ut::Color8)];
}

inline void SetVtxColorElement(ut::Color8 cols[],u32 idx,u8 value){
    reinterpret_cast<u8*>(&cols[idx / sizeof(ut::Color8)])[idx % sizeof(ut::Color8)] = value;
}

inline HorizontalPosition GetHorizontalPosition(u8 var){
    return static_cast<HorizontalPosition>(var % HORIZONTALPOSITION_MAX);
}

inline VerticalPosition GetVerticalPosition(u8 var){
    return static_cast<VerticalPosition>(var / HORIZONTALPOSITION_MAX);
}

inline void SetHorizontalPosition(u8* pVar, u8 newVal){
    *pVar = u8(GetVerticalPosition(*pVar) * HORIZONTALPOSITION_MAX + newVal);
}

inline void SetVerticalPosition(u8* pVar, u8 newVal){
    *pVar = u8(newVal * HORIZONTALPOSITION_MAX + GetHorizontalPosition(*pVar));
}

class GL{
public:
    static void SetTextureSamplerType(GraphicsResource& graphicsResource, int index, int value);
    static void SetTevCombineRgb(GraphicsResource& graphicsResource, int index, TevMode value);
    static void SetTevCombineAlpha(GraphicsResource& graphicsResource, int index, TevMode value);
    static void SetTevSrcRgb(GraphicsResource& graphicsResource, int index, TevSrc value0, TevSrc value1, TevSrc value2);
    static void SetTevSrcAlpha(GraphicsResource& graphicsResource, int index, TevSrc value0, TevSrc value1, TevSrc value2);
    static void SetTevOperandRgb(GraphicsResource& graphicsResource, int index, TevOpRgb value0, TevOpRgb value1, TevOpRgb value2);
    static void SetTevOperandAlpha(GraphicsResource& graphicsResource, int index, TevOpAlp value0, TevOpAlp value1, TevOpAlp value2);
    static void SetTevScaleRgb(GraphicsResource& graphicsResource, int index, TevScale value);
    static void SetTevScaleAlpha(GraphicsResource& graphicsResource, int index, TevScale value);
    static void SetTevConstRgba(GraphicsResource& graphicsResource, int index, ut::Color8 value);
    static void SetTevConstRgba(GraphicsResource& graphicsResource, int index, ut::Color8 value0, ut::Color8 value1);
    static void SetTevBufferColor(GraphicsResource &graphicsResource, ut::Color8 value);
    static void SetTevBufferInput(GraphicsResource &graphicsResource, int index, bool valueRgb, bool valueAlpha);
    static void SetEnableAlphaTest(GraphicsResource& graphicsResource, bool value);
    static void SetAlphaRefValue(GraphicsResource& graphicsResource, f32 value);
    static void SetAlphaTestFunc(GraphicsResource& graphicsResource, AlphaTest value);
    static void SetTexMinFilter(GraphicsResource& graphicsResoruce, TexFilter value);
    static void SetTexMagFilter(GraphicsResource& graphicsResoruce, TexFilter value);
};

}
}
}