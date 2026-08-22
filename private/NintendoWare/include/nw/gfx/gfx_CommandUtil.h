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
#ifndef NW_GFX_COMMAND_UTIL_H_
#define NW_GFX_COMMAND_UTIL_H_

#include <nw/types.h>
#include <nw/ut/ut_Inlines.h>
#include <nw/gfx/gfx_GlImplement.h>
#include <nw/dev.h>
#ifdef NW_PLATFORM_CTR
#include <nw/gfx/gfx_MemoryUtil.h>
#endif

// #define NW_UNIFORM_DUMP

#include <gles2/gl2.h>
#include <gles2/gl2ext.h>
#include <nn/gx.h>
#include <cstring>

namespace nw {
namespace gfx {


namespace internal {


//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class CommandHeader
{
public:
    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    CommandHeader(u64 rawData) : m_RawData( rawData >> 32 ) {}

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    CommandHeader(u32 rawData) : m_RawData( rawData ) {}

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    u32 GetAddress() const
    {
        return (this->m_RawData & ADDRESS_MASK);
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    s32 GetSize() const
    {
        return ((this->m_RawData & SIZE_MASK) >> SIZE_SHIFT) + 1;
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    u8 GetByteEnable() const
    {
        return static_cast<u8>((this->m_RawData & BYTE_ENABLE_MASK) >> BYTE_ENABLE_SHIFT);
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    u32 GetByteEnableMask() const
    {
        u8 be = this->GetByteEnable();
        u32 mask = 0;

        mask |= (be & 0x1) ? 0x000000FF : 0;
        mask |= (be & 0x2) ? 0x0000FF00 : 0;
        mask |= (be & 0x4) ? 0x00FF0000 : 0;
        mask |= (be & 0x8) ? 0xFF000000 : 0;
        return mask;
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    u32 GetBurstModeFlag() const
    {
        return this->m_RawData & BURST_FLAG;
    }

private:

    enum
    {
        ADDRESS_SHIFT = 0,
        ADDRESS_WIDTH = 16,
        ADDRESS_MASK  = 0xFFFF,
        BYTE_ENABLE_SHIFT = 16,
        BYTE_ENABLE_WIDTH = 4,
        BYTE_ENABLE_MASK  = 0xF0000,
        SIZE_SHIFT = 20,
        SIZE_WIDTH = 8,
        SIZE_MASK  = 0x0FF00000,
        BURST_FLAG = 0x80000000
    };

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    CommandHeader() {}

    u32 m_RawData;
};

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
inline u32
GetCmdValue(u32 value, u32 mask, s32 shift)
{
    return (value >> shift) & mask;
}

template <typename TValue>
inline void
SetCmdValue(u32* addr, TValue value, u32 mask, s32 shift)
{
    u32 result = *addr & ~(mask << shift);
    *addr = result | ((static_cast<u32>(value) & static_cast<u32>(mask)) << shift);
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class GlSystem
{
public:
    //
    static void SetGlManagers(void*) {}

    //
    static void SetGlVbManager(void*) {}

    //
    static void SetGlTexManager(void*) {}

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    static void*
    GetBufferAddress(u32 bufferId)
    {
        s32 addr;
        glBindBuffer(GL_ARRAY_BUFFER, bufferId);
        glGetBufferParameteriv(GL_ARRAY_BUFFER,  GL_BUFFER_DATA_ADDR_DMP, &addr);
        NW_GL_ASSERT();

        return reinterpret_cast<void*>( addr );
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    static void*
    GetElementBufferAddress(u32 bufferId)
    {
        s32 addr;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId);
        glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER,  GL_BUFFER_DATA_ADDR_DMP, &addr);
        NW_GL_ASSERT();

        return reinterpret_cast<void*>( addr );
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    static void*
    GetTextureAddress(u32 texId)
    {
        s32 addr;

        glBindTexture( GL_TEXTURE_2D, texId );
        glGetTexParameteriv( GL_TEXTURE_2D, GL_TEXTURE_DATA_ADDR_DMP, &addr );

        NW_GL_ASSERT();
        return reinterpret_cast<void*>(addr);
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    static void*
    GetCubeTextureAddress(u32 texId, int face)
    {
        s32 addr[6];

        glBindTexture( GL_TEXTURE_CUBE_MAP, texId );
        glGetTexParameteriv( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_DATA_ADDR_DMP, &addr[0] );
        NW_GL_ASSERT();

        return reinterpret_cast<void*>(addr[face]);
    }
};

//---------------------------------------------------------------------------
//
//
//
//
//---------------------------------------------------------------------------
template <typename T>
NW_FORCE_INLINE void
NWUseCmdlist(const T* buffer, int size)
{
#if 0 // This is enabled if the nngxAdd3DCommand 8byte alignment restriction is eliminated due to updating the SDK.
    nngxAdd3DCommand( const_cast<GLvoid*>(buffer), GLsizei(size), GL_TRUE );
#else
    NW_ASSERT(size > 0);
    NW_ALIGN_ASSERT(size, 4);
    NW_NULL_ASSERT( buffer );

    nw::os::MemCpy(__cb_current_command_buffer, buffer, size);
    __cb_current_command_buffer += size >> 2;
#endif
}

template <int size>
NW_FORCE_INLINE void
NWUseCmdlist(const void* buffer)
{
    NW_ASSERT(size > 0);
    NW_ALIGN_ASSERT(size, 4);
    NW_NULL_ASSERT( buffer );

#if 1
    __cb_current_command_buffer =
        internal::FastWordCopy((u32*)__cb_current_command_buffer, (u32*)buffer, size);
#else
    nw::os::MemCpy(__cb_current_command_buffer, buffer, size);
    __cb_current_command_buffer += size >> 2;
#endif
}

template <>
NW_FORCE_INLINE void
NWUseCmdlist<4>(const void* buffer)
{
    NW_NULL_ASSERT( buffer );

    *reinterpret_cast<u32*>(__cb_current_command_buffer) = *reinterpret_cast<const u32*>(buffer);
    __cb_current_command_buffer += 1;
}

template <>
NW_FORCE_INLINE void
NWUseCmdlist<8>(const void* buffer)
{
    NW_NULL_ASSERT( buffer );

    *reinterpret_cast<u64*>(__cb_current_command_buffer) = *reinterpret_cast<const u64*>(buffer);
    __cb_current_command_buffer += 2;
}

template <>
NW_FORCE_INLINE void
NWUseCmdlist<12>(const void* buffer)
{
    NW_NULL_ASSERT( buffer );

    *reinterpret_cast<u64*>(__cb_current_command_buffer) = *reinterpret_cast<const u64*>(buffer);
    *reinterpret_cast<u32*>(__cb_current_command_buffer + 2) = *(reinterpret_cast<const u32*>(buffer) + 2);
    __cb_current_command_buffer += 3;
}

template <>
NW_FORCE_INLINE void
NWUseCmdlist<16>(const void* buffer)
{
    NW_NULL_ASSERT( buffer );

    *reinterpret_cast<u64*>(__cb_current_command_buffer) = *reinterpret_cast<const u64*>(buffer);
    *reinterpret_cast<u64*>(__cb_current_command_buffer + 2) = *(reinterpret_cast<const u64*>(buffer) + 1);
    __cb_current_command_buffer += 4;
}


//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------
NW_FORCE_INLINE void*
NWGetCurrentCmdBuffer()
{
    return __cb_current_command_buffer;
}


//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------
NW_FORCE_INLINE void*
NWGetCmdBufferEnd()
{
    return __cb_current_max_command_buffer;
}

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------
NW_FORCE_INLINE void
NWForwardCurrentCmdBuffer(u32 size)
{
    NW_ALIGN_ASSERT(size, 4);

    __cb_current_command_buffer += size >> 2;
}


//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------
NW_FORCE_INLINE void
NWBackwardCurrentCmdBuffer(u32 size)
{
    NW_ALIGN_ASSERT(size, 4);

    __cb_current_command_buffer -= size >> 2;
}

//---------------------------------------------------------------------------
//
//
//
//
//
//
//
//
//
// |seq:1|rsv:3|size:8|be:4|addr:16|
//
// seq: If 0, writes to a single register. If 1, writes to sequential registers.
// rsv: Reserved.
// size: The 32-bit data count - 1.
// be: The byte unit mask to 32-bit data.
// addr: Write location address.
//---------------------------------------------------------------------------
NW_INLINE u32
MakeCommandHeader(u32 address, int count, bool incremental, u8 byteEnable)
{
    enum
    {
        ADDRESS_SHIFT     = 0,
        ADDRESS_WIDTH     = 16,
        BYTE_ENABLE_SHIFT = 16,
        BYTE_ENABLE_WIDTH = 4,
        SIZE_SHIFT        = 20,
        SIZE_WIDTH        = 8,
        BURST_MODE_SHIFT  = 31
    };

    NW_ASSERT(0 < count && count <= 256);

    u32 result = 0;

    if (incremental)
    {
        result |= (0x1U << BURST_MODE_SHIFT);
    }

    result |= ut::internal::MakeBits(count - 1, SIZE_WIDTH, SIZE_SHIFT);
    result |= ut::internal::MakeBits(address, ADDRESS_WIDTH, ADDRESS_SHIFT);
    result |= ut::internal::MakeBits(byteEnable, BYTE_ENABLE_WIDTH, BYTE_ENABLE_SHIFT);

    return result;
}

#define NW_GFX_GPU_HEADER(addr, count, incremental, byteEnable)  (u32)((incremental << 31) | (count << 20) | (byteEnable << 16) | address)


enum UniformRegistry
{
    REG_VERTEX_UNIFORM_FLOAT_INDEX = 0x2C0,
    REG_VERTEX_UNIFORM_FLOAT_BASE = 0x2C1,
    REG_GEOMETRY_UNIFORM_FLOAT_INDEX = 0x290,
    REG_GEOMETRY_UNIFORM_FLOAT_BASE = 0x291
};

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void NWCopyVec4Reverse(f32* dst, const f32*  src);

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void NWCopyVec4WithHeader(f32* dst, const f32* src, u32 header);

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void NWCopyMtx24Reverse(f32* dst, const f32* src);

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void NWCopyMtx24WithHeader(f32* dst, const f32* src, u32 header);

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void NWCopyMtx34Reverse(f32* dst, const f32* src);

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void NWCopyMtx34WithHeader(f32* dst, const f32* src, u32 header);

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void NWCopyMtx44Reverse(f32* dst, const f32* src);

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void NWCopyMtx44WithHeader(f32*  dst, const f32* src, u32 header);

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void NWCopyVec3Reverse(f32* dst, const f32* src);

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void NWCopyVec3WithHeader(f32* dst, const f32* src, u32 header );

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void NWCopyMtx23Reverse(f32* dst, const f32* src);

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void NWCopyMtx23WithHeader(f32* dst, const f32* src, u32 header );

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void NWCopyMtx33Reverse(f32* dst, const f32* src);

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void NWCopyMtx33WithHeader(f32* dst, const f32* src, u32 header );

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void NWCopyMtx43Reverse(f32* dst, const f32* src);

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void NWCopyMtx43WithHeader(f32* dst, const f32* src, u32 header );

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void NWCopyVec2Reverse(f32* dst, const f32* src);

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void NWCopyVec2WithHeader(f32* dst, const f32* src, u32 header );

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void NWCopyMtx22Reverse(f32* dst, const f32* src);

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void NWCopyMtx22WithHeader(f32* dst, const f32* src, u32 header );

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void NWCopyMtx32Reverse(f32* dst, const f32* src);

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void NWCopyMtx32WithHeader(f32* dst, const f32* src, u32 header );

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void NWCopyMtx42Reverse(f32* dst, const f32* src);

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void NWCopyMtx42WithHeader(f32* dst, const f32* src, u32 header );

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void NWCopyVec1Reverse(f32* dst, const f32* src);

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void NWCopyVec1WithHeader(f32* dst, const f32* src, u32 header );

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void NWCopyMtx21Reverse(f32* dst, const f32* src);

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void NWCopyMtx21WithHeader(f32* dst, const f32* src, u32 header );

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void NWCopyMtx31Reverse(f32* dst, const f32* src);

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void NWCopyMtx31WithHeader(f32* dst, const f32* src, u32 header );

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void NWCopyMtx41Reverse(f32* dst, const f32* src);

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void NWCopyMtx41WithHeader(f32* dst, const f32* src, u32 header );

//---------------------------------------------------------------------------
//
//
//
//
//
//---------------------------------------------------------------------------
template <u32 RegFloatIndex>
NW_FORCE_INLINE void
NWSetUniform4fv(u32 index, int count, const f32* data)
{
#if defined(NW_UNIFORM_DUMP)
    NW_LOG("Set Uniform 4fv: %d\n", index);
#endif

    enum
    {
        REG_UNIFORM_FLOAT_INDEX = RegFloatIndex,
        REG_UNIFORM_FLOAT_BASE  = REG_UNIFORM_FLOAT_INDEX + 1,
        SIZE_SHIFT = 20,
        VECTOR4 = 1,
        MATRIX2x4 = 2,
        MATRIX3x4 = 3,
        MATRIX4x4 = 4,
        REG_COUNT = 4
    };

     // The maximum word count sent by one command is 256 words. The limit is 64 registers.
    NW_MINMAX_ASSERT(count, 1, 64);
    NW_NULL_ASSERT( data );
    NW_MINMAX_ASSERT(index, 0, 96);

    const u32 SHORT_VALUE_HEADER = MakeCommandHeader(REG_UNIFORM_FLOAT_INDEX, 1, true, 0xF);

    // Sending is possible using one command for up to two registers.
    const u32 SHORT_HEADER[2] =
    {
        0x80000000 + index,
        SHORT_VALUE_HEADER | (REG_COUNT << SIZE_SHIFT)
    };

    // Sending is possible using one command for up to two registers.
    const u32 SHORT_HEADER2[2] =
    {
        0x80000000 + index,
        SHORT_VALUE_HEADER | ((REG_COUNT * 2) << SIZE_SHIFT)
    };

    // If there are two or more registers, commands have to be divided up and padded with 8-byte alignment at the end.
    const u32 LONG_ADDRESS_HEADER = MakeCommandHeader(REG_UNIFORM_FLOAT_INDEX, 1, false, 0xF);
    const u32 LONG_VALUE_HEADER   = MakeCommandHeader(REG_UNIFORM_FLOAT_BASE, 1, false, 0xF);

    const u32 LONG_HEADER[2] =
    {
        0x80000000 + index,
        LONG_ADDRESS_HEADER
    };

    u32* command = (u32*)NWGetCurrentCmdBuffer();

    switch (count)
    {
        case VECTOR4:
        {
            command[0] = SHORT_HEADER[0];
            command[1] = SHORT_HEADER[1];
            NWCopyVec4Reverse(reinterpret_cast<f32*>(&command[2]), data);
            // Advances 2 words in the address header.
            NWForwardCurrentCmdBuffer( sizeof(u32) * 2 + sizeof(f32) * REG_COUNT );

            //NWUseCmdlist(data, sizeof(f32) * REG_COUNT);
            break;
        }
        case MATRIX2x4:
        {
            command[0] = SHORT_HEADER2[0];
            command[1] = SHORT_HEADER2[1];
            NWCopyMtx24Reverse(reinterpret_cast<f32*>(&command[2]), data);
            // Advances 2 words in the address header.
            NWForwardCurrentCmdBuffer( sizeof(u32) * 2 + sizeof(f32) * REG_COUNT * MATRIX2x4 );

            //NWUseCmdlist(data, sizeof(f32) * REG_COUNT * MATRIX2x4);
            break;
        }
        case MATRIX3x4:
        {
            command[0] = LONG_HEADER[0];
            command[1] = LONG_HEADER[1];
            NWCopyMtx34WithHeader(
                reinterpret_cast<f32*>(&command[2]), data, LONG_VALUE_HEADER | ((MATRIX3x4 * REG_COUNT - 1) << SIZE_SHIFT));
            // Advances 2 words in the address header and 2 words in the value header and padding.
            NWForwardCurrentCmdBuffer( sizeof(u32) * 4 + (REG_COUNT * MATRIX3x4) * sizeof(f32) );

            //const u32 HEADER[4] =
            //{
            //    0x80000000 + index,
            //    ADDRESS_HEADER,
            //    *reinterpret_cast<const u32*>(&data[0]),
            //    VALUE_HEADER | ((count * 4 - 1) << SIZE_SHIFT)
            //};

            //NWUseCmdlist(HEADER, sizeof(u32) * 4);
            //NWUseCmdlist(&data[1], sizeof(f32) * (REG_COUNT * MATRIX3x4 - 1));

            // Padding at the 64 bit boundary
            //NWUseCmdlist(&PADDING[0], sizeof(f32));
            break;
        }
        case MATRIX4x4:
        {
            command[0] = LONG_HEADER[0];
            command[1] = LONG_HEADER[1];
            NWCopyMtx44WithHeader(
                reinterpret_cast<f32*>(&command[2]), data, LONG_VALUE_HEADER | ((MATRIX4x4 * REG_COUNT - 1) << SIZE_SHIFT));
            // Advances 2 words in the address header and 2 words in the value header and padding.
            NWForwardCurrentCmdBuffer( sizeof(u32) * 4 + ( REG_COUNT * MATRIX4x4) * sizeof(f32) );

            //const u32 HEADER[4] =
            //{
            //    0x80000000 + index,
            //    ADDRESS_HEADER,
            //    *reinterpret_cast<const u32*>(&data[0]),
            //    VALUE_HEADER | ((count * 4 - 1) << SIZE_SHIFT)
            //};

            //NWUseCmdlist(HEADER, sizeof(u32) * REG_COUNT);
            //NWUseCmdlist(&data[1], sizeof(f32) * (REG_COUNT * MATRIX4x4 - 1));

            // Padding at the 64 bit boundary
            //NWUseCmdlist(&PADDING[0], sizeof(f32));
            break;
        }
        default:
        {
            NW_FATAL_ERROR("Not supported yet.\n");
            break;
        }
    }
}

//---------------------------------------------------------------------------
//
//
//
//
//
//---------------------------------------------------------------------------
template <u32 RegFloatIndex>
NW_FORCE_INLINE void
NWSetUniform3fv(u32 index, int count, const f32* data)
{
#if defined(NW_UNIFORM_DUMP)
    NW_LOG("Set Uniform 3fv: %d\n", index);
#endif

    enum
    {
        REG_UNIFORM_FLOAT_INDEX = RegFloatIndex,
        REG_UNIFORM_FLOAT_BASE  = REG_UNIFORM_FLOAT_INDEX + 1,
        SIZE_SHIFT = 20,
        VECTOR3 = 1,
        MATRIX2x3 = 2,
        MATRIX3x3 = 3,
        MATRIX4x3 = 4,
        // The register is always 4 because 0.0f is set in areas where no data exists.
        REG_COUNT = 4
    };

     // The maximum word count sent by one command is 256 words. The limit is 64 registers.
    NW_MINMAX_ASSERT(count, 1, 64);
    NW_NULL_ASSERT( data );
    NW_MINMAX_ASSERT(index, 0, 96);

    const u32 SHORT_VALUE_HEADER = MakeCommandHeader(REG_UNIFORM_FLOAT_INDEX, 1, true, 0xF);

    // Sending is possible using one command for up to two registers.
    const u32 SHORT_HEADER[2] =
    {
        0x80000000 + index,
        SHORT_VALUE_HEADER | (REG_COUNT << SIZE_SHIFT)
    };

    // Sending is possible using one command for up to two registers.
    const u32 SHORT_HEADER2[2] =
    {
        0x80000000 + index,
        SHORT_VALUE_HEADER | ((REG_COUNT * 2) << SIZE_SHIFT)
    };

    // If there are two or more registers, commands have to be divided up and padded with 8-byte alignment at the end.
    const u32 LONG_ADDRESS_HEADER = MakeCommandHeader(REG_UNIFORM_FLOAT_INDEX, 1, false, 0xF);
    const u32 LONG_VALUE_HEADER   = MakeCommandHeader(REG_UNIFORM_FLOAT_BASE, 1, false, 0xF);

    const u32 LONG_HEADER[2] =
    {
        0x80000000 + index,
        LONG_ADDRESS_HEADER
    };

    u32* command = (u32*)NWGetCurrentCmdBuffer();

    switch (count)
    {
        case VECTOR3:
        {
            command[0] = SHORT_HEADER[0];
            command[1] = SHORT_HEADER[1];
            NWCopyVec3Reverse(reinterpret_cast<f32*>(&command[2]), data);
            // Advances 2 words in the address header.
            NWForwardCurrentCmdBuffer( sizeof(u32) * 2 + sizeof(f32) * REG_COUNT );
            break;
        }
        case MATRIX2x3:
        {
            command[0] = SHORT_HEADER2[0];
            command[1] = SHORT_HEADER2[1];
            NWCopyMtx23Reverse(reinterpret_cast<f32*>(&command[2]), data);
            // Advances 2 words in the address header.
            NWForwardCurrentCmdBuffer( sizeof(u32) * 2 + sizeof(f32) * REG_COUNT * MATRIX2x3 );
            break;
        }
        case MATRIX3x3:
        {
            command[0] = LONG_HEADER[0];
            command[1] = LONG_HEADER[1];
            NWCopyMtx33WithHeader(
                reinterpret_cast<f32*>(&command[2]), data, LONG_VALUE_HEADER | ((MATRIX3x3 * REG_COUNT - 1) << SIZE_SHIFT));
            // Advances 2 words in the address header and 2 words in the value header and padding.
            NWForwardCurrentCmdBuffer( sizeof(u32) * 4 + (REG_COUNT * MATRIX3x3) * sizeof(f32) );
            break;
        }
        case MATRIX4x3:
        {
            command[0] = LONG_HEADER[0];
            command[1] = LONG_HEADER[1];
            NWCopyMtx43WithHeader(
                reinterpret_cast<f32*>(&command[2]), data, LONG_VALUE_HEADER | ((MATRIX4x3 * REG_COUNT - 1) << SIZE_SHIFT));
            // Advances 2 words in the address header and 2 words in the value header and padding.
            NWForwardCurrentCmdBuffer( sizeof(u32) * 4 + ( REG_COUNT * MATRIX4x3) * sizeof(f32) );
            break;
        }
        default:
        {
            NW_FATAL_ERROR("Not supported yet.\n");
            break;
        }
    }
}

//---------------------------------------------------------------------------
//
//
//
//
//
//---------------------------------------------------------------------------
template <u32 RegFloatIndex>
NW_FORCE_INLINE void
NWSetUniform2fv(u32 index, int count, const f32* data)
{
#if defined(NW_UNIFORM_DUMP)
    NW_LOG("Set Uniform 2fv: %d\n", index);
#endif

    enum
    {
        REG_UNIFORM_FLOAT_INDEX = RegFloatIndex,
        REG_UNIFORM_FLOAT_BASE  = REG_UNIFORM_FLOAT_INDEX + 1,
        SIZE_SHIFT = 20,
        VECTOR2 = 1,
        MATRIX2x2 = 2,
        MATRIX3x2 = 3,
        MATRIX4x2 = 4,
        // The register is always 4 because 0.0f is set in areas where no data exists.
        REG_COUNT = 4
    };

     // The maximum word count sent by one command is 256 words. The limit is 64 registers.
    NW_MINMAX_ASSERT(count, 1, 64);
    NW_NULL_ASSERT( data );
    NW_MINMAX_ASSERT(index, 0, 96);

    const u32 SHORT_VALUE_HEADER = MakeCommandHeader(REG_UNIFORM_FLOAT_INDEX, 1, true, 0xF);

    // Sending is possible using one command for up to two registers.
    const u32 SHORT_HEADER[2] =
    {
        0x80000000 + index,
        SHORT_VALUE_HEADER | (REG_COUNT << SIZE_SHIFT)
    };

    // Sending is possible using one command for up to two registers.
    const u32 SHORT_HEADER2[2] =
    {
        0x80000000 + index,
        SHORT_VALUE_HEADER | ((REG_COUNT * 2) << SIZE_SHIFT)
    };

    // If there are two or more registers, commands have to be divided up and padded with 8-byte alignment at the end.
    const u32 LONG_ADDRESS_HEADER = MakeCommandHeader(REG_UNIFORM_FLOAT_INDEX, 1, false, 0xF);
    const u32 LONG_VALUE_HEADER   = MakeCommandHeader(REG_UNIFORM_FLOAT_BASE, 1, false, 0xF);

    const u32 LONG_HEADER[2] =
    {
        0x80000000 + index,
        LONG_ADDRESS_HEADER
    };

    u32* command = (u32*)NWGetCurrentCmdBuffer();

    switch (count)
    {
        case VECTOR2:
        {
            command[0] = SHORT_HEADER[0];
            command[1] = SHORT_HEADER[1];
            NWCopyVec2Reverse(reinterpret_cast<f32*>(&command[2]), data);
            // Advances 2 words in the address header.
            NWForwardCurrentCmdBuffer( sizeof(u32) * 2 + sizeof(f32) * REG_COUNT );
            break;
        }
        case MATRIX2x2:
        {
            command[0] = SHORT_HEADER2[0];
            command[1] = SHORT_HEADER2[1];
            NWCopyMtx22Reverse(reinterpret_cast<f32*>(&command[2]), data);
            // Advances 2 words in the address header.
            NWForwardCurrentCmdBuffer( sizeof(u32) * 2 + sizeof(f32) * REG_COUNT * MATRIX2x2 );
            break;
        }
        case MATRIX3x2:
        {
            command[0] = LONG_HEADER[0];
            command[1] = LONG_HEADER[1];
            NWCopyMtx32WithHeader(
                reinterpret_cast<f32*>(&command[2]), data, LONG_VALUE_HEADER | ((MATRIX3x2 * REG_COUNT - 1) << SIZE_SHIFT));
            // Advances 2 words in the address header and 2 words in the value header and padding.
            NWForwardCurrentCmdBuffer( sizeof(u32) * 4 + (REG_COUNT * MATRIX3x2) * sizeof(f32) );
            break;
        }
        case MATRIX4x2:
        {
            command[0] = LONG_HEADER[0];
            command[1] = LONG_HEADER[1];
            NWCopyMtx42WithHeader(
                reinterpret_cast<f32*>(&command[2]), data, LONG_VALUE_HEADER | ((MATRIX4x2 * REG_COUNT - 1) << SIZE_SHIFT));
            // Advances 2 words in the address header and 2 words in the value header and padding.
            NWForwardCurrentCmdBuffer( sizeof(u32) * 4 + ( REG_COUNT * MATRIX4x2) * sizeof(f32) );
            break;
        }
        default:
        {
            NW_FATAL_ERROR("Not supported yet.\n");
            break;
        }
    }
}

//---------------------------------------------------------------------------
//
//
//
//
//
//---------------------------------------------------------------------------
template <u32 RegFloatIndex>
NW_FORCE_INLINE void
NWSetUniform1fv(u32 index, int count, const f32* data)
{
#if defined(NW_UNIFORM_DUMP)
    NW_LOG("Set Uniform 1fv: %d\n", index);
#endif

    enum
    {
        REG_UNIFORM_FLOAT_INDEX = RegFloatIndex,
        REG_UNIFORM_FLOAT_BASE  = REG_UNIFORM_FLOAT_INDEX + 1,
        SIZE_SHIFT = 20,
        VECTOR1 = 1,
        MATRIX2x1 = 2,
        MATRIX3x1 = 3,
        MATRIX4x1 = 4,
        // The register is always 4 because 0.0f is set in areas where no data exists.
        REG_COUNT = 4
    };

     // The maximum word count sent by one command is 256 words. The limit is 64 registers.
    NW_MINMAX_ASSERT(count, 1, 64);
    NW_NULL_ASSERT( data );
    NW_MINMAX_ASSERT(index, 0, 96);

    const u32 SHORT_VALUE_HEADER = MakeCommandHeader(REG_UNIFORM_FLOAT_INDEX, 1, true, 0xF);

    // Sending is possible using one command for up to two registers.
    const u32 SHORT_HEADER[2] =
    {
        0x80000000 + index,
        SHORT_VALUE_HEADER | (REG_COUNT << SIZE_SHIFT)
    };

    // Sending is possible using one command for up to two registers.
    const u32 SHORT_HEADER2[2] =
    {
        0x80000000 + index,
        SHORT_VALUE_HEADER | ((REG_COUNT * 2) << SIZE_SHIFT)
    };

    // If there are two or more registers, commands have to be divided up and padded with 8-byte alignment at the end.
    const u32 LONG_ADDRESS_HEADER = MakeCommandHeader(REG_UNIFORM_FLOAT_INDEX, 1, false, 0xF);
    const u32 LONG_VALUE_HEADER   = MakeCommandHeader(REG_UNIFORM_FLOAT_BASE, 1, false, 0xF);

    const u32 LONG_HEADER[2] =
    {
        0x80000000 + index,
        LONG_ADDRESS_HEADER
    };

    u32* command = (u32*)NWGetCurrentCmdBuffer();

    switch (count)
    {
        case VECTOR1:
        {
            command[0] = SHORT_HEADER[0];
            command[1] = SHORT_HEADER[1];
            NWCopyVec1Reverse(reinterpret_cast<f32*>(&command[2]), data);
            // Advances 2 words in the address header.
            NWForwardCurrentCmdBuffer( sizeof(u32) * 2 + sizeof(f32) * REG_COUNT );
            break;
        }
        case MATRIX2x1:
        {
            command[0] = SHORT_HEADER2[0];
            command[1] = SHORT_HEADER2[1];
            NWCopyMtx21Reverse(reinterpret_cast<f32*>(&command[2]), data);
            // Advances 2 words in the address header.
            NWForwardCurrentCmdBuffer( sizeof(u32) * 2 + sizeof(f32) * REG_COUNT * MATRIX2x1 );
            break;
        }
        case MATRIX3x1:
        {
            command[0] = LONG_HEADER[0];
            command[1] = LONG_HEADER[1];
            NWCopyMtx31WithHeader(
                reinterpret_cast<f32*>(&command[2]), data, LONG_VALUE_HEADER | ((MATRIX3x1 * REG_COUNT - 1) << SIZE_SHIFT));
            // Advances 2 words in the address header and 2 words in the value header and padding.
            NWForwardCurrentCmdBuffer( sizeof(u32) * 4 + (REG_COUNT * MATRIX3x1) * sizeof(f32) );
            break;
        }
        case MATRIX4x1:
        {
            command[0] = LONG_HEADER[0];
            command[1] = LONG_HEADER[1];
            NWCopyMtx41WithHeader(
                reinterpret_cast<f32*>(&command[2]), data, LONG_VALUE_HEADER | ((MATRIX4x1 * REG_COUNT - 1) << SIZE_SHIFT));
            // Advances 2 words in the address header and 2 words in the value header and padding.
            NWForwardCurrentCmdBuffer( sizeof(u32) * 4 + ( REG_COUNT * MATRIX4x1) * sizeof(f32) );
            break;
        }
        default:
        {
            NW_FATAL_ERROR("Not supported yet.\n");
            break;
        }
    }
}

//---------------------------------------------------------------------------
//
//
//
//
//
//
//---------------------------------------------------------------------------
template <u32 RegFloatIndex>
NW_FORCE_INLINE void
NWSetUniform4fvBegin(u32 index, int totalCount, int count, const f32* data)
{
#if defined(NW_UNIFORM_DUMP)
    NW_LOG("Set Uniform 4fv Begin: %d\n", index);
#endif

    enum
    {
        REG_UNIFORM_FLOAT_INDEX = RegFloatIndex,
        REG_UNIFORM_FLOAT_BASE = REG_UNIFORM_FLOAT_INDEX + 1,
        SIZE_SHIFT = 20,
        VECTOR4 = 1,
        MATRIX2x4 = 2,
        MATRIX3x4 = 3,
        MATRIX4x4 = 4,
        REG_COUNT = 4
    };

    const u32 ADDRESS_HEADER = MakeCommandHeader(REG_UNIFORM_FLOAT_INDEX, 1, false, 0xF);
    const u32 VALUE_HEADER   = MakeCommandHeader(REG_UNIFORM_FLOAT_BASE, 1, false, 0xF);

     // The maximum word count sent by one command is 256 words. The limit is 64 registers.
    NW_MINMAX_ASSERT(count, 1, 64);
    NW_MINMAX_ASSERT(totalCount, 1, 64);
    NW_NULL_ASSERT( data );
    NW_MINMAX_ASSERT(index, 0, 96);

    // If there are two or more registers, commands have to be divided up and padded with 8-byte alignment at the end.
    const u32 HEADER[2] =
    {
        0x80000000 + index,
        ADDRESS_HEADER
    };

    u32* command = (u32*)NWGetCurrentCmdBuffer();
    command[0] = HEADER[0];
    command[1] = HEADER[1];

    switch (count)
    {
        case VECTOR4:
        {
            NWCopyVec4WithHeader(
                reinterpret_cast<f32*>(&command[2]), data, VALUE_HEADER | ((totalCount * REG_COUNT - 1) << SIZE_SHIFT));
            // Advances 2 words if an address header, 1 word if a value header.
            NWForwardCurrentCmdBuffer( sizeof(u32) * 3 + (REG_COUNT * VECTOR4) * sizeof(f32) );
            break;
        }
        case MATRIX2x4:
        {
            NWCopyMtx24WithHeader(
                reinterpret_cast<f32*>(&command[2]), data, VALUE_HEADER | ((totalCount * REG_COUNT - 1) << SIZE_SHIFT));
            // Advances 2 words if an address header, 1 word if a value header.
            NWForwardCurrentCmdBuffer( sizeof(u32) * 3 + (REG_COUNT * MATRIX2x4) * sizeof(f32) );
            break;
        }
        case MATRIX3x4:
        {
            NWCopyMtx34WithHeader(
                reinterpret_cast<f32*>(&command[2]), data, VALUE_HEADER | ((totalCount * REG_COUNT - 1) << SIZE_SHIFT));
            // Advances 2 words if an address header, 1 word if a value header.
            NWForwardCurrentCmdBuffer( sizeof(u32) * 3 + (REG_COUNT * MATRIX3x4) * sizeof(f32) );
            break;
        }
        case MATRIX4x4:
        {
            NWCopyMtx44WithHeader(
                reinterpret_cast<f32*>(&command[2]), data, VALUE_HEADER | ((totalCount * REG_COUNT - 1) << SIZE_SHIFT));
            // Advances 2 words if an address header, 1 word if a value header.
            NWForwardCurrentCmdBuffer( sizeof(u32) * 3 + (REG_COUNT * MATRIX4x4) * sizeof(f32) );
            break;
        }
        default:
        {
            NW_FATAL_ERROR("Not supported yet.\n");
            break;
        }
    }
}

//---------------------------------------------------------------------------
//
//
//
//
//---------------------------------------------------------------------------
NW_FORCE_INLINE void
NWSetUniform4fvContinuous(int count, const f32* data)
{
#if defined(NW_UNIFORM_DUMP)
    NW_LOG("Set Uniform 4fv Continuous: %d\n");
#endif

    enum
    {
        VECTOR4 = 1,
        MATRIX2x4 = 2,
        MATRIX3x4 = 3,
        MATRIX4x4 = 4,
        REG_COUNT = 4
    };

    f32* command = (f32*)NWGetCurrentCmdBuffer();

    switch (count)
    {
        case VECTOR4:
        {
            NWCopyVec4Reverse(command, data);
            NWForwardCurrentCmdBuffer( (REG_COUNT * VECTOR4) * sizeof(f32) );
            break;
        }
        case MATRIX2x4:
        {
            NWCopyMtx24Reverse(command, data);
            NWForwardCurrentCmdBuffer( (REG_COUNT * MATRIX2x4) * sizeof(f32) );
            break;
        }
        case MATRIX3x4:
        {
            NWCopyMtx34Reverse(command, data);
            NWForwardCurrentCmdBuffer( (REG_COUNT * MATRIX3x4) * sizeof(f32) );
            break;
        }
        case MATRIX4x4:
        {
            NWCopyMtx44Reverse(command, data);
            NWForwardCurrentCmdBuffer( (REG_COUNT * MATRIX4x4) * sizeof(f32) );
            break;
        }
        default:
        {
            NW_FATAL_ERROR("Not supported yet.\n");
            break;
        }
    }
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
NW_FORCE_INLINE void
NWSetUniform4fvEnd()
{
#if defined(NW_UNIFORM_DUMP)
    NW_LOG("Set Uniform 4fv End: %d\n");
#endif

    NWForwardCurrentCmdBuffer( sizeof(f32) );
}

NW_FORCE_INLINE void
NWSetVertexUniform4fv(u32 index, int count, const f32* data)
{
    NWSetUniform4fv<REG_VERTEX_UNIFORM_FLOAT_INDEX>(index, count, data);
}

NW_FORCE_INLINE void
NWSetVertexUniform3fv(u32 index, int count, const f32* data)
{
    NWSetUniform3fv<REG_VERTEX_UNIFORM_FLOAT_INDEX>(index, count, data);
}

NW_FORCE_INLINE void
NWSetVertexUniform2fv(u32 index, int count, const f32* data)
{
    NWSetUniform2fv<REG_VERTEX_UNIFORM_FLOAT_INDEX>(index, count, data);
}

NW_FORCE_INLINE void
NWSetVertexUniform1fv(u32 index, int count, const f32* data)
{
    NWSetUniform1fv<REG_VERTEX_UNIFORM_FLOAT_INDEX>(index, count, data);
}

NW_FORCE_INLINE void
NWSetGeometryUniform4fv(u32 index, int count, const f32* data)
{
    NWSetUniform4fv<REG_GEOMETRY_UNIFORM_FLOAT_INDEX>(index, count, data);
}

NW_FORCE_INLINE void
NWSetGeometryUniform3fv(u32 index, int count, const f32* data)
{
    NWSetUniform3fv<REG_GEOMETRY_UNIFORM_FLOAT_INDEX>(index, count, data);
}

NW_FORCE_INLINE void
NWSetGeometryUniform2fv(u32 index, int count, const f32* data)
{
    NWSetUniform2fv<REG_GEOMETRY_UNIFORM_FLOAT_INDEX>(index, count, data);
}

NW_FORCE_INLINE void
NWSetGeometryUniform1fv(u32 index, int count, const f32* data)
{
    NWSetUniform1fv<REG_GEOMETRY_UNIFORM_FLOAT_INDEX>(index, count, data);
}

NW_FORCE_INLINE void
NWSetVertexUniform4fvBegin(u32 index, int totalCount, int count, const f32* data)
{
    NWSetUniform4fvBegin<REG_VERTEX_UNIFORM_FLOAT_INDEX>(index, totalCount, count, data);
}

NW_FORCE_INLINE void
NWSetVertexUniform4fvContinuous(int count, const f32* data)
{
    NWSetUniform4fvContinuous(count, data);
}

NW_FORCE_INLINE void
NWSetVertexUniform4fvEnd()
{
    NWSetUniform4fvEnd();
}

NW_FORCE_INLINE void
NWSetGeometryUniform4fvBegin(u32 index, int totalCount, int count, const f32* data)
{
    NWSetUniform4fvBegin<REG_GEOMETRY_UNIFORM_FLOAT_INDEX>(index, totalCount, count, data);
}

NW_FORCE_INLINE void
NWSetGeometryUniform4fvContinuous(int count, const f32* data)
{
    NWSetUniform4fvContinuous(count, data);
}

NW_FORCE_INLINE void
NWSetGeometryUniform4fvEnd()
{
    NWSetUniform4fvEnd();
}


//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class CommandBufferInfo
{
public:

    enum BufferResult
    {
        RESULT_OK            = 0,     //
        RESULT_OUT_OF_MEMORY = 1 << 1 //
    };

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    CommandBufferInfo(void* buffer, size_t size)
    {
        m_TopAddress     = static_cast<u8*>( buffer );
        m_CurrentAddress = m_TopAddress;
        m_Size           = size;
        m_BottomAddress  = reinterpret_cast<u8*>(nw::ut::AddOffsetToPtr( m_TopAddress, size ));
    }

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    CommandBufferInfo()
    {
        m_TopAddress     = NULL;
        m_CurrentAddress = NULL;
        m_Size           = 0;
        m_BottomAddress  = NULL;
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    u8*  GetCurrentAddress()
    {
        return (m_CurrentAddress != NULL)? m_CurrentAddress : reinterpret_cast<u8*>(__cb_current_command_buffer);
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    const u8* GetCurrentAddress() const
    {
        return (m_CurrentAddress != NULL)? m_CurrentAddress : reinterpret_cast<const u8*>(__cb_current_command_buffer);
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    size_t GetCurrentSize() const { return nw::ut::GetOffsetFromPtr( m_TopAddress, m_CurrentAddress ); }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    Result
    ForwardCommand(int size)
    {
        if (!this->CheckRestMemory(size))
        {
            return Result(RESULT_OUT_OF_MEMORY);
        }

        this->ForwardAddress(size);

        return Result(RESULT_OK);
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    Result
    PutCommand(const void* buffer, int size)
    {
        NW_NULL_ASSERT(buffer);
        NW_ASSERT(size > 0);
        NW_ALIGN_ASSERT(size, 4);

        if (!this->CheckRestMemory(size))
        {
            return Result(RESULT_OUT_OF_MEMORY);
        }

        nw::os::MemCpy(this->GetCurrentAddress(), buffer, size);

        this->ForwardAddress(size);

        return Result(RESULT_OK);
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    template <int size>
    Result
    PutCommand(const void* buffer)
    {
        NW_NULL_ASSERT(buffer);
        NW_STATIC_ASSERT(size > 0);
        NW_STATIC_ASSERT((size % 4) == 0);

        if (!this->CheckRestMemory(size))
        {
            return Result(RESULT_OUT_OF_MEMORY);
        }

        internal::FastWordCopy((u32*)this->GetCurrentAddress(), (u32*)buffer, size);

        this->ForwardAddress(size);
    }

private:
    u8*    m_TopAddress;     //
    u8*    m_BottomAddress;  //
    u8*    m_CurrentAddress; //
    size_t m_Size;           //

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    bool CheckRestMemory(size_t size)
    {
        if (m_CurrentAddress == NULL)
        {
            return nw::ut::AddOffsetToPtr(__cb_current_command_buffer, size) <= __cb_current_max_command_buffer;
        }
        else
        {
            return nw::ut::AddOffsetToPtr(m_CurrentAddress, size) <= m_BottomAddress;
        }
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    void ForwardAddress(int size)
    {
        if (m_CurrentAddress != NULL)
        {
            m_CurrentAddress += size;
        }
        else
        {
            NWForwardCurrentCmdBuffer(size);
        }
    }
};


//---------------------------------------------------------------------------
//
//
//
//
//
//---------------------------------------------------------------------------
template <>
NW_INLINE Result
CommandBufferInfo::PutCommand<4>(const void* buffer)
{
    NW_NULL_ASSERT( buffer );

    if (!this->CheckRestMemory(4))
    {
        return Result(CommandBufferInfo::RESULT_OUT_OF_MEMORY);
    }

    *reinterpret_cast<u32*>(this->GetCurrentAddress()) = *reinterpret_cast<const u32*>(buffer);
    this->ForwardAddress(4);

    return Result(CommandBufferInfo::RESULT_OK);
}

template <>
NW_INLINE Result
CommandBufferInfo::PutCommand<8>(const void* buffer)
{
    NW_NULL_ASSERT( buffer );

    if (!this->CheckRestMemory(8))
    {
        return Result(CommandBufferInfo::RESULT_OUT_OF_MEMORY);
    }

    *reinterpret_cast<u64*>(this->GetCurrentAddress()) = *reinterpret_cast<const u64*>(buffer);
    this->ForwardAddress(8);

    return Result(CommandBufferInfo::RESULT_OK);
}

template <>
NW_INLINE Result
CommandBufferInfo::PutCommand<12>(const void* buffer)
{
    NW_NULL_ASSERT( buffer );

    if (!this->CheckRestMemory(12))
    {
        return Result(CommandBufferInfo::RESULT_OUT_OF_MEMORY);
    }

    *reinterpret_cast<u64*>(this->GetCurrentAddress()) = *reinterpret_cast<const u64*>(buffer);
    *reinterpret_cast<u32*>(this->GetCurrentAddress() + 8) = *(reinterpret_cast<const u32*>(buffer) + 2);
    this->ForwardAddress(12);

    return Result(CommandBufferInfo::RESULT_OK);
}

template <>
NW_INLINE Result
CommandBufferInfo::PutCommand<16>(const void* buffer)
{
    NW_NULL_ASSERT( buffer );

    if (!this->CheckRestMemory(16))
    {
        return (CommandBufferInfo::RESULT_OUT_OF_MEMORY);
    }

    *reinterpret_cast<u64*>(this->GetCurrentAddress()) = *reinterpret_cast<const u64*>(buffer);
    *reinterpret_cast<u64*>(this->GetCurrentAddress() + 8) = *(reinterpret_cast<const u64*>(buffer) + 1);
    this->ForwardAddress(16);

    return Result(CommandBufferInfo::RESULT_OK);
}



//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class CommandCacheBuilder
{
public:
    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    CommandCacheBuilder() : m_StartAddr(NULL), m_EndAddr(NULL) {}

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    void Begin()
    {
        m_StartAddr = NWGetCurrentCmdBuffer();
    }

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    void End()
    {
        if (m_StartAddr)
        {
            m_EndAddr = NWGetCurrentCmdBuffer();
        }
    }

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    void Rollback()
    {
        void* current = NWGetCurrentCmdBuffer();

        NWBackwardCurrentCmdBuffer( ut::GetOffsetFromPtr(m_StartAddr, current) );
    }

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    void Reset()
    {
        m_StartAddr = NULL;
        m_EndAddr = NULL;
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    s32 GetSize()
    {
        if (m_StartAddr && m_EndAddr)
        {
            return ut::GetOffsetFromPtr(m_StartAddr, m_EndAddr);
        }
        return 0;
    }

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    void* AllocAndCopy(nw::os::IAllocator* allocator)
    {
        NW_NULL_ASSERT(allocator);

        s32 size = this->GetSize();

        if (size == 0) { return NULL; }

        void* buffer = allocator->Alloc(size, 4);

        if (! buffer ) { return NULL; }

        nw::os::MemCpy(buffer, m_StartAddr, size);

        return buffer;
    }

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    void Report(bool detail = false)
    {
        if (this->GetSize() == 0) { return; }

        {
            u32* addr = reinterpret_cast<u32*>(m_StartAddr);
            u32* endAddr = reinterpret_cast<u32*>(m_EndAddr);

            NW_LOG("----------\n");

            char tmpStr[256];
            s32 offset = 0;
            int count = 0;
            while (addr < endAddr)
            {
                offset += nw::ut::snprintf(&tmpStr[offset], 256 - offset, 256 - offset - 1, "0x%08x, ", *addr);
                ++addr;
                ++count;
                if (count == 4)
                {
                    NN_LOG("%s\n", tmpStr);
                    offset = 0;
                    count = 0;
                }
            }

            if (offset > 0)
            {
                NW_LOG("%s\n", tmpStr);
            }
        }

        if (detail)
        {
            struct command_t
            {
                // data
                u32 data : 32;

                // header
                u32 addr : 16;
                u32 be   : 4;
                u32 size : 7;
                u32 rsv  : 4;
                u32 seq  : 1;
            };

            u32* addr = reinterpret_cast<u32*>(m_StartAddr);
            u32* endAddr = reinterpret_cast<u32*>(m_EndAddr);

            NW_LOG("---- Detail ------\n");
            NN_LOG("Size : %d\n", this->GetSize());

            while (addr < endAddr)
            {
                command_t* command = (command_t*)addr;
                u32* command_u32 = (u32*)addr;

                int commandSize = (command->size + 1) & ~1;
                commandSize += 2;

                addr += commandSize;

                NN_LOG("Raw Data:");
                for (int i = 0; i < commandSize; i += 2)
                {
                    if (&command_u32[i] >= endAddr) // For the sake of safety:
                    {
                        commandSize = i - 2;
                        break;
                    }

                    if (i != 0 && i % 8 == 0)
                    {
                        NN_LOG("\n         ");
                    }

                    NN_LOG(" 0x%08x, 0x%08x,",
                        command_u32[i],
                        command_u32[i + 1]);
                }

                NN_LOG("\n");

                NN_LOG("addr : 0x%x\n", command->addr);
                NN_LOG("be   : 0x%x\n", command->be);   // Byte enabled
                NN_LOG("size : 0x%x (%d)\n", command->size, command->seq); // Number of data -1 0: single > 1: burst
                NN_LOG("rsv  : 0x%x\n", command->rsv);
                NN_LOG("seq  : 0x%x\n", command->seq);

                NN_LOG("data : 0x%x (%f),\n",
                    command->data,
                    nn::math::U32AsF32(command->data));

                for (int i = 2; i < commandSize; i += 2)
                {
                    if ((i - 2) % 4 == 0)
                    {
                        NN_LOG("\n      ");
                    }

                    NN_LOG(" 0x%x (%f), 0x%x (%f),",
                        command_u32[i],
                        nn::math::U32AsF32(command_u32[i]),
                        command_u32[i + 1],
                        nn::math::U32AsF32(command_u32[i + 1]));
                }

                if (commandSize != 2)
                {
                    NN_LOG("\n\n");
                }
                else
                {
                    NN_LOG("\n");
                }
            }
        }
    }

private:
    void* m_StartAddr;
    void* m_EndAddr;
};


} // namespace internal
} // namespace gfx
} // namespace nw


#endif // NW_GFX_COMMAND_UTIL_H_
