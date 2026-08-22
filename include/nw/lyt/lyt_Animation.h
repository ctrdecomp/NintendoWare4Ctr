#pragma once

#include <nw/ut/ut_LinkList.h>

#include <nw/lyt/lyt_Types.h>

namespace nw {
namespace lyt {
namespace res {

struct BinaryFileHeader;
struct AnimationBlock;
struct AnimationTagBlock;
struct AnimationShareBlock;

} // namespace res

namespace internal {
class AnimPaneTreeLink;
} // namespace internal

class Pane;
class Layout;
class Group;
class Material;
class ResourceAccessor;
class TextureInfo;
class AnimationLink;

class AnimTransform{
public:
    AnimTransform();
    virtual ~AnimTransform();

    f32  GetFrame()    const { return mFrame; }
    void SetFrame(f32 frame) { mFrame = frame; }
    u16  GetFrameSize() const;
    f32  GetFrameMax()  const { return GetFrameSize(); }

    const res::AnimationBlock* GetAnimResource() const { return mpRes; }

    bool IsLoopData() const;

    virtual void Animate(u32 idx, Pane* pPane) = 0;
    virtual void Animate(u32 idx, Material* pMaterial) = 0;

    virtual void SetResource(const res::AnimationBlock* pRes, ResourceAccessor* pResAccessor) = 0;
    virtual void SetResource(const res::AnimationBlock* pRes, ResourceAccessor* pResAccessor, u16 animNum) = 0;
    virtual void Bind(Pane* pPane, bool bRecursive, bool bDisable = false) = 0;
    virtual void Bind(Material* pMaterial, bool bDisable = false) = 0;

    ut::LinkListNode mLink;

protected:
    void SetAnimResource(const res::AnimationBlock* pRes) { mpRes = pRes; }

protected:
    const res::AnimationBlock* mpRes;
    f32 mFrame;
};

class AnimTransformBasic : public AnimTransform{
private:
    typedef AnimTransform Base;
public:
    AnimTransformBasic();
    virtual ~AnimTransformBasic();
    virtual void Animate(u32 idx, Pane* pPane);
    virtual void Animate(u32 idx, Material* pMaterial);
    virtual void SetResource(const res::AnimationBlock* pRes, ResourceAccessor* pResAccessor);
    virtual void SetResource(const res::AnimationBlock* pRes, ResourceAccessor* pResAccessor, u16 animNum);
    virtual void Bind(Pane* pPane, bool bRecursive, bool bDisable = false);
    virtual void Bind(Material* pMaterial, bool bDisable = false);

    template<typename T>
    AnimationLink* Bind(T* pTarget, AnimationLink* pAnimLink, u16 idx, bool bDisable){
        pAnimLink = this->FindUnbindLink(pAnimLink);
        if (!pAnimLink){
            return 0;
        }
        pAnimLink->Set(this, idx, bDisable);
        pTarget->AddAnimationLink(pAnimLink);
        return ++pAnimLink;
    }
protected:
    AnimationLink* FindUnbindLink(AnimationLink* pLink) const;

    TextureInfo*   mpTexAry;
    AnimationLink* mpAnimLinkAry;
    u16            mAnimLinkNum;
};

class AnimResource{
public:
    AnimResource();
    explicit AnimResource(const void* anmResBuf){ Set(anmResBuf); }

    void Set(const void* anmResBuf);

    const ut::BinaryFileHeader* GetFileHeader() const { return mpFileHeader; }
    const res::AnimationBlock* GetResourceBlock() const { return mpResBlock; }
    const res::AnimationTagBlock* GetTagBlock() const { return mpTagBlock; }

    u16 GetTagOrder() const;
    const char* GetTagName() const;
    u16 GetGroupNum() const;
    const AnimationGroupRef* GetGroupArray() const;
    bool IsDescendingBind() const;
    u16 GetAnimationShareInfoNum() const;
    const AnimationShareInfo* GetAnimationShareInfoArray() const;

    u16 CalcAnimationNum(Pane* pPane, bool bRecursive) const;
    u16 CalcAnimationNum(Material* pMaterial)          const;
    u16 CalcAnimationNum(Group* pGroup, bool bRecursive) const;

protected:
    void Init();
    bool CheckResource() const;

    const ut::BinaryFileHeader*      mpFileHeader;
    const res::AnimationBlock*       mpResBlock;
    const res::AnimationTagBlock*    mpTagBlock;
    const res::AnimationShareBlock*  mpShareBlock;
};

namespace internal {

class AnimPaneTree{
public:
    AnimPaneTree();
    AnimPaneTree(Pane* pTargetPane, const AnimResource& animRes);

    void Set(Pane* pTargetPane, const AnimResource& animRes);

    AnimTransform* Bind(Layout* pLayout, Pane* pTargetPane, ResourceAccessor* pResAccessor) const;

    bool IsEnabled() const { return mLinkNum > 0; }
    const AnimResource& GetAnimResource() const { return mAnimRes; }

protected:
    static u16 FindAnimContent(const res::AnimationBlock* pAnimBlock, const char* animContName, u8 animContType);
    void Init();

    static const u16 NOBIND = u16(-1);
    static const int MATERIAL_NUM_MAX = 1 + 4 + 4;

    AnimResource mAnimRes;
    u16          mAnimPaneIdx;
    u16          mLinkNum;
    u16          mAnimMatIdxs[MATERIAL_NUM_MAX];
    u8           mAnimMatCnt;
};

AnimationLink* FindAnimationLink(AnimationList* pAnimList, AnimTransform* pAnimTrans);
AnimationLink* FindAnimationLink(AnimationList* pAnimList, const AnimResource& animRes);
void UnbindAnimationLink(AnimationList* pAnimList, AnimTransform* pAnimTrans);

}
}
}