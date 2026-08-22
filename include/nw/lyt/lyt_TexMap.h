#pragma once

#include <nw/types.h>
#include <nw/lyt/lyt_Types.h>
#include <nw/lyt/lyt_Drawer.h>

namespace nw{
namespace lyt{
namespace res{
    struct Image;
}

class TextureInfo;

class TexMap{
public:
    TexMap();
    TexMap(const TextureInfo& textureInfo);
    void ResetU32Info();

    /* Inlines */

    void Set(const TextureInfo& src){
        this->mTexObject = src.GetTextureObject();
        this->mPhysicalAddress = src.GetPhysicalAddress();
        this->mWidth = src.GetSize().width;
        this->mHeight = src.GetSize().height;
        this->mRealWidth = src.GetRealSize().width;
        this->mRealHeight = src.GetRealSize().height;
        this->mBits.format = src.GetFormat();
        this->ResetU32Info();
    }
    void SetWrapMode(TexWrap wrapS, TexWrap wrapT){
        mBits.wrapS = wrapS;
        mBits.wrapT = wrapT;

        this->ResetU32Info();
    }

    void SetFilter(TexFilter minFlt, TexFilter magFlt){
        mBits.minFilter = minFlt;
        mBits.magFilter = magFlt;

        this->ResetU32Info();
    }
    const TexSize GetSize() const{
        return TexSize(this->mWidth, this->mHeight);
    }
    ushort GetHeight() const{ return mHeight; }
    ushort GetWidth() const{return mWidth; }
    ushort GetRealWidth() const{return mRealWidth;}
    ushort GetRealHeight() const{return mRealHeight;}

    TexFormat GetFormat() const{return TexFormat(mBits.format);}

    TexFilter GetMinFilter() const{return TexFilter(mBits.minFilter);}
    TexFilter GetMagFilter() const{return TexFilter(mBits.magFilter);}

    TexWrap GetWrapModeS() const{return TexWrap(mBits.wrapS);}
    TexWrap GetWrapModeT() const{return TexWrap(mBits.wrapT);}

    u32 GetTextureObject() const{ return mTexObject; }

    uptr GetPhysicalAddress() const { return mPhysicalAddress; }

    u32 GetU32WrapFilter() const{ return mu32WrapFilter; }
    u32 GetU32Format() const { return mu32Format; }
    u32 GetU32WidthHeight() const{ return mu32WidthHeight; }

protected:
    struct Bits{
        u32 wrapS: 2;
        u32 wrapT: 2;
        u32 minFilter: 3;
        u32 magFilter: 1;
        u32 format: 4;
    };

    u32 mTexObject;
    uptr mPhysicalAddress;
    u16 mWidth;
    u16 mHeight;
    u16 mRealWidth;
    u16 mRealHeight;
    Bits mBits;
    u32 mu32WrapFilter;
    u32 mu32WidthHeight;
    u32 mu32Format;
};

}
}