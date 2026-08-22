#pragma once

#include <nw/lyt/lyt_Arc.h>
#include <nw/lyt/lyt_Resources.h>
#include <nw/lyt/lyt_Types.h>

namespace nw{
namespace lyt{

class TexResource{
public:
    TexResource(): 
      mpTop(NULL), 
      mpImage(NULL), 
      mpImageSize(NULL)
    {}
    TexResource(void* pTexRes, u32 size){
        this->Set(pTexRes, size);
    }
    explicit TexResource(ARCFileInfo& fileInfo){
        this->Set(fileInfo);
    }

    bool IsValid() const{return mpTop != NULL;}
    bool Set(void* pTexRes, u32 size);
    bool Set(ARCFileInfo& fileInfo){
        return this->Set(
            ARCGetStartAddrInMem(&fileInfo),
            ARCGetLength(&fileInfo));
    }

    const void* GetImageAddress() const{return mpTop;}
    void* GetImageAddress(){return mpTop;}
    u32 GetImageSize() const{return mpImageSize->imageSize;}
    lyt::TexFormat GetFormat() const{return static_cast<lyt::TexFormat>(mpImage->format);}

    bool Is4bitFormat() const{
        return mpImage->format == lyt::TEXFORMAT_L4
            || mpImage->format == lyt::TEXFORMAT_A4;
    }
    u16 GetWidth() const{return mpImage->width;}
    u16 GetHeight() const{return mpImage->height;}
    MemArea GetImageArea() const{
        return static_cast<lyt::MemArea>(internal::GetBits(this->mpImage->flag, lyt::IMAGEFLAG_AREA_POS,lyt::IMAGEFLAG_AREA_LEN));
    }
    void SetImageArea(lyt::MemArea area){
        mpImage->flag = internal::SetBits(this->mpImage->flag, lyt::IMAGEFLAG_AREA_POS, lyt::IMAGEFLAG_AREA_LEN,static_cast<u8>(area));
    }

    res::Image* GetResImage(){return mpImage;}
    const res::Image* GetResImage() const{return mpImage;}

protected:
    void* mpTop;
    res::Image* mpImage;
    res::ImageSize* mpImageSize;
};

}
}