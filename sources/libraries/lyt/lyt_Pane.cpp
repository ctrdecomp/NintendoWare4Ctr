// Filename: lyt_Pane.cpp
//
// Project: NintendoWare4Ctr

#include <nw/lyt/lyt_Pane.h>
#include <nw/lyt/lyt_Common.h>
#include <nw/lyt/lyt_DrawInfo.h>
#include <nw/lyt/lyt_GraphicsResource.h>
#include <nw/lyt/lyt_Layout.h>
#include <nw/lyt/lyt_Animation.h>
#include <nw/lyt/lyt_Material.h>

namespace nw { 
namespace lyt {
namespace internal {

PaneBase::PaneBase()
{
}

PaneBase::~PaneBase()
{
    // this defition has more than your love life, this has nothing. Think about it.
}
} // namespace internal

NW_UT_RUNTIME_TYPEINFO_ROOT_DEFINITION(Pane);

Pane::Pane()
{
    using namespace std;

    Init();

    m_BasePosition = HORIZONTALPOSITION_CENTER + VERTICALPOSITION_CENTER * VERTICALPOSITION_MAX;
    memset(m_Name, 0, sizeof(m_Name));
    memset(m_UserData, 0, sizeof(m_UserData));

    m_Translate = VEC3(0, 0, 0);
    m_Rotate = VEC3(0, 0, 0);
    m_Scale = VEC2(1, 1);
    m_Size = Size(0.f, 0.f);
    m_Alpha = ut::Color8::ALPHA_MAX;
    m_GlbAlpha = m_Alpha;

    SetVisible(true);
}

Pane::Pane(const res::Pane* pRes)
{
    using namespace std;

    Init();

    m_BasePosition = pRes->basePosition;
    SetName(pRes->name);
    SetUserData(pRes->userData);

    m_Translate = pRes->translate;
    m_Rotate = pRes->rotate;
    m_Scale = pRes->scale;
    m_Size = pRes->size;
    m_Alpha = pRes->alpha;
    m_GlbAlpha = m_Alpha;

    m_Flag = pRes->flag;
}

void Pane::Init()
{
    m_pParent = 0;
    m_Flag = 0;
    m_pExtUserDataList = 0;

    MTX34Identity(&m_Mtx);
    MTX34Identity(&m_GlbMtx);
}

Pane::~Pane() 
{
    NW_UT_LINKLIST_FOREACH_SAFE(it, m_ChildList, {
        m_ChildList.Erase(it);

        if (!it->IsUserAllocated()) 
        {
            it->~Pane();
            Layout::FreeMemory(&*it);
        }
    })

    UnbindAnimationSelf(NULL);
}

void Pane::AppendChild(Pane* pChild) 
{
    InsertChild(m_ChildList.GetEndIter(), pChild);
}

void Pane::InsertChild(PaneList::Iterator next, Pane* pChild) 
{
    m_ChildList.Insert(next, pChild);
    pChild->m_pParent = this;
}

void Pane::RemoveChild(Pane* pChild) 
{
    m_ChildList.Erase(pChild);
    pChild->m_pParent = NULL;
}

const ut::Rect Pane::GetPaneRect() const 
{
    ut::Rect rect;
    math::VEC2 base = GetVtxPos();

    rect.left = base.x;
    rect.top = base.y;
    rect.right = base.x + m_Size.width;
    rect.bottom = base.y + m_Size.height;

    return rect;
}

const ut::Color Pane::GetVtxColor(u32 idx) const 
{
    return ut::Color::WHITE;
}

void Pane::SetVtxColor(u32 idx, ut::Color color) 
{ 

}

void Pane::SetColorElement(u32 idx, u8 value) 
{
    switch (idx) 
    {
    case ANIMTARGET_PANE_ALPHA: 
    {
        m_Alpha = value;
        break;
    }

    default: 
    {
        SetVtxColorElement(idx, value);
        break;
    }
    }
}

void Pane::SetVtxColorElement(u32 idx, u8 value) 
{ 

}

Pane* Pane::FindPaneByName(const char* pName, bool recursive) 
{
    if (internal::EqualsResName(m_Name, pName)) 
{
        return this;
    }

    if (recursive) 
    {
        NW_UT_LINKLIST_FOREACH (it, m_ChildList, {
            Pane* pResult = it->FindPaneByName(pName, true);

            if (pResult != NULL)
            {
                return pResult;
            }
        })
    }

    return NULL;
}

Material* Pane::FindMaterialByName(const char* pName, bool recursive)
{
    u32 nbMaterial = GetMaterialNum();
    for (u32 idx = 0; idx < nbMaterial; ++idx)
    {
        Material* pMaterial = GetMaterial(idx);
        if (pMaterial)
        {
            if (internal::EqualsMaterialName(pMaterial->GetName(), pName))
        {
                return pMaterial;
            }
        }
    }

    if (recursive) 
    {
        NW_UT_LINKLIST_FOREACH (it, m_ChildList, {
            Material* pResult = it->FindMaterialByName(pName, true);

            if (pResult != NULL) 
            {
                return pResult;
            }
        })
    }

    return NULL;
}

void Pane::Animate(u32 option) 
{
    AnimateSelf(option);

    if (IsVisible() || !(option & ANIMATEOPT_NOANIMATEINVISIBLE)) 
    {
        NW_UT_LINKLIST_FOREACH (it, m_ChildList, { it->Animate(option); })
    }
}

void Pane::AnimateSelf(u32 option) 
{
    NW_UT_LINKLIST_FOREACH (it, m_AnimList, {
        if (!it->IsEnable()) 
        {
            continue;
        }

        AnimTransform* pAnimTrans = it->GetAnimTransform();
        pAnimTrans->Animate(it->GetIndex(), this);
    })

    if (IsVisible() || !(option & ANIMATEOPT_NOANIMATEINVISIBLE)) 
    {
        u32 nbMaterial = GetMaterialNum();
        for (u32 idx = 0; idx < nbMaterial; ++idx)
        {
            Material* pMaterial = GetMaterial(idx);
            if (pMaterial)
            {
                pMaterial->Animate();
            }
        }
    }
}

void Pane::BindAnimation(AnimTransform* pAnimTrans, bool bRecursive, bool bDisable) 
{
    pAnimTrans->Bind(this, bRecursive, bDisable);
}

void Pane::UnbindAnimation(AnimTransform* pAnimTrans, bool recursive) 
{
    UnbindAnimationSelf(pAnimTrans);

    if (recursive) 
    {
        NW_UT_LINKLIST_FOREACH (it, m_ChildList, 
            { it->UnbindAnimation(pAnimTrans, recursive); })
    }
}

void Pane::UnbindAllAnimation(bool recursive) 
{
    UnbindAnimation(NULL, recursive);
}

void Pane::UnbindAnimationSelf(AnimTransform* pAnimTrans) 
{
    u32 nbMaterial = GetMaterialNum();
    for (u32 idx = 0; idx < nbMaterial; ++idx)
    {
        Material* pMaterial = GetMaterial(idx);
        if (pMaterial)
        {
            pMaterial->UnbindAnimation(pAnimTrans);
        }
    }

    internal::UnbindAnimationLink(&m_AnimList, pAnimTrans);
}

void Pane::AddAnimationLink(AnimationLink* pAnimLink) 
{
    m_AnimList.PushBack(pAnimLink);
}

void Pane::SetAnimationEnable(AnimTransform* pAnimTrans, bool enable, bool recursive) 
{
    AnimationLink* pAnimLink = internal::FindAnimationLink(&m_AnimList, pAnimTrans);

    if (pAnimLink != NULL) 
    {
        pAnimLink->SetEnable(enable);
    }

    const u32 materialNum = GetMaterialNum();
    for (u32 i = 0; i < materialNum; ++i)
    {
        GetMaterial(i)->SetAnimationEnable(pAnimTrans, enable);
    }

    if (recursive)
    {
        NW_UT_LINKLIST_FOREACH (it, m_ChildList, 
            { it->SetAnimationEnable(pAnimTrans, enable, recursive); })
    }
}

const VEC2 Pane::GetVtxPos() const
{
    math::VEC2 base(0.0f, 0.0f);

    switch (GetBasePositionH())
    {
    case HORIZONTALPOSITION_LEFT:

    default:
    {
        base.x = 0;
        break;
    }

    case HORIZONTALPOSITION_CENTER:
    {
        base.x = -m_Size.width / 2;
        break;
    }

    case HORIZONTALPOSITION_RIGHT:
    {
        base.x = -m_Size.width;
        break;
    }
    }

    switch (GetBasePositionV())
    {
    case VERTICALPOSITION_TOP:

    default:
    {
        base.y = 0.0f;
        break;
    }

    case VERTICALPOSITION_CENTER:
    {
        base.y = -m_Size.height / 2;
        break;
    }

    case VERTICALPOSITION_BOTTOM:
    {
        base.y = -m_Size.height;
        break;
    }
    }

    return base;
}

u16 Pane::GetExtUserDataNum() const
{
    if (!m_pExtUserDataList)
    {
        return 0;
    }

    return m_pExtUserDataList->num;
}

const ExtUserData* Pane::GetExtUserDataArray() const
{
    if (!m_pExtUserDataList)
    {
        return 0;
    }

    return internal::ConvertOffsToPtr<const ExtUserData>(m_pExtUserDataList, sizeof(*m_pExtUserDataList));
}

Material* Pane::GetMaterial() const
{
    if (GetMaterialNum() > 0)
    {
        return GetMaterial(0);
    }
    else
    {
        return 0;
    }
}

Material* Pane::GetMaterial(u32 idx) const
{
    return NULL;
}

u8 Pane::GetMaterialNum() const
{
    return 0;
}

} // namespace lyt
} // namespace nw