#pragma once

#include <cstddef>
#include <cstdio>

#include <GLES2/gl2.h>
#include <nn/types.h>
#include <nn/math/math_Vec2.h>
#include <nn/math/math_Vec3.h>

#include <nw/ut/ut_Color.h>
#include <nw/font/font_Font.h>
#include <nw/font/font_TextWriterResource.h>

#define NW_FONT_TEXTWRITER_SHBIN L"nwfont_TextWriterShader.shbin"
#define NW_FONT_RECTRAWER_SHBIN L"nwfont_RectDrawerShader.shbin"

namespace nw {
namespace font {

    class DispStringBuffer;
    
    static const u32 DEFAULT_COLOR_MAPPING_MIN = 0x00000000UL;
    static const u32 DEFAULT_COLOR_MAPPING_MAX = 0xFFFFFFFFUL;
class CharWriter{
private:
    typedef nn::math::VEC2 CharScale;
    typedef nn::math::VEC3 CursorPos;

    void SetupGXDefault(bool bAlphaTex = false);
    void LoadTexture(const Glyph& glyph);
    void PrintGlyph(f32 x,const Glyph& glyph);
    void SetupGXCommon();
    void SetupGXWithColorMapping(bool bAlphaTex = false);
    void SetupVertexFormat();

    struct ColorMapping{
        ut::Color8 min;
        ut::Color8 max;
    };
public:
    enum GradationMode{
        GRADMODE_NONE,
        GRADMODE_H,
        GRADMODE_V,
        NUM_OF_GRADMODE
    };

    CharWriter();
    ~CharWriter();

    void SetupGX();
    void UseCommandBuffer();
    static void FinalizeGX(){TextWriterResource::FinalizeGX();}
    static u32 GetDispStringBufferSize(u32 charNum);
    static DispStringBuffer* InitDispStringBuffer(void* drawBuffer,u32 charNum);
    void SetFontSize(f32 width,f32 height);
    void SetFontSize(f32 height);
    void SetAlpha(u8 alpha){mAlpha = alpha;}
    f32 GetFontWidth() const;
    f32 GetFontHeight() const;
    f32 GetFontAscent() const;
    f32 GetFontDescent() const;
    f32 Print(CharCode code);
    void StartPrint();
    void EndPrint();
    void DrawGlyph(const Glyph& glyph);
    void UpdateVertexColor();

    void SetColorMapping(ut::Color8  min,ut::Color8  max){
        mColorMapping.min = min;
        mColorMapping.max = max;
    }

    void ResetColorMapping(){
        this->SetColorMapping(DEFAULT_COLOR_MAPPING_MIN, DEFAULT_COLOR_MAPPING_MAX);
    }

    void SetGradationMode(GradationMode mode){
        mGradationMode = mode;
        this->UpdateVertexColor();
    }

    void SetTextColor(ut::Color8 color){
        mTextColors[internal::TEXTCOLOR_START] = color;
        this->UpdateVertexColor();
    }

    void SetTextColor(ut::Color8 start,ut::Color8 end){
        mTextColors[internal::TEXTCOLOR_START] = start;
        mTextColors[internal::TEXTCOLOR_END  ] = end;
        this->UpdateVertexColor();
    }

    void SetDispStringBuffer(DispStringBuffer* buffer){
        mpDispStringBuffer = buffer;
    }

    void SetFont(const Font* pFont){
        mpFont = pFont; 
    }

    void SetScale(f32 hScale,f32 vScale){
        mScale.x = hScale;
        mScale.y = vScale;
    }

    void SetCursor(f32 x,f32 y){
        mCursorPos.x = x;
        mCursorPos.y = y;
    }

    void SetCursor(f32 x,f32 y,f32 z){
        mCursorPos.x = x;
        mCursorPos.y = y;
        mCursorPos.z = z;
    }
    void SetCursorX(f32 x){ mCursorPos.x = x; }
    void SetCursorY(f32 y){ mCursorPos.y = y; }
    void SetCursorZ(f32 z){ mCursorPos.z = z; }

    void SetTextWriterResource(TextWriterResource* pTextWriterResource){
        mpTextWriterResource = pTextWriterResource;
    }


    bool IsWidthFixed() const { return mIsWidthFixed; }
    f32 GetFixedWidth() const { return mFixedWidth; }
    const Font* GetFont() const{ return mpFont; }
    f32 GetScaleH() const { return mScale.x; }
    f32 GetScaleV() const { return mScale.y; }

    f32 GetCursorX() const { return mCursorPos.x; }
    f32 GetCursorY() const { return mCursorPos.y; }
    f32 GetCursorZ() const { return mCursorPos.z; }

    static const GLushort sVertexIndexs[];

    ColorMapping mColorMapping;
    ut::Color8 mVertexColors[internal::TRIFAN_VTX_MAX];
    ut::Color8 mTextColors[internal::TEXTCOLOR_MAX];
    GradationMode mGradationMode;
    CharScale mScale;
    CursorPos mCursorPos;
    f32 mFixedWidth;
    const Font* mpFont;
    TextWriterResource* mpTextWriterResource;
    DispStringBuffer* mpDispStringBuffer;
    bool mIsWidthFixed;
    u8 mAlpha;
};

struct DispStringBuffer;

}
}