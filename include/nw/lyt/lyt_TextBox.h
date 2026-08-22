#pragma once

#include <nw/ut/ut_Color.h>
#include <nw/ut/ut_RuntimeTypeInfo.h>
#include <nw/font/font_WideTextWriter.h>
#include <nw/font/font_DispStringBuffer.h>
#include <nw/lyt/lyt_Pane.h>

namespace nw {
namespace font {

class Font;

template <typename CharType>
class TagProcessorBase;

template <typename CharType>
class TextWriterBase;

}
namespace lyt {

class DrawInfo;
struct ResBlockSet;

class TextBox : public Pane{
    typedef Pane Base;

public:
    NW_UT_RUNTIME_TYPEINFO;

    typedef font::TagProcessorBase<wchar_t> TagProcessor;

    explicit TextBox(u16 allocStrLen = 0);
    TextBox(u16 allocStrLen, const wchar_t* str, const font::Font* pFont);
    TextBox(u16 allocStrLen, const wchar_t* str, u16 strLen, const font::Font* pFont);
    TextBox(const res::TextBox* pBlock, const ResBlockSet& resBlockSet);
    virtual ~TextBox();

    // String
    const wchar_t* GetString() const { return mpTextBuf; }
    const wchar_t* GetStringBuffer() const { return mpTextBuf; }
    u16 GetStringLength() const { return mTextLen; }
    u16 GetStringBufferLength() const;

    virtual void AllocStringBuffer(u16 minLen);
    virtual void FreeStringBuffer();
    virtual u16  SetString(const wchar_t* str, u16 dstIdx = 0);
    virtual u16  SetString(const wchar_t* str, u16 dstIdx, u16 strLen);

    // Color
    const ut::Color8 GetTextColor(u32 type) const{
        return mTextColors[type];
    }
    void SetTextColor(u32 type, ut::Color8 value){
        this->mTextColors[type] = value;
    }
    void SetTextColor(ut::Color8 top, ut::Color8 bottom){
        this->mTextColors[TEXTCOLOR_TOP]    = top;
        this->mTextColors[TEXTCOLOR_BOTTOM] = bottom;
    }

    // Font
    const font::Font* GetFont() const;
    void SetFont(const font::Font* pFont);

    const Size& GetFontSize() const { return mFontSize; }
    void SetFontSize(const Size& fontSize){
        if (UpdatePTDirty(!(this->mFontSize == fontSize)))
            this->mFontSize = fontSize;
    }

    f32  GetLineSpace() const { return mLineSpace; }
    void SetLineSpace(f32 space){
        if (UpdatePTDirty(this->mLineSpace != space))
            this->mLineSpace = space;
    }

    f32  GetCharSpace() const { return mCharSpace; }
    void SetCharSpace(f32 space){
        if (UpdatePTDirty(this->mCharSpace != space))
            this->mCharSpace = space;
    }

    HorizontalPosition GetTextPositionH() const { return internal::GetHorizontalPosition(this->mTextPosition); }
    void SetTextPositionH(HorizontalPosition val){
        if (UpdatePTDirty(GetTextPositionH() != val))
            internal::SetHorizontalPosition(&this->mTextPosition, u8(val));
    }

    VerticalPosition GetTextPositionV() const { return internal::GetVerticalPosition(this->mTextPosition); }
    void SetTextPositionV(VerticalPosition val){
        if (UpdatePTDirty(GetTextPositionV() != val))
            internal::SetVerticalPosition(&mTextPosition, u8(val));
    }

    TextAlignment GetTextAlignment() const { return static_cast<TextAlignment>(mBits.textAlignment); }
    void SetTextAlignment(TextAlignment val){
        if (UpdatePTDirty(GetTextAlignment() != val))
            mBits.textAlignment = val;
    }

    TagProcessor* GetTagProcessor() const { return mpTagProcessor; }
    void SetTagProcessor(TagProcessor* pTagProcessor){
        if (UpdatePTDirty(mpTagProcessor != pTagProcessor))
            mpTagProcessor = pTagProcessor;
    }

    virtual const ut::Color8 GetVtxColor(u32 idx) const;
    virtual void SetVtxColor(u32 idx, ut::Color8 value);
    virtual u8 GetVtxColorElement(u32 idx) const;
    virtual void SetVtxColorElement(u32 idx, u8 value);

    void UpdateDrawCharData(Drawer* pDrawer){
        if (mTextLen <= 0 || !mpFont) return;
        UpdateDrawCharDataImpl(pDrawer);
    }

    const ut::Rect GetTextDrawRect() const;

    using Base::GetMaterial;
    virtual u8        GetMaterialNum() const;
    virtual Material* GetMaterial(u32 idx) const;
    void SetMaterial(Material* pMaterial);

    font::DispStringBuffer* GetDispStringBuffer() const { return mpDispStringBuf; }
    void GetTextGlobalMtx(nw::math::MTX34* pMtx) const;

    virtual void MakeUniformDataSelf(DrawInfo* pDrawInfo, Drawer* pDrawer) const;
protected:
    virtual void DrawSelf(const DrawInfo& drawInfo);
    virtual void LoadMtx(const DrawInfo& drawInfo);

    void SetFontInfo(font::WideTextWriter* pWriter) const;
    void SetTextPos(font::WideTextWriter* pWriter) const;
    math::VEC2 AdjustTextPos(const Size& size, bool isCeil) const;
    void Init(u16 allocStrLen);
    void InitMaterial();
    u16  SetStringImpl(const wchar_t* str, u16 dstIdx, u32 strLen);

    bool UpdatePTDirty(bool isChanged){
        mBits.isPTDirty |= isChanged ? 1 : 0;
        return isChanged;
    }

    void UpdateDrawCharDataImpl(Drawer* pDrawer){
        if (mBits.isPTDirty || (!mpDispStringBuf->IsGeneratedCommand() && pDrawer))
            SetupDrawCharData(pDrawer);
    }

    void SetupDrawCharData(Drawer* pDrawer);
    void SetupTextWriter(font::WideTextWriter* pWriter);

private:
    wchar_t*                mpTextBuf;
    ut::Color8              mTextColors[TEXTCOLOR_MAX];
    const font::Font*       mpFont;
    Size                    mFontSize;
    f32                     mLineSpace;
    f32                     mCharSpace;
    TagProcessor*           mpTagProcessor;
    u16                     mTextBufBytes;
    u16                     mTextLen;
    u8                      mTextPosition;

    struct Bits{
        u8 textAlignment : 2;
        u8 isPTDirty     : 1;
    };
    Bits mBits;
    Material* mpMaterial;
    font::DispStringBuffer* mpDispStringBuf;
};

} // namespace lyt
} // namespace nw