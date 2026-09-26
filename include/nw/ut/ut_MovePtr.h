#pragma once

#include <nw/ut/ut_TypeTraits.h>

namespace nw{
namespace gfx{
    class GfxObject;
}
namespace os{

class IAllocator;
}

namespace ut{
namespace internal{

template<typename TObject>
class MovePtrDeleterBase
{
public:
    typedef void* (*Deleter)(TObject*);

    MovePtrDeleterBase(Deleter deleter): 
        m_Allocator(0) 
    { 
        m_DeleteFunction = deleter; 
    }
    
    MovePtrDeleterBase(Deleter deleter, os::IAllocator* allocator): 
        m_Allocator(allocator)
    { 
        m_DeleteFunction = deleter; 
    }
    
    void operator() (TObject* object)
    {
        void* memory = m_DeleteFunction(object);
        if (m_Allocator)
        {
            m_Allocator->Free(memory);
        }
    }
    
    static Deleter m_DeleteFunction;
    os::IAllocator* m_Allocator;
};

template<class TObject>
typename MovePtrDeleterBase<TObject>::Deleter MovePtrDeleterBase<TObject>::m_DeleteFunction;

template<typename TScalar>
class MovePtrScalarDeleter : public MovePtrDeleterBase<TScalar>
{
public:
    typedef MovePtrDeleterBase<TScalar> base;
    MovePtrScalarDeleter() : base(DoDelete) { }
    MovePtrScalarDeleter(os::IAllocator* allocator) : base(DoDelete, allocator) { }

    static void* DoDelete(TScalar* scalar)
    {
        scalar->~TScalar();
        return scalar;
    }
};

template<typename TArray>
class MovePtrArrayDeleter :  public MovePtrDeleterBase<typename remove_bounds<TArray>::type>
{
public:
    typedef typename remove_bounds<TArray>::type element_type;
    typedef MovePtrDeleterBase<element_type> base;

    MovePtrArrayDeleter(): 
        base(DoDelete) {}
    MovePtrArrayDeleter(os::IAllocator* allocator): 
        base(DoDelete, allocator) {}

    static void* DoDelete(element_type* array)
    {
        size_t arraySize = *(reinterpret_cast<int*>(array) - 1);
        
        element_type* end = array + arraySize;
        for (element_type* i = array; i != end; ++i)
        {
            i->~element_type();
        }

        return reinterpret_cast<char*>(array) - 4;
    }
};

template<typename TPointer>
class MoveSource
{
public:
    MoveSource(TPointer& pointer): 
        m_Pointer(pointer) {}

    TPointer& Ptr() const { return m_Pointer; }

private:
    TPointer& m_Pointer;
    MoveSource(const TPointer&);
};

}

template<typename TPointer>
internal::MoveSource<TPointer> Move(TPointer& pointer)
{
    return internal::MoveSource<TPointer>(pointer);
}

template<typename TObject>
class MovePtrDefaultDeleter : public If_< IsArray<TObject>,internal::MovePtrArrayDeleter<TObject>,internal::MovePtrScalarDeleter<TObject> >::type
{
public:
    MovePtrDefaultDeleter(os::IAllocator* allocator = 0) { this->m_Allocator = allocator; }

    template<typename TTObject>
    MovePtrDefaultDeleter(MovePtrDefaultDeleter<TTObject> object) { }
};

template<typename TObject, typename TDeleter = MovePtrDefaultDeleter<TObject> >
class MovePtr
{
public:
    typedef typename remove_bounds<TObject>::type element_type;
    typedef TDeleter deleter_type;
    typedef TDeleter& deleter_reference;
    typedef const TDeleter& deleter_const_reference;

    struct SafeBoolHelper 
    { 
        int x; 
    };

    typedef int SafeBoolHelper::* SafeBool;

    MovePtr(): 
        m_Object(0), 
        m_Deleter() {}

    MovePtr(const MovePtr& pointer): 
        m_Object(const_cast<element_type*>(pointer.Get())), 
        m_Deleter(pointer.GetDeleter())
    {
        const_cast<MovePtr&>(pointer).Release();
    }

    template<typename TTObject>
    explicit MovePtr(TTObject* pointer): 
        m_Object(pointer), 
        m_Deleter() {}

    template<typename TTObject>
    MovePtr(TTObject* pointer, os::IAllocator* allocator): 
        m_Object(pointer), 
        m_Deleter(allocator) {}

    template<typename TTObject, typename TTDeleter>
    MovePtr(TTObject* pointer, TTDeleter deleter): 
        m_Object(pointer), 
        m_Deleter(deleter) {}

    template<typename TTObject, typename TTDeleter>
    MovePtr(internal::MoveSource<MovePtr<TTObject, TTDeleter> > source):
        m_Object(source.Ptr().Get()), 
        m_Deleter(source.Ptr().GetDeleter())
    {
        source.Ptr().Release();
    }

    ~MovePtr()
    {
        if (this->Ptr())
        {
            this->GetDeleter()((this->Ptr()));
        }
    }

    MovePtr& operator=(MovePtr rhs)
    {
        rhs.Swap(*this);
        return *this;
    }

    element_type* Get() const { return Ptr(); }

    element_type& operator*() const
    {
        return *Ptr();
    }

    element_type* operator->() const
    {
        return Ptr();
    }

    element_type& operator[](std::size_t i) const
    {
        return Ptr()[i];
    }

    element_type* Release()
    {
        element_type* result = Ptr();
        Ptr() = 0;
        return result;
    }

    void Reset()
    {
        if (Ptr())
        {
            GetDeleter()(Ptr());
        }
        Ptr() = 0;
    }

    template<typename TTObject>
    void Reset(TTObject* object)
    {
        MovePtr(object).Swap(*this);
    }

    template<typename TTObject, typename TTDeleter>
    void Reset(TTObject* object, TTDeleter deleter)
    {
        MovePtr(object, deleter).Swap(*this);
    }

    template<typename TTObject>
    void Reset(TTObject* object, os::IAllocator* allocator)
    {
        MovePtr(object, TDeleter(allocator)).Swap(*this);
    }

    operator SafeBool() const { return Ptr() ? &SafeBoolHelper::x : 0; }

    void Swap(MovePtr& pointer)
    {
        if (&pointer == this)
        {
            return;
        }
        
        element_type* object = this->m_Object;
        deleter_type deleter = this->m_Deleter;

        this->m_Object = pointer.m_Object;
        this->m_Deleter = pointer.m_Deleter;

        pointer.m_Object = object;
        pointer.m_Deleter = deleter;
    }

    deleter_reference GetDeleter() { return m_Deleter; }
    deleter_const_reference GetDeleter() const { return m_Deleter; }

private:
    template<typename Pointer> struct CantMoveFromConst;
    template<typename TTObject, typename TTDeleter>
    struct CantMoveFromConst<const MovePtr<TTObject, TTDeleter> >
    {
        typedef typename MovePtr<TTObject, TTDeleter>::error type;
    };

    template<typename Pointer>
    MovePtr(Pointer&, typename CantMoveFromConst<Pointer>::type = 0);

    element_type*& Ptr() { return m_Object; }
    element_type* Ptr() const { return m_Object; }
    
    element_type* m_Object;
    deleter_type m_Deleter;
};

}
}