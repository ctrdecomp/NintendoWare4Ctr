// Filename: lyt_Bounding.cpp
//
// Project: NintendoWare4Ctr

#include <nw/lyt/lyt_Bounding.h>
#include <nw/lyt/lyt_Common.h>
#include <nw/lyt/lyt_DrawInfo.h>
#include <nw/lyt/lyt_GraphicsResource.h>
#include <nw/lyt/lyt_ResourceAccessor.h>

namespace nw{
namespace lyt{

using namespace math;

NW_UT_RUNTIME_TYPEINFO_DEFINITION(Bounding, Bounding::Base);

Bounding::Bounding(const res::Bounding* pBlock,const ResBlockSet& resBlockSet): 
    Base(pBlock)
    {
}

Bounding::~Bounding()
{
}

void Bounding::DrawSelf(const DrawInfo& drawInfo)
{  
}

}
}