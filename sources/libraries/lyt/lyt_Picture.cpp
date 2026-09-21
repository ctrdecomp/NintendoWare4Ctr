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

Picture::Picture(u8 texNum)
{
    this->Init(texNum);
    m_pMaterial = Layout::NewObj<Material>();
    if (m_pMaterial)
    {
        m_pMaterial->ReserveMem(texNum, texNum, texNum);
    }
}

Picture::Picture(const TexMap& texMap)
{
    const int texNum = 1;
    this->Init(texNum);

    m_pMaterial = Layout::NewObj<Material>();
    if (m_pMaterial)
    {
        m_pMaterial->ReserveMem(texNum, texNum, texNum);
        this->Append(texMap);
    }
}

Picture::Picture(const res::Picture* pBlock,const ResBlockSet& resBlockSet):   
    Base(pBlock)
    {
    const u8 texCoordNum = ut::Min((int)pBlock->texCoordNum, TexMapMax);

    this->Init(texCoordNum);

    for (int i = 0; i < VERTEXCOLOR_MAX; ++i)
    {
        this->m_VtxColors[i] = pBlock->vtxCols[i];
    }

    if (texCoordNum > 0)
    {
        if (!m_TexCoordAry.IsEmpty())
        {
            m_TexCoordAry.Copy(reinterpret_cast<const char*>(pBlock) + sizeof(*pBlock), texCoordNum);
        }
    }

    {
        const u32 *const matOffsTbl = internal::ConvertOffsToPtr<u32>(resBlockSet.pMaterialList, sizeof(*resBlockSet.pMaterialList));
        const res::Material *const pResMaterial = internal::ConvertOffsToPtr<res::Material>(resBlockSet.pMaterialList, matOffsTbl[pBlock->materialIdx]);
        m_pMaterial = Layout::NewObj<Material>(pResMaterial, resBlockSet);
    }
}

void Picture::Init(u8 texNum)
{
    if (texNum > 0)
    {
        this->ReserveTexCoord(texNum);
    }
    this->m_IsTexCoordInited = false;
}

Picture::~Picture()
{
    if (m_pMaterial && ! this->m_pMaterial->IsUserAllocated())
    {
        Layout::DeleteObj(this->m_pMaterial);
        m_pMaterial = 0;
    }

    this->m_TexCoordAry.Free();
}

u8 Picture::GetMaterialNum() const
{
    return m_pMaterial ? 1 : 0;
}

Material* Picture::GetMaterial(u32 idx) const
{
    return idx == 0 ? m_pMaterial : 0;
}

void Picture::SetMaterial(Material* pMaterial)
{
    if (m_pMaterial == pMaterial)
    {
        return;
    }

    if (m_pMaterial != NULL && !m_pMaterial->IsUserAllocated())
    {
        Layout::DeleteObj(m_pMaterial);
    }

    m_pMaterial = pMaterial;
    if (pMaterial != NULL)
    {
        pMaterial->SetTextureDirty();
    }
}

void Picture::Append(const TexMap& texMap)
{
    if (this->m_pMaterial->GetTexMapNum() >= this->m_pMaterial->GetTexMapCap() || this->m_pMaterial->GetTexCoordGenNum() >= this->m_pMaterial->GetTexCoordGenCap())
    {
        return;
    }

    const u8 texIdx = this->m_pMaterial->GetTexMapNum();
    this->m_pMaterial->SetTexMapNum(u8(texIdx + 1));
    this->m_pMaterial->SetTexMap(texIdx, texMap);

    this->m_pMaterial->SetTexCoordGenNum(this->m_pMaterial->GetTexMapNum());
    this->m_pMaterial->SetTexCoordGen(texIdx, TexCoordGen());

    SetTexCoordNum(this->m_pMaterial->GetTexMapNum());

    if (GetSize() == Size(0.f, 0.f) && this->m_pMaterial->GetTexMapNum() == 1)
    {
        const TexSize& texSize = this->m_pMaterial->GetTexMap(0).GetSize();
        SetSize(Size(texSize.width, texSize.height));
    }
}

void Picture::ReserveTexCoord(u8 num)
{
    this->m_TexCoordAry.Reserve(num);
}

u8 Picture::GetTexCoordNum() const
{
    return m_TexCoordAry.GetSize();
}

void Picture::SetTexCoordNum(u8 num)
{
    this->m_TexCoordAry.SetSize(num);
}

void Picture::GetTexCoord(u32 idx,TexCoordQuad coords) const
{
    return this->m_TexCoordAry.GetCoord(idx, coords);
}

void Picture::SetTexCoord(u32 idx,const TexCoordQuad coords)
{
    this->m_TexCoordAry.SetCoord(idx, coords);

    if (m_pMaterial != NULL)
    {
        this->m_pMaterial->SetTextureDirty();
    }
}

const ut::Color8 Picture::GetVtxColor(u32 idx) const
{
    return m_VtxColors[idx];
}

void Picture::SetVtxColor(u32 idx,ut::Color8 value)
{
    m_VtxColors[idx] = value;
}

u8 Picture::GetVtxColorElement(u32 idx) const
{
    return internal::GetVtxColorElement(this->m_VtxColors, idx);
}

void Picture::SetVtxColorElement(u32 idx, u8 value)
{
    internal::SetVtxColorElement(this->m_VtxColors, idx, value);
}

void Picture::DrawSelf(const DrawInfo& drawInfo)
{
    if (!m_pMaterial)
    {
        return;
    }

    LoadMtx(drawInfo);

    this->m_pMaterial->SetupGraphics(drawInfo, GetGlobalAlpha());

    internal::DrawQuad(drawInfo,GetVtxPos(),GetSize(),this->m_TexCoordAry.GetSize(),this->m_TexCoordAry.GetArray(),this->m_VtxColors);
}

}
}