#pragma once

#include <nw/ut/ut_MoveArray.h>
#include <nw/ut/ut_Preprocessor.h>
#include <nw/ut/ut_TypeTraits.h>
#include <nw/os/os_Memory.h>

namespace nw {
namespace ut {

template<typename TChild>
class ChildDeleter
{
public:
    ChildDeleter(): m_Allocator(0) {}
    ChildDeleter(os::IAllocator* allocator):
        m_Allocator(allocator)
    {
    }

    void operator()(TChild* child)
    {
        child->~TChild();
        if (m_Allocator)
        {
            m_Allocator->Free(child);
        }
    }

    os::IAllocator* m_Allocator;
};

template<typename TChild>
class ChildDetacher
{
public:
    ChildDetacher() {}
    ChildDetacher(os::IAllocator* allocator) { NW_UNUSED_VARIABLE(allocator); }

    void operator()(TChild* child)
    {
        if (child)
        {
            child->SetParent(NULL);
        }
    }
};

template<typename TChild,typename TParent,typename TDeleter = ChildDeleter<TChild>,typename TChildList = MoveArray<TChild*> >
class Children
{
public:
    typedef TChild*&                            reference;
    typedef TChild*                             difference_type;
    typedef TChild*                             value_type;
    typedef typename TChildList::iterator       iterator;
    typedef typename TChildList::const_iterator const_iterator;

    typedef std::reverse_iterator<iterator>       reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    typedef TDeleter        deleter_type;
    typedef TDeleter&       deleter_reference;
    typedef const TDeleter& deleter_const_reference;

public:
    Children(): m_Children(), m_Deleter() {}

    Children(TParent* parent, void* elements, size_t size, os::IAllocator* allocator = 0, ArrayKind kind = ARRAY_WRAPPER):
        m_Parent(parent),
        m_Children(elements, size, allocator, kind),
        m_Deleter(allocator) {}

    Children(TParent* parent, size_t size, os::IAllocator* allocator, ArrayKind kind = ARRAY_WRAPPER):
        m_Parent(parent),
        m_Children(size, allocator, kind),
        m_Deleter(allocator) {}

    Children(TParent* parent, os::IAllocator* allocator):
        m_Parent(parent),
        m_Children(allocator),
        m_Deleter(allocator) {}

    ~Children()
    {
        std::for_each(this->m_Children.begin(), this->m_Children.end(), m_Deleter);
    }

public:
    struct SafeBoolHelper
    { 
        int x; 
    };
    
    typedef int SafeBoolHelper::* SafeBool;
    operator SafeBool() const { return m_Children; }

    iterator begin() { return m_Children.begin(); }
    const_iterator begin() const { return m_Children.begin(); }
    iterator end() { return m_Children.end(); }
    const_iterator end() const { return m_Children.end(); }

    void clear()
    {
        std::for_each(this->m_Children.begin(), this->m_Children.end(), ChildDetacher<TChild>());
        this->m_Children.clear();
    }

    iterator Begin() { return this->begin(); }
    const_iterator Begin() const { return this->begin(); }
    iterator End() { return this->end(); }
    const_iterator End() const { return this->end(); }

    void Clear() { this->clear(); }

    bool Attach(TChild* child)
    {
        if (child->GetParent() != 0)
        {
            return false;
        }
        bool pushed = m_Children.push_back(child);
        if (pushed)
        {
            child->SetParent(m_Parent);
        }
        return pushed;
    }

    bool Detach(TChild* child)
    {
        bool result = m_Children.erase_find(child);
        if (result)
        {
            child->SetParent(NULL);
        }
        return result;
    }

    void SetParent(TParent* parent)
    {
        NW_NULL_ASSERT(parent);
        NW_ASSERT(m_Parent == NULL);
        m_Parent = parent;
    }

    deleter_reference GetDeleter() { return m_Deleter; }
    deleter_const_reference GetDeleter() const { return m_Deleter; }

private:
    TParent* m_Parent;
    TChildList m_Children;
    deleter_type m_Deleter;
};

#define NW_DECLARE_CHILD_PARENT(MParent) \
public:\
    const MParent* GetParent() const { return m_Parent; }\
    MParent* GetParent() { return m_Parent; }\
    void SetParent(MParent* parent) { m_Parent = parent; }\
private:\
    MParent* m_Parent

}
}