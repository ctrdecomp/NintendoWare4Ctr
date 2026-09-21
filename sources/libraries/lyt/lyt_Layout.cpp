// Filename: lyt_Layout.cpp
//
// Project: NintendoWare4Ctr

#include <nw/lyt/lyt_Animation.h>
#include <nw/lyt/lyt_Bounding.h>
#include <nw/lyt/lyt_Common.h>
#include <nw/lyt/lyt_DrawInfo.h>
#include <nw/lyt/lyt_GraphicsResource.h>
#include <nw/lyt/lyt_Group.h>
#include <nw/lyt/lyt_Layout.h>
#include <nw/lyt/lyt_Material.h>
#include <nw/lyt/lyt_Picture.h>
#include <nw/lyt/lyt_ResourceAccessor.h>
#include <nw/lyt/lyt_Resources.h>
#include <nw/lyt/lyt_TextBox.h>
#include <nw/lyt/lyt_Util.h>
#include <nw/lyt/lyt_Window.h>

#define ARRAY_LENGTH(a)   (sizeof(a) / sizeof((a)[0]))

namespace nw{
namespace lyt{
namespace{

void SetTagProcessorImpl(Pane* pPane,font::TagProcessorBase<wchar_t>* pTagProcessor)
{
    if (TextBox* pTextBox = ut::DynamicCast<TextBox*>(pPane))
    {
        pTextBox->SetTagProcessor(pTagProcessor);
    }

    for (PaneList::Iterator it = pPane->GetChildList().GetBeginIter(); it != pPane->GetChildList().GetEndIter(); ++it)
    {
        SetTagProcessorImpl(&(*it), pTagProcessor);
    }
}

bool IsIncludeAnimationGroupRef(GroupContainer* pGroupContainer,const AnimationGroupRef *const  groupRefs,u16 bindGroupNum,bool bDescendingBind,Pane* pTargetPane)
{
    for (u16 grpIdx = 0; grpIdx < bindGroupNum; ++grpIdx)
    {
        Group *const pGroup = pGroupContainer->FindGroupByName(groupRefs[grpIdx].GetName());
        PaneLinkList& paneList = pGroup->GetPaneList();
        for (PaneLinkList::Iterator it = paneList.GetBeginIter(); it != paneList.GetEndIter(); ++it)
        {
            if (it->target == pTargetPane)
            {
                return true;
            }

            if (bDescendingBind)
            {
                for (Pane* pParentPane = pTargetPane->GetParent(); pParentPane; pParentPane = pParentPane->GetParent())
                {
                    if (it->target == pParentPane)
                    {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

}

nw::os::IAllocator* Layout::s_pAllocator = 0;
nw::os::IAllocator* Layout::s_pDeviceMemoryAllocator = 0;
bool Layout::s_LayoutDrawEnable = false;

Layout::Layout(): 
    mpRootPane(0),
    mpGroupContainer(0),
    m_LayoutSize(0.f, 0.f)
{
}

Layout::~Layout()
{
    DeleteObj(this->mpGroupContainer);

    if (mpRootPane && !this->mpRootPane->IsUserAllocated())
    {
        DeleteObj(this->mpRootPane);
    }

    for (AnimTransformList::Iterator it = this->m_AnimTransList.GetBeginIter(); it != this->m_AnimTransList.GetEndIter(); )
    {
        AnimTransformList::Iterator currIt = it++;
        this->m_AnimTransList.Erase(currIt);
        DeleteObj(&(*currIt));
    }
}

void Layout::SetAllocator(nw::os::IAllocator* pAllocator)
{
    s_pAllocator = pAllocator;
}

void Layout::SetDeviceMemoryAllocator(nw::os::IAllocator* pAllocator)
{
    s_pDeviceMemoryAllocator = pAllocator;
}

void* Layout::AllocMemory(u32 size, u8 alignment)
{
    void *const pMem = s_pAllocator->Alloc(size, alignment);
    return pMem;
}

void* Layout::AllocDeviceMemory(u32 size, u8 alignment)
{
    void *const pMem = s_pDeviceMemoryAllocator->Alloc(size, alignment);
    return pMem;
}

void Layout::FreeMemory(void* mem)
{
    s_pAllocator->Free(mem);
}

void Layout::FreeDeviceMemory(void* mem)
{
    s_pDeviceMemoryAllocator->Free(mem);
}

bool Layout::Build(const void* lytResBuf,ResourceAccessor* pResAcsr)
{
    const ut::BinaryFileHeader *const pFileHead = static_cast<const ut::BinaryFileHeader*>(lytResBuf);

    ResBlockSet resBlockSet = { 0 };
    resBlockSet.pResAccessor = pResAcsr;
    Pane* pParentPane = 0;
    Pane* pLastPane = 0;

    bool bReadRootGroup = false;
    int groupNestLevel = 0;

    const void* dataPtr = static_cast<const u8*>(lytResBuf) + pFileHead->headerSize;
    for (int i = 0; i < pFileHead->dataBlocks; ++i)
    {
        const ut::BinaryBlockHeader* pDataBlockHead = static_cast<const ut::BinaryBlockHeader*>(dataPtr);
        ut::SigWord kind = pDataBlockHead->kind;
        switch (kind)
        {
        case res::DATABLOCKKIND_LAYOUT:
        {
                const res::Layout* pResLyt = static_cast<const res::Layout*>(dataPtr);
                this->SetLayoutSize(pResLyt->layoutSize);
            }
            break;

        case res::DATABLOCKKIND_TEXTURELIST:
            resBlockSet.pTextureList = static_cast<const res::TextureList*>(dataPtr);
            break;

        case res::DATABLOCKKIND_FONTLIST:
            resBlockSet.pFontList = static_cast<const res::FontList*>(dataPtr);
            break;

        case res::DATABLOCKKIND_MATERIALLIST:
            resBlockSet.pMaterialList = static_cast<const res::MaterialList*>(dataPtr);
            break;

        case res::DATABLOCKKIND_PANE:
        case res::DATABLOCKKIND_PICTURE:
        case res::DATABLOCKKIND_TEXTBOX:
        case res::DATABLOCKKIND_WINDOW:
        case res::DATABLOCKKIND_BOUNDING:
        {
                Pane* pPane = BuildPaneObj(kind, dataPtr, resBlockSet);
                if (pPane)
                {
                    if (this->GetRootPane() == 0)
                    {
                        this->SetRootPane(pPane);
                    }
                    if (pParentPane)
                    {
                        pParentPane->AppendChild(pPane);
                    }

                    pLastPane = pPane;
                }
            }
            break;

        case res::DATABLOCKKIND_USERDATALIST:
            pLastPane->SetExtUserDataList(reinterpret_cast<const res::ExtUserDataList*>(pDataBlockHead));
            break;

        case res::DATABLOCKKIND_PANEBEGIN:
            pParentPane = pLastPane;
            break;

        case res::DATABLOCKKIND_PANEEND:
            pLastPane = pParentPane;
            pParentPane = pLastPane->GetParent();
            break;

        case res::DATABLOCKKIND_GROUP:
            if (!bReadRootGroup)
            {
                bReadRootGroup = true;
                this->SetGroupContainer(NewObj<GroupContainer>());
            }

            else{

                if (this->GetGroupContainer() && groupNestLevel == 1)
                {
                    if (Group* pGroup = NewObj<Group>(reinterpret_cast<const res::Group*>(pDataBlockHead), this->GetRootPane()))
                {
                        this->GetGroupContainer()->AppendGroup(pGroup);
                    }
                }
            }
            break;
        case res::DATABLOCKKIND_GROUPBEGIN:
            groupNestLevel++;
            break;
        case res::DATABLOCKKIND_GROUPEND:
            groupNestLevel--;
            break;
        default:
            break;
        }
        dataPtr = static_cast<const u8*>(dataPtr) + pDataBlockHead->size;
    }
    return true;
}

AnimTransform* Layout::CreateAnimTransform()
{
    AnimTransformBasic *const pAnimTrans = NewObj<AnimTransformBasic>();
    if (pAnimTrans)
    {
        this->GetAnimTransformList().PushBack(pAnimTrans);
    }
    return pAnimTrans;
}

void Layout::DeleteAnimTransform(AnimTransform *pAnimTransform)
{
    this->GetAnimTransformList().erase(pAnimTransform);
    DeleteObj(pAnimTransform);
}

AnimTransform* Layout::CreateAnimTransform(const void* animResBuf,ResourceAccessor* pResAcsr)
{
    return this->CreateAnimTransform(AnimResource(animResBuf), pResAcsr);
}

AnimTransform* Layout::CreateAnimTransform(const AnimResource& animRes,ResourceAccessor* pResAcsr)
{
    const res::AnimationBlock *const pAnimBlock = animRes.GetResourceBlock();
    if (!pAnimBlock)
    {
        return 0;
    }

    AnimTransform *const pAnimTrans = CreateAnimTransform();
    if (pAnimTrans)
    {
        pAnimTrans->SetResource(pAnimBlock, pResAcsr);
    }

    return pAnimTrans;
}

void Layout::BindAnimation(AnimTransform* pAnimTrans)
{
    if (this->GetRootPane())
    {
        this->GetRootPane()->BindAnimation(pAnimTrans, true);
    }
}

void Layout::UnbindAnimation(AnimTransform* pAnimTrans)
{
    if (this->GetRootPane())
    {
        this->GetRootPane()->UnbindAnimation(pAnimTrans, true);
    }
}

void Layout::UnbindAllAnimation()
{
    UnbindAnimation(0);
}

bool Layout::BindAnimationAuto(const AnimResource& animRes,ResourceAccessor* pResAcsr)
{
    if (!this->GetRootPane())
    {
        return false;
    }

    if (!animRes.GetResourceBlock())
    {
        return false;
    }

    AnimTransform *const pAnimTrans = CreateAnimTransform();
    if (pAnimTrans == NULL)
    {
        return false;
    }

    bool bResult = true;

    const u16 bindGroupNum = animRes.GetGroupNum();

    u16 animNum = 0;
    if (bindGroupNum == 0)
    {
        animNum = animRes.GetResourceBlock()->animContNum;

        pAnimTrans->SetResource(animRes.GetResourceBlock(), pResAcsr, animNum);

        const bool bRecursive = true;
        this->GetRootPane()->BindAnimation(pAnimTrans, bRecursive, true/* bDisable */);
    }
    else
    {
        const AnimationGroupRef *const groupRefs = animRes.GetGroupArray();
        for (int grpIdx = 0; grpIdx < bindGroupNum; ++grpIdx)
        {
            Group *const pGroup = this->GetGroupContainer()->FindGroupByName(groupRefs[grpIdx].GetName());
            if (pGroup == NULL)
            {
                bResult = false;
                continue;
            }

            animNum += animRes.CalcAnimationNum(pGroup, animRes.IsDescendingBind());
        }

        pAnimTrans->SetResource(animRes.GetResourceBlock(), pResAcsr, animNum);

        for (int grpIdx = 0; grpIdx < bindGroupNum; ++grpIdx)
        {
            Group *const pGroup = this->GetGroupContainer()->FindGroupByName(groupRefs[grpIdx].GetName());
            if (pGroup == NULL)
            {
                continue;
            }

            nw::lyt::BindAnimation(pGroup, pAnimTrans, animRes.IsDescendingBind(), true/* bDisable */);
        }
    }

    const u16 animShareInfoNum = animRes.GetAnimationShareInfoNum();
    if (animShareInfoNum > 0)
    {
        const AnimationShareInfo *const animShareInfoAry = animRes.GetAnimationShareInfoArray();

        for (int i = 0; i < animShareInfoNum; ++i)
        {
            Pane *const pSrcPane = this->GetRootPane()->FindPaneByName(animShareInfoAry[i].GetSrcPaneName());
            if (pSrcPane == NULL)
            {
                bResult = false;
                continue;
            }

            internal::AnimPaneTree animPaneTree(pSrcPane, animRes);
            if (!animPaneTree.IsEnabled())
            {
                continue;
            }

            Group *const pGroup = this->GetGroupContainer()->FindGroupByName(animShareInfoAry[i].GetTargetGroupName());
            if (pGroup == NULL)
            {
                bResult = false;
                continue;
            }

            PaneLinkList& paneList = pGroup->GetPaneList();
            u32 animIdx = 0;
            for (PaneLinkList::Iterator it = paneList.GetBeginIter(); it != paneList.GetEndIter(); ++it, ++animIdx)
            {
                if (it->target != pSrcPane)
                {
                    if (bindGroupNum > 0)
                    {
                        const bool bInclude = IsIncludeAnimationGroupRef(this->GetGroupContainer(),animRes.GetGroupArray(),
                            bindGroupNum,animRes.IsDescendingBind(),it->target);

                        if (!bInclude)
                        {
                            continue;
                        }
                    }
                    animPaneTree.Bind(this, it->target, pResAcsr);
                }
            }
        }
    }
    return bResult;
}

void Layout::SetAnimationEnable(AnimTransform* pAnimTrans,bool bEnable)
{
    if (this->GetRootPane())
    {
        this->GetRootPane()->SetAnimationEnable(pAnimTrans, bEnable, true);
    }
}

void Layout::CalculateMtx(const DrawInfo& drawInfo)
{
    if (!this->GetRootPane())
    {
        return;
    }
    this->GetRootPane()->CalculateMtx(drawInfo);
}

void Layout::Draw(const DrawInfo& drawInfo)
{
    if (!this->GetRootPane())
    {
        return;
    }

    GraphicsResource* graphicsResource = drawInfo.GetGraphicsResource();
    if (graphicsResource == NULL)
    {
        return;
    }

    graphicsResource->ResetGlState();
    graphicsResource->ResetGlProgramState();

    graphicsResource->SetProjectionMtx(drawInfo.GetProjectionMtx());

    drawInfo.SetLayout(this);

    this->GetRootPane()->Draw(drawInfo);

    internal::FinalizeGraphics();

    drawInfo.SetLayout(0);
}

void Layout::Animate(u32 option)
{
    if (!this->GetRootPane())
    {
        return;
    }

    this->GetRootPane()->Animate(option);
}

const ut::Rect Layout::GetLayoutRect() const
{
    return ut::Rect(- this->m_LayoutSize.width / 2, this->m_LayoutSize.height / 2, this->m_LayoutSize.width / 2, - this->m_LayoutSize.height / 2);
}

void Layout::SetTagProcessor(font::TagProcessorBase<wchar_t>* pTagProcessor)
{
    SetTagProcessorImpl(this->GetRootPane(), pTagProcessor);
}

Pane* Layout::BuildPaneObj(s32 kind,const void* dataPtr,const ResBlockSet& resBlockSet)
{
    switch (kind)
    {
    case res::DATABLOCKKIND_PANE:{
            const res::Pane* pResPane = static_cast<const res::Pane*>(dataPtr);
            return NewObj<Pane>(pResPane);
        }
    case res::DATABLOCKKIND_PICTURE:{
            const res::Picture* pResPic = static_cast<const res::Picture*>(dataPtr);
            return NewObj<Picture>(pResPic, resBlockSet);
        }
    case res::DATABLOCKKIND_TEXTBOX:{
            const res::TextBox* pBlock = static_cast<const res::TextBox*>(dataPtr);
            return NewObj<TextBox>(pBlock, resBlockSet);
        }
    case res::DATABLOCKKIND_WINDOW:{
            const res::Window* pBlock = static_cast<const res::Window*>(dataPtr);
            return NewObj<Window>(pBlock, resBlockSet);
        }
    case res::DATABLOCKKIND_BOUNDING:{
            const res::Bounding* pResBounding = static_cast<const res::Bounding*>(dataPtr);
            return NewObj<Bounding>(pResBounding, resBlockSet);
        }
    default:
        break;
    }
    return 0;
}

}
}