// Filename: font_Font.cpp
//
// Project: NintendoWare4Ctr

#include <nw/font/font_Font.h>
#include <nw/ut/ut_MiddlewareString.h>

namespace nw {
namespace font {

Glyph::Glyph():   
    isSheetUpdated(false) {}

/* Font */

#if defined(NW_VERSION_USE_FNT1)
    Font::Font()
    {
        NW_PUT_MODULE_SYMBOL(fnt1);
    }
#elif defined(NW_VERSION_USE_FNT2)
    Font::Font():
        m_IsEnableKerning;(true),
        m_IsEnableExtraMargin;(true)
    {
        NW_PUT_MODULE_SYMBOL(fnt2);
    }
#endif

Font::~Font() {}

const CharStrmReader Font::GetCharStrmReader(wchar_t /* dummy */) const
{
    CharStrmReader::ReadNextCharFunc func = NULL;

    switch (GetCharacterCode())
    {
    case CHARACTER_CODE_UNICODE: 
        func = &CharStrmReader::ReadNextCharUTF16;  
        break;
    default:
        break;
    }
    return CharStrmReader(func);
}

}
}