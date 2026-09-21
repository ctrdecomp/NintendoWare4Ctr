// Filename: lyt_TextureContainer.cpp
//
// Project: NintendoWare4Ctr

#include <nw/lyt/lyt_Layout.h>
#include <nw/lyt/lyt_TextureContainer.h>
#include <nw/lyt/lyt_Util.h>

namespace nw{
namespace lyt{

TextureRefLink::TextureRefLink()
{
    m_Name[0] = '\0';
}

TextureRefLink::~TextureRefLink()
{
    lyt::DeleteTexture(this->m_TexInfo);
}

void TextureRefLink::Set(const char* name,const TextureInfo& texInfo)
{
    ut::strcpy(this->m_Name, sizeof(m_Name), name);
    m_TexInfo = texInfo;
}

/* Texture Container */

TextureContainer::~TextureContainer()
{
    this->Finalize();
}

void TextureContainer::Finalize()
{
    while (!this->empty())
    {
        TextureRefLink* pLink = &this->front();
        this->erase(pLink);
        Layout::DeleteObj(pLink);
    }
}

const TextureInfo TextureContainer::FindTextureByName(const char* name)
{
    for (Iterator it = this->GetBeginIter(); it != this->GetEndIter(); ++it)
    {
        if (0 == std::strcmp(name, it->GetResourceName()))
        {
            return it->GetTextureInfo();
        }
    }

    return TextureInfo();
}

const TextureInfo TextureContainer::FindTextureByKey(TextureKey key)
{
    TextureRefLink* pLink = (TextureRefLink*)(key);

    for (Iterator it = this->GetBeginIter(); it != this->GetEndIter(); ++it)
    {
        if (&(*it) == pLink)
        {
            return it->GetTextureInfo();
        }
    }

    return TextureInfo();
}

TextureKey TextureContainer::RegistTexture(const char* name, const TextureInfo& textureInfo)
{
    TextureRefLink* pLink = Layout::NewObj<TextureRefLink>();
    if (pLink == NULL)
    {
        return NULL;
    }

    pLink->Set(name, textureInfo);
    this->push_back(pLink);

    return (TextureKey) pLink;
}

void TextureContainer::UnregistTexture(TextureKey key)
{
    TextureRefLink* pLink = (TextureRefLink*)(key);
    this->erase(pLink);
    Layout::DeleteObj(pLink);
}

}
}