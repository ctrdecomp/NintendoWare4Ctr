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
    m_LocationAdjustScale(1.f, 1.f),
    m_GlobalAlpha(1.f),
    m_pGraphicsResource(0),
    m_pLayout(0)
{
    std::memset(&this->m_Flag, 0, sizeof(this->m_Flag));

    MTX34Identity(&this->m_ViewMtx);
}

DrawInfo::~DrawInfo()
{   
}

void DrawInfo::SetProjectionMtx(const nw::math::MTX44& mtx)
{
    math::MTX44Copy(&this->m_ProjMtx, &mtx);
}

}
}