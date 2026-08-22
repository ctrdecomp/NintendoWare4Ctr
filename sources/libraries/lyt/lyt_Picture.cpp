// Filename: lyt_Picture.cpp
//
// Project: NintendoWare4Ctr

#include <nw/lyt/lyt_Common.h>
#include <nw/lyt/lyt_DrawInfo.h>
#include <nw/lyt/lyt_Layout.h>
#include <nw/lyt/lyt_Picture.h>
#include <nw/lyt/lyt_TexMap.h>
#include <nw/lyt/lyt_Material.h>
#include <nw/lyt/lyt_Animation.h>
#include <nw/lyt/lyt_ResourceAccessor.h>

namespace nw{
namespace lyt{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(Picture, Picture::Base);
Picture::Picture(u8 texNum){
    this->Init(texNum);
    mpMaterial = Layout::NewObj<Material>();
    if (mpMaterial){
        mpMaterial->ReserveMem(texNum, texNum, texNum);
    }
}

Picture::Picture(const TexMap& texMap){
    const int texNum = 1;
    this->Init(texNum);

    mpMaterial = Layout::NewObj<Material>();
    if (mpMaterial){
        mpMaterial->ReserveMem(texNum, texNum, texNum);
        this->Append(texMap);
    }
}

Picture::Picture(const res::Picture* pBlock,const ResBlockSet& resBlockSet):   
Base(pBlock){
    const u8 texCoordNum = ut::Min((int)pBlock->texCoordNum, TexMapMax);

    this->Init(texCoordNum);

    for (int i = 0; i < VERTEXCOLOR_MAX; ++i){
        this->mVtxColors[i] = pBlock->vtxCols[i];
    }

    if (texCoordNum > 0){
        if (!mTexCoordAry.IsEmpty()){
            mTexCoordAry.Copy(reinterpret_cast<const char*>(pBlock) + sizeof(*pBlock), texCoordNum);
        }
    }

    {
        const u32 *const matOffsTbl = internal::ConvertOffsToPtr<u32>(resBlockSet.pMaterialList, sizeof(*resBlockSet.pMaterialList));
        const res::Material *const pResMaterial = internal::ConvertOffsToPtr<res::Material>(resBlockSet.pMaterialList, matOffsTbl[pBlock->materialIdx]);
        mpMaterial = Layout::NewObj<Material>(pResMaterial, resBlockSet);
    }
}

void Picture::Init(u8 texNum){
    if (texNum > 0){
        this->ReserveTexCoord(texNum);
    }
    this->mIsTexCoordInited = false;
}

Picture::~Picture(){
    if (mpMaterial && ! this->mpMaterial->IsUserAllocated()){
        Layout::DeleteObj(this->mpMaterial);
        mpMaterial = 0;
    }

    this->mTexCoordAry.Free();
}

u8 Picture::GetMaterialNum() const{
    return mpMaterial ? 1 : 0;
}

Material* Picture::GetMaterial(u32 idx) const{
    return idx == 0 ? mpMaterial : 0;
}

void Picture::SetMaterial(Material* pMaterial){
    if (mpMaterial == pMaterial){
        return;
    }

    if (mpMaterial != NULL && !mpMaterial->IsUserAllocated()){
        Layout::DeleteObj(mpMaterial);
    }

    mpMaterial = pMaterial;
    if (pMaterial != NULL){
        pMaterial->SetTextureDirty();
    }
}

void Picture::Append(const TexMap& texMap){
    if (this->mpMaterial->GetTexMapNum() >= this->mpMaterial->GetTexMapCap() || this->mpMaterial->GetTexCoordGenNum() >= this->mpMaterial->GetTexCoordGenCap()){
        return;
    }

    const u8 texIdx = this->mpMaterial->GetTexMapNum();
    this->mpMaterial->SetTexMapNum(u8(texIdx + 1));
    this->mpMaterial->SetTexMap(texIdx, texMap);

    this->mpMaterial->SetTexCoordGenNum(this->mpMaterial->GetTexMapNum());
    this->mpMaterial->SetTexCoordGen(texIdx, TexCoordGen());

    SetTexCoordNum(this->mpMaterial->GetTexMapNum());

    if (GetSize() == Size(0.f, 0.f) && this->mpMaterial->GetTexMapNum() == 1){
        const TexSize& texSize = this->mpMaterial->GetTexMap(0).GetSize();
        SetSize(Size(texSize.width, texSize.height));
    }
}

void Picture::ReserveTexCoord(u8 num){
    this->mTexCoordAry.Reserve(num);
}

u8 Picture::GetTexCoordNum() const{
    return mTexCoordAry.GetSize();
}

void Picture::SetTexCoordNum(u8 num){
    this->mTexCoordAry.SetSize(num);
}

void Picture::GetTexCoord(u32 idx,TexCoordQuad coords) const{
    return this->mTexCoordAry.GetCoord(idx, coords);
}

void Picture::SetTexCoord(u32 idx,const TexCoordQuad coords){
    this->mTexCoordAry.SetCoord(idx, coords);

    if (mpMaterial != NULL){
        this->mpMaterial->SetTextureDirty();
    }
}

const ut::Color8 Picture::GetVtxColor(u32 idx) const{
    return mVtxColors[idx];
}

void Picture::SetVtxColor(u32 idx,ut::Color8 value){
    mVtxColors[idx] = value;
}

u8 Picture::GetVtxColorElement(u32 idx) const{
    return internal::GetVtxColorElement(this->mVtxColors, idx);
}

void Picture::SetVtxColorElement(u32 idx, u8 value){
    internal::SetVtxColorElement(this->mVtxColors, idx, value);
}

void Picture::DrawSelf(const DrawInfo& drawInfo){
    if (!mpMaterial){
        return;
    }

    LoadMtx(drawInfo);

    this->mpMaterial->SetupGraphics(drawInfo, GetGlobalAlpha());

    internal::DrawQuad(
        drawInfo,
        GetVtxPos(),
        GetSize(),
        this->mTexCoordAry.GetSize(),
        this->mTexCoordAry.GetArray(),
        this->mVtxColors);
}

}
}