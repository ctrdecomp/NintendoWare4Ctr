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

class PaneBase{
public:
    PaneBase();
    virtual ~PaneBase();
public:
    ut::LinkListNode mLink;
};

} // namespace internal

class AnimTransform;
class AnimationLin;
class AnimResource;
class Material;
class DrawInfo;
class Pane;

typedef ut::LinkList<Pane, offsetof(internal::PaneBase, mLink)> PaneList;

class Pane : public internal::PaneBase{
public:
    NW_UT_RUNTIME_TYPEINFO;

    Pane();
    Pane(const res::Pane* pBlock);
    virtual ~Pane();

    const char* GetName()     const { return mName; }
    const char* GetUserData() const { return mUserData; }

    void SetName(const char* name);
    void SetUserData(const char* userData);

    bool IsInfluencedAlpha() const { return internal::TestBit(mFlag, PANEFLAG_INFLUENCEDALPHA); }
    void SetInfluencedAlpha(bool bAlpha){ internal::SetBit(&mFlag, PANEFLAG_INFLUENCEDALPHA, bAlpha); }

    bool IsLocationAdjust() const { return internal::TestBit(mFlag, PANEFLAG_LOCATIONADJUST); }
    void SetLocationAdjust(bool bAdjust){
        internal::SetBit(&mFlag, PANEFLAG_LOCATIONADJUST, bAdjust);
        this->SetMtxDirty();
    }

    const math::VEC3& GetTranslate() const { return mTranslate; }
    void SetTranslate(const math::VEC3& value){
        mTranslate = value;
        this->SetMtxDirty();
    }
    void SetTranslate(const math::VEC2& value){
        SetTranslate(math::VEC3(value.x, value.y, 0.f));
    }

    const math::VEC3& GetRotate() const { return mRotate; }
    void SetRotate(const math::VEC3& value){
        mRotate = value;
        this->SetMtxDirty();
    }

    const math::VEC2& GetScale() const { return mScale; }
    void SetScale(const math::VEC2& value){
        mScale = value;
        this->SetMtxDirty();
    }

    const Size& GetSize() const { return mSize; }
    void SetSize(const Size& value) { mSize = value; }

    u8 GetBasePositionH() const { return internal::GetHorizontalPosition(mBasePosition); }
    void SetBasePositionH(u8 val) { internal::SetHorizontalPosition(&mBasePosition, val); }

    u8 GetBasePositionV() const { return internal::GetVerticalPosition(mBasePosition); }
    void SetBasePositionV(u8 val) { internal::SetVerticalPosition(&mBasePosition, val); }

    const math::MTX34& GetMtx() const { return mMtx; }
    void SetMtx(const math::MTX34& mtx){
        mMtx = mtx;
        mFlag = internal::SetBit(mFlag, PANEFLAG_USERMTX, true);
    }
    bool IsUserMtx() { return internal::TestBit(mFlag, PANEFLAG_USERMTX); }
    void ResetMtx()  { mFlag = internal::SetBit(mFlag, PANEFLAG_USERMTX, false); }

    const math::MTX34& GetGlobalMtx() const { return mGlbMtx; }
    void SetGlobalMtx(const math::MTX34& mtx){
        mGlbMtx = mtx;
        mFlag = internal::SetBit(mFlag, PANEFLAG_USERGLOBALMTX, true);
    }
    bool IsUserGlobalMtx() { return internal::TestBit(mFlag, PANEFLAG_USERGLOBALMTX); }
    void ResetGlobalMtx()  { mFlag = internal::SetBit(mFlag, PANEFLAG_USERGLOBALMTX, false); }

    const ut::Rect GetPaneRect() const;

    virtual const ut::Color8 GetVtxColor(u32 idx) const;
    virtual void SetVtxColor(u32 idx, ut::Color8 value);

    u8 GetAlpha()      const { return mAlpha; }
    void SetAlpha(u8 alpha)  { mAlpha = alpha; }
    u8 GetGlobalAlpha() const { return mGlbAlpha; }
    void SetGlobalAlpha(u8 alpha) { mGlbAlpha = alpha; }

    f32 GetSRTElement(u32 idx) const{
        const f32* srtAry = &mTranslate.x;
        return srtAry[idx];
    }
    void SetSRTElement(u32 idx, f32 value){
        f32* srtAry = &mTranslate.x;
        srtAry[idx] = value;
        if (idx < ANIMTARGET_PANE_SIZEW){
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

    __forceinline Pane* GetParent() const { return mpParent; }

    __forceinline Pane* GetFirstChild() const{
        const PaneList& list = this->GetChildList();
        if (!list.IsEmpty()){
            return const_cast<Pane*>(&list.front());
        }
        return NULL;
    }

    __forceinline Pane* GetNextChild(const Pane* pChild) const{
        const PaneList& list = this->GetChildList();
        return const_cast<Pane*>(list.GetNext(pChild));
    }

    __forceinline const PaneList& GetChildList() const { return mChildList; }
    __forceinline PaneList& GetChildList() { return mChildList; }

    void AppendChild(Pane* pChild);
    void PrependChild(Pane* pChild);
    void InsertChild(Pane* pNext, Pane* pChild);
    void InsertChild(PaneList::iterator next, Pane* pChild);
    void RemoveChild(Pane* pChild);

    virtual Pane*     FindPaneByName(const char* findName, bool bRecursive = true);
    virtual Material* FindMaterialByName(const char* findName, bool bRecursive = true);
    virtual void Animate(u32 option);
    virtual void AnimateSelf(u32 option);

    const AnimationList& GetAnimationList() const { return mAnimList; }
    AnimationList& GetAnimationList() { return mAnimList; }

    virtual void BindAnimation(AnimTransform* pAnimTrans, bool bRecursive = true, bool bDisable = false);
    virtual void UnbindAnimation(AnimTransform* pAnimTrans, bool bRecursive = true);
    virtual void UnbindAllAnimation(bool bRecursive = true);
    virtual void UnbindAnimationSelf(AnimTransform* pAnimTrans);
    void AddAnimationLink(AnimationLink* pAnimationLink);
    virtual AnimationLink* FindAnimationLinkSelf(AnimTransform* pAnimTrans);
    virtual AnimationLink* FindAnimationLinkSelf(const AnimResource& animRes);
    virtual void SetAnimationEnable(AnimTransform* pAnimTrans, bool bEnable, bool bRecursive = true);
    virtual void SetAnimationEnable(const AnimResource& animRes, bool bEnable, bool bRecursive = true);

    bool IsVisible() const { return internal::TestBit(mFlag, PANEFLAG_VISIBLE); }
    void SetVisible(bool bVisible) { internal::SetBit(&mFlag, PANEFLAG_VISIBLE, bVisible); }

    virtual void CalculateMtx(const DrawInfo& drawInfo);

    bool IsUserAllocated() const { return internal::TestBit(mFlag, PANEFLAG_USERALLOCATED); }
    void SetUserAllocated() { internal::SetBit(&mFlag, PANEFLAG_USERALLOCATED, true); }
    void SetExtUserDataList(const res::ExtUserDataList* pBlock) { mpExtUserDataList = pBlock; }

    virtual void Draw(const DrawInfo& drawInfo);
    virtual void DrawSelf(const DrawInfo& drawInfo);

    void MakeUniformData(DrawInfo* pDrawInfo, Drawer* pDrawer) const;
    virtual void MakeUniformDataSelf(DrawInfo* pDrawInfo, Drawer* pDrawer) const;

    __forceinline void SetMtxDirty(){ this->SetMtxCondition(MTXCONDITION_DIRTY); }

protected:
    virtual void LoadMtx(const DrawInfo& drawInfo);
    const math::VEC2 GetVtxPos() const;

    __forceinline MtxCondition GetMtxCondition() const{
        return static_cast<MtxCondition>(internal::GetBits(mFlag, PANEFLAG_MTXCONDITION, PANEFLAG_MTXCONDITION_LENGTH));
    }
    __forceinline void SetMtxCondition(MtxCondition mtxCondition){
        mFlag = internal::SetBits(mFlag, PANEFLAG_MTXCONDITION, PANEFLAG_MTXCONDITION_LENGTH, static_cast<u8>(mtxCondition));
    }

    void Init();

protected:
    Pane* mpParent;
    PaneList mChildList;
    AnimationList mAnimList;
    math::VEC3 mTranslate;
    math::VEC3 mRotate;
    math::VEC2 mScale;
    Size mSize;
    math::MTX34 mMtx;
    math::MTX34 mGlbMtx;
    const res::ExtUserDataList* mpExtUserDataList;
    u8 mAlpha;
    u8 mGlbAlpha;
    u8 mBasePosition;
    u8 mFlag;
    char mName[ResourceNameStrMax + 1];
    char mUserData[UserDataStrMax + 1];

private:
    Pane(const Pane& other);
    Pane& operator=(const Pane& other);
};

} // namespace lyt
} // namespace nw