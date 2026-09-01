#pragma once

#include <nw/types.h>
#include <nw/assert.h>
#include <nw/ut/ut_Inlines.h>
#include <nw/ut/ut_Iterator.h>
#include <nw/ut/ut_Preprocessor.h>

namespace nw {
namespace ut {

class LinkListNode;

namespace internal {
class LinkListImpl;
} // namespace internal

class LinkListNode{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(LinkListNode);

public:
    typedef LinkListNode Self;
    
    explicit LinkListNode(): mNext(NULL), mPrev(NULL) {}

    Self* GetNext() const { return mNext; }
    Self* GetPrev() const { return mPrev; }
    friend class internal::LinkListImpl;

    friend bool operator==(const Self& r1, const Self& r2) { return &r1 == &r2; }
    friend bool operator!=(const Self& r1, const Self& r2) { return !(r1 == r2); }

private:
    Self* mNext;
    Self* mPrev;

    friend class internal::LinkListImpl;
};

namespace internal {

class LinkListImpl{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(LinkListImpl);

public:
    typedef LinkListImpl Self;
    typedef u32 size_type;
    typedef PtrDiff difference_type;

    typedef LinkListNode Node;
    typedef Node value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef std::bidirectional_iterator_tag iterator_category;

    class iterator;
    class const_iterator;

    class iterator : public internal::iterator<iterator_category, value_type>{
    public:
        typedef iterator TIt;
        typedef internal::iterator<iterator_category, value_type> TBaseIt;

        explicit iterator(): mPointer(NULL) {}

        reference operator*() const { return *mPointer; }
        pointer operator->() const { return mPointer; }

        TIt& operator++() { mPointer = mPointer->GetNext(); return *this; }
        TIt operator++(int) { const TIt it(*this); (void)++*this; return it; }
        TIt& operator--() { mPointer = mPointer->GetPrev(); return *this; }
        TIt operator--(int) { const TIt it(*this); (void)--*this; return it; }

        friend bool operator==(TIt it1, TIt it2) { return it1.mPointer == it2.mPointer; }
        friend bool operator!=(TIt it1, TIt it2) { return !(it1 == it2); }

    private:
        explicit iterator(pointer p): mPointer(p) {}
        pointer mPointer;

        friend class LinkListImpl;
        friend class const_iterator;
    };

    typedef iterator Iterator_alias_;

    class const_iterator : public internal::iterator<iterator_category, value_type>{
    public:
        typedef const_iterator TIt;
        typedef internal::iterator<iterator_category, value_type> TBaseIt;
        typedef const_pointer pointer;
        typedef const_reference reference;

        explicit const_iterator(): mPointer(NULL) {}
        const_iterator(Iterator_alias_ it): mPointer(it.mPointer) {}

        reference operator*() const { return *mPointer; }
        pointer operator->() const { return mPointer; }

        TIt& operator++() { mPointer = mPointer->GetNext(); return *this; }
        TIt operator++(int) { const TIt it(*this); (void)++*this; return it; }
        TIt& operator--() { mPointer = mPointer->GetPrev(); return *this; }
        TIt operator--(int) { const TIt it(*this); (void)--*this; return it; }

        friend bool operator==(TIt it1, TIt it2) { return it1.mPointer == it2.mPointer; }
        friend bool operator!=(TIt it1, TIt it2) { return !(it1 == it2); }

    private:
        explicit const_iterator(pointer p): mPointer(p) {}
        pointer mPointer;

        friend class LinkListImpl;
    };

    typedef internal::reverse_iterator<iterator> reverse_iterator;
    typedef internal::reverse_iterator<const_iterator> const_reverse_iterator;

    explicit LinkListImpl() { Initialize_(); }
    ~LinkListImpl() { clear(); }

    size_type size() const { return mSize; }
    bool empty() const { return mSize == 0; }

    iterator begin() { return iterator(mBaseNode.GetNext()); }
    const_iterator begin() const { return const_iterator(mBaseNode.GetNext()); }
    iterator end() { return iterator(&mBaseNode); }
    const_iterator end() const { return const_iterator(const_cast<Node*>(&mBaseNode)); }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

    reference front(){
        return *begin();
    }
    const_reference front() const{
        return *begin();
    }
    reference back(){
        return *--end();
    }
    const_reference back() const{
        return *--end();
    }

    void push_front(pointer p) { (void)insert(begin(), p); }
    void push_back(pointer p) { (void)insert(end(), p); }
    void pop_front() { (void)erase(begin()); }
    void pop_back() { (void)erase(--end()); }

    iterator insert(iterator it, pointer p);
    iterator erase(pointer p);
    iterator erase(iterator it);
    iterator erase(iterator itFirst, iterator itLast);
    void clear() { (void)this->erase(begin(), end()); }

    void reverse();

    static iterator GetIteratorFromPointer(pointer p){
        return iterator(p);
    }
    static const_iterator GetIteratorFromPointer(const_pointer p){
        return const_iterator(p);
    }

protected:
    Node* GetBaseNode() { return &mBaseNode; }
    const Node* GetBaseNode() const { return &mBaseNode; }

private:
    void Initialize_(){
        mSize = 0;
        mBaseNode.mNext = &mBaseNode;
        mBaseNode.mPrev = &mBaseNode;
    }

    size_type mSize;
    Node mBaseNode;
};

} // namespace internal

template<typename T, PtrDiff TNOffset>
class LinkList : private internal::LinkListImpl{
private:
    typedef internal::LinkListImpl Base;

public:
    typedef LinkList Self;
    using Base::Node;
    using Base::size_type;
    using Base::difference_type;

    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef std::bidirectional_iterator_tag iterator_category;

private:
    typedef Base::iterator TIt_base_;
    typedef Base::const_iterator TItC_base_;

public:
    class iterator;
    class const_iterator;

    class iterator : public internal::iterator<iterator_category, value_type>{
    public:
        typedef iterator TIt;
        typedef internal::iterator<iterator_category, value_type> TBaseIt;

        explicit iterator() {}

        reference operator*() const { pointer p = operator->(); NW_NULL_ASSERT(p); return *p; }
        pointer operator->() const { return GetPointerFromNode(it_.operator->()); }

        TIt& operator++() { (void)++it_; return *this; }
        TIt operator++(int) { const TIt it(*this); (void)++*this; return it; }
        TIt& operator--() { (void)--it_; return *this; }
        TIt operator--(int) { const TIt it(*this); (void)--*this; return it; }

        friend bool operator==(TIt it1, TIt it2) { return it1.it_ == it2.it_; }
        friend bool operator!=(TIt it1_, TIt it2_) { return !(it1_ == it2_); }

    public:
        explicit iterator(TIt_base_ it): it_(it) {}
        TIt_base_ it_;

        friend class LinkList;
        friend class const_iterator;
    };

    typedef iterator Iterator_alias_;

    class const_iterator : public internal::iterator<iterator_category, value_type>{
    public:
        typedef const_iterator TIt;
        typedef internal::iterator<iterator_category, value_type> TBaseIt;
        typedef const_pointer pointer;
        typedef const_reference reference;

        explicit const_iterator() {}
        const_iterator(Iterator_alias_ it): it_(it.it_) {}

        reference operator*() const { pointer p = operator->(); NW_NULL_ASSERT(p); return *p; }
        pointer operator->() const { return GetPointerFromNode(it_.operator->()); }

        TIt& operator++() { (void)++it_; return *this; }
        TIt operator++(int) { const TIt it(*this); (void)++*this; return it; }
        TIt& operator--() { (void)--it_; return *this; }
        TIt operator--(int) { const TIt it(*this); (void)--*this; return it; }

        friend bool operator==(TIt it1, TIt it2) { return it1.it_ == it2.it_; }
        friend bool operator!=(TIt it1_, TIt it2_) { return !(it1_ == it2_); }

    protected:
        explicit const_iterator(TItC_base_ it): it_(it) {}
        TItC_base_ it_;

        friend class LinkList;
    };

    typedef internal::reverse_iterator<iterator> reverse_iterator;
    typedef internal::reverse_iterator<const_iterator> const_reverse_iterator;

    explicit LinkList() {}

    using Base::size;
    using Base::empty;

    iterator begin() { return iterator(Base::begin()); }
    const_iterator begin() const { return const_iterator(const_cast<Self*>(this)->begin()); }
    iterator end() { return iterator(Base::end()); }
    const_iterator end() const { return const_iterator(const_cast<Self*>(this)->end()); }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

    reference front() { NW_ASSERT(!empty()); return *begin(); }
    const_reference front() const { NW_ASSERT(!empty()); return *begin(); }
    reference back() { NW_ASSERT(!empty()); return *--end(); }
    const_reference back() const { NW_ASSERT(!empty()); return *--end(); }

    iterator insert(iterator it, pointer p) { return iterator(Base::insert(it.it_, GetNodeFromPointer(p))); }
    void push_front(pointer p) { (void)insert(begin(), p); }
    void push_back(pointer p) { (void)insert(end(), p); }

    using Base::pop_front;
    using Base::pop_back;

    iterator erase(iterator it) { return iterator(Base::erase(it.it_)); }
    iterator erase(iterator itFirst, iterator itLast) { return iterator(Base::erase(itFirst.it_, itLast.it_)); }
    iterator erase(pointer p) { return iterator(Base::erase(GetNodeFromPointer(p))); }

    pointer GetPrev(pointer p){
        Node* baseNode = GetBaseNode();
        Node* node = (p == NULL) ? baseNode : GetNodeFromPointer(p);
        node = node->GetPrev();
        return (node == baseNode) ? NULL : GetPointerFromNode(node);
    }
    const_pointer GetPrev(const_pointer p) const{
        const Node* baseNode = GetBaseNode();
        const Node* node = (p == NULL) ? baseNode : GetNodeFromPointer(p);
        node = node->GetPrev();
        return (node == baseNode) ? NULL : GetPointerFromNode(node);
    }

    pointer GetNext(pointer p){
        Node* baseNode = GetBaseNode();
        Node* node = (p == NULL) ? baseNode : GetNodeFromPointer(p);
        node = node->GetNext();
        return (node == baseNode) ? NULL : GetPointerFromNode(node);
    }
    const_pointer GetNext(const_pointer p) const{
        const Node* baseNode = GetBaseNode();
        const Node* node = (p == NULL) ? baseNode : GetNodeFromPointer(p);
        node = node->GetNext();
        return (node == baseNode) ? NULL : GetPointerFromNode(node);
    }

    using Base::clear;
    using Base::reverse;

    static iterator GetIteratorFromPointer(Node* p){
        NW_NULL_ASSERT(p);
        return iterator(Base::GetIteratorFromPointer(p));
    }
    static const_iterator GetIteratorFromPointer(const Node* p){
        NW_NULL_ASSERT(p);
        return const_iterator(Base::GetIteratorFromPointer(p));
    }
    static iterator GetIteratorFromPointer(pointer p){
        NW_NULL_ASSERT(p);
        return GetIteratorFromPointer(GetNodeFromPointer(p));
    }
    static const_iterator GetIteratorFromPointer(const_pointer p){
        NW_NULL_ASSERT(p);
        return GetIteratorFromPointer(GetNodeFromPointer(p));
    }

    static Node* GetNodeFromPointer(pointer p){
        NW_NULL_ASSERT(p);
        return reinterpret_cast<Node*>(reinterpret_cast<IntPtr>(p) + TNOffset);
    }
    static const Node* GetNodeFromPointer(const_pointer p){
        NW_NULL_ASSERT(p);
        return reinterpret_cast<const Node*>(reinterpret_cast<IntPtr>(p) + TNOffset);
    }

    static pointer GetPointerFromNode(Node* p){
        NW_NULL_ASSERT(p);
        return reinterpret_cast<pointer>(reinterpret_cast<IntPtr>(p) - TNOffset);
    }
    static const_pointer GetPointerFromNode(const Node* p){
        NW_NULL_ASSERT(p);
        return reinterpret_cast<const_pointer>(reinterpret_cast<IntPtr>(p) - TNOffset);
    }

    typedef iterator Iterator;
    typedef const_iterator ConstIterator;
    typedef reverse_iterator ReverseIterator;
    typedef const_reverse_iterator ConstReverseIterator;

    size_type GetSize() const { return this->size(); }
    bool IsEmpty() const { return this->empty(); }

    iterator GetBeginIter() { return this->begin(); }
    const_iterator GetBeginIter() const { return this->begin(); }
    iterator GetEndIter() { return this->end(); }
    const_iterator GetEndIter() const { return this->end(); }

    reverse_iterator GetBeginReverseIter() { return this->rbegin(); }
    const_reverse_iterator GetBeginReverseIter() const { return this->rbegin(); }
    reverse_iterator GetEndReverseIter() { return this->rend(); }
    const_reverse_iterator GetEndReverseIter() const { return this->rend(); }

    reference GetFront() { return this->front(); }
    const_reference GetFront() const { return this->front(); }
    reference GetBack() { return this->back(); }
    const_reference GetBack() const { return this->back(); }

    iterator Insert(iterator it, pointer p) { return this->insert(it, p); }
    void PushFront(pointer p) { this->push_front(p); }
    void PushBack(pointer p) { this->push_back(p); }
    void PopFront() { this->pop_front(); }
    void PopBack() { this->pop_back(); }

    iterator Erase(iterator it) { return this->erase(it); }
    iterator Erase(iterator itFirst, iterator itLast) { return this->erase(itFirst, itLast); }
    iterator Erase(pointer p) { return this->erase(p); }

    void Clear() { this->clear(); }
    void Reverse() { this->reverse(); }
};

} // namespace ut
} // namespace nw

/*
Macro for Lists, as for ones used in snd.

NAME = nameOfIter
LIST = list member, for instance like `ut::LinkList m_ListTask` goes in here,as its a link list.
lastly inside its { NAME->Function(); }

*/

#define NW_UT_LINKLIST_FOREACH_SAFE(NAME, LIST, ...)                         \
    {                                                                          \
        typedef decltype((LIST).GetBeginIter()) IterType;                      \
                                                                               \
        for (IterType __impl__ = (LIST).GetBeginIter();                        \
             __impl__ != (LIST).GetEndIter();) {                               \
                                                                               \
            IterType NAME = __impl__++;                                        \
            __VA_ARGS__;                                                       \
        }                                                                      \
    }