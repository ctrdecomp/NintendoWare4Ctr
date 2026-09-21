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

class TextBox : public Pane
{
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
    const wchar_t* GetString() const { return m_pTextBuf; }
    const wchar_t* GetStringBuffer() const { return m_pTextBuf; }
    u16 GetStringLength() const { return m_TextLen; }
    u16 GetStringBufferLength() const;

    virtual void AllocStringBuffer(u16 minLen);
    virtual void FreeStringBuffer();
    virtual u16  SetString(const wchar_t* str, u16 dstIdx = 0);
    virtual u16  SetString(const wchar_t* str, u16 dstIdx, u16 strLen);

    // Color
    const ut::Color8 GetTextColor(u32 type) const
    {
        return m_TextColors[type];
    }
    void SetTextColor(u32 type, ut::Color8 value)
    {
        this->m_TextColors[type] = value;
    }
    void SetTextColor(ut::Color8 top, ut::Color8 bottom)
    {
        this->m_TextColors[TEXTCOLOR_TOP]    = top;
        this->m_TextColors[TEXTCOLOR_BOTTOM] = bottom;
    }

    // Font
    const font::Font* GetFont() const;
    void SetFont(const font::Font* pFont);

    const Size& GetFontSize() const { return m_FontSize; }
    void SetFontSize(const Size& fontSize)
    {
        if (UpdatePTDirty(!(this->m_FontSize == fontSize)))
            this->m_FontSize = fontSize;
    }

    f32  GetLineSpace() const { return m_LineSpace; }
    void SetLineSpace(f32 space)
    {
        if (UpdatePTDirty(this->m_LineSpace != space))
            this->m_LineSpace = space;
    }

    f32  GetCharSpace() const { return m_CharSpace; }
    void SetCharSpace(f32 space)
    {
        if (UpdatePTDirty(this->m_CharSpace != space))
            this->m_CharSpace = space;
    }

    HorizontalPosition GetTextPositionH() const { return internal::GetHorizontalPosition(this->m_TextPosition); }
    void SetTextPositionH(HorizontalPosition val)
    {
        if (UpdatePTDirty(GetTextPositionH() != val))
            internal::SetHorizontalPosition(&this->m_TextPosition, u8(val));
    }

    VerticalPosition GetTextPositionV() const { return internal::GetVerticalPosition(this->m_TextPosition); }
    void SetTextPositionV(VerticalPosition val)
    {
        if (UpdatePTDirty(GetTextPositionV() != val))
            internal::SetVerticalPosition(&m_TextPosition, u8(val));
    }

    TextAlignment GetTextAlignment() const { return static_cast<TextAlignment>(m_Bits.textAlignment); }
    void SetTextAlignment(TextAlignment val)
    {
        if (UpdatePTDirty(GetTextAlignment() != val))
            m_Bits.textAlignment = val;
    }

    TagProcessor* GetTagProcessor() const { return m_pTagProcessor; }
    void SetTagProcessor(TagProcessor* pTagProcessor)
    {
        if (UpdatePTDirty(m_pTagProcessor != pTagProcessor))
            m_pTagProcessor = pTagProcessor;
    }

    virtual const ut::Color8 GetVtxColor(u32 idx) const;
    virtual void SetVtxColor(u32 idx, ut::Color8 value);
    virtual u8 GetVtxColorElement(u32 idx) const;
    virtual void SetVtxColorElement(u32 idx, u8 value);

    void UpdateDrawCharData(Drawer* pDrawer)
    {
        if (m_TextLen <= 0 || !m_pFont) return;
        UpdateDrawCharDataImpl(pDrawer);
    }

    const ut::Rect GetTextDrawRect() const;

    using Base::GetMaterial;
    virtual u8        GetMaterialNum() const;
    virtual Material* GetMaterial(u32 idx) const;
    void SetMaterial(Material* pMaterial);

    font::DispStringBuffer* GetDispStringBuffer() const { return m_pDispStringBuf; }
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

    bool UpdatePTDirty(bool isChanged)
    {
        m_Bits.isPTDirty |= isChanged ? 1 : 0;
        return isChanged;
    }

    void UpdateDrawCharDataImpl(Drawer* pDrawer)
    {
        if (m_Bits.isPTDirty || (!m_pDispStringBuf->IsGeneratedCommand() && pDrawer))
            SetupDrawCharData(pDrawer);
    }

    void SetupDrawCharData(Drawer* pDrawer);
    void SetupTextWriter(font::WideTextWriter* pWriter);

private:
    wchar_t*                m_pTextBuf;
    ut::Color8              m_TextColors[TEXTCOLOR_MAX];
    const font::Font*       m_pFont;
    Size                    m_FontSize;
    f32                     m_LineSpace;
    f32                     m_CharSpace;
    TagProcessor*           m_pTagProcessor;
    u16                     m_TextBufBytes;
    u16                     m_TextLen;
    u8                      m_TextPosition;

    struct Bits
    {
        u8 textAlignment : 2;
        u8 isPTDirty     : 1;
    };
    Bits m_Bits;
    Material* m_pMaterial;
    font::DispStringBuffer* m_pDispStringBuf;
};

} // namespace lyt
} // namespace nw