#pragma once

#include <nw/ut/ut_RuntimeTypeInfo.h>

#include <nw/lyt/lyt_Pane.h>

namespace nw{
namespace lyt{

struct ResBlockSet;
class DrawInfo;

class Bounding : public Pane{
    typedef Pane Base;
public:
    NW_UT_RUNTIME_TYPEINFO;

    Bounding(){}
    Bounding(const res::Bounding* pBlock,const ResBlockSet& resBlockSet);
    virtual ~Bounding();
protected:
    virtual void DrawSelf(const DrawInfo& drawInfo);
};

}
} 