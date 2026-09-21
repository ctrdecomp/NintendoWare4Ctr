// Filename: ut_LinkList.cpp
//
// Project: NintendoWare4Ctr

#include <nw/ut/ut_LinkList.h>

namespace nw {
namespace ut {
namespace internal {

LinkListImpl::iterator LinkListImpl::erase(iterator it)
{
    iterator itNext = it;
    (void)++itNext;
    return erase(it,itNext);
}

LinkListImpl::iterator LinkListImpl::erase(iterator itFirst,iterator itLast)
{
    Node* pIt = itFirst.m_Pointer;
    Node* const pItLast = itLast.m_Pointer;
    Node* pNext;
    
    for (; pIt != pItLast ; pIt = pNext)
    {
        pNext = pIt->m_Next;
        (void)erase(pIt);
    }

    return itLast;
}

void LinkListImpl::reverse(void)
{
    if (empty()) { return; }
    
    Node* p = m_BaseNode.m_Next;
    Node* pNext;
    
    Initialize_();
    
    for (; p != &this->m_BaseNode ; p = pNext)
    {
        pNext = p->m_Next;
        p->m_Next = NULL;
        p->m_Prev = NULL;
        push_front(p);
    }
}

LinkListImpl::iterator LinkListImpl::insert(iterator it, pointer p)
{
    Node *const pIt = it.m_Pointer;

    Node *const pItPrev = pIt->m_Prev;

    p->m_Next = pIt;
    p->m_Prev = pItPrev;

    pIt->m_Prev = p;
    pItPrev->m_Next = p;

    ++this->m_Size;
    return iterator(p);
}

LinkListImpl::iterator LinkListImpl::erase(pointer p)
{
    Node* const pNext = p->m_Next;
    Node* const pPrev = p->m_Prev;
    
    pNext->m_Prev = pPrev;
    pPrev->m_Next = pNext;
    --m_Size;

    p->m_Next = NULL;
    p->m_Prev = NULL;
    
    return iterator(pNext);
}

}
}
}
