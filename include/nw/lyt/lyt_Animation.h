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

class AnimTransform
{
public:
    AnimTransform();
    virtual ~AnimTransform();

    f32  GetFrame()    const { return m_Frame; }
    void SetFrame(f32 frame) { m_Frame = frame; }
    u16  GetFrameSize() const;
    f32  GetFrameMax()  const { return GetFrameSize(); }

    const res::AnimationBlock* GetAnimResource() const { return m_pRes; }

    bool IsLoopData() const;

    virtual void Animate(u32 idx, Pane* pPane) = 0;
    virtual void Animate(u32 idx, Material* pMaterial) = 0;

    virtual void SetResource(const res::AnimationBlock* pRes, ResourceAccessor* pResAccessor) = 0;
    virtual void SetResource(const res::AnimationBlock* pRes, ResourceAccessor* pResAccessor, u16 animNum) = 0;
    virtual void Bind(Pane* pPane, bool bRecursive, bool bDisable = false) = 0;
    virtual void Bind(Material* pMaterial, bool bDisable = false) = 0;

    ut::LinkListNode m_Link;

protected:
    void SetAnimResource(const res::AnimationBlock* pRes) { m_pRes = pRes; }

protected:
    const res::AnimationBlock* m_pRes;
    f32 m_Frame;
};

class AnimTransformBasic : public AnimTransform
{
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
    AnimationLink* Bind(T* pTarget, AnimationLink* pAnimLink, u16 idx, bool bDisable)
    {
        pAnimLink = this->FindUnbindLink(pAnimLink);
        if (!pAnimLink)
        {
            return 0;
        }
        pAnimLink->Set(this, idx, bDisable);
        pTarget->AddAnimationLink(pAnimLink);
        return ++pAnimLink;
    }
protected:
    AnimationLink* FindUnbindLink(AnimationLink* pLink) const;

    TextureInfo*   m_pTexAry;
    AnimationLink* m_pAnimLinkAry;
    u16            m_AnimLinkNum;
};

class AnimResource
{
public:
    AnimResource();
    explicit AnimResource(const void* anmResBuf) { Set(anmResBuf); }

    void Set(const void* anmResBuf);

    const ut::BinaryFileHeader* GetFileHeader() const { return m_pFileHeader; }
    const res::AnimationBlock* GetResourceBlock() const { return m_pResBlock; }
    const res::AnimationTagBlock* GetTagBlock() const { return m_pTagBlock; }

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

    const ut::BinaryFileHeader*      m_pFileHeader;
    const res::AnimationBlock*       m_pResBlock;
    const res::AnimationTagBlock*    m_pTagBlock;
    const res::AnimationShareBlock*  m_pShareBlock;
};

namespace internal {

class AnimPaneTree
{
public:
    AnimPaneTree();
    AnimPaneTree(Pane* pTargetPane, const AnimResource& animRes);

    void Set(Pane* pTargetPane, const AnimResource& animRes);

    AnimTransform* Bind(Layout* pLayout, Pane* pTargetPane, ResourceAccessor* pResAccessor) const;

    bool IsEnabled() const { return m_LinkNum > 0; }
    const AnimResource& GetAnimResource() const { return m_AnimRes; }

protected:
    static u16 FindAnimContent(const res::AnimationBlock* pAnimBlock, const char* animContName, u8 animContType);
    void Init();

    static const u16 NOBIND = u16(-1);
    static const int MATERIAL_NUM_MAX = 1 + 4 + 4;

    AnimResource m_AnimRes;
    u16          m_AnimPaneIdx;
    u16          m_LinkNum;
    u16          m_AnimMatIdxs[MATERIAL_NUM_MAX];
    u8           m_AnimMatCnt;
};

AnimationLink* FindAnimationLink(AnimationList* pAnimList, AnimTransform* pAnimTrans);
AnimationLink* FindAnimationLink(AnimationList* pAnimList, const AnimResource& animRes);
void UnbindAnimationLink(AnimationList* pAnimList, AnimTransform* pAnimTrans);

}
}
}