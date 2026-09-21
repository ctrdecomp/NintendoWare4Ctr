// Filename: lyt_Group.cpp
//
// Project: NintendoWare4Ctr

#include <nw/lyt/lyt_Common.h>
#include <nw/lyt/lyt_Group.h>
#include <nw/lyt/lyt_Layout.h>

namespace nw{
namespace lyt{

/* Group */

Group::Group()
{
    this->Init();

    std::memset(this->m_Name, 0, sizeof(m_Name));
}

Group::Group(const res::Group* pResGroup,Pane* pRootPane)
{
    Init();

    ut::strcpy(this->m_Name, sizeof(this->m_Name), pResGroup->name);

    const char *const paneNameBase = internal::ConvertOffsToPtr<char>(pResGroup, sizeof(res::Group));

    for (int i = 0; i < pResGroup->paneNum; ++i)
    {
        if (Pane* pFindPane = pRootPane->FindPaneByName(paneNameBase + i * ResourceNameStrMax, true))
        {
            AppendPane(pFindPane);
        }
    }
}

void Group::Init()
{
    m_UserAllocated = false;
}

Group::~Group()
{
    for (PaneLinkList::Iterator it = this->m_PaneLinkList.GetBeginIter(); it != this->m_PaneLinkList.GetEndIter();)
    {
        PaneLinkList::Iterator currIt = it++;
        this->m_PaneLinkList.Erase(currIt);
        Layout::DeleteObj(&(*currIt));
    }
}

void Group::AppendPane(Pane* pPane)
{
    if (PaneLink* pPaneLink = Layout::NewObj<PaneLink>())
    {
        pPaneLink->target = pPane;
        this->m_PaneLinkList.PushBack(pPaneLink);
    }
}

/* GroupContainer */

GroupContainer::~GroupContainer()
{
    for (GroupList::Iterator it = this->m_GroupList.GetBeginIter(); it != this->m_GroupList.GetEndIter();)
    {
        GroupList::Iterator currIt = it++;
        this->m_GroupList.Erase(currIt);
        if (!currIt->IsUserAllocated())
        {
            Layout::DeleteObj(&(*currIt));
        }
    }
}

void GroupContainer::AppendGroup(Group* pGroup)
{
    this->m_GroupList.PushBack(pGroup);
}

Group* GroupContainer::FindGroupByName(const char* findName)
{
    for (GroupList::Iterator it = this->m_GroupList.GetBeginIter(); it != this->m_GroupList.GetEndIter(); ++it)
    {
        if (internal::EqualsResName(it->GetName(), findName))
        {
            return &(*it);
        }
    }

    return 0;
}

}
}