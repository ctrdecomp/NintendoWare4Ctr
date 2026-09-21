// Filename: gfx_AnimGroup.cpp
//
// Project: NintendoWare4Ctr

#include <nw/anim/anim_AnimBlend.h>
#include <nw/gfx/gfx_AnimObject.h>
#include <nw/gfx/gfx_TransformAnim.h>

namespace nw{
namespace gfx{

namespace {

anim::AnimBlendOp* GetAnimBlendOpByType(int blendOpType)
{
    static anim::AnimBlendOpBool blendOpBool;
    static anim::AnimBlendOpInt blendOpInt;
    static anim::AnimBlendOpFloat blendOpFloat;
    static anim::AnimBlendOpVector2 blendOpVector2;
    static anim::AnimBlendOpVector3 blendOpVector3;
    static anim::AnimBlendOpRgbaColor blendOpRgbaColor;
    static anim::AnimBlendOpTexture blendOpTexture;

    static TransformAnimBlendOpStandard     blendOpTransform;
    static TransformAnimBlendOpAccScale     blendOpTransformAccScale;
    static TransformAnimBlendOpQuat         blendOpTransformQuat;
    static TransformAnimBlendOpAccScaleQuat blendOpTransformAccScaleQuat;

    static AnimBlendOpTransform blendOpTransformNode;

    switch (blendOpType)
    {
    case anim::ResAnimGroup::BLENDOP_BOOL:
        return &blendOpBool;
    case anim::ResAnimGroup::BLENDOP_INT:
        return &blendOpInt;
    case anim::ResAnimGroup::BLENDOP_FLOAT:
        return &blendOpFloat;
    case anim::ResAnimGroup::BLENDOP_VECTOR2:
        return &blendOpVector2;
    case anim::ResAnimGroup::BLENDOP_VECTOR3:
        return &blendOpVector3;
    case anim::ResAnimGroup::BLENDOP_RGBA_COLOR:
        return &blendOpRgbaColor;
    case anim::ResAnimGroup::BLENDOP_TEXTURE:
        return &blendOpTexture;

    case anim::ResAnimGroup::BLENDOP_CALCULATED_TRANSFORM:
        return &blendOpTransform;
    case anim::ResAnimGroup::BLENDOP_CALCULATED_TRANSFORM_ACCURATE_SCALE:
        return &blendOpTransformAccScale;
    case anim::ResAnimGroup::BLENDOP_CALCULATED_TRANSFORM_QUAT:
        return &blendOpTransformQuat;
    case anim::ResAnimGroup::BLENDOP_CALCULATED_TRANSFORM_ACCURATE_SCALE_QUAT:
        return &blendOpTransformAccScaleQuat;

    case anim::ResAnimGroup::BLENDOP_TRANSFORM:
        return &blendOpTransformNode;

    default:
        return NULL;
    }
}

}


AnimGroup::AnimGroup(anim::ResAnimGroup resAnimGroup,SceneNode* sceneNode,os::IAllocator* allocator): 
    GfxObject(allocator),
    m_ResAnimGroup(resAnimGroup),
    m_SceneNode(sceneNode),
    m_PreEvaluateCallback(NULL),
    m_FullBakedAnimEnabled(false) {}

void AnimGroup::GetMemorySizeForInitialize(os::MemorySizeCalculator* pSize,const anim::ResAnimGroup resAnimGroup, bool useOriginalValue)
    {
    const int blendOpCount = resAnimGroup.GetBlendOperationsCount();
    const int memberCount = resAnimGroup.GetMemberInfoSetCount();

    os::MemorySizeCalculator& size = *pSize;

    size += sizeof(anim::AnimBlendOp*) * blendOpCount;
    size += sizeof(int) * memberCount;
    size += sizeof(void*) * memberCount;
    size += sizeof(void*) * memberCount;

    if (useOriginalValue)
    {
        size += sizeof(void*) * memberCount;
    }
}

Result AnimGroup::Initialize(bool useOriginalValue)
{
    Result result = INITIALIZE_RESULT_OK;
    {
        const int blendOpCount = this->m_ResAnimGroup.GetBlendOperationsCount();
        NW_ASSERT(blendOpCount > 0);

        void* memory = GetAllocator().Alloc(sizeof(anim::AnimBlendOp*) * blendOpCount);

        if (memory == NULL)
        {
            result |= Result::MASK_FAIL_BIT;
        }
        NW_ENSURE_AND_RETURN(result);

        m_BlendOperations = ut::MoveArray<anim::AnimBlendOp*>(memory, blendOpCount, &GetAllocator());
        for (int blendOpIdx = 0; blendOpIdx < blendOpCount; ++blendOpIdx)
        {
            const int blendOpType = this->m_ResAnimGroup.GetBlendOperations(blendOpIdx);
            this->m_BlendOperations.PushBackFast(GetAnimBlendOpByType(blendOpType));
        }
    }

    const int memberCount = GetMemberCount();
    NW_ASSERT(memberCount > 0);

    {
        void* memory = GetAllocator().Alloc(sizeof(int) * memberCount);
        if (memory == NULL)
        {
            result |= Result::MASK_FAIL_BIT;
        }
        NW_ENSURE_AND_RETURN(result);

        m_TargetObjectIndicies = ut::MoveArray<int>(memory, memberCount, &GetAllocator());
        this->m_TargetObjectIndicies.Resize(memberCount);
    }

    {
        void* memory = GetAllocator().Alloc(sizeof(void*) * memberCount);
        if (memory == NULL)
        {
            result |= Result::MASK_FAIL_BIT;
        }
        NW_ENSURE_AND_RETURN(result);

        m_TargetObjects = ut::MoveArray<void*>(memory, memberCount, &GetAllocator());
        this->m_TargetObjects.Resize(memberCount);
    }
    
    {
        void* memory = GetAllocator().Alloc(sizeof(void*) * memberCount);
        if (memory == NULL)
        {
            result |= Result::MASK_FAIL_BIT;
        }
        NW_ENSURE_AND_RETURN(result);

        m_TargetPtrs = ut::MoveArray<void*>(memory, memberCount, &GetAllocator());
        this->m_TargetPtrs.Resize(memberCount);
    }
    
    if (useOriginalValue)
    {
        void* memory = GetAllocator().Alloc(sizeof(void*) * memberCount);
        if (memory == NULL)
        {
            result |= Result::MASK_FAIL_BIT;
        }
        NW_ENSURE_AND_RETURN(result);

        m_OriginalValues = ut::MoveArray<const void*>(memory, memberCount, &GetAllocator());
        this->m_OriginalValues.Resize(memberCount);
    }

    return result;
}

void AnimGroup::Reset()
{
    if (!this->HasOriginalValue())
    {
        return;
    }

    for ( int memberIdx = 0 ; memberIdx < this->GetMemberCount() ; ++memberIdx )
    {
        const anim::ResAnimGroupMember resAnimGroupMember = this->GetResAnimGroupMember(memberIdx);
        resAnimGroupMember.SetValueForType(this->GetTargetObject(memberIdx),this->GetOriginalValue(memberIdx));
    }
}

}
}