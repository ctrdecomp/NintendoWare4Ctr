#pragma once

#include <cstddef>
#include <nw/ut/ut_LinkList.h>

#include <nw/lyt/lyt_Pane.h>
#include <nw/lyt/lyt_Types.h>

namespace nw{
namespace lyt{

struct PaneLink
{
    ut::LinkListNode link;
    Pane* target;
};

typedef ut::LinkList<PaneLink, offsetof(PaneLink, link)> PaneLinkList;

class Group
{
public:
    Group();
    Group(const res::Group* pResGroup, Pane* pRootPane);
    virtual ~Group();
    void AppendPane(Pane* pPane);

    const char* GetName() const {return m_Name;}
    PaneLinkList& GetPaneList() {return m_PaneLinkList;}
    bool IsUserAllocated() const {return m_UserAllocated != 0;}
    void SetUserAllocated() {m_UserAllocated = 1;}
    ut::LinkListNode m_Link_;

protected:
    void Init();

    PaneLinkList m_PaneLinkList;
    char m_Name[ResourceNameStrMax + 1];
    u8 m_UserAllocated;
    u8 m_Padding[2];
};

typedef ut::LinkList<Group, offsetof(Group, m_Link_)> GroupList;

class GroupContainer
{
public:
    GroupContainer() {}
    ~GroupContainer();
    void AppendGroup(Group* pGroup);
    Group* FindGroupByName(const char* findName);

    GroupList& GetGroupList() {return m_GroupList;}
protected:
    GroupList m_GroupList;
};

}
}