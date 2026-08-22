#pragma once

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/res/gfx_ResTypeInfo.h>
#include <nw/gfx/res/gfx_ResVertex.h>
#include <nw/gfx/res/gfx_ResShape.h>

namespace nw {
namespace gfx {
namespace res {

struct ResParticleShapeData : public ResShapeData{
    nw::ut::ResS32 mVertexAttributesTableCount;
    nw::ut::Offset toVertexAttributesTable;
};

class ResParticleShape : public ResShape{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleShape) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('PSHP') };

    NW_RES_CTOR_INHERIT(ResParticleShape, ResShape)
    NW_RES_FIELD_CLASS_LIST_DECL(ResVertexAttribute, VertexAttributes)

    void    Setup();
};

}
}
}