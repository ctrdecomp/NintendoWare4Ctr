#pragma once

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/res/gfx_ResTypeInfo.h>
#include <nw/gfx/res/gfx_ResVertex.h>
#include <nw/gfx/res/gfx_ResShape.h>

namespace nw {
namespace gfx {
namespace res {

struct ResParticleShapeData : public ResShapeData
{
    nw::ut::ResS32 m_VertexAttributesTableCount;
    nw::ut::Offset toVertexAttributesTable;
};

class ResParticleShape : public ResShape
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleShape) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('PSHP') };

    NW_RES_CTOR_INHERIT(ResParticleShape, ResShape)

    s32 GetVertexAttributesCount() const
        { return ref().m_VertexAttributesTableCount; }
    ResVertexAttribute GetVertexAttributes(int idx)
        {
            nw::ut::Offset* tbl_ptr = static_cast<nw::ut::Offset*>(ref().toVertexAttributesTable.to_table_ptr());
            return ResVertexAttribute(tbl_ptr[idx].to_ptr());
        }
    const ResVertexAttribute GetVertexAttributes(int idx) const
        {
            const nw::ut::Offset* tbl_ptr = static_cast<const nw::ut::Offset*>(ref().toVertexAttributesTable.to_table_ptr());
            return ResVertexAttribute(tbl_ptr[idx].to_ptr());
        }
    nw::ut::internal::ResArray<ResVertexAttribute, nw::ut::internal::ResArrayClassTraits> GetVertexAttributes()
        {
            return nw::ut::internal::ResArray<ResVertexAttribute, nw::ut::internal::ResArrayClassTraits>(ref().toVertexAttributesTable.to_table_ptr(), GetVertexAttributesCount());
        }
    const nw::ut::internal::ResArray<const ResVertexAttribute, nw::ut::internal::ResArrayClassTraits> GetVertexAttributes() const
        {
            return nw::ut::internal::ResArray<const ResVertexAttribute, nw::ut::internal::ResArrayClassTraits>(ref().toVertexAttributesTable.to_table_ptr(), GetVertexAttributesCount());
        }

    void    Setup();
};

}
}
}

