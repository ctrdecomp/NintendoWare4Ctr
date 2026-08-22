#pragma once

#include <cstddef>
#include <nw/ut/ut_LinkList.h>

#include <nw/lyt/lyt_Pane.h>
#include <nw/lyt/lyt_Types.h>

namespace nw{
namespace lyt{

struct PaneLink{
    ut::LinkListNode link;
    Pane* target;
};

typedef ut::LinkList<PaneLink, offsetof(PaneLink, link)> PaneLinkList;

class Group{
public:
    Group();
    Group(const res::Group* pResGroup, Pane* pRootPane);
    virtual ~Group();
    void AppendPane(Pane* pPane);

    const char* GetName() const{return mName;}
    PaneLinkList& GetPaneList(){return mPaneLinkList;}
    bool IsUserAllocated() const{return mUserAllocated != 0;}
    void SetUserAllocated(){mUserAllocated = 1;}
    ut::LinkListNode mLink_;

protected:
    void Init();

    PaneLinkList mPaneLinkList;
    char mName[ResourceNameStrMax + 1];
    u8 mUserAllocated;
    u8 mPadding[2];
};

typedef ut::LinkList<Group, offsetof(Group, mLink_)> GroupList;

class GroupContainer{
public:
    GroupContainer(){}
    ~GroupContainer();
    void AppendGroup(Group* pGroup);
    Group* FindGroupByName(const char* findName);

    GroupList& GetGroupList(){return mGroupList;}
protected:
    GroupList mGroupList;
};

}
}