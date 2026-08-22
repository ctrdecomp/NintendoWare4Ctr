#pragma once

#include <stddef.h>
#include <nw/types.h>

namespace nw {
namespace ut   {

#define NW_UT_RUNTIME_TYPEINFO \
    virtual const nw::ut::internal::RuntimeTypeInfo*    GetRuntimeTypeInfo() const { return &s_TypeInfo; } \
    static  const nw::ut::internal::RuntimeTypeInfo     s_TypeInfo
    
#define NW_UT_RUNTIME_TYPEINFO_DEFINITION(derived,base) \
    const nw::ut::internal::RuntimeTypeInfo derived::s_TypeInfo( &base::s_TypeInfo )

#define NW_UT_RUNTIME_TYPEINFO_ROOT_DEFINITION(root) \
    const nw::ut::internal::RuntimeTypeInfo root::s_TypeInfo( NULL )


namespace internal {

struct RuntimeTypeInfo{
    const RuntimeTypeInfo*  mParentTypeInfo;
    
    explicit RuntimeTypeInfo( const RuntimeTypeInfo* parent) : mParentTypeInfo(parent) {}
    bool IsDerivedFrom( const RuntimeTypeInfo* s_TypeInfo ) const{
        const RuntimeTypeInfo *self = this;
        while (self){
            if (self == s_TypeInfo){
                return true;
            }
            self = self->mParentTypeInfo;
        }
        return false;
    }
};

template<class T>
inline const RuntimeTypeInfo* GetTypeInfoFromPtr_(T* /* dummy */){
    return &T::s_TypeInfo;
}

}


template<class UPtr, class T>
inline UPtr DynamicCast(T* obj){
    const internal::RuntimeTypeInfo* s_TypeInfoU = internal::GetTypeInfoFromPtr_(UPtr(0));
    if ( obj && obj->GetRuntimeTypeInfo()->IsDerivedFrom(s_TypeInfoU) ){
        return static_cast<UPtr>(obj);
    }
    return NULL;
}


template <typename T, typename U>
inline bool IsTypeOf(const U* instance){
    if (instance == NULL) { return false; }
    return instance->GetRuntimeTypeInfo() == &T::s_TypeInfo;
}



}
}