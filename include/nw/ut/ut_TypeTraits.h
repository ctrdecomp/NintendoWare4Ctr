#pragma once

namespace nw {
namespace ut {
namespace internal {

template<typename TType, TType Value>
struct integral_constant{
    static const TType value = Value;
    typedef TType value_type;
};

typedef integral_constant<bool, true> TrueType;
typedef integral_constant<bool, false> FalseType;

template<bool Cond, typename Then, typename Else>
struct IfCond;

template<typename Then, typename Else>
struct IfCond<true, Then, Else>{
    typedef Then type;
};

template<typename Then, typename Else>
struct IfCond<false, Then, Else>{
    typedef Else type;
};

struct True{
    char a;
};

struct False{
    True a[2];
};

}

#define NW_UT_DECLARE_TYPE_REMOVE_TEMPLATE_BASE_(name, ttype) \
template<typename ttype> \
struct name{ \
    typedef ttype type; \
};

#define NW_UT_DECLARE_TYPE_REMOVE_TEMPLATE_(name, ttype, modifier) \
template<typename ttype> \
struct name<modifier>{ \
    typedef TType type; \
};

#define NW_UT_DECLARE_TYPE_REMOVE_TEMPLATE2_(name, ttype, modifier, ...) \
template<typename __VA_ARGS__> \
struct name<modifier>{ \
    typedef TType type; \
};

NW_UT_DECLARE_TYPE_REMOVE_TEMPLATE_BASE_(remove_bounds, TType)
NW_UT_DECLARE_TYPE_REMOVE_TEMPLATE_(remove_bounds, TType, TType[])
NW_UT_DECLARE_TYPE_REMOVE_TEMPLATE2_(remove_bounds, TType, TType[Size], TType, int Size)

NW_UT_DECLARE_TYPE_REMOVE_TEMPLATE_BASE_(remove_const, TType)
NW_UT_DECLARE_TYPE_REMOVE_TEMPLATE_(remove_const, TType, const TType)

NW_UT_DECLARE_TYPE_REMOVE_TEMPLATE_BASE_(remove_reference, TType)
NW_UT_DECLARE_TYPE_REMOVE_TEMPLATE_(remove_reference, TType, TType&)

NW_UT_DECLARE_TYPE_REMOVE_TEMPLATE_BASE_(remove_pointer, TType)
NW_UT_DECLARE_TYPE_REMOVE_TEMPLATE_(remove_pointer, TType, TType*)
NW_UT_DECLARE_TYPE_REMOVE_TEMPLATE_(remove_pointer, TType, TType* const)

#define NW_TRAITS_SPEC0(Spec, Value) \
template<> \
struct Spec : public internal::integral_constant<bool, Value>{};

#define NW_TRAITS_SPEC1(Spec, Value) \
template<typename Type> \
struct Spec : public internal::integral_constant<bool, Value>{};

#define NW_TRAITS_SPEC2(Spec, Value) \
template<typename Type, typename ClassName> \
struct Spec : public internal::integral_constant<bool, Value>{};

#define NW_TRAITS_SPEC(Order, Traits, SpecialType, Value) \
NW_TRAITS_SPEC##Order(Traits<SpecialType>, Value) \
NW_TRAITS_SPEC##Order(Traits<SpecialType const>, Value) \
NW_TRAITS_SPEC##Order(Traits<SpecialType volatile>, Value) \
NW_TRAITS_SPEC##Order(Traits<SpecialType const volatile>, Value)

template<typename TType>
struct IsArray : public internal::FalseType{};

template<typename TType, int Size>
struct IsArray<TType[Size]> : public internal::TrueType{};

template<typename TType>
struct IsArray<TType[]> : public internal::TrueType{};

template<typename Type>
struct IsPointer : public internal::FalseType{};

NW_TRAITS_SPEC(1, IsPointer, Type*, true)

template<typename, typename>
struct IsSame : public internal::FalseType{};

template<typename Type>
struct IsSame<Type, Type> : public internal::TrueType{};

template<class T>
struct IsClass{
    template<class C>
    static internal::True test(int C::*);

    template<class C>
    static internal::False test(...);

    static const bool value =
        (sizeof(test<T>(0)) == sizeof(internal::True));
};

template<typename Cond, typename Then, typename Else>
struct If_ : public internal::IfCond<Cond::value, Then, Else>{};

} // ut
} // nw