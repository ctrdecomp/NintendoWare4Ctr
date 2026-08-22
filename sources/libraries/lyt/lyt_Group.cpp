// Filename: lyt_Group.cpp
//
// Project: NintendoWare4Ctr

#include <nw/lyt/lyt_Common.h>
#include <nw/lyt/lyt_Group.h>
#include <nw/lyt/lyt_Layout.h>

namespace nw{
namespace lyt{

Group::Group(){
    this->Init();

    std::memset(this->mName, 0, sizeof(mName));
}

Group::Group(const res::Group* pResGroup,Pane* pRootPane){
    Init();

    ut::strcpy(this->mName, sizeof(this->mName), pResGroup->name);

    const char *const paneNameBase = internal::ConvertOffsToPtr<char>(pResGroup, sizeof(res::Group));

    for (int i = 0; i < pResGroup->paneNum; ++i){
        if (Pane* pFindPane = pRootPane->FindPaneByName(paneNameBase + i * ResourceNameStrMax, true)){
            AppendPane(pFindPane);
        }
    }
}

void Group::Init(){
    mUserAllocated = false;
}

Group::~Group(){

    for (PaneLinkList::Iterator it = this->mPaneLinkList.GetBeginIter(); it != this->mPaneLinkList.GetEndIter();){
        PaneLinkList::Iterator currIt = it++;
        this->mPaneLinkList.Erase(currIt);
        Layout::DeleteObj(&(*currIt));
    }
}

void Group::AppendPane(Pane* pPane){
    if (PaneLink* pPaneLink = Layout::NewObj<PaneLink>()){
        pPaneLink->target = pPane;
        this->mPaneLinkList.PushBack(pPaneLink);
    }
}

/* GroupContainer */

GroupContainer::~GroupContainer(){
    for (GroupList::Iterator it = this->mGroupList.GetBeginIter(); it != this->mGroupList.GetEndIter();){
        GroupList::Iterator currIt = it++;
        this->mGroupList.Erase(currIt);
        if (!currIt->IsUserAllocated()){
            Layout::DeleteObj(&(*currIt));
        }
    }
}

void GroupContainer::AppendGroup(Group* pGroup){
    this->mGroupList.PushBack(pGroup);
}

Group* GroupContainer::FindGroupByName(const char* findName){
    for (GroupList::Iterator it = this->mGroupList.GetBeginIter(); it != this->mGroupList.GetEndIter(); ++it){
        if (internal::EqualsResName(it->GetName(), findName)){
            return &(*it);
        }
    }

    return 0;
}

}
}