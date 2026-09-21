#pragma once

#include <nw/types.h>

#include <nw/ut/ut_LinkList.h>
#include <nw/lyt/lyt_ResourceAccessor.h>
#include <cstddef>

namespace nw{
namespace lyt{

class FontRefLink
{
public:
    static const int FONTNAMEBUF_MAX = 128;

    FontRefLink();
    ~FontRefLink();
    void Set(const char* name,font::Font* pFont,bool own);
    const char* GetFontName() const {return m_FontName;}
    font::Font* GetFont() const {return m_pFont;}
    
    ut::LinkListNode m_Link;
protected:
    char m_FontName[FONTNAMEBUF_MAX];
    font::Font* m_pFont;
    bool m_Own;
};

class FontContainer : public ut::LinkList<FontRefLink, offsetof(FontRefLink, m_Link)>
{
public:
    ~FontContainer();
    void Finalize();
    FontKey RegistFont(const char* name, font::Font* pFont, bool own);
    void UnregistFont(FontKey key);
    nw::font::Font* FindFontByName(const char* name);
};

}
}