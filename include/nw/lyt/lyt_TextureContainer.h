#pragma once

#include <nw/ut/ut_LinkList.h>
#include <nw/lyt/lyt_Types.h>

namespace nw{
namespace lyt{

class TextureRefLink
{
public:
    static const int TEXIMAGE_FILENAME_MAX = 127;

    ut::LinkListNode m_Link;

    TextureRefLink();
    ~TextureRefLink();
    void Set(const char* name,const TextureInfo& texInfo);

    const char* GetResourceName() const { return m_Name; }
    const TextureInfo GetTextureInfo() {return m_TexInfo; }

protected:
    char m_Name[TEXIMAGE_FILENAME_MAX + 1];
    TextureInfo m_TexInfo;
};

class TextureContainer : public ut::LinkList<TextureRefLink, offsetof(TextureRefLink, m_Link)>
{
public:
    ~TextureContainer();
    void Finalize();
    TextureKey RegistTexture(const char* name, const TextureInfo& textureInfo);
    void UnregistTexture(TextureKey key);
    const TextureInfo FindTextureByName(const char* name);
    const TextureInfo FindTextureByKey(TextureKey key);
};

}
}