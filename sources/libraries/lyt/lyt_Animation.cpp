// Filename: lyt_Animation.cpp
//
// Project: NintendoWare4Ctr

#include <nw/lyt/lyt_Common.h>
#include <nw/lyt/lyt_Pane.h>
#include <nw/lyt/lyt_Group.h>
#include <nw/lyt/lyt_Material.h>
#include <nw/lyt/lyt_Animation.h>
#include <nw/lyt/lyt_Layout.h>
#include <nw/lyt/lyt_ResourceAccessor.h>
#include <nw/lyt/lyt_Util.h>

const f32 R_SAME_TOLERANCE = 1.0e-5F;
const f32 R_FRAME_TOLERANCE = 0.001F;

namespace nw{
namespace lyt{
namespace{

/* Some Impl math shit */

inline bool RIsSame(const f32 a,const f32 b,const f32 tolerance = R_SAME_TOLERANCE)
{
    f32 c = a - b;
    return (-tolerance < c && c < tolerance);
}

u16 GetStepCurveValue(f32 frame,const res::StepKey* keyArray,u32 keySize)
{
    NW_ASSERT(keySize > 0);

    if (keySize == 1 || frame <= keyArray[0].frame)
    {
        return keyArray[0].value;
    }
    else if (frame >= keyArray[keySize - 1].frame)
    {
        return keyArray[keySize - 1].value;
    }

    int ikeyL = 0;
    int ikeyR = (int)keySize - 1;
    while (ikeyL != ikeyR - 1 && ikeyL != ikeyR)
    {

        int ikeyCenter = (ikeyL + ikeyR) / 2;
        const res::StepKey& centerKey = keyArray[ikeyCenter];
        if (frame < centerKey.frame)
        {
            ikeyR = ikeyCenter;
        }
        else
        {
            ikeyL = ikeyCenter;
        }
    }

    if (RIsSame(frame, keyArray[ikeyR].frame, R_FRAME_TOLERANCE))
    {
        return keyArray[ikeyR].value;
    }
    else
    {
        return keyArray[ikeyL].value;
    }
}

f32 GetHermiteCurveValue(f32 frame,const res::HermiteKey*  keyArray,u32 keySize)
{
    if (keySize == 1 || frame <= keyArray[0].frame)
    {
        return keyArray[0].value;
    }
    else if (frame >= keyArray[keySize - 1].frame)
    {
        return keyArray[keySize - 1].value;
    }

    u32 ikeyL = 0;
    u32 ikeyR = keySize - 1;
    while (ikeyL != ikeyR - 1 && ikeyL != ikeyR)
    {
        int ikeyCenter = (ikeyL + ikeyR) / 2;
        if (frame <= keyArray[ikeyCenter].frame)
        {
            ikeyR = ikeyCenter;
        }
        else
        {
            ikeyL = ikeyCenter;
        }
    }

    const res::HermiteKey& key0 = keyArray[ikeyL];
    const res::HermiteKey& key1 = keyArray[ikeyR];
    if (RIsSame(frame, key1.frame, R_FRAME_TOLERANCE))
    {

        if (ikeyR < keySize - 1 && key1.frame == keyArray[ikeyR + 1].frame)
        {
            return keyArray[ikeyR + 1].value;
        }
        else
        {
            return key1.value;
        }
    }
    f32 t1 = frame - key0.frame;
    f32 t2 = 1.0F / (key1.frame - key0.frame);
    f32 v0 = key0.value;
    f32 v1 = key1.value;
    f32 s0 = key0.slope;
    f32 s1 = key1.slope;

    f32 t1t1t2 = t1 * t1 * t2;
    f32 t1t1t2t2 = t1t1t2 * t2;
    f32 t1t1t1t2t2 = t1 * t1t1t2t2;
    f32 t1t1t1t2t2t2 = t1t1t1t2t2 * t2;

    return v0 * (2.0F * t1t1t1t2t2t2 - 3.0F * t1t1t2t2 + 1.0F) + v1 * (-2.0F * t1t1t1t2t2t2 + 3.0F * t1t1t2t2) + s0 * (t1t1t1t2t2 - 2.0F * t1t1t2 + t1) + s1 * (t1t1t1t2t2 - t1t1t2);
}

/* Animate Inlines */

void AnimatePaneSRT(Pane* pPane,const res::AnimationInfo* pAnimInfo,const u32* animTargetOffsets,f32 frame)
{
    for (int i = 0; i < pAnimInfo->num; ++i)
    {
        const res::AnimationTarget* pAnimTarget = internal::ConvertOffsToPtr<res::AnimationTarget>(pAnimInfo, animTargetOffsets[i]);

        NW_ASSERT(pAnimTarget->target < ANIMTARGET_PANE_MAX);
        NW_ASSERT(pAnimTarget->curveType == ANIMCURVE_HERMITE);

        const res::HermiteKey* keys = internal::ConvertOffsToPtr<res::HermiteKey>(pAnimTarget, pAnimTarget->keysOffset);
        pPane->SetSRTElement(pAnimTarget->target, GetHermiteCurveValue(frame, keys, pAnimTarget->keyNum));
    }
}

void AnimateVisibility(Pane* pPane,const res::AnimationInfo* pAnimInfo,const u32* animTargetOffsets,f32 frame)
{
    for (int i = 0; i < pAnimInfo->num; ++i)
    {
        const res::AnimationTarget* pAnimTarget = internal::ConvertOffsToPtr<res::AnimationTarget>(pAnimInfo, animTargetOffsets[i]);

        NW_ASSERT(pAnimTarget->target < ANIMTARGET_PANE_MAX);
        NW_ASSERT(pAnimTarget->curveType == ANIMCURVE_STEP);
    
        const res::StepKey* keys = internal::ConvertOffsToPtr<res::StepKey>(pAnimTarget, pAnimTarget->keysOffset);
        pPane->SetVisible(0 != GetStepCurveValue(frame, keys, pAnimTarget->keyNum));
    }
}

void AnimateVertexColor(Pane* pPane,const res::AnimationInfo* pAnimInfo,const u32* animTargetOffsets,f32 frame)
{
    for (int i = 0; i < pAnimInfo->num; ++i)
    {
        const res::AnimationTarget* pAnimTarget = internal::ConvertOffsToPtr<res::AnimationTarget>(pAnimInfo, animTargetOffsets[i]);

        NW_ASSERT(pAnimTarget->target < ANIMTARGET_PANE_COLOR_MAX);
        NW_ASSERT(pAnimTarget->curveType == ANIMCURVE_HERMITE);

        const res::HermiteKey* keys = internal::ConvertOffsToPtr<res::HermiteKey>(pAnimTarget, pAnimTarget->keysOffset);
        f32 value = GetHermiteCurveValue(frame, keys, pAnimTarget->keyNum);
        value += 0.5f;
        u8 u8Val = static_cast<u8>(value);
        pPane->SetColorElement(pAnimTarget->target, u8Val);
    }
}

void AnimateMaterialColor(Material* pMaterial,const res::AnimationInfo* pAnimInfo,const u32* animTargetOffsets,f32 frame)
{
    for (int i = 0; i < pAnimInfo->num; ++i)
    {
        const res::AnimationTarget* pAnimTarget = internal::ConvertOffsToPtr<res::AnimationTarget>(pAnimInfo, animTargetOffsets[i]);

        NW_ASSERT(pAnimTarget->target < ANIMTARGET_MATCOLOR_MAX);
        NW_ASSERT(pAnimTarget->curveType == ANIMCURVE_HERMITE);

        const res::HermiteKey* keys = internal::ConvertOffsToPtr<res::HermiteKey>(pAnimTarget, pAnimTarget->keysOffset);
        f32 value = GetHermiteCurveValue(frame, keys, pAnimTarget->keyNum);
        value += 0.5f;
        u8 val = static_cast<u8>(ut::Min(ut::Max(value, 0.f), 255.f));
        pMaterial->SetColorElement(pAnimTarget->target, val);
    }
}

void AnimateTextureSRT(Material* pMaterial,const res::AnimationInfo* pAnimInfo,const u32* animTargetOffsets,f32 frame)
{
    for (int i = 0; i < pAnimInfo->num; ++i)
    {
        const res::AnimationTarget* pAnimTarget = internal::ConvertOffsToPtr<res::AnimationTarget>(pAnimInfo, animTargetOffsets[i]);
        if (pAnimTarget->id < pMaterial->GetTexSRTCap())
        {

            NW_ASSERT(pAnimTarget->target < ANIMTARGET_TEXSRT_MAX);
            NW_ASSERT(pAnimTarget->curveType == ANIMCURVE_HERMITE);

            const res::HermiteKey* keys = internal::ConvertOffsToPtr<res::HermiteKey>(pAnimTarget, pAnimTarget->keysOffset);
            pMaterial->SetTexSRTElement(pAnimTarget->id, pAnimTarget->target, GetHermiteCurveValue(frame, keys, pAnimTarget->keyNum));
        }
    }
}

void AnimateTexturePattern(Material* pMaterial,const res::AnimationInfo* pAnimInfo,const u32* animTargetOffsets,f32 frame,const TextureInfo* texInfos)
{
    for (int j = 0; j < pAnimInfo->num; ++j)
    {
        const res::AnimationTarget* pAnimTarget = internal::ConvertOffsToPtr<res::AnimationTarget>(pAnimInfo, animTargetOffsets[j]);

        if (pAnimTarget->id < pMaterial->GetTexMapNum())
        {
            NW_ASSERT(pAnimTarget->curveType == ANIMCURVE_STEP);
            NW_ASSERT(pAnimTarget->target == ANIMTARGET_TEXPATTURN_IMAGE);

            const res::StepKey* keys = internal::ConvertOffsToPtr<res::StepKey>(pAnimTarget, pAnimTarget->keysOffset);
            const u16 fileIdx = GetStepCurveValue(frame, keys, pAnimTarget->keyNum);

            if (texInfos[fileIdx].IsValid())
            {
                pMaterial->SetTexMap(pAnimTarget->id, texInfos[fileIdx]);
            }
        }
    }
}

inline bool IsBindAnimation(Pane* pPane,AnimTransform* pAnimTrans)
{
    return false;
}

inline bool IsBindAnimation(Material* pMaterial,AnimTransform* pAnimTrans)
{
    return false;
}

}

/* AnimTransform */

AnimTransform::AnimTransform(): 
    m_pRes(0),
    m_Frame(0)
    {
}

AnimTransform::~AnimTransform()
{ 
}

u16 AnimTransform::GetFrameSize() const
{
    return m_pRes->frameSize;
}

bool AnimTransform::IsLoopData() const
{
    return m_pRes->loop != 0;
}

bool AnimResource::IsDescendingBind() const
{
    if (!m_pTagBlock)
    {
        return false;
    }

    return internal::TestBit(this->m_pTagBlock->flag, ANIMTAGFLAG_DESCENDINGBIND);
}

const AnimationShareInfo* AnimResource::GetAnimationShareInfoArray() const
{
    if (!m_pShareBlock)
    {
        return 0;
    }

    return internal::ConvertOffsToPtr<const AnimationShareInfo>(this->m_pShareBlock, this->m_pShareBlock->animShareInfoOffset);
}

/* AnimTransformBasic */

AnimTransformBasic::AnimTransformBasic(): 
    m_pTexAry(0),
    m_pAnimLinkAry(0),
    m_AnimLinkNum(0)
{
}

AnimTransformBasic::~AnimTransformBasic()
{
    Layout::DeleteArray(this->m_pAnimLinkAry, this->m_AnimLinkNum);
    Layout::DeletePrimArray(this->m_pTexAry);
}

void AnimTransformBasic::SetResource(const res::AnimationBlock* pRes,ResourceAccessor* pResAccessor)
{
    NW_NULL_ASSERT(pRes);
    this->SetResource(pRes, pResAccessor, pRes->animContNum);
}

void AnimTransformBasic::SetResource(const res::AnimationBlock* pRes,ResourceAccessor* pResAccessor,u16 animNum)
{
    NW_ASSERT(m_pTexAry == 0);
    NW_ASSERT(m_pAnimLinkAry == 0);
    NW_NULL_ASSERT(pRes);

    this->SetAnimResource(pRes);
    m_pTexAry = 0;
    if (pRes->fileNum > 0)
    {
        NW_NULL_ASSERT(pResAccessor);
        m_pTexAry = Layout::NewArray<TextureInfo>(pRes->fileNum);
        if (m_pTexAry)
        {
            const u32* fileNameOffsets = internal::ConvertOffsToPtr<u32>(pRes, sizeof(*pRes));

            for (int i = 0; i < pRes->fileNum; ++i)
            {
                const char *const fileName = internal::GetStrTableStr(fileNameOffsets, i);
                m_pTexAry[i] = pResAccessor->GetTexture(fileName);
            }
        }
    }

    m_pAnimLinkAry = Layout::NewArray<AnimationLink>(animNum);
    if (m_pAnimLinkAry)
    {
        m_AnimLinkNum = animNum;
    }
}

void AnimTransformBasic::Bind(Pane* pPane,bool bRecursive,bool bDisable)
{
    NW_NULL_ASSERT(pPane);

    AnimationLink* pCrAnimLink = 0;
    const res::AnimationBlock* pRes = this->GetAnimResource();

    const u32 *const animContOffsets = internal::ConvertOffsToPtr<u32>(pRes, pRes->animContOffsetsOffset);
    for (u16 i = 0; i < pRes->animContNum; ++i)
    {
        const res::AnimationContent& animCont = *internal::ConvertOffsToPtr<res::AnimationContent>(pRes, animContOffsets[i]);
        if (animCont.type == ANIMCONTENTTYPE_PANE)
        {
            if (Pane *const pFindPane = pPane->FindPaneByName(animCont.name, bRecursive))
            {
                if (!IsBindAnimation(pFindPane, this))
                {
                    pCrAnimLink = Bind(pFindPane, pCrAnimLink, i, bDisable);
                    if (!pCrAnimLink)
                    {
                        break;
                    }
                }
            }
        }
        else
        {
            if (Material *const pFindMat = pPane->FindMaterialByName(animCont.name, bRecursive))
            {
                if (!IsBindAnimation(pFindMat, this))
                {
                    pCrAnimLink = Bind(pFindMat, pCrAnimLink, i, bDisable);
                    if (!pCrAnimLink)
                    {
                        break;
                    }
                }
            }
        }
    }
}

void AnimTransformBasic::Bind(Material* pMaterial,bool bDisable)
{
    NW_NULL_ASSERT(pMaterial);

    AnimationLink* pCrAnimLink = 0;
    const res::AnimationBlock* pRes = this->GetAnimResource();

    const u32 *const animContOffsets = internal::ConvertOffsToPtr<u32>(pRes, pRes->animContOffsetsOffset);

    for (u16 i = 0; i < pRes->animContNum; ++i)
    {
        const res::AnimationContent& animCont = *internal::ConvertOffsToPtr<res::AnimationContent>(pRes, animContOffsets[i]);

        if (animCont.type == ANIMCONTENTTYPE_MATERIAL)
        {
            if (internal::EqualsMaterialName(pMaterial->GetName(), animCont.name))
            {
                if (!IsBindAnimation(pMaterial, this))
                {
                    pCrAnimLink = Bind(pMaterial, pCrAnimLink, i, bDisable);
                    if (!pCrAnimLink)
                    {
                        break;
                    }
                }
            }
        }
    }
}

void AnimTransformBasic::Animate(u32 idx, Pane* pPane)
{
    NW_NULL_ASSERT(pPane);

    const res::AnimationBlock* pRes = this->GetAnimResource();

    u32 animContOffsets = internal::ConvertOffsToPtr<u32>(pRes, pRes->animContOffsetsOffset)[idx];
    const res::AnimationContent* pAnimCont = internal::ConvertOffsToPtr<res::AnimationContent>(pRes, animContOffsets);

    const u32* animInfoOffsets = internal::ConvertOffsToPtr<u32>(pAnimCont, sizeof(*pAnimCont));
    for (int i = 0; i < pAnimCont->num; ++i)
    {
        const res::AnimationInfo* pAnimInfo = internal::ConvertOffsToPtr<res::AnimationInfo>(pAnimCont, animInfoOffsets[i]);
        const u32* animTargetOffsets = internal::ConvertOffsToPtr<u32>(pAnimInfo, sizeof(*pAnimInfo));

        switch (pAnimInfo->kind)
        {
        case res::ANIMATIONTYPE_PANESRT:
            AnimatePaneSRT(pPane, pAnimInfo, animTargetOffsets, this->GetFrame());
            break;
        case res::ANIMATIONTYPE_VISIBILITY:
            AnimateVisibility(pPane, pAnimInfo, animTargetOffsets, this->GetFrame());
            break;
        case res::ANIMATIONTYPE_VTXCOLOR:
            AnimateVertexColor(pPane, pAnimInfo, animTargetOffsets, this->GetFrame());
            break;
        }
    }
}

void AnimTransformBasic::Animate(u32 idx, Material* pMaterial)
{
    NW_NULL_ASSERT(pMaterial);

    const res::AnimationBlock* pRes = this->GetAnimResource();

    u32 animContOffsets = internal::ConvertOffsToPtr<u32>(pRes, pRes->animContOffsetsOffset)[idx];
    const res::AnimationContent* pAnimCont = internal::ConvertOffsToPtr<res::AnimationContent>(pRes, animContOffsets);

    const u32* animInfoOffsets = internal::ConvertOffsToPtr<u32>(pAnimCont, sizeof(*pAnimCont));
    for (int i = 0; i < pAnimCont->num; ++i)
    {
        const res::AnimationInfo* pAnimInfo = internal::ConvertOffsToPtr<res::AnimationInfo>(pAnimCont, animInfoOffsets[i]);
        const u32* animTargetOffsets = internal::ConvertOffsToPtr<u32>(pAnimInfo, sizeof(*pAnimInfo));

        switch (pAnimInfo->kind)
        {
        case res::ANIMATIONTYPE_MATCOLOR:
            AnimateMaterialColor(pMaterial, pAnimInfo, animTargetOffsets, this->GetFrame());
            break;
        case res::ANIMATIONTYPE_TEXSRT:
            AnimateTextureSRT(pMaterial, pAnimInfo, animTargetOffsets, this->GetFrame());
            break;
        case res::ANIMATIONTYPE_TEXPATTERN:
            if (m_pTexAry)
            {
                AnimateTexturePattern(pMaterial, pAnimInfo, animTargetOffsets, this->GetFrame(), this->m_pTexAry);
            }
            break;
        }
    }
}

/* AnimResource */

AnimResource::AnimResource()
{
    this->Init();
}

void AnimResource::Set(const void* anmResBuf)
{
    NW_NULL_ASSERT(anmResBuf);

    this->Init();

    const ut::BinaryFileHeader *const pFileHeader = static_cast<const ut::BinaryFileHeader*>(anmResBuf);

    if (!ut::IsValidBinaryFile(pFileHeader, res::FILESIGNATURE_CLAN, res::BinaryFileFormatVersion))
    {
        NW_WARNING(false, "not valid layout animation file.");
        return;
    }

    m_pFileHeader = pFileHeader;

    const ut::BinaryBlockHeader* pDataBlockHead = internal::ConvertOffsToPtr<ut::BinaryBlockHeader>(this->m_pFileHeader, this->m_pFileHeader->headerSize);
    for (int i = 0; i < m_pFileHeader->dataBlocks; ++i)
    {
        SigWord kind = pDataBlockHead->kind;
        switch (kind)
        {
        case res::DATABLOCKKIND_PANEANIMTAG:
            m_pTagBlock = reinterpret_cast<const res::AnimationTagBlock*>(pDataBlockHead);
            break;

        case res::DATABLOCKKIND_PANEANIMSHARE:
            m_pShareBlock = reinterpret_cast<const res::AnimationShareBlock*>(pDataBlockHead);
            break;

        case res::DATABLOCKKIND_PANEANIMINFO:
            m_pResBlock = reinterpret_cast<const res::AnimationBlock*>(pDataBlockHead);
            break;
        }
        pDataBlockHead = internal::ConvertOffsToPtr<ut::BinaryBlockHeader>(pDataBlockHead, pDataBlockHead->size);
    }

    NW_WARNING(m_pResBlock != NULL, "Animation resource is empty.");
}

void AnimResource::Init()
{
    m_pFileHeader = NULL;
    m_pResBlock = NULL;
    m_pTagBlock = NULL;
    m_pShareBlock = NULL;
}

}
}