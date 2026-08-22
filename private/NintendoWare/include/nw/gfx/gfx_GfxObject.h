/*---------------------------------------------------------------------------*
  Project:  NintendoWare

  Copyright (C)Nintendo/HAL Laboratory, Inc.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary
  information of Nintendo and/or its licensed developers and are protected by
  national and international copyright laws. They may not be disclosed to third
  parties or copied or duplicated in any form, in whole or in part, without the
  prior written consent of Nintendo.

  The content herein is highly confidential and should be handled accordingly.
 *---------------------------------------------------------------------------*/

#ifndef NW_GFX_GFXOBJECT_H_
#define NW_GFX_GFXOBJECT_H_

#include <nw/types.h>
#include <nw/os/os_Memory.h>
#include <nw/ut/ut_MovePtr.h>
#include <nw/ut/ut_Inlines.h>

#ifdef NW_PLATFORM_CTR
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 2530 // padding added to end of struct.
#endif
#endif
namespace nw {
namespace gfx {

class GfxObject;

//---------------------------------------------------------------------------
//
//
//
//
//
//---------------------------------------------------------------------------
class GfxObject
{
public:
    //----------------------------------------
    //
    //

    //
    void Destroy() { this->~GfxObject(); m_Allocator.Free( this ); }

    //

protected:
    //----------------------------------------
    //
    //

    //
    //
    //
    static void*    operator new( size_t, void* buf ) NW_NO_THROW { return buf; }

    //
    //
    //
    static void     operator delete( void* ) NW_NO_THROW {}

#ifdef NW_COMPILER_MSVC
    //
    //
    //
    static void     operator delete( void*, void* ) NW_NO_THROW {}
#endif
    //

    //----------------------------------------
    //
    //

    //
    //
    //
    GfxObject( nw::os::IAllocator* allocator )
    : m_Allocator( *allocator )
    {
        NW_NULL_ASSERT( allocator );
    }

    //
    //
    //
    virtual  ~GfxObject() {}

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    nw::os::IAllocator& GetAllocator() { return m_Allocator; }

    //

private:
    nw::os::IAllocator&  m_Allocator;
};

using ut::SafeDestroy;
using ut::SafeDestroyer;
using ut::SafeDestroyAll;

//
//

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class GfxDeleter
{
public:
    //
    GfxDeleter()
    : m_HasOwnership(true)
    {}

    //
    //
    //
    GfxDeleter(bool hasOwnership)
    : m_HasOwnership(hasOwnership)
    {}

    //
    void operator() (GfxObject* object)
    {
        if (m_HasOwnership)
        {
            object->Destroy();
        }
    }

private:
    bool m_HasOwnership;
};

//

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
template<typename TObject>
class GfxPtr
{
public:
    typedef ut::MovePtr<TObject, GfxDeleter> GfxMovePtr;
    typedef typename GfxMovePtr::element_type element_type;
    typedef typename GfxMovePtr::SafeBool SafeBool;     //

    //----------------------------------------
    //
    //

    //
    GfxPtr() : m_MovePtr() {}

    //
    GfxPtr(const GfxPtr& pointer)
    : m_MovePtr(const_cast<GfxPtr*>(&pointer)->m_MovePtr) {}

    //
    template<typename TTObject>
    explicit GfxPtr(TTObject* pointer)
    : m_MovePtr(pointer) {}

    //
    //
    //
    //
    //
    //
    //
    template<typename TTObject>
    GfxPtr(TTObject* pointer, bool hasOwnership)
    : m_MovePtr(pointer, GfxDeleter(hasOwnership)) {}

    //
    template<typename TTObject, typename TTDeleter>
    GfxPtr(ut::internal::MoveSource< ut::MovePtr<TTObject, TTDeleter> > source)
    : m_MovePtr(source) {}


    //
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

    //
    //
    //
    //
    //
    //
    //
    template<typename TTObject>
    void Reset(TTObject* object, bool hasOwnership)
    {
        GfxPtr(object, hasOwnership).Swap(*this);
    }

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    operator SafeBool() const { return m_MovePtr.operator SafeBool(); }

    void Swap(GfxPtr& pointer) { m_MovePtr.Swap(pointer.m_MovePtr); }

    typename GfxMovePtr::deleter_reference GetDeleter() { return m_MovePtr.GetDeleter(); }
    typename GfxMovePtr::deleter_const_reference GetDeleter() const { return m_MovePtr.GetDeleter(); }

private:
    GfxMovePtr m_MovePtr;
    //
};

} // namespace gfx
} // namespace nw
#ifdef NW_PLATFORM_CTR
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 2530 // padding added to end of struct.
#endif
#endif

#endif // NW_GFX_GFXOBJECT_H_
