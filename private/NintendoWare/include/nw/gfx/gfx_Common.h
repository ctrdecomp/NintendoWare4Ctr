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

#ifndef NW_GFX_COMMON_H_
#define NW_GFX_COMMON_H_

#include <nw/types.h>
#include <nw/os/os_Memory.h>
#include <nw/ut/ut_BinaryFileFormat.h>
#include <nw/ut/ut_RuntimeTypeInfo.h>
#include <nw/ut/ut_ResUtil.h>
#include <nw/gfx/gfx_Config.h>

#include <algorithm>

namespace nw {
namespace gfx {

namespace res {}
using namespace ::nw::gfx::res;

//
enum PlatformConstants
{
    VERTEX_ATTRIBUTE_COUNT = 12,
    COMBINER_COUNT = 6,
    COMBINER_AVAILABLE_COUNT = COMBINER_COUNT,
    PIXELBASED_TEXTURE_UNIT_COUNT = 3,
    LOOKUP_TABLE_COUNT = 32,
    LIGHT_COUNT = 8,
    TEXTURE_COORDINATE_COUNT = 3,
    FOG_TABLE_SIZE = 256
};

#define NW_ENSURE_MEMORY_ENABLED

#if defined (NW_ENSURE_MEMORY_ENABLED)

  #define NW_ENSURE_AND_RETURN(result) \
    if ( result.IsFailure() ) { return result; }

#else
  #define NW_ENSURE_AND_RETURN(result) \
    NW_ASSERT( result.IsSuccess() );

#endif


#if defined( NW_ASSERTION_ENABLE )
  #define NW_ENSURE_AND_ASSERT(exp)     \
    do {                                \
      Result result = exp;              \
      NW_ASSERT( result.IsSuccess() );  \
    } while (0)
#else
  #define NW_ENSURE_AND_ASSERT(exp)     exp
#endif


//
//
//
//
//
class Result
{
    typedef Result self_type; //
public:
    //
    Result() : m_Code(0)
    {}

    Result(u32 code) : m_Code(code)
    {}

    //
    static const bit32 MASK_FAIL_BIT        = 0x80000000;
    //
    static const bit32 MASK_DESCRIPTION     = 0x0000ffff;

    //
    self_type operator = (u32 code) { m_Code = code; return *this; }
    //
    self_type operator |= (u32 code) { m_Code |= code; return *this; }
    //
    self_type operator &= (u32 code) { m_Code &= code; return *this; }
    //
    self_type operator = (self_type result) { m_Code = result.GetCode(); return *this; }
    //
    self_type operator |= (self_type result) { m_Code |= result.GetCode(); return *this; }
    //
    self_type operator &= (self_type result) { m_Code &= result.GetCode(); return *this; }

    //
    u32 GetCode() const
    {
        return m_Code;
    }

    //
    bool IsFailure() const
    {
        return (m_Code & MASK_FAIL_BIT) != 0;
    }

    //
    bool IsSuccess() const
    {
        return !IsFailure();
    }

    //
    int GetDescription() const
    {
        return static_cast<int>( m_Code & MASK_DESCRIPTION );
    }

private:
    u32 m_Code;
};

//
enum ResourceResult
{
    RESOURCE_RESULT_OK = 0, //
    RESOURCE_RESULT_NOT_FOUND_TEXTURE = 1 << 0, //
    RESOURCE_RESULT_NOT_FOUND_SHADER = 1 << 1, //
    RESOURCE_RESULT_NOT_FOUND_LUT  = 1 << 2, //
    RESOURCE_RESULT_IRRELEVANT_LOCATION_LUT  = 1 << 3, //
    RESOURCE_RESULT_IRRELEVANT_LOCATION_SHADER_SYMBOL  = 1 << 4, //
    RESOURCE_RESULT_IRRELEVANT_TEXTURE_MAPPING_METHOD = 1 << 5 //
};

//
enum InitializeResult
{
    INITIALIZE_RESULT_OK = 0 //
};

//
enum BindResult
{
    BIND_RESULT_OK = 0, //
    BIND_RESULT_NOT_ALL_ANIM_MEMBER_BOUND = 1 << 0, //
    BIND_RESULT_NO_MEMBER_BOUND = 1 << 1, //
    BIND_RESULT_IRRELEVANT_ANIM_TYPE = 1 << 2 //
};

// Since nw::gfx::ResXXX inherits nw::ut::ResCommon, the Koenig Lookup comes into play and a determination between ut and gfx cannot be made.
// There should be no problem if you draw in gfx from ut with using.

using nw::ut::ResStaticCast;
using nw::ut::ResDynamicCast;

using nw::os::AllocateAndFill;
using nw::os::AllocateAndFillN;
using nw::os::AllocateAndAssignN;
using nw::os::AllocateAndCopyString;

namespace internal {

//---------------------------------------------------------------------------
//
//
//
//
//
//
//
//
//---------------------------------------------------------------------------
template<typename TRes>
NW_INLINE bool
ResCheckRevision(const TRes res )
{
    if (!res.IsValid()) { return false; }

    return nw::ut::internal::CheckRevision( res.GetRevision(), TRes::BINARY_REVISION );
}

} // namespace internal
} // namespace gfx
} // namespace nw

#endif // NW_GFX_COMMON_H_

