// Filename: gfx_BaseAnimEvaluator.cpp
//
// Project: NintendoWare4Ctr

#include <nw/gfx/gfx_AnimObject.h>
#include <nw/gfx/res/gfx_ResMaterial.h>

namespace nw{
namespace gfx{

namespace internal{
    using namespace anim;

    void ClearMaterialHash(ResAnimGroupMember member)
    {
        ResMaterial material(reinterpret_cast<void*>(member.GetResMaterialPtr()));

        switch (member.GetObjectType())
        {
        case ResAnimGroupMember::OBJECT_TYPE_MATERIAL_COLOR:
            material.SetMaterialColorHash(0x0);
            break;

        case ResAnimGroupMember::OBJECT_TYPE_TEXTURE_SAMPLER:
            material.SetTextureMappersHash(0x0);
            material.SetTextureSamplersHash(0x0);
            break;

        case ResAnimGroupMember::OBJECT_TYPE_TEXTURE_MAPPER:
            material.SetTextureMappersHash(0x0);
            break;

        case ResAnimGroupMember::OBJECT_TYPE_BLEND_OPERATION:
            material.SetFragmentOperationHash(0x0);
            break;

        case ResAnimGroupMember::OBJECT_TYPE_TEXTURE_COORDINATOR:
            material.SetTextureCoordinatorsHash(0x0);
            break;

        default:
            break;
        }
    }
}

NW_UT_RUNTIME_TYPEINFO_DEFINITION(BaseAnimEvaluator, AnimObject);

const int BaseAnimEvaluator::NotFoundIndex = -1;

Result BaseAnimEvaluator::TryBind(AnimGroup* animGroup)
{
    return TryBindTemplate(animGroup, BasicIndexGetterFunctor());
}

void BaseAnimEvaluator::ResetMember(int memberIdx)
{
    NW_NULL_ASSERT(this->m_AnimGroup);

    if (!this->m_AnimGroup->HasOriginalValue())
    {
        return;
    }

    anim::ResGraphicsAnimGroup data = this->m_AnimGroup->GetResGraphicsAnimGroup();
    NW_ASSERT(data.IsValid());

    const anim::ResAnimGroupMember resAnimGroupMember = this->m_AnimGroup->GetResAnimGroupMember(memberIdx);
    resAnimGroupMember.SetValueForType(this->m_AnimGroup->GetTargetObject(memberIdx),this->m_AnimGroup->GetOriginalValue(memberIdx));
    
    m_IsCacheDirty = true;
}

}
}