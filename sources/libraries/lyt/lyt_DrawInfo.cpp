// Filename: lyt_DrawInfo.cpp
//
// Project: NintendoWare4Ctr

#include <nw/lyt/lyt_DrawInfo.h>
#include <nw/lyt/lyt_Types.h>
#include <nw/lyt/lyt_Common.h>
#include <nw/lyt/lyt_GraphicsResource.h>

namespace nw{
namespace lyt{

DrawInfo::DrawInfo():   
    mLocationAdjustScale(1.f, 1.f),
    mGlobalAlpha(1.f),
    mpGraphicsResource(0),
    mpLayout(0){

    std::memset(&this->mFlag, 0, sizeof(this->mFlag));

    MTX34Identity(&this->mViewMtx);
}

DrawInfo::~DrawInfo(){ }

void DrawInfo::SetProjectionMtx(const nw::math::MTX44& mtx){
    math::MTX44Copy(&this->mProjMtx, &mtx);
}

}
}