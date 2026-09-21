#pragma once

#include <cstddef>

#include <nw/ut/ut_LinkList.h>
#include <nw/ut/ut_Rect.h>
#include <nw/ut/ut_RuntimeTypeInfo.h>
#include <nw/lyt/lyt_Common.h>
#include <nw/lyt/lyt_Resources.h>
#include <nw/lyt/lyt_Types.h>

namespace nw {
namespace lyt {
namespace internal {

class PaneBase
{
public:
    PaneBase();
    virtual ~PaneBase();
public:
    ut::LinkListNode m_Link;
};

} // namespace internal

class AnimTransform;
class AnimationLin;
class AnimResource;
class Material;
class DrawInfo;
class Pane;

typedef ut::LinkList<Pane, offsetof(internal::PaneBase, m_Link)> PaneList;

class Pane : public internal::PaneBase
{
public:
    NW_UT_RUNTIME_TYPEINFO;

    Pane();
    Pane(const res::Pane* pBlock);
    virtual ~Pane();

    const char* GetName()     const { return m_Name; }
    const char* GetUserData() const { return m_UserData; }

    void SetName(const char* name);
    void SetUserData(const char* userData);

    bool IsInfluencedAlpha() const { return internal::TestBit(m_Flag, PANEFLAG_INFLUENCEDALPHA); }
    void SetInfluencedAlpha(bool bAlpha) { internal::SetBit(&m_Flag, PANEFLAG_INFLUENCEDALPHA, bAlpha); }

    bool IsLocationAdjust() const { return internal::TestBit(m_Flag, PANEFLAG_LOCATIONADJUST); }
    void SetLocationAdjust(bool bAdjust)
    {
        internal::SetBit(&m_Flag, PANEFLAG_LOCATIONADJUST, bAdjust);
        this->SetMtxDirty();
    }

    const math::VEC3& GetTranslate() const { return m_Translate; }
    void SetTranslate(const math::VEC3& value)
    {
        m_Translate = value;
        this->SetMtxDirty();
    }
    void SetTranslate(const math::VEC2& value)
    {
        SetTranslate(math::VEC3(value.x, value.y, 0.f));
    }

    const math::VEC3& GetRotate() const { return m_Rotate; }
    void SetRotate(const math::VEC3& value)
    {
        m_Rotate = value;
        this->SetMtxDirty();
    }

    const math::VEC2& GetScale() const { return m_Scale; }
    void SetScale(const math::VEC2& value)
    {
        m_Scale = value;
        this->SetMtxDirty();
    }

    const Size& GetSize() const { return m_Size; }
    void SetSize(const Size& value) { m_Size = value; }

    u8 GetBasePositionH() const { return internal::GetHorizontalPosition(m_BasePosition); }
    void SetBasePositionH(u8 val) { internal::SetHorizontalPosition(&m_BasePosition, val); }

    u8 GetBasePositionV() const { return internal::GetVerticalPosition(m_BasePosition); }
    void SetBasePositionV(u8 val) { internal::SetVerticalPosition(&m_BasePosition, val); }

    const math::MTX34& GetMtx() const { return m_Mtx; }
    void SetMtx(const math::MTX34& mtx)
    {
        m_Mtx = mtx;
        m_Flag = internal::SetBit(m_Flag, PANEFLAG_USERMTX, true);
    }
    bool IsUserMtx() { return internal::TestBit(m_Flag, PANEFLAG_USERMTX); }
    void ResetMtx() { m_Flag = internal::SetBit(m_Flag, PANEFLAG_USERMTX, false); }

    const math::MTX34& GetGlobalMtx() const { return m_GlbMtx; }
    void SetGlobalMtx(const math::MTX34& mtx)
    {
        m_GlbMtx = mtx;
        m_Flag = internal::SetBit(m_Flag, PANEFLAG_USERGLOBALMTX, true);
    }
    bool IsUserGlobalMtx() { return internal::TestBit(m_Flag, PANEFLAG_USERGLOBALMTX); }
    void ResetGlobalMtx() { m_Flag = internal::SetBit(m_Flag, PANEFLAG_USERGLOBALMTX, false); }

    const ut::Rect GetPaneRect() const;

    virtual const ut::Color8 GetVtxColor(u32 idx) const;
    virtual void SetVtxColor(u32 idx, ut::Color8 value);

    u8 GetAlpha()      const { return m_Alpha; }
    void SetAlpha(u8 alpha) { m_Alpha = alpha; }
    u8 GetGlobalAlpha() const { return m_GlbAlpha; }
    void SetGlobalAlpha(u8 alpha) { m_GlbAlpha = alpha; }

    f32 GetSRTElement(u32 idx) const
    {
        const f32* srtAry = &m_Translate.x;
        return srtAry[idx];
    }
    void SetSRTElement(u32 idx, f32 value)
    {
        f32* srtAry = &m_Translate.x;
        srtAry[idx] = value;
        if (idx < ANIMTARGET_PANE_SIZEW)
        {
            this->SetMtxDirty();
        }
    }

    virtual u8   GetColorElement(u32 idx) const;
    virtual void SetColorElement(u32 idx, u8 value);

    virtual u8   GetVtxColorElement(u32 idx) const;
    virtual void SetVtxColorElement(u32 idx, u8 value);

    Material* GetMaterial() const;
    virtual u8        GetMaterialNum() const;
    virtual Material* GetMaterial(u32 idx) const;

    u16 GetExtUserDataNum() const;
    const ExtUserData* GetExtUserDataArray() const;
    const ExtUserData* FindExtUserDataByName(const char* name);

    Pane* GetParent() const { return m_pParent; }

    Pane* GetFirstChild() const
    {
        const PaneList& list = this->GetChildList();
        if (!list.IsEmpty())
        {
            return const_cast<Pane*>(&list.front());
        }
        return NULL;
    }

    Pane* GetNextChild(const Pane* pChild) const
    {
        const PaneList& list = this->GetChildList();
        return const_cast<Pane*>(list.GetNext(pChild));
    }

    const PaneList& GetChildList() const { return m_ChildList; }
    PaneList& GetChildList() { return m_ChildList; }

    void AppendChild(Pane* pChild);
    void PrependChild(Pane* pChild);
    void InsertChild(Pane* pNext, Pane* pChild);
    void InsertChild(PaneList::iterator next, Pane* pChild);
    void RemoveChild(Pane* pChild);

    virtual Pane* FindPaneByName(const char* findName, bool bRecursive = true);
    virtual Material* FindMaterialByName(const char* findName, bool bRecursive = true);
    virtual void Animate(u32 option);
    virtual void AnimateSelf(u32 option);

    const AnimationList& GetAnimationList() const { return m_AnimList; }
    AnimationList& GetAnimationList() { return m_AnimList; }

    virtual void BindAnimation(AnimTransform* pAnimTrans, bool bRecursive = true, bool bDisable = false);
    virtual void UnbindAnimation(AnimTransform* pAnimTrans, bool bRecursive = true);
    virtual void UnbindAllAnimation(bool bRecursive = true);
    virtual void UnbindAnimationSelf(AnimTransform* pAnimTrans);
    void AddAnimationLink(AnimationLink* pAnimationLink);
    virtual AnimationLink* FindAnimationLinkSelf(AnimTransform* pAnimTrans);
    virtual AnimationLink* FindAnimationLinkSelf(const AnimResource& animRes);
    virtual void SetAnimationEnable(AnimTransform* pAnimTrans, bool bEnable, bool bRecursive = true);
    virtual void SetAnimationEnable(const AnimResource& animRes, bool bEnable, bool bRecursive = true);

    bool IsVisible() const { return internal::TestBit(m_Flag, PANEFLAG_VISIBLE); }
    void SetVisible(bool bVisible) { internal::SetBit(&m_Flag, PANEFLAG_VISIBLE, bVisible); }

    virtual void CalculateMtx(const DrawInfo& drawInfo);

    bool IsUserAllocated() const { return internal::TestBit(m_Flag, PANEFLAG_USERALLOCATED); }
    void SetUserAllocated() { internal::SetBit(&m_Flag, PANEFLAG_USERALLOCATED, true); }
    void SetExtUserDataList(const res::ExtUserDataList* pBlock) { m_pExtUserDataList = pBlock; }

    virtual void Draw(const DrawInfo& drawInfo);
    virtual void DrawSelf(const DrawInfo& drawInfo);

    void MakeUniformData(DrawInfo* pDrawInfo, Drawer* pDrawer) const;
    virtual void MakeUniformDataSelf(DrawInfo* pDrawInfo, Drawer* pDrawer) const;

    void SetMtxDirty() { this->SetMtxCondition(MTXCONDITION_DIRTY); }

protected:
    virtual void LoadMtx(const DrawInfo& drawInfo);
    const math::VEC2 GetVtxPos() const;

    MtxCondition GetMtxCondition() const
    {
        return static_cast<MtxCondition>(internal::GetBits(m_Flag, PANEFLAG_MTXCONDITION, PANEFLAG_MTXCONDITION_LENGTH));
    }
    void SetMtxCondition(MtxCondition mtxCondition)
    {
        m_Flag = internal::SetBits(m_Flag, PANEFLAG_MTXCONDITION, PANEFLAG_MTXCONDITION_LENGTH, static_cast<u8>(mtxCondition));
    }

    void Init();

protected:
    Pane* m_pParent;
    PaneList m_ChildList;
    AnimationList m_AnimList;
    math::VEC3 m_Translate;
    math::VEC3 m_Rotate;
    math::VEC2 m_Scale;
    Size m_Size;
    math::MTX34 m_Mtx;
    math::MTX34 m_GlbMtx;
    const res::ExtUserDataList* m_pExtUserDataList;
    u8 m_Alpha;
    u8 m_GlbAlpha;
    u8 m_BasePosition;
    u8 m_Flag;
    char m_Name[ResourceNameStrMax + 1];
    char m_UserData[UserDataStrMax + 1];

private:
    Pane(const Pane& other);
    Pane& operator=(const Pane& other);
};

} // namespace lyt
} // namespace nw