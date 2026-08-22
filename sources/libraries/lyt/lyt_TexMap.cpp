// Filename: lyt_TexMap.cpp
//
// Project: NintendoWare4Ctr

#include <nn/gr.h>
#include <nw/lyt/lyt_Types.h>
#include <nw/lyt/lyt_TexMap.h>
#include <nw/lyt/lyt_Common.h>
#include <nw/lyt/lyt_DrawerCommand.h>

namespace nw{
namespace lyt{

TexMap::TexMap(): 
    mTexObject(0), 
    mPhysicalAddress(NULL), 
    mWidth(0), 
    mHeight(0), 
    mRealWidth(0), 
    mRealHeight(0){
    this->mBits.format = TEXFORMAT_MAX;
    this->mBits.wrapS = TEXWRAP_CLAMP;
    this->mBits.wrapT = TEXWRAP_CLAMP;
    this->mBits.minFilter = TEXFILTER_LINEAR;
    this->mBits.magFilter = TEXFILTER_LINEAR;
    this->ResetU32Info();
}

TexMap::TexMap(const TextureInfo& textureInfo){
    this->mBits.wrapS = TEXWRAP_CLAMP;
    this->mBits.wrapT = TEXWRAP_CLAMP;
    this->mBits.minFilter = TEXFILTER_LINEAR;
    this->mBits.magFilter = TEXFILTER_LINEAR;

    this->Set(textureInfo);
}


void TexMap::ResetU32Info(){
    switch (this->GetFormat()){
      case TEXFORMAT_RGBA8  : mu32Format = PICA_DATA_TEXTURE_FORMAT_RGBA_UNSIGNED_BYTE;  break;
      case TEXFORMAT_RGB8   : mu32Format = PICA_DATA_TEXTURE_FORMAT_RGB_UNSIGNED_BYTE;   break;
      case TEXFORMAT_RGB5A1 : mu32Format = PICA_DATA_TEXTURE_FORMAT_RGBA_UNSIGNED_SHORT_5_5_5_1; break;
      case TEXFORMAT_RGB565 : mu32Format = PICA_DATA_TEXTURE_FORMAT_RGB_UNSIGNED_SHORT_5_6_5; break;
      case TEXFORMAT_RGBA4  : mu32Format = PICA_DATA_TEXTURE_FORMAT_RGBA_UNSIGNED_SHORT_4_4_4_4;  break;
      case TEXFORMAT_LA8    : mu32Format = PICA_DATA_TEXTURE_FORMAT_LUMINANCE_ALPHA_UNSIGNED_BYTE;    break;
      case TEXFORMAT_HILO8  : mu32Format = PICA_DATA_TEXTURE_FORMAT_HILO8_DMP_UNSIGNED_BYTE_DMP;  break;
      case TEXFORMAT_L8     : mu32Format = PICA_DATA_TEXTURE_FORMAT_LUMINANCE_UNSIGNED_BYTE;     break;
      case TEXFORMAT_A8     : mu32Format = PICA_DATA_TEXTURE_FORMAT_ALPHA_UNSIGNED_BYTE;     break;
      case TEXFORMAT_LA4    : mu32Format = PICA_DATA_TEXTURE_FORMAT_LUMINANCE_ALPHA_UNSIGNED_BYTE_4_4_DMP;    break;
      case TEXFORMAT_L4     : mu32Format = PICA_DATA_TEXTURE_FORMAT_LUMINANCE_UNSIGNED_4BITS_DMP;     break;
      case TEXFORMAT_A4     : mu32Format = PICA_DATA_TEXTURE_FORMAT_ALPHA_UNSIGNED_4BITS_DMP;     break;
      case TEXFORMAT_ETC1   : mu32Format = PICA_DATA_TEXTURE_FORMAT_ETC1_RGB8_NATIVE_DMP;   break;
      case TEXFORMAT_ETC1A4 : mu32Format = PICA_DATA_TEXTURE_FORMAT_ETC1_ALPHA_RGB8_A4_NATIVE_DMP; break;
    }

    /*mu32WidthHeight = PICA_CMD_DATA_TEXTURE_SIZE(GetRealWidth(), GetRealHeight());

    const u32 minFilter[] ={
        PICA_DATA_TEXTURE_MIN_FILTER_NEAREST,
        PICA_DATA_TEXTURE_MIN_FILTER_LINEAR,
    };

    const u32 wrap[] ={
        PICA_DATA_TEXTURE_WRAP_CLAMP_TO_EDGE,
        PICA_DATA_TEXTURE_WRAP_REPEAT,
        PICA_DATA_TEXTURE_WRAP_MIRRORED_REPEAT,
    };

    mu32WrapFilter = PICA_CMD_DATA_TEXTURE_WRAP_FILTER(GetMagFilter(),
    minFilter[GetMinFilter()],
    this->mu32Format,wrap[GetWrapModeT()],
    wrap[GetWrapModeS()],
    0,
    0);*/
}

}
}