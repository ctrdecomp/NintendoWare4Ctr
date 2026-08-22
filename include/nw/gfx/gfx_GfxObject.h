#pragma once

#include <nw/types.h>
#include <nw/os/os_Memory.h>
#include <nw/ut/ut_Inlines.h>
#include <nw/ut/ut_MovePtr.h>
#include <nw/math/math_Types.h>

namespace nw {
namespace gfx {

class GfxObject{
public:
    void Destroy() { this->~GfxObject(); mAllocator.Free(this); }  
protected:
    static void* operator new(size_t, void* buf) { return buf; }
    static void operator delete(void*) {}
    GfxObject(nw::os::IAllocator* allocator): 
    mAllocator(*allocator) {
        NW_NULL_ASSERT( allocator );
    }

    virtual  ~GfxObject() {}

    nw::os::IAllocator& GetAllocator() { return mAllocator; }
private:
    nw::os::IAllocator& mAllocator;
};

using nw::ut::SafeDestroy;
using nw::ut::SafeDestroyer;
using nw::ut::SafeDestroyAll;

class GfxDeleter{
public:
    GfxDeleter(): 
        mHasOwnership(true)
    {}
    GfxDeleter(bool hasOwnership): 
        mHasOwnership(hasOwnership)
    {}

    void operator()(GfxObject* object){
        if(mHasOwnership && object){
            object->Destroy();
        }
    }


private:    
    bool mHasOwnership;
};

template<typename TObject>
class GfxPtr{
public:
    typedef nw::ut::MovePtr<TObject, GfxDeleter> GfxMovePtr;
    typedef typename GfxMovePtr::element_type element_type;
    typedef typename GfxMovePtr::SafeBool SafeBool;

    GfxPtr(): 
        mMovePtr() 
    {}

    GfxPtr(const GfxPtr& pointer): 
        mMovePtr(const_cast<GfxPtr*>(&pointer)->mMovePtr) 
    {}

    template<typename TTObject>
    explicit GfxPtr(TTObject* pointer)
    : mMovePtr(pointer) {}

    template<typename TTObject>
    GfxPtr(TTObject* pointer, bool hasOwnership): 
        mMovePtr(pointer, GfxDeleter(hasOwnership)) 
    {}

    template<typename TTObject, typename TTDeleter>
    GfxPtr(nw::ut::internal::MoveSource< nw::ut::MovePtr<TTObject, TTDeleter> > source): 
        mMovePtr(source) 
    {}

    ~GfxPtr() {}

    GfxPtr& operator=(GfxPtr<TObject> rhs){
        mMovePtr.operator=(rhs.mMovePtr);
        return *this;
    }

    element_type* Get() const { return mMovePtr.Get(); }
    element_type& operator*() const { return *mMovePtr; }
    element_type* operator->() const { return mMovePtr.operator->(); }
    element_type& operator[](std::size_t i) const { return mMovePtr[i]; }
    element_type* Release() { return mMovePtr.Release(); }
    
    void Reset() { mMovePtr.Reset(); }

    template<typename TTObject>
    void Reset(TTObject* object) { GfxPtr(object).Swap(*this); }

    template<typename TTObject>
    void Reset(TTObject* object, bool hasOwnership){
        GfxPtr(object, hasOwnership).Swap(*this);
    }

    operator SafeBool() const { return mMovePtr.operator SafeBool(); }

    void Swap(GfxPtr& pointer) { mMovePtr.Swap(pointer.mMovePtr); }

    typename GfxMovePtr::deleter_reference GetDeleter() { return mMovePtr.GetDeleter(); }
    typename GfxMovePtr::deleter_const_reference GetDeleter() const { return mMovePtr.GetDeleter(); }

private:
    GfxMovePtr mMovePtr;
};

}
}