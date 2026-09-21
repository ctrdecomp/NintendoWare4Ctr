#pragma once

namespace nw{
namespace ut{
namespace internal{

struct static_any_base
{
    operator bool() const
    {
        return false;
    }
};

template <typename Type>
struct static_any : public static_any_base
{
    static_any(const Type& item) : m_Item(item) {}
    mutable Type m_Item;
};

typedef const static_any_base& static_any_t;


template <typename Type>
inline Type& static_any_cast(static_any_t value)
{
    return static_cast<const static_any<Type>&>(value).m_Item;
}

template <typename Type>
struct contain_type
{
    typedef Type type;
};

template <typename Type>
inline contain_type<Type>* encode_type(Type&)
{
    return 0;
}

template <typename Type>
inline contain_type<const Type>* encode_type(const Type&)
{
    return 0;
}

#define NW_FOREACH_TYPEOF(COL) \
    (true ? 0 : nw::ut::internal::encode_type(COL))


template <typename Type>
inline static_any<Type> contain(const Type& col)
{
    return col;
}

template <typename Type>
inline static_any<Type*> contain(Type& col)
{
    return &col;
}

template <typename Type, int Size>
inline static_any<Type*> contain(Type (&col)[Size])
{
    return col;
}

template <typename Type, int Size>
inline static_any<Type*> begin(static_any_t cur, contain_type<Type[Size]>*)
{
    return static_any_cast<Type*>(cur);
}

template <typename Type, int Size>
inline static_any<Type*> end(static_any_t cur, contain_type<Type[Size]>*)
{
    return static_any_cast<Type*>(cur) + Size;
}

template <typename Type, int Size>
inline void next(static_any_t cur, contain_type<Type[Size]>*)
{
    ++static_any_cast<Type*>(cur);
}

template <typename Type, int Size>
inline Type& extract(static_any_t cur, contain_type<Type[Size]>*)
{
    return *static_any_cast<Type*>(cur);
}


template <typename Type, int Size>
inline bool done(static_any_t cur, static_any_t end, contain_type<Type[Size]>*)
{
    return static_any_cast<Type*>(cur) == static_any_cast<Type*>(end);
}

template <typename Type>
inline static_any<typename Type::iterator> begin(static_any_t cur, contain_type<Type>*)
{
    return static_any_cast<Type*>(cur)->begin();
}

template <typename Type>
inline static_any<typename Type::const_iterator> begin(static_any_t cur, contain_type<const Type>*)
{
        return static_any_cast<Type>(cur).begin();
}

template <typename Type>
inline static_any<typename Type::iterator> end(static_any_t cur, contain_type<Type>*)
{
    return static_any_cast<Type*>(cur)->end();
}

template <typename Type>
inline static_any<typename Type::const_iterator> end(static_any_t cur, contain_type<const Type>*)
{
    return static_any_cast<Type>(cur).end();
}

template <typename Type>
inline void next(static_any_t cur, contain_type<Type>*)
{
    ++static_any_cast<typename Type::iterator>(cur);
}

template <typename Type>
inline typename Type::reference extract(static_any_t cur, contain_type<Type>*)
{
    return *static_any_cast<typename Type::iterator>(cur);
}

template <typename Type>
inline bool done(static_any_t cur, static_any_t end, contain_type<Type>*)
{
    typedef typename Type::iterator Iter;
    return static_any_cast<Iter>(cur) == static_any_cast<Iter>(end);
}

template <typename Type>
inline static_any<Type> begin(static_any_t cur, contain_type<std::pair<Type, Type> >*)
{
    return static_any_cast<std::pair<Type, Type> >(cur).first;
}

template <typename Type>
inline static_any<Type> end(static_any_t cur, contain_type<std::pair<Type, Type> >*)
{
    return static_any_cast<std::pair<Type, Type> >(cur).second;
}

template <typename Type>
inline void next(static_any_t cur, contain_type<std::pair<Type, Type> >*)
{
    ++static_any_cast<Type>(cur);
}

template <typename Type>
inline typename Type::reference extract(static_any_t cur, contain_type<std::pair<Type, Type> >*)
{
    return *static_any_cast<Type>(cur);
}

template <typename Type>
inline bool done(static_any_t cur, static_any_t end, contain_type<std::pair<Type, Type> >*)
{
    return static_any_cast<Type>(cur) == static_any_cast<Type>(end);
}

template <typename Type>
inline static_any<Type*> begin(static_any_t cur, contain_type<std::pair<Type*, Type*> >*)
{
    return static_any_cast<std::pair<Type*, Type*> >(cur).first;
}

template <typename Type>
inline static_any<Type*> end(static_any_t cur, contain_type<std::pair<Type*, Type*> >*)
{
    return static_any_cast<std::pair<Type*, Type*> >(cur).second;
}

template <typename Type>
inline void next(static_any_t cur, contain_type<std::pair<Type*, Type*> >*)
{
    ++static_any_cast<Type*>(cur);
}

template <typename Type>
inline Type& extract(static_any_t cur, contain_type<std::pair<Type*, Type*> >*)
{
    return *static_any_cast<Type*>(cur);
}

template <typename Type>
inline bool done(static_any_t cur, static_any_t end, contain_type<std::pair<Type*, Type*> >*)
{
    return static_any_cast<Type*>(cur) == static_any_cast<Type*>(end);
}

}
}
}

#define NW_FOREACH_CONTAIN(COL) \
    nw::ut::internal::contain(COL)

#define NW_FOREACH_BEGIN(COL) \
    nw::ut::internal::begin(_contain, NW_FOREACH_TYPEOF(COL))

#define NW_FOREACH_END(COL) \
    nw::ut::internal::end(_contain, NW_FOREACH_TYPEOF(COL))

#define NW_FOREACH_DONE(COL) \
    nw::ut::internal::done(_cur, _end, NW_FOREACH_TYPEOF(COL))

#define NW_FOREACH_NEXT(COL) \
    nw::ut::internal::next(_cur, NW_FOREACH_TYPEOF(COL))

#define NW_FOREACH_EXTRACT(COL) \
    nw::ut::internal::extract(_cur, NW_FOREACH_TYPEOF(COL))

#define NW_FOREACH(VAR, COL) \
    if       (nw::ut::internal::static_any_t _contain = NW_FOREACH_CONTAIN(COL)) {} \
    else if  (nw::ut::internal::static_any_t _cur     = NW_FOREACH_BEGIN(COL)) {} \
    else if  (nw::ut::internal::static_any_t _end     = NW_FOREACH_END(COL)) {} \
    else for (bool _continue = true;                                                \
              _continue && !NW_FOREACH_DONE(COL);                                   \
              _continue ? NW_FOREACH_NEXT(COL) : (void)0)                           \
             if      ((_continue = false) == true) {}                               \
             else for (VAR = NW_FOREACH_EXTRACT(COL); !_continue; _continue = true)
