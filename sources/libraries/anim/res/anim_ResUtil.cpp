
#include <string.h>

#include <nw/anim/res/anim_ResUtil.h>

namespace {

nw::anim::ResTextureAnim CloneTextureAnimMember(
    nw::os::IAllocator* allocator, const nw::anim::ResTextureAnim& src)
{
    const int texturesCount = src.GetTexturesCount();
    void* texturesTableMemory = allocator->Alloc(sizeof(nw::ut::Offset) * texturesCount);

    nw::ut::Offset* texturesTable =
        reinterpret_cast<nw::ut::Offset*>(texturesTableMemory);

    void* texturesMemory = allocator->Alloc(sizeof(nw::gfx::ResReferenceTextureData) * texturesCount);

    nw::gfx::ResReferenceTextureData* textures =
        reinterpret_cast<nw::gfx::ResReferenceTextureData*>(texturesMemory);

    for (int i = 0; i < texturesCount; i++)
    {
        nw::gfx::ResReferenceTextureData& texture = textures[i];
        const nw::gfx::ResReferenceTextureData* srcTexture = src.GetTextures(i).ptr();

        texture.typeInfo = srcTexture->typeInfo;
        texture.m_Header.signature = srcTexture->m_Header.signature;
        texture.m_Header.revision = srcTexture->m_Header.revision;
        texture.toName.set_ptr(srcTexture->toName.to_ptr());
        texture.m_UserDataDicCount = srcTexture->m_UserDataDicCount;
        texture.toUserDataDic.set_ptr(srcTexture->toUserDataDic.to_ptr());

        texture.toPath.set_ptr(srcTexture->toPath.to_ptr());
        texture.toTargetTexture.set_ptr(NULL);

        texturesTable[i].set_ptr(&texture);
    }

    void* animMemory = allocator->Alloc(sizeof(nw::anim::ResTextureAnimData));
    nw::anim::ResTextureAnimData* anim = new(animMemory) nw::anim::ResTextureAnimData;

    if (nw::ut::CheckFlag(src.ptr()->m_Flags, nw::anim::ResTextureAnimData::FLAG_CONSTANT))
    { 
        anim->constantValueCurve = src.ptr()->constantValueCurve; 
    }
    else
    { 
        anim->toCurve.set_ptr(src.ptr()->toCurve.to_ptr()); 
    }

    anim->m_TexturesTableCount = texturesCount;
    anim->toTexturesTable.set_ptr(texturesTable);

    anim->m_Flags = src.GetFlags();
    anim->toPath.set_ptr(src.ptr()->toPath.to_ptr());
    anim->m_PrimitiveType = src.GetPrimitiveType();

    return nw::anim::ResTextureAnim(anim);
}

void DestroyTextureAnimMember(nw::os::IAllocator* allocator, nw::anim::ResTextureAnim anim)
{
    const int texturesCount = anim.GetTexturesCount();

    allocator->Free(anim.GetTextures(0).ptr());
    allocator->Free(anim.ptr()->toTexturesTable.to_ptr());
    allocator->Free(anim.ptr());
}

}

namespace nw {
namespace anim {
namespace res {

ResAnim CloneTextureAnim(os::IAllocator* allocator, const ResAnim& src)
{
    NW_ASSERT(strcmp(src.GetTargetAnimGroupName(), "MaterialAnimation") == 0);

    void* animMemory = allocator->Alloc(sizeof(ResAnimData));
    ResAnimData* animData = reinterpret_cast<ResAnimData*>(animMemory);

    animData->m_Header.revision = src.ptr()->m_Header.revision;
    animData->m_Header.signature = src.ptr()->m_Header.signature;
    animData->toName.set_ptr(src.ptr()->toName.to_ptr());
    animData->toTargetAnimGroupName.set_ptr(src.ptr()->toTargetAnimGroupName.to_ptr());
    animData->m_LoopMode = src.ptr()->m_LoopMode;
    animData->m_FrameSize = src.ptr()->m_FrameSize;
    animData->m_MemberAnimSetDicCount = src.ptr()->m_MemberAnimSetDicCount;

    animData->m_UserDataDicCount = src.ptr()->m_UserDataDicCount;
    animData->toUserDataDic.set_ptr(src.ptr()->toUserDataDic.to_ptr());

    const int memberCount = src.ptr()->m_MemberAnimSetDicCount;
    const size_t size = sizeof(ut::ResDicPatriciaData) + (sizeof(ut::ResDicPatriciaData::ResDicNodeData) * memberCount);
    void* memberMemory = allocator->Alloc(size);

    memcpy(memberMemory, src.ptr()->toMemberAnimSetDic.to_ptr(), size);
    ut::ResDicPatriciaData* memberDic = reinterpret_cast<ut::ResDicPatriciaData*>(memberMemory);

    const ut::ResDicPatriciaData* origDic =
        reinterpret_cast<const ut::ResDicPatriciaData*>(src.ptr()->toMemberAnimSetDic.to_ptr());

    for (int i = 0; i < memberCount; i++)
    {
        const int idx = i+1;

        memberDic->data[idx].ofsString.set_ptr( origDic->data[idx].ofsString.to_ptr());
        memberDic->data[idx].ofsData.set_ptr( origDic->data[idx].ofsData.to_ptr());

        if (src.GetMemberAnimSet(i).GetPrimitiveType() == ResMemberAnim::PRIMITIVETYPE_TEXTURE)
        {
            ResTextureAnim dup = CloneTextureAnimMember(allocator, src.GetMemberAnimSet(i));
            memberDic->data[idx].ofsData.set_ptr(dup.ptr());
        }
    }

    animData->toMemberAnimSetDic.set_ptr(memberDic);

    return ResAnim(animData);
}

void DestroyClonedTextureAnim(os::IAllocator* allocator, ResAnim anim)
{
    if (!anim.IsValid())
    {
        return;
    }

    for (int i = 0; i < anim.GetMemberAnimSetCount(); ++i)
    {
        if (anim.GetMemberAnimSet(i).GetPrimitiveType() == ResMemberAnim::PRIMITIVETYPE_TEXTURE)
        {
            DestroyTextureAnimMember(allocator, anim.GetMemberAnimSet(i));
        }
    }

    allocator->Free(anim.ptr()->toMemberAnimSetDic.to_ptr());
    allocator->Free(anim.ptr());
}

} // namespace res
} // namespace anim
} // namespace nw