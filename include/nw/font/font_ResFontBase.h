#pragma once

#include <nw/types.h>
#include <nw/font/font_Font.h>
#include <nw/font/font_Types.h>

namespace nw{
namespace font{
namespace internal{
    inline u32 GetCellsInASheet(const FontTextureGlyph& tg)
    {
        return static_cast<u32>(tg.sheetRow * tg.sheetLine);
    }

}
class ResFontBase : public Font
{
protected:
    typedef ushort GlyphIndex;
    static const GlyphIndex GLYPH_INDEX_NOT_FOUND = INVALID_GLYPH_INDEX;

    void* mpResource;
    FontInformation* mpFontInfo;
    internal::TextureObject* mpTexObjs;
    u32 m_WrapFilter;
    mutable CharCode m_LastCharCode;
    mutable GlyphIndex m_LastGlyphIndex;

    bool IsManaging(const void* ptr) const { return mpResource == ptr; }

    internal::TextureObject* GetTextureObjectsBufferPtr() {return mpTexObjs;}
    void SetTextureObjectsBufferPtr(void* buffer) {mpTexObjs = static_cast<internal::TextureObject*>(buffer);}

    const internal::TextureObject*GetTextureObjectsBufferPtr() const {return mpTexObjs;}
    const internal::TextureObject* GetTextureObject(int index) const {return &GetTextureObjectsBufferPtr()[index];}
public:
    ResFontBase();
    virtual ~ResFontBase();
    virtual int GetWidth() const;
    virtual int GetHeight() const;
    virtual int GetAscent() const;
    virtual int GetDescent() const;
    virtual int GetMaxCharWidth() const;
    virtual Type GetType() const;
    virtual TexFmt GetTextureFormat() const;
    virtual int GetLineFeed() const;
    virtual const CharWidths GetDefaultCharWidths() const;
    virtual void SetLineFeed(int linefeed);
    virtual void SetDefaultCharWidths(const CharWidths& widths);
    virtual bool SetAlternateChar(CharCode code);
    virtual int GetCharWidth(CharCode code) const;
    virtual const CharWidths GetCharWidths(CharCode code) const;
    virtual void GetGlyph(Glyph* pGlyph,CharCode code) const;
    virtual bool HasGlyph(CharCode c) const;
    virtual CharacterCode GetCharacterCode() const;
    virtual int GetBaselinePos() const;
    virtual int GetCellHeight() const;
    virtual int GetCellWidth() const;
    virtual void EnableLinearFilter(bool atSmall,bool atLarge);
    virtual bool IsLinearFilterEnableAtSmall() const;
    virtual bool IsLinearFilterEnableAtLarge() const;
    virtual u32 GetTextureWrapFilterValue() const;
    virtual int GetActiveSheetNum() const;

    GlyphIndex FindGlyphIndex(CharCode code) const;
    GlyphIndex FindGlyphIndex(const FontCodeMap* pMap,CharCode c) const;

    void GenTextureNames();
    void SetResourceBuffer(void* pUserBuffer,FontInformation* pFontInfo);
    static void SetGlyphMember(Glyph* glyph,GlyphIndex index,const FontTextureGlyph& tg);
    void* RemoveResourceBuffer();
    GlyphIndex GetGlyphIndex(CharCode c) const;
    void GetGlyphFromIndex(Glyph* glyph,GlyphIndex index) const;
    void DeleteTextureNames();

    const CharWidths& GetCharWidthsFromIndex(GlyphIndex index) const;
    const CharWidths& GetCharWidthsFromIndex(const FontWidth* pWidth,GlyphIndex index) const;
};

}
}