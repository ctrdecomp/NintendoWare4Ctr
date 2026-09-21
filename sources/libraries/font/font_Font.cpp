// Filename: font_Font.cpp
//
// Project: NintendoWare4Ctr

#include <nw/font/font_Font.h>

namespace nw {
namespace font {

Glyph::Glyph():   
    isSheetUpdated(false) {}

/* Font */

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