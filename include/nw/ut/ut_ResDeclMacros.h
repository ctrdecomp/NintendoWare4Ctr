#pragma once

#include <nw/types.h>
#include <nw/ut/ut_ResTypes.h>
#include <nw/ut/ut_Iterator.h>
#include <nw/ut/ut_ResArray.h>
#include <nw/ut/ut_Flag.h>
#include <cstring>

#define NN


#define NW_RES_FIELD_PRIMITIVE_DECL( type, name )                                   \
    type        Get##name() const { return static_cast<type>(ref().m##name); }     \
    void        Set##name(type value) { ref().m##name = value; }                   \

#define NW_RES_FIELD_FLAGS_DECL( type, name )                                       \
    type        Get##name() const { return static_cast<type>(ref().m##name); }     \
    void        Set##name(type value) { ref().m##name = value; }                   \
    void        Enable##name(type value) { ref().m##name = nw::ut::EnableFlag(ref().m##name, value); }   \
    void        Disable##name(type value) { ref().m##name = nw::ut::DisableFlag(ref().m##name, value); } \

#define NW_RES_FIELD_BOOL_PRIMITIVE_DECL( name )                                    \
    bool        Is##name() const { return static_cast<bool>(ref().mIs##name); }    \
    void        Set##name(bool value) { ref().mIs##name = value; }                 \


#define NW_RES_FIELD_STRING_DECL( name )                                            \
    const char*     Get##name() const { return ref().to##name.to_ptr(); }           \


#define NW_RES_FIELD_CLASS_DECL( type, name )                                       \
    type        Get##name() { return type( ref().to##name.to_ptr() ); }             \
    const type  Get##name() const { return type( ref().to##name.to_ptr() ); }       \

#define NW_RES_FIELD_STRUCT_DECL( type, name )                                      \
    type&       Get##name() { return ref().m##name; }                              \
    const type& Get##name() const { return ref().m##name; }                        \
    void        Set##name(const type& value) { ref().m##name = value; }            \

#define NW_RES_FIELD_VECTOR2_DECL( type, name )                                     \
    void        Set##name( f32 x, f32 y ) { ref().m##name.Set(x, y); }   \
    const type& Get##name() const { return ref().m##name; }                        \
    void        Set##name(const type& value) { ref().m##name = value; }            \

#define NW_RES_FIELD_VECTOR3_DECL( type, name )                                     \
    void        Set##name( f32 x, f32 y, f32 z ) { ref().m##name.Set(x, y, z); }   \
    const type& Get##name() const { return ref().m##name; }                        \
    void        Set##name(const type& value) { ref().m##name = value; }            \

#define NW_RES_FIELD_RECT_DECL( type, name )                                        \
    void        Set##name##WithoutFlip( f32 l, f32 r, f32 b, f32 t ) { ref().m##name.SetWithoutFlip(l, r, b, t); }   \
    void        Set##name##WithYFlip( f32 l, f32 r, f32 b, f32 t ) { ref().m##name.SetWithYFlip(l, r, b, t); }   \
    const type& Get##name() const { return ref().m##name; }                        \
    void        Set##name(const type& value) { ref().m##name = value; }            \

#define NW_RES_FIELD_FLOAT_COLOR_DECL( type, name )                                 \
    void        Set##name( f32 r, f32 g, f32 b ) { ref().m##name.Set(r, g, b); }          \
    void        Set##name( f32 r, f32 g, f32 b, f32 a ) { ref().m##name.Set(r, g, b, a); } \
    const       type& Get##name() const { return ref().m##name; }                        \
    void        Set##name(const type& value) { ref().m##name = value; }            \

#define NW_RES_FIELD_FLOAT_U32_COLOR_DECL( type, name )                             \
    u32         Get##name##U32() const { return ref().m##name##U32; }                      \
    void        Set##name( f32 r, f32 g, f32 b ) { ref().m##name.Set(r, g, b); ref().m##name##U32 = ref().m##name.ToPicaU32(); } \
    void        Set##name( f32 r, f32 g, f32 b, f32 a ) { ref().m##name.Set(r, g, b, a); ref().m##name##U32 = ref().m##name.ToPicaU32(); } \
    const type& Get##name() const { return ref().m##name; }                        \
    void        Set##name(const type& value) { ref().m##name = value; ref().m##name##U32 = ref().m##name.ToPicaU32(); } \

#define NW_RES_FIELD_RESSTRUCT_DECL( type, name )                                   \
    type       Get##name() { return type( &(ref().m##name) ); }                    \
    const type Get##name() const { return type( &(ref().m##name) ); }              \
    type##Data&       Get##name##Data() { return ref().m##name; }                  \
    const type##Data& Get##name##Data() const { return ref().m##name; }            \

#define NW_RES_FIELD_CONST_RESSTRUCT_DECL( type, name )                             \
    const type Get##name() const { return type( &(ref().m##name) ); }              \
    const type##Data& Get##name##Data() const { return ref().m##name; }            \

#define NW_RES_FIELD_PRIMITIVE_FIXED_LIST_DECL(type, name)                         \
    s32 Get##name##Count() const                                                   \
        { return sizeof(ref().m##name) / sizeof(type); }                           \
                                                                                   \
    type Get##name(int idx) const                                                  \
        { return ref().m##name[idx]; }                                             \
                                                                                   \
    void Set##name(int idx, type value)                                            \
        { ref().m##name[idx] = value; }                                            \
                                                                                   \
    nw::ut::internal::ResArray<                                                    \
        type,                                                                      \
        nw::ut::internal::ResArrayPrimitiveTraits                                  \
    > Get##name()                                                                  \
    {                                                                              \
        return nw::ut::internal::ResArray<                                         \
            type,                                                                  \
            nw::ut::internal::ResArrayPrimitiveTraits                              \
        >(&(ref().m##name[0]), Get##name##Count());                                \
    }                                                                              \
                                                                                   \
    nw::ut::internal::ResArray<                                                    \
        const type,                                                                \
        nw::ut::internal::ResArrayPrimitiveTraits                                  \
    > Get##name() const                                                            \
    {                                                                              \
        return nw::ut::internal::ResArray<                                         \
            const type,                                                            \
            nw::ut::internal::ResArrayPrimitiveTraits                              \
        >(&(ref().m##name[0]), Get##name##Count());                                \
    }


#define NW_RES_FIELD_STRING_FIXED_LIST_DECL(name)                                  \
    s32 Get##name##Count() const                                                   \
        { return sizeof(ref().to##name) / sizeof(nw::ut::BinString); }             \
                                                                                   \
    const char* Get##name(int idx) const;                                          \
                                                                                   \
    nw::ut::internal::ResArray<                                                    \
        const nw::ut::BinString,                                                   \
        nw::ut::internal::ResArrayPrimitiveTraits                                  \
    > Get##name() const                                                            \
    {                                                                              \
        return nw::ut::internal::ResArray<                                         \
            const nw::ut::BinString,                                               \
            nw::ut::internal::ResArrayPrimitiveTraits                              \
        >(&(ref().to##name[0]), Get##name##Count());                               \
    }


#define NW_RES_FIELD_CLASS_FIXED_LIST_DECL( type, name )                                 \
    s32      Get##name##Count() const                                                    \
        { return sizeof(ref().to##name) / sizeof(nw::ut::Offset); }                      \
    type        Get##name(int idx)                                                       \
        { NW_INDEX_ASSERT( name, idx ); return type( ref().to##name[ idx ].to_ptr() ); } \
    const type  Get##name(int idx) const                                                 \
        { return type( ref().to##name[ idx ].to_ptr() ); }                               \
    nw::ut::internal::ResArray<type, nw::ut::internal::ResArrayClassTraits> Get##name()  \
        { return nw::ut::internal::ResArray<type, nw::ut::internal::ResArrayClassTraits>( &(ref().to##name[ 0 ]), Get##name##Count() ); } \
    const nw::ut::internal::ResArray<const type, nw::ut::internal::ResArrayClassTraits> Get##name() const \
        { return nw::ut::internal::ResArray<const type, nw::ut::internal::ResArrayClassTraits>( &(ref().to##name[ 0 ]), Get##name##Count() ); } \


#define NW_RES_FIELD_STRUCT_FIXED_LIST_DECL(type, name)                            \
    s32 Get##name##Count() const                                                   \
        { return sizeof(ref().m##name) / sizeof(type##Data); }                     \
                                                                                   \
    type Get##name(int idx)                                                        \
        { return type(&ref().m##name[idx]); }                                      \
                                                                                   \
    const type Get##name(int idx) const                                            \
        { return type(&ref().m##name[idx]); }                                      \
                                                                                   \
    type##Data& Get##name##Data(int idx)                                           \
        { return ref().m##name[idx]; }                                             \
                                                                                   \
    const type##Data& Get##name##Data(int idx) const                               \
        { return ref().m##name[idx]; }                                             \
                                                                                   \
    type##Data* Get##name##Data()                                                  \
        { return ref().m##name; }                                                  \
                                                                                   \
    const type##Data* Get##name##Data() const                                      \
        { return ref().m##name; }


#define NW_RES_FIELD_PRIMITIVE_LIST_DECL( type, name )                              \
    s32      Get##name##Count() const                                               \
        { return ref().m##name##TableCount; }                                      \
    type        Get##name(int idx) const                                            \
        {                                                                           \
            const type* tbl_ptr = static_cast<const type*>( ref().to##name##Table.to_table_ptr() ); \
            return tbl_ptr[ idx ];                                                  \
        }                                                                           \
    void        Set##name(int idx, type value)                                      \
        {                                                                           \
            type* tbl_ptr = Get##name();                                            \
            tbl_ptr[ idx ] = value;                                                 \
        }                                                                           \
    type*       Get##name()                                                         \
        { return static_cast<type*>(ref().to##name##Table.to_table_ptr()); }        \
    const type* Get##name() const                                                   \
        { return static_cast<const type*>(ref().to##name##Table.to_table_ptr()); }  \



#define NW_RES_FIELD_STRING_LIST_DECL( name )                                       \
    s32      Get##name##Count() const                                               \
        { return ref().m##name##TableCount; }                                      \
    const char* Get##name(int idx) const                                            \
        {                                                                           \
            const BinString* tbl_ptr = static_cast<const BinString*>( ref().to##name##Table.to_table_ptr() ); \
            return tbl_ptr[ idx ].to_ptr();                                         \
        }                                                                           \


#define NW_RES_FIELD_CLASS_LIST_DECL( type, name )                                  \
    s32      Get##name##Count() const                                               \
        { return ref().m##name##TableCount; }                                      \
    type        Get##name(int idx)                                                  \
        {                                                                           \
            nw::ut::Offset* tbl_ptr = static_cast<nw::ut::Offset*>( ref().to##name##Table.to_table_ptr() ); \
            return type( tbl_ptr[ idx ].to_ptr() );                                 \
        }                                                                           \
    const type  Get##name(int idx) const                                            \
        {                                                                           \
            const nw::ut::Offset* tbl_ptr = static_cast<const nw::ut::Offset*>( ref().to##name##Table.to_table_ptr() ); \
            return type( tbl_ptr[ idx ].to_ptr() );                                 \
        }                                                                           \
    nw::ut::internal::ResArray<type, nw::ut::internal::ResArrayClassTraits> Get##name() \
        {                                                                           \
            return nw::ut::internal::ResArray<type, nw::ut::internal::ResArrayClassTraits>( ref().to##name##Table.to_table_ptr(), Get##name##Count() );  \
        }                                                                           \
    const nw::ut::internal::ResArray<const type, nw::ut::internal::ResArrayClassTraits> Get##name() const \
        {                                                                           \
            return nw::ut::internal::ResArray<const type, nw::ut::internal::ResArrayClassTraits>( ref().to##name##Table.to_table_ptr(), Get##name##Count() );  \
        }                                                                           \


#define NW_RES_FIELD_STRUCT_LIST_DECL( type, name )                                 \
    s32              Get##name##Count() const                                       \
        { return ref().m##name##TableCount; }                                      \
    type##Data*         Get##name##Data()                                           \
        {                                                                           \
            return static_cast<type##Data*>( ref().to##name##Table.to_table_ptr() ); \
        }                                                                           \
    const type##Data*   Get##name##Data() const                                     \
        {                                                                           \
            return static_cast<const type##Data*>( ref().to##name##Table.to_table_ptr() ); \
        }                                                                           \
    type##Data&         Get##name##Data(int idx)                                    \
        {                                                                           \
            type##Data* tbl_ptr = this->Get##name##Data();                          \
            return tbl_ptr[ idx ];                                                  \
        }                                                                           \
    const type##Data&   Get##name##Data(int idx) const                              \
        {                                                                           \
            const type##Data* tbl_ptr = this->Get##name##Data();                    \
            return tbl_ptr[ idx ];                                                  \
        }                                                                           \
    type                Get##name(int idx)                                          \
        {                                                                           \
            return type( &(this->Get##name##Data(idx)) );                           \
        }                                                                           \
    const type          Get##name(int idx) const                                    \
        {                                                                           \
            return type( &(this->Get##name##Data(idx)) );                           \
        }                                                                           \


#define NW_RES_FIELD_CLASS_DIC_DECL( type, name, ResDicType )                       \
    s32      Get##name##Count() const                                               \
        {                                                                           \
            return ref().m##name##DicCount;                                        \
        }                                                                           \
    type        Get##name(int idx)                                                  \
        {                                                                           \
            ResDicType dic = ResDicType( ref().to##name##Dic.to_ptr() );            \
            return type( dic[ idx ] );                                              \
        }                                                                           \
    type        Get##name(const char* key)                                          \
        {                                                                           \
            ResDicType dic = ResDicType( ref().to##name##Dic.to_ptr() );            \
            return type( dic[ key ] );                                              \
        }                                                                           \
    const type  Get##name(int idx) const                                            \
        {                                                                           \
            ResDicType dic = ResDicType( ref().to##name##Dic.to_ptr() );            \
            return type( dic[ idx ] );                                              \
        }                                                                           \
    const type  Get##name(const char* key) const                                    \
        {                                                                           \
            ResDicType dic = ResDicType( ref().to##name##Dic.to_ptr() );            \
            return type( dic[ key ] );                                              \
        }                                                                           \
    s32         Get##name##Index(const char* key) const                             \
        {                                                                           \
            ResDicType dic = ResDicType( ref().to##name##Dic.to_ptr() );            \
            return dic.GetIndex( key );                                             \
        }                                                                           \
    nw::ut::internal::ResArray< type, nw::ut::internal::ResArrayDicTraits<ResDicType##Data>::Traits > Get##name() \
        {                                                                           \
            ResDicType dic = ResDicType( ref().to##name##Dic.to_ptr() );            \
            return  nw::ut::internal::ResArray<                                     \
                        type,                                                       \
                        nw::ut::internal::ResArrayDicTraits<ResDicType##Data>::Traits \
                    >( (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, Get##name##Count() ); \
        }                                                                           \
    const nw::ut::internal::ResArray< const type, nw::ut::internal::ResArrayDicTraits<ResDicType##Data>::Traits > Get##name() const \
        {                                                                           \
            ResDicType dic = ResDicType( ref().to##name##Dic.to_ptr() );            \
            return  nw::ut::internal::ResArray<                                     \
                        const type,                                                 \
                        nw::ut::internal::ResArrayDicTraits<ResDicType##Data>::Traits \
                    >( (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, Get##name##Count() ); \
        }                                                                           \


#define NW_RES_FIELD_PRIMITIVE_LIST_INLINE_DECL( type, name )                       \
    s32      Get##name##Count() const                                               \
        { return ref().m##name##Count; }                                           \
    type        Get##name(int idx) const                                            \
        {                                                                           \
            return ref().m##name##Table[ idx ];                                    \
        }                                                                           \
    void        Set##name(int idx, type value)                                      \
        {                                                                           \
            ref().m##name##Table[ idx ] = value;                                   \
        }                                                                           \
    type*       Get##name()                                                         \
        { return static_cast<type*>(ref().m##name##Table); }                       \
    const type* Get##name() const                                                   \
        { return static_cast<const type*>(ref().m##name##Table); }                 \


#define NW_RES_FIELD_STRING_LIST_INLINE_DECL( name )                                \
    s32      Get##name##Count() const                                               \
        { return ref().m##name##Count; }                                           \
    const char* Get##name(int idx) const                                            \
        {                                                                           \
            return ref().m##name##Table[ idx ].to_ptr();                           \
        }                                                                           \


#define NW_RES_FIELD_CLASS_LIST_INLINE_DECL( type, name )                           \
    s32      Get##name##Count() const                                               \
        { return ref().m##name##Count; }                                           \
    type        Get##name(int idx)                                                  \
        {                                                                                                                    \
            return type( ref().m##name##Table[ idx ].to_ptr() );                   \
        }                                                                           \
    const type  Get##name(int idx) const                                            \
        {                                                                                                                     \
            return type( ref().m##name##Table[ idx ].to_ptr() );                   \
        }                                                                           \
    nw::ut::internal::ResArray<type, nw::ut::internal::ResArrayClassTraits> Get##name() \
        {                                                                           \
            return nw::ut::internal::ResArray<                                      \
                        type,                                                       \
                        nw::ut::internal::ResArrayClassTraits                       \
                   >( &(ref().m##name##Table[0]), Get##name##Count() );            \
        }                                                                           \
    const nw::ut::internal::ResArray<const type, nw::ut::internal::ResArrayClassTraits> Get##name() const \
        {                                                                           \
            return nw::ut::internal::ResArray<                                      \
                        const type,                                                 \
                        nw::ut::internal::ResArrayClassTraits                       \
                   >( &(ref().m##name##Table[0]), Get##name##Count() );            \
        }                                                                           \

#define NW_RES_FIELD_COMMAND_BUFFER()                                               \
    u32* GetCommandBuffer()                                                         \
        {                                                                           \
            return &ref().mCommandBuffer[0];                                       \
        }                                                                           \
    const u32* GetCommandBuffer() const                                             \
        {                                                                           \
            return &ref().mCommandBuffer[0];                                       \
        }                                                                           