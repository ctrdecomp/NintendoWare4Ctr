#pragma once

#include <nw/types.h>
#include <nw/os/os_Memory.h>
#include <nw/ut/ut_Inlines.h>
#include <nw/ut/ut_MovePtr.h>
#include <nw/math/math_Types.h>

namespace nw {
namespace gfx {

class GfxObject
{
public:
    void Destroy()
    {
        this->~GfxObject();
        m_Allocator.Free(this);
    }
protected:
    static void* operator new(size_t, void* buf) { return buf; }
    static void operator delete(void*) {}
    GfxObject(nw::os::IAllocator* allocator): 
        m_Allocator(*allocator)
        {
        NW_NULL_ASSERT(allocator);
    }

    virtual  ~GfxObject() {}

    nw::os::IAllocator& GetAllocator() { return m_Allocator; }
private:
    nw::os::IAllocator& m_Allocator;
};

using nw::ut::SafeDestroy;
using nw::ut::SafeDestroyer;
using nw::ut::SafeDestroyAll;

class GfxDeleter
{
public:
    GfxDeleter(): 
        m_HasOwnership(true) {}
    GfxDeleter(bool hasOwnership): 
        m_HasOwnership(hasOwnership) {}

    void operator()(GfxObject* object)
    {
        if (m_HasOwnership && object)
        {
            object->Destroy();
        }
    }


private:    
    bool m_HasOwnership;
};

template<typename TObject>
class GfxPtr
{
public:
    typedef nw::ut::MovePtr<TObject, GfxDeleter> GfxMovePtr;
    typedef typename GfxMovePtr::element_type element_type;
    typedef typename GfxMovePtr::SafeBool SafeBool;

    GfxPtr(): 
        m_MovePtr() {}

    GfxPtr(const GfxPtr& pointer): 
        m_MovePtr(const_cast<GfxPtr*>(&pointer)->m_MovePtr) {}

    template<typename TTObject>
    explicit GfxPtr(TTObject* pointer)
        :m_MovePtr(pointer) {}

    template<typename TTObject>
    GfxPtr(TTObject* pointer, bool hasOwnership): 
        m_MovePtr(pointer, GfxDeleter(hasOwnership)) {}

    template<typename TTObject, typename TTDeleter>
    GfxPtr(nw::ut::internal::MoveSource< nw::ut::MovePtr<TTObject, TTDeleter> > source): 
        m_MovePtr(source) {}

    ~GfxPtr() {}

    GfxPtr& operator=(GfxPtr<TObject> rhs)
    {
        m_MovePtr.operator=(rhs.m_MovePtr);
        return *this;
    }

    element_type* Get() const { return m_MovePtr.Get(); }
    element_type& operator*() const { return *m_MovePtr; }
    element_type* operator->() const { return m_MovePtr.operator->(); }
    element_type& operator[](std::size_t i) const { return m_MovePtr[i]; }
    element_type* Release() { return m_MovePtr.Release(); }
    
    void Reset() { m_MovePtr.Reset(); }

    template<typename TTObject>
    void Reset(TTObject* object) { GfxPtr(object).Swap(*this); }

    template<typename TTObject>
    void Reset(TTObject* object, bool hasOwnership)
    {
        GfxPtr(object, hasOwnership).Swap(*this);
    }

    operator SafeBool() const { return m_MovePtr.operator SafeBool(); }

    void Swap(GfxPtr& pointer) { m_MovePtr.Swap(pointer.m_MovePtr); }

    typename GfxMovePtr::deleter_reference GetDeleter() { return m_MovePtr.GetDeleter(); }
    typename GfxMovePtr::deleter_const_reference GetDeleter() const { return m_MovePtr.GetDeleter(); }

private:
    GfxMovePtr m_MovePtr;
};

}
}