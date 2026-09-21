#pragma once

#include <nw/types.h>
#include <nw/font/font_CharStrmReader.h>
#include <nw/font/font_ResourceFormat.h>
#include <nw/version.h>
#include <nn/module.h>

#include <climits>

namespace nw{
namespace font{
    class Font;
    typedef ushort TexFmt;

    union TextureSize
    {
        struct
        {
            u16 height;
            u16 width;
        } HW;
        u32 size;
    };

namespace internal{
    void LoadTexture(u16 texWidth,u16 texHeight,TexFmt texFormat,const void* pImage,bool isSmallLinearFilter,bool isLargeLinearFilter);
    class TextureObject
    {
    public:
        TextureObject();
        void Set(u32 name,const Font* pFont,const void* pImage,TexFmt format,u16 width,u16 height);
        u32 GetName() const {return this->m_Name;}
        void SetName(u32 texName) {this->m_Name = texName;}
        const Font* GetFont() const {return this->mpFont;}
        uptr GetImage() const {return this->mpImage;}
        u8 GetFormat() const {return this->m_Format;}
        const TextureSize GetSize() const {return this->m_Size;}
        u32 GetWrapFilter() const;

    private:
        u32 m_Name;
        const Font* mpFont;
        uptr mpImage;
        TextureSize m_Size;
        u8 m_Format;
    };
}

typedef ushort CharCode;

enum Type
{
    TYPE_NULL,
    TYPE_ROM,
    TYPE_RESOURCE,
    TYPE_PAIR
};

struct Glyph
{
    const void* pTexture;
    CharWidths widths;
    u8 height;
    u16 texWidth;
    u16 texHeight;
    u16 cellX;
    u16 cellY;
    u8 isSheetUpdated;
    s8 rev;
    TexFmt texFormat;
    const internal::TextureObject* pTextureObject;
    Glyph();
};

class Font
{
public:
    enum Type
    {
        TYPE_NULL,
        TYPE_ROM,
        TYPE_RESOURCE,
        TYPE_PAIR
    };
    static const CharCode INVALID_CHARACTER_CODE = INVALID_CHAR_CODE;
#if NW_VERSION_MAJOR > 3
    Font():
        m_IsEnableKerning(true),
        m_IsEnableExtraMargin(true)
        {
        NN_MAKE_MODULE(moduleInfo, "NINTENDO", NW_CURRENT_VERSION_NUMBER "_fnt1");
        NN_REFER_MODULE(moduleInfo);
    }
#else
    Font()
    {
    }
#endif

    virtual ~Font();
    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual int GetAscent() const = 0;
    virtual int GetDescent() const = 0;
    virtual int GetMaxCharWidth() const = 0;
    virtual Type GetType() const = 0;
    virtual TexFmt GetTextureFormat() const = 0;
    virtual int GetLineFeed() const = 0;
    virtual const CharWidths GetDefaultCharWidths() const = 0;
    virtual void SetLineFeed(int linefeed) = 0;
    virtual void SetDefaultCharWidths(const CharWidths& widths) = 0;
    virtual bool SetAlternateChar(CharCode code) = 0;
    virtual int GetCharWidth(CharCode code) const = 0;
    virtual const CharWidths GetCharWidths(CharCode code) const = 0;
    virtual void GetGlyph(Glyph* pGlyph,CharCode code) const = 0;
    virtual bool HasGlyph(CharCode code) const = 0;
    virtual CharacterCode GetCharacterCode() const = 0;
    const CharStrmReader GetCharStrmReader(char dummy) const;
    const CharStrmReader GetCharStrmReader(wchar_t dummy) const;
    virtual int GetBaselinePos() const = 0;
    virtual int GetCellHeight() const = 0;
    virtual int GetCellWidth() const = 0;
    virtual void EnableLinearFilter(bool atSmall, bool atLarge) = 0;
    virtual bool IsLinearFilterEnableAtSmall() const = 0;
    virtual bool IsLinearFilterEnableAtLarge() const = 0;
    virtual u32 GetTextureWrapFilterValue() const = 0;

private:
#if NW_VERSION_MAJOR > 3
    bool m_IsEnableKerning;
    bool m_IsEnableExtraMargin;
#endif
};

}
}