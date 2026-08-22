#pragma once

#include <nw/ut/ut_MoveArray.h>
#include <nw/ut/ut_Preprocessor.h>
#include <nw/ut/ut_TypeTraits.h>
#include <nw/os/os_Memory.h>

namespace nw {
namespace ut {

template<typename TChild>
class ChildDeleter{
public:
    ChildDeleter(): mAllocator(0) {}
    ChildDeleter(os::IAllocator* allocator): mAllocator(allocator) {}

    void operator()(TChild* child){
        child->~TChild();
        if (mAllocator){
            mAllocator->Free(child);
        }
    }

    os::IAllocator* mAllocator;
};

template<typename TChild>
class ChildDetacher{
public:
    ChildDetacher() {}
    ChildDetacher(os::IAllocator* allocator) { NW_UNUSED_VARIABLE(allocator); }

    void operator()(TChild* child){
        if (child){
            child->SetParent(NULL);
        }
    }
};

template<typename TChild,typename TParent,typename TDeleter = ChildDeleter<TChild>,typename TChildList = MoveArray<TChild*> >
class Children{
public:
    typedef TChild*&                            reference;
    typedef TChild*                             difference_type;
    typedef TChild*                             value_type;
    typedef typename TChildList::iterator       iterator;
    typedef typename TChildList::const_iterator const_iterator;

#if defined(_MSC_VER) && _MSC_VER <= 1201
    typedef std::reverse_iterator<iterator, TChild*>       reverse_iterator;
    typedef std::reverse_iterator<const_iterator, TChild*> const_reverse_iterator;
#else
    typedef std::reverse_iterator<iterator>       reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
#endif

    typedef TDeleter        deleter_type;
    typedef TDeleter&       deleter_reference;
    typedef const TDeleter& deleter_const_reference;

public:
    Children(): mChildren(), mDeleter() {}

    Children(TParent* parent, void* elements, size_t size, os::IAllocator* allocator = 0, ArrayKind kind = ARRAY_WRAPPER):
        mParent(parent),
        mChildren(elements, size, allocator, kind),
        mDeleter(allocator)
    {}

    Children(TParent* parent, size_t size, os::IAllocator* allocator, ArrayKind kind = ARRAY_WRAPPER):
        mParent(parent),
        mChildren(size, allocator, kind),
        mDeleter(allocator)
    {}

    Children(TParent* parent, os::IAllocator* allocator):
        mParent(parent),
        mChildren(allocator),
        mDeleter(allocator)
    {}

    ~Children(){
        std::for_each(this->mChildren.begin(), this->mChildren.end(), mDeleter);
    }

public:
    struct SafeBoolHelper { int x; };
    typedef int SafeBoolHelper::* SafeBool;
    operator SafeBool() const { return mChildren; }

    iterator begin() { return mChildren.begin(); }
    const_iterator begin() const { return mChildren.begin(); }
    iterator end() { return mChildren.end(); }
    const_iterator end() const { return mChildren.end(); }

    void clear(){
        std::for_each(this->mChildren.begin(), this->mChildren.end(), ChildDetacher<TChild>());
        this->mChildren.clear();
    }

    iterator Begin() { return this->begin(); }
    const_iterator Begin() const { return this->begin(); }
    iterator End() { return this->end(); }
    const_iterator End() const { return this->end(); }

    void Clear() { this->clear(); }

    bool Attach(TChild* child){
        if (child->GetParent() != 0){
            return false;
        }
        bool pushed = mChildren.push_back(child);
        if (pushed){
            child->SetParent(mParent);
        }
        return pushed;
    }

    bool Detach(TChild* child){
        bool result = mChildren.erase_find(child);
        if (result){
            child->SetParent(NULL);
        }
        return result;
    }

    void SetParent(TParent* parent){
        NW_NULL_ASSERT(parent);
        NW_ASSERT(mParent == NULL);
        mParent = parent;
    }

    deleter_reference GetDeleter() { return mDeleter; }
    deleter_const_reference GetDeleter() const { return mDeleter; }

private:
    TParent* mParent;
    TChildList mChildren;
    deleter_type mDeleter;
};

#define NW_DECLARE_CHILD_PARENT(MParent) \
public:\
    const MParent* GetParent() const { return mParent; }\
    MParent* GetParent() { return mParent; }\
    void SetParent(MParent* parent) { mParent = parent; }\
private:\
    MParent* mParent

}
}