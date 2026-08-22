// Filename: ut_LinkList.cpp
//
// Project: NintendoWare4Ctr

#include <nw/ut/ut_LinkList.h>

namespace nw {
namespace ut {
namespace internal {

LinkListImpl::iterator LinkListImpl::erase(iterator it){
    iterator itNext = it;
    (void)++itNext;
    return erase(it,itNext);
}

LinkListImpl::iterator LinkListImpl::erase(iterator itFirst,iterator itLast){
    Node* pIt = itFirst.mPointer;
    Node* const pItLast = itLast.mPointer;
    Node* pNext;
    
    for (; pIt != pItLast ; pIt = pNext){
        pNext = pIt->mNext;
        (void)erase(pIt);
    }

    return itLast;
}

void LinkListImpl::reverse(void){
    if (empty()) { return; }
    
    Node* p = mBaseNode.mNext;
    Node* pNext;
    
    Initialize_();
    
    for (; p != &this->mBaseNode ; p = pNext){
        pNext = p->mNext;
        p->mNext = NULL;
        p->mPrev = NULL;
        push_front(p);
    }
}

LinkListImpl::iterator LinkListImpl::insert(iterator it, pointer p){
    Node *const pIt = it.mPointer;

    Node *const pItPrev = pIt->mPrev;

    p->mNext = pIt;
    p->mPrev = pItPrev;

    pIt->mPrev = p;
    pItPrev->mNext = p;

    ++this->mSize;
    return iterator(p);
}

LinkListImpl::iterator LinkListImpl::erase(pointer p){
    Node* const pNext = p->mNext;
    Node* const pPrev = p->mPrev;
    
    pNext->mPrev = pPrev;
    pPrev->mNext = pNext;
    --mSize;

    p->mNext = NULL;
    p->mPrev = NULL;
    
    return iterator(pNext);
}

}
}
}
