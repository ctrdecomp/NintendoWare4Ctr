// Filename: lyt_ResourceAccessor.cpp
//
// Project: NintendoWare4Ctr

#include <nw/font/font_ResFont.h>
#include <nw/lyt/lyt_Layout.h>
#include <nw/lyt/lyt_ResourceAccessor.h>
#include <nw/lyt/lyt_Resources.h>
#include <nw/lyt/lyt_Util.h>

namespace nw{
namespace lyt{

ResourceAccessor::~ResourceAccessor(){}

ResourceAccessor::ResourceAccessor(){}

const TextureInfo ResourceAccessor::LoadTexture(const char *name){
    u32 size = 0;
    void* pTexRes = this->GetResource(res::RESOURCETYPE_TEXTURE, name, &size);
    if (!pTexRes || size == 0){
        return TextureInfo();
    }

    TextureInfo texInfo = lyt::LoadTexture(pTexRes, size);
    if (!texInfo.IsValid()){
        return TextureInfo();
    }

    return texInfo;
}

font::Font* ResourceAccessor::LoadFont(const char *name){
    u32 size = 0;
    void* pFontRes = this->GetResource(res::RESOURCETYPE_FONT, name, &size);
    if (!pFontRes || size == 0){
        return NULL;
    }

    font::ResFont* pResFont = Layout::NewObj<font::ResFont>();
    if (pResFont == NULL){
        return NULL;
    }

    bool bSuccess = pResFont->SetResource(pFontRes);
    if (!bSuccess){
        Layout::DeleteObj(pResFont);
        return NULL;
    }

    {
        const u32 drawBufferSize = font::ResFont::GetDrawBufferSize(pFontRes);
        void* drawBuffer = Layout::AllocMemory(drawBufferSize, 4);
        pResFont->SetDrawBuffer(drawBuffer);
    }

    return pResFont;
}

}
}