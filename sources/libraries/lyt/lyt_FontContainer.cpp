// Filename: lyt_FontContainer.cpp
//
// Project: NintendoWare4Ctr

#include <nw/font/font_ResFont.h>
#include <nw/lyt/lyt_FontContainer.h>
#include <nw/lyt/lyt_Layout.h>

#define stricmp std::strcasecmp

namespace nw{
namespace lyt{

/* FontRefLink */

FontRefLink::FontRefLink():   
    m_pFont(0),
    m_Own(false)
{ 
}

FontRefLink::~FontRefLink()
{
    if (m_Own)
    {
        font::ResFont* pResFont = (font::ResFont*)(this->m_pFont);
        if (pResFont != NULL)
        {
            void* drawBuffer = pResFont->GetDrawBuffer();
            if (drawBuffer != NULL)
            {
                pResFont->SetDrawBuffer(NULL);
                Layout::FreeMemory(drawBuffer);
            }
        }
        Layout::DeleteObj(this->m_pFont);
        m_pFont = NULL;
    }
}

void FontRefLink::Set(const char* name,font::Font* pFont,bool own)
{
    ut::strcpy(this->m_FontName, sizeof(this->m_FontName), name);
    this->m_pFont = pFont;
    this->m_Own = own;
}

/* FontContainer */

FontContainer::~FontContainer()
{
    this->Finalize();
}

void FontContainer::Finalize()
{
    while (!this->empty())
    {
        FontRefLink* pLink = &this->front();
        this->erase(pLink);
        Layout::DeleteObj(pLink);
    }
}

font::Font* FontContainer::FindFontByName(const char* name)
{
    for (Iterator it = this->GetBeginIter(); it != this->GetEndIter(); ++it)
    {
        if (0 == std::strcmp(name, it->GetFontName()))
        {
            return it->GetFont();
        }
    }

    return NULL;
}

FontKey FontContainer::RegistFont(const char* name, font::Font* pFont, bool own)
{
    FontRefLink* pLink = Layout::NewObj<FontRefLink>();

    if (pLink == NULL)
    {
        return NULL;
    }

    pLink->Set(name, pFont, own);
    this->push_back(pLink);
    return reinterpret_cast<FontKey>(pLink);
}

void FontContainer::UnregistFont(FontKey key)
{
    FontRefLink* pLink = (FontRefLink*)(key);
    this->erase(pLink);
    Layout::DeleteObj(pLink);
}

}
}