#pragma once

#include <nw/types.h>

#include <nw/ut/ut_LinkList.h>
#include <nw/lyt/lyt_ResourceAccessor.h>
#include <cstddef>

namespace nw{
namespace lyt{

class FontRefLink{
public:
    static const int FONTNAMEBUF_MAX = 128;

    FontRefLink();
    ~FontRefLink();
    void Set(const char* name,font::Font* pFont,bool own);
    const char* GetFontName() const{return mFontName;}
    font::Font* GetFont() const{return mpFont;}
    
    ut::LinkListNode mLink;
protected:
    char mFontName[FONTNAMEBUF_MAX];
    font::Font* mpFont;
    bool mOwn;
};

class FontContainer : public ut::LinkList<FontRefLink, offsetof(FontRefLink, mLink)>{
public:
    ~FontContainer();
    void Finalize();
    FontKey RegistFont(const char* name, font::Font* pFont, bool own);
    void UnregistFont(FontKey key);
    nw::font::Font* FindFontByName(const char* name);
};

}
}