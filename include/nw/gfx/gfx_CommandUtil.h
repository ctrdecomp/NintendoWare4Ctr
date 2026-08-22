#pragma once

#include "nw/types.h"
#include "nw/ut/ut_Inlines.h"
#include "nw/gfx/gfx_GlImplement.h"
#include "nw/gfx/gfx_MemoryUtil.h"

#include <gles2/gl2.h>
#include <gles2/gl2extern.h>
#include <cstring>

namespace nw {
namespace gfx {

class CommandCacheManager{
public:
    static void SetAllocator(nw::os::IAllocator* allocator) { sAllocator = allocator; }
    static nw::os::IAllocator* GetAllocator() { return sAllocator; }

    static void* Allocate(s32 size){
        NW_NULL_ASSERT(sAllocator);
        return sAllocator->Alloc(size, 4);
    }

    static void Free(void* buffer){
        if (sAllocator != NULL)
            sAllocator->Free(buffer);
        else
            NW_WARNING(false, "No allocator available");
    }

private:
    static nw::os::IAllocator* sAllocator;
};

namespace internal {

class CommandHeader{
public:
    CommandHeader(u64 rawData): mRawData(rawData >> 32) {}
    CommandHeader(u32 rawData): mRawData(rawData) {}

    u32 GetAddress()  const { return (mRawData & ADDRESS_MASK); }
    s32 GetSize()     const { return ((mRawData & SIZE_MASK) >> SIZE_SHIFT) + 1; }
    u8  GetByteEnable() const { return static_cast<u8>((mRawData & BYTE_ENABLE_MASK) >> BYTE_ENABLE_SHIFT); }

    u32 GetByteEnableMask() const{
        u8  be   = this->GetByteEnable();
        u32 mask = 0;
        mask |= (be & 0x1) ? 0x000000FF : 0;
        mask |= (be & 0x2) ? 0x0000FF00 : 0;
        mask |= (be & 0x4) ? 0x00FF0000 : 0;
        mask |= (be & 0x8) ? 0xFF000000 : 0;
        return mask;
    }

    u32 GetBurstModeFlag() const { return mRawData & BURST_FLAG; }

private:
    enum{
        ADDRESS_SHIFT     = 0,
        ADDRESS_WIDTH     = 16,
        ADDRESS_MASK      = 0xFFFF,
        BYTE_ENABLE_SHIFT = 16,
        BYTE_ENABLE_WIDTH = 4,
        BYTE_ENABLE_MASK  = 0xF0000,
        SIZE_SHIFT        = 20,
        SIZE_WIDTH        = 8,
        SIZE_MASK         = 0x0FF00000,
        BURST_FLAG        = 0x80000000
    };

    CommandHeader() {}
    u32 mRawData;
};

inline u32 GetCmdValue(u32 value, u32 mask, s32 shift){
    return (value >> shift) & mask;
}

template<typename TValue>
inline void SetCmdValue(u32* addr, TValue value, u32 mask, s32 shift){
    u32 result = *addr & ~(mask << shift);
    *addr = result | ((static_cast<u32>(value) & static_cast<u32>(mask)) << shift);
}

class GlSystem{
public:
    static void SetGlManagers(void*) {}
    static void SetGlVbManager(void*) {}
    static void SetGlTexManager(void*) {}

    static void* GetBufferAddress(u32 bufferId){
        s32 addr;
        glBindBuffer(GL_ARRAY_BUFFER, bufferId);
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_DATA_ADDR_DMP, &addr);
        return reinterpret_cast<void*>(addr);
    }

    static void* GetElementBufferAddress(u32 bufferId){
        s32 addr;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId);
        glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_DATA_ADDR_DMP, &addr);
        return reinterpret_cast<void*>(addr);
    }

    static void* GetTextureAddress(u32 texId){
        s32 addr;
        glBindTexture(GL_TEXTURE_2D, texId);
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_DATA_ADDR_DMP, &addr);
        return reinterpret_cast<void*>(addr);
    }

    static void* GetCubeTextureAddress(u32 texId, int face){
        s32 addr[6];
        glBindTexture(GL_TEXTURE_CUBE_MAP, texId);
        glGetTexParameteriv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_DATA_ADDR_DMP, &addr[0]);
        return reinterpret_cast<void*>(addr[face]);
    }
};

template<typename T>
__forceinline void NWUseCmdlist(const T* buffer, int size){
    NW_ASSERT(size > 0);
    NW_NULL_ASSERT(buffer);
    nw::os::MemCpy(__cb_current_command_buffer, buffer, size);
    __cb_current_command_buffer += size >> 2;
}

template<int size>
__forceinline void NWUseCmdlist(const void* buffer){
    NW_ASSERT(size > 0);
    NW_NULL_ASSERT(buffer);
    __cb_current_command_buffer = internal::FastWordCopy((u32*)__cb_current_command_buffer, (u32*)buffer, size);
}

template<>
__forceinline void NWUseCmdlist<4>(const void* buffer){
    NW_NULL_ASSERT(buffer);
    *reinterpret_cast<u32*>(__cb_current_command_buffer) = *reinterpret_cast<const u32*>(buffer);
    __cb_current_command_buffer += 1;
}

template<>
__forceinline void NWUseCmdlist<8>(const void* buffer){
    NW_NULL_ASSERT(buffer);
    *reinterpret_cast<u64*>(__cb_current_command_buffer) = *reinterpret_cast<const u64*>(buffer);
    __cb_current_command_buffer += 2;
}

template<>
__forceinline void NWUseCmdlist<12>(const void* buffer){
    NW_NULL_ASSERT(buffer);
    *reinterpret_cast<u64*>(__cb_current_command_buffer) = *reinterpret_cast<const u64*>(buffer);
    *reinterpret_cast<u32*>(__cb_current_command_buffer + 2) = *(reinterpret_cast<const u32*>(buffer) + 2);
    __cb_current_command_buffer += 3;
}

template<>
__forceinline void NWUseCmdlist<16>(const void* buffer){
    NW_NULL_ASSERT(buffer);
    *reinterpret_cast<u64*>(__cb_current_command_buffer) = *reinterpret_cast<const u64*>(buffer);
    *reinterpret_cast<u64*>(__cb_current_command_buffer + 2) = *(reinterpret_cast<const u64*>(buffer) + 1);
    __cb_current_command_buffer += 4;
}

__forceinline void* NWGetCurrentCmdBuffer(){ return __cb_current_command_buffer; }
__forceinline void* NWGetCmdBufferEnd()    { return __cb_current_max_command_buffer; }

__forceinline void NWForwardCurrentCmdBuffer(u32 size){
    __cb_current_command_buffer += size >> 2;
}

__forceinline void NWBackwardCurrentCmdBuffer(u32 size){
    __cb_current_command_buffer -= size >> 2;
}

inline u32 MakeCommandHeader(u32 address, int count, bool incremental, u8 byteEnable){
    enum{
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
        result |= (0x1U << BURST_MODE_SHIFT);
    result |= nw::ut::internal::MakeBits(count - 1, SIZE_WIDTH, SIZE_SHIFT);
    result |= nw::ut::internal::MakeBits(address, ADDRESS_WIDTH, ADDRESS_SHIFT);
    result |= nw::ut::internal::MakeBits(byteEnable, BYTE_ENABLE_WIDTH, BYTE_ENABLE_SHIFT);
    return result;
}

#define NW_GFX_GPU_HEADER(addr, count, incremental, byteEnable) (u32)((incremental << 31) | (count << 20) | (byteEnable << 16) | address)

enum UniformRegistry{
    REG_VERTEX_UNIFORM_FLOAT_INDEX   = 0x2C0,
    REG_VERTEX_UNIFORM_FLOAT_BASE    = 0x2C1,
    REG_GEOMETRY_UNIFORM_FLOAT_INDEX = 0x290,
    REG_GEOMETRY_UNIFORM_FLOAT_BASE  = 0x291
};

void NWCopyVec4Reverse(f32* dst, const f32* src);
void NWCopyVec4WithHeader(f32* dst, const f32* src, u32 header);
void NWCopyMtx24Reverse(f32* dst, const f32* src);
void NWCopyMtx24WithHeader(f32* dst, const f32* src, u32 header);
void NWCopyMtx34Reverse(f32* dst, const f32* src);
void NWCopyMtx34WithHeader(f32* dst, const f32* src, u32 header);
void NWCopyMtx44Reverse(f32* dst, const f32* src);
void NWCopyMtx44WithHeader(f32* dst, const f32* src, u32 header);
void NWCopyVec3Reverse(f32* dst, const f32* src);
void NWCopyVec3WithHeader(f32* dst, const f32* src, u32 header);
void NWCopyMtx23Reverse(f32* dst, const f32* src);
void NWCopyMtx23WithHeader(f32* dst, const f32* src, u32 header);
void NWCopyMtx33Reverse(f32* dst, const f32* src);
void NWCopyMtx33WithHeader(f32* dst, const f32* src, u32 header);
void NWCopyMtx43Reverse(f32* dst, const f32* src);
void NWCopyMtx43WithHeader(f32* dst, const f32* src, u32 header);
void NWCopyVec2Reverse(f32* dst, const f32* src);
void NWCopyVec2WithHeader(f32* dst, const f32* src, u32 header);
void NWCopyMtx22Reverse(f32* dst, const f32* src);
void NWCopyMtx22WithHeader(f32* dst, const f32* src, u32 header);
void NWCopyMtx32Reverse(f32* dst, const f32* src);
void NWCopyMtx32WithHeader(f32* dst, const f32* src, u32 header);
void NWCopyMtx42Reverse(f32* dst, const f32* src);
void NWCopyMtx42WithHeader(f32* dst, const f32* src, u32 header);
void NWCopyVec1Reverse(f32* dst, const f32* src);
void NWCopyVec1WithHeader(f32* dst, const f32* src, u32 header);
void NWCopyMtx21Reverse(f32* dst, const f32* src);
void NWCopyMtx21WithHeader(f32* dst, const f32* src, u32 header);
void NWCopyMtx31Reverse(f32* dst, const f32* src);
void NWCopyMtx31WithHeader(f32* dst, const f32* src, u32 header);
void NWCopyMtx41Reverse(f32* dst, const f32* src);
void NWCopyMtx41WithHeader(f32* dst, const f32* src, u32 header);

template<u32 RegFloatIndex>
__forceinline void NWSetUniform4fv(u32 index, int count, const f32* data){
    enum{
        REG_UNIFORM_FLOAT_INDEX = RegFloatIndex,
        REG_UNIFORM_FLOAT_BASE  = REG_UNIFORM_FLOAT_INDEX + 1,
        SIZE_SHIFT  = 20,
        VECTOR4     = 1,
        MATRIX2x4   = 2,
        MATRIX3x4   = 3,
        MATRIX4x4   = 4,
        REG_COUNT   = 4
    };

    NW_MINMAX_ASSERT(count, 1, 64);
    NW_NULL_ASSERT(data);
    NW_MINMAX_ASSERT(index, 0, 96);

    const u32 SHORT_VALUE_HEADER = MakeCommandHeader(REG_UNIFORM_FLOAT_INDEX, 1, true, 0xF);
    const u32 SHORT_HEADER[2]  = { 0x80000000 + index, SHORT_VALUE_HEADER | (REG_COUNT << SIZE_SHIFT) };
    const u32 SHORT_HEADER2[2] = { 0x80000000 + index, SHORT_VALUE_HEADER | ((REG_COUNT * 2) << SIZE_SHIFT) };
    const u32 LONG_ADDRESS_HEADER = MakeCommandHeader(REG_UNIFORM_FLOAT_INDEX, 1, false, 0xF);
    const u32 LONG_VALUE_HEADER   = MakeCommandHeader(REG_UNIFORM_FLOAT_BASE,  1, false, 0xF);
    const u32 LONG_HEADER[2] = { 0x80000000 + index, LONG_ADDRESS_HEADER };

    u32* command = (u32*)NWGetCurrentCmdBuffer();

    switch (count){
    case VECTOR4:
        command[0] = SHORT_HEADER[0]; command[1] = SHORT_HEADER[1];
        NWCopyVec4Reverse(reinterpret_cast<f32*>(&command[2]), data);
        NWForwardCurrentCmdBuffer(sizeof(u32) * 2 + sizeof(f32) * REG_COUNT);
        break;
    case MATRIX2x4:
        command[0] = SHORT_HEADER2[0]; command[1] = SHORT_HEADER2[1];
        NWCopyMtx24Reverse(reinterpret_cast<f32*>(&command[2]), data);
        NWForwardCurrentCmdBuffer(sizeof(u32) * 2 + sizeof(f32) * REG_COUNT * MATRIX2x4);
        break;
    case MATRIX3x4:
        command[0] = LONG_HEADER[0]; command[1] = LONG_HEADER[1];
        NWCopyMtx34WithHeader(reinterpret_cast<f32*>(&command[2]), data, LONG_VALUE_HEADER | ((MATRIX3x4 * REG_COUNT - 1) << SIZE_SHIFT));
        NWForwardCurrentCmdBuffer(sizeof(u32) * 4 + (REG_COUNT * MATRIX3x4) * sizeof(f32));
        break;
    case MATRIX4x4:
        command[0] = LONG_HEADER[0]; command[1] = LONG_HEADER[1];
        NWCopyMtx44WithHeader(reinterpret_cast<f32*>(&command[2]), data, LONG_VALUE_HEADER | ((MATRIX4x4 * REG_COUNT - 1) << SIZE_SHIFT));
        NWForwardCurrentCmdBuffer(sizeof(u32) * 4 + (REG_COUNT * MATRIX4x4) * sizeof(f32));
        break;
    default:
        NW_FATAL_ERROR("Not supported yet.\n");
        break;
    }
}

template<u32 RegFloatIndex>
__forceinline void NWSetUniform3fv(u32 index, int count, const f32* data){
    enum{
        REG_UNIFORM_FLOAT_INDEX = RegFloatIndex,
        REG_UNIFORM_FLOAT_BASE  = REG_UNIFORM_FLOAT_INDEX + 1,
        SIZE_SHIFT  = 20,
        VECTOR3     = 1,
        MATRIX2x3   = 2,
        MATRIX3x3   = 3,
        MATRIX4x3   = 4,
        REG_COUNT   = 4
    };

    NW_MINMAX_ASSERT(count, 1, 64);
    NW_NULL_ASSERT(data);
    NW_MINMAX_ASSERT(index, 0, 96);

    const u32 SHORT_VALUE_HEADER  = MakeCommandHeader(REG_UNIFORM_FLOAT_INDEX, 1, true, 0xF);
    const u32 SHORT_HEADER[2]     = { 0x80000000 + index, SHORT_VALUE_HEADER | (REG_COUNT << SIZE_SHIFT) };
    const u32 SHORT_HEADER2[2]    = { 0x80000000 + index, SHORT_VALUE_HEADER | ((REG_COUNT * 2) << SIZE_SHIFT) };
    const u32 LONG_ADDRESS_HEADER = MakeCommandHeader(REG_UNIFORM_FLOAT_INDEX, 1, false, 0xF);
    const u32 LONG_VALUE_HEADER   = MakeCommandHeader(REG_UNIFORM_FLOAT_BASE,  1, false, 0xF);
    const u32 LONG_HEADER[2]      = { 0x80000000 + index, LONG_ADDRESS_HEADER };

    u32* command = (u32*)NWGetCurrentCmdBuffer();

    switch (count){
    case VECTOR3:
        command[0] = SHORT_HEADER[0]; command[1] = SHORT_HEADER[1];
        NWCopyVec3Reverse(reinterpret_cast<f32*>(&command[2]), data);
        NWForwardCurrentCmdBuffer(sizeof(u32) * 2 + sizeof(f32) * REG_COUNT);
        break;
    case MATRIX2x3:
        command[0] = SHORT_HEADER2[0]; command[1] = SHORT_HEADER2[1];
        NWCopyMtx23Reverse(reinterpret_cast<f32*>(&command[2]), data);
        NWForwardCurrentCmdBuffer(sizeof(u32) * 2 + sizeof(f32) * REG_COUNT * MATRIX2x3);
        break;
    case MATRIX3x3:
        command[0] = LONG_HEADER[0]; command[1] = LONG_HEADER[1];
        NWCopyMtx33WithHeader(reinterpret_cast<f32*>(&command[2]), data, LONG_VALUE_HEADER | ((MATRIX3x3 * REG_COUNT - 1) << SIZE_SHIFT));
        NWForwardCurrentCmdBuffer(sizeof(u32) * 4 + (REG_COUNT * MATRIX3x3) * sizeof(f32));
        break;
    case MATRIX4x3:
        command[0] = LONG_HEADER[0]; command[1] = LONG_HEADER[1];
        NWCopyMtx43WithHeader(reinterpret_cast<f32*>(&command[2]), data, LONG_VALUE_HEADER | ((MATRIX4x3 * REG_COUNT - 1) << SIZE_SHIFT));
        NWForwardCurrentCmdBuffer(sizeof(u32) * 4 + (REG_COUNT * MATRIX4x3) * sizeof(f32));
        break;
    default:
        NW_FATAL_ERROR("Not supported yet.\n");
        break;
    }
}

template<u32 RegFloatIndex>
__forceinline void NWSetUniform2fv(u32 index, int count, const f32* data){
    enum{
        REG_UNIFORM_FLOAT_INDEX = RegFloatIndex,
        REG_UNIFORM_FLOAT_BASE  = REG_UNIFORM_FLOAT_INDEX + 1,
        SIZE_SHIFT  = 20,
        VECTOR2     = 1,
        MATRIX2x2   = 2,
        MATRIX3x2   = 3,
        MATRIX4x2   = 4,
        REG_COUNT   = 4
    };

    NW_MINMAX_ASSERT(count, 1, 64);
    NW_NULL_ASSERT(data);
    NW_MINMAX_ASSERT(index, 0, 96);

    const u32 SHORT_VALUE_HEADER  = MakeCommandHeader(REG_UNIFORM_FLOAT_INDEX, 1, true, 0xF);
    const u32 SHORT_HEADER[2]     = { 0x80000000 + index, SHORT_VALUE_HEADER | (REG_COUNT << SIZE_SHIFT) };
    const u32 SHORT_HEADER2[2]    = { 0x80000000 + index, SHORT_VALUE_HEADER | ((REG_COUNT * 2) << SIZE_SHIFT) };
    const u32 LONG_ADDRESS_HEADER = MakeCommandHeader(REG_UNIFORM_FLOAT_INDEX, 1, false, 0xF);
    const u32 LONG_VALUE_HEADER   = MakeCommandHeader(REG_UNIFORM_FLOAT_BASE,  1, false, 0xF);
    const u32 LONG_HEADER[2]      = { 0x80000000 + index, LONG_ADDRESS_HEADER };

    u32* command = (u32*)NWGetCurrentCmdBuffer();

    switch (count){
    case VECTOR2:
        command[0] = SHORT_HEADER[0]; command[1] = SHORT_HEADER[1];
        NWCopyVec2Reverse(reinterpret_cast<f32*>(&command[2]), data);
        NWForwardCurrentCmdBuffer(sizeof(u32) * 2 + sizeof(f32) * REG_COUNT);
        break;
    case MATRIX2x2:
        command[0] = SHORT_HEADER2[0]; command[1] = SHORT_HEADER2[1];
        NWCopyMtx22Reverse(reinterpret_cast<f32*>(&command[2]), data);
        NWForwardCurrentCmdBuffer(sizeof(u32) * 2 + sizeof(f32) * REG_COUNT * MATRIX2x2);
        break;
    case MATRIX3x2:
        command[0] = LONG_HEADER[0]; command[1] = LONG_HEADER[1];
        NWCopyMtx32WithHeader(reinterpret_cast<f32*>(&command[2]), data, LONG_VALUE_HEADER | ((MATRIX3x2 * REG_COUNT - 1) << SIZE_SHIFT));
        NWForwardCurrentCmdBuffer(sizeof(u32) * 4 + (REG_COUNT * MATRIX3x2) * sizeof(f32));
        break;
    case MATRIX4x2:
        command[0] = LONG_HEADER[0]; command[1] = LONG_HEADER[1];
        NWCopyMtx42WithHeader(reinterpret_cast<f32*>(&command[2]), data, LONG_VALUE_HEADER | ((MATRIX4x2 * REG_COUNT - 1) << SIZE_SHIFT));
        NWForwardCurrentCmdBuffer(sizeof(u32) * 4 + (REG_COUNT * MATRIX4x2) * sizeof(f32));
        break;
    default:
        NW_FATAL_ERROR("Not supported yet.\n");
        break;
    }
}

template<u32 RegFloatIndex>
__forceinline void NWSetUniform1fv(u32 index, int count, const f32* data){
    enum{
        REG_UNIFORM_FLOAT_INDEX = RegFloatIndex,
        REG_UNIFORM_FLOAT_BASE  = REG_UNIFORM_FLOAT_INDEX + 1,
        SIZE_SHIFT  = 20,
        VECTOR1     = 1,
        MATRIX2x1   = 2,
        MATRIX3x1   = 3,
        MATRIX4x1   = 4,
        REG_COUNT   = 4
    };

    NW_MINMAX_ASSERT(count, 1, 64);
    NW_NULL_ASSERT(data);
    NW_MINMAX_ASSERT(index, 0, 96);

    const u32 SHORT_VALUE_HEADER  = MakeCommandHeader(REG_UNIFORM_FLOAT_INDEX, 1, true, 0xF);
    const u32 SHORT_HEADER[2]     = { 0x80000000 + index, SHORT_VALUE_HEADER | (REG_COUNT << SIZE_SHIFT) };
    const u32 SHORT_HEADER2[2]    = { 0x80000000 + index, SHORT_VALUE_HEADER | ((REG_COUNT * 2) << SIZE_SHIFT) };
    const u32 LONG_ADDRESS_HEADER = MakeCommandHeader(REG_UNIFORM_FLOAT_INDEX, 1, false, 0xF);
    const u32 LONG_VALUE_HEADER   = MakeCommandHeader(REG_UNIFORM_FLOAT_BASE,  1, false, 0xF);
    const u32 LONG_HEADER[2]      = { 0x80000000 + index, LONG_ADDRESS_HEADER };

    u32* command = (u32*)NWGetCurrentCmdBuffer();

    switch (count){
    case VECTOR1:
        command[0] = SHORT_HEADER[0]; command[1] = SHORT_HEADER[1];
        NWCopyVec1Reverse(reinterpret_cast<f32*>(&command[2]), data);
        NWForwardCurrentCmdBuffer(sizeof(u32) * 2 + sizeof(f32) * REG_COUNT);
        break;
    case MATRIX2x1:
        command[0] = SHORT_HEADER2[0]; command[1] = SHORT_HEADER2[1];
        NWCopyMtx21Reverse(reinterpret_cast<f32*>(&command[2]), data);
        NWForwardCurrentCmdBuffer(sizeof(u32) * 2 + sizeof(f32) * REG_COUNT * MATRIX2x1);
        break;
    case MATRIX3x1:
        command[0] = LONG_HEADER[0]; command[1] = LONG_HEADER[1];
        NWCopyMtx31WithHeader(reinterpret_cast<f32*>(&command[2]), data, LONG_VALUE_HEADER | ((MATRIX3x1 * REG_COUNT - 1) << SIZE_SHIFT));
        NWForwardCurrentCmdBuffer(sizeof(u32) * 4 + (REG_COUNT * MATRIX3x1) * sizeof(f32));
        break;
    case MATRIX4x1:
        command[0] = LONG_HEADER[0]; command[1] = LONG_HEADER[1];
        NWCopyMtx41WithHeader(reinterpret_cast<f32*>(&command[2]), data, LONG_VALUE_HEADER | ((MATRIX4x1 * REG_COUNT - 1) << SIZE_SHIFT));
        NWForwardCurrentCmdBuffer(sizeof(u32) * 4 + (REG_COUNT * MATRIX4x1) * sizeof(f32));
        break;
    default:
        NW_FATAL_ERROR("Not supported yet.\n");
        break;
    }
}

template<u32 RegFloatIndex>
__forceinline void NWSetUniform4fvBegin(u32 index, int totalCount, int count, const f32* data){
    enum{
        REG_UNIFORM_FLOAT_INDEX = RegFloatIndex,
        REG_UNIFORM_FLOAT_BASE  = REG_UNIFORM_FLOAT_INDEX + 1,
        SIZE_SHIFT  = 20,
        VECTOR4     = 1,
        MATRIX2x4   = 2,
        MATRIX3x4   = 3,
        MATRIX4x4   = 4,
        REG_COUNT   = 4
    };

    const u32 ADDRESS_HEADER = MakeCommandHeader(REG_UNIFORM_FLOAT_INDEX, 1, false, 0xF);
    const u32 VALUE_HEADER   = MakeCommandHeader(REG_UNIFORM_FLOAT_BASE,  1, false, 0xF);

    NW_MINMAX_ASSERT(count, 1, 64);
    NW_MINMAX_ASSERT(totalCount, 1, 64);
    NW_NULL_ASSERT(data);
    NW_MINMAX_ASSERT(index, 0, 96);

    const u32 HEADER[2] = { 0x80000000 + index, ADDRESS_HEADER };

    u32* command = (u32*)NWGetCurrentCmdBuffer();
    command[0] = HEADER[0];
    command[1] = HEADER[1];

    switch (count){
    case VECTOR4:
        NWCopyVec4WithHeader(reinterpret_cast<f32*>(&command[2]), data, VALUE_HEADER | ((totalCount * REG_COUNT - 1) << SIZE_SHIFT));
        NWForwardCurrentCmdBuffer(sizeof(u32) * 3 + (REG_COUNT * VECTOR4) * sizeof(f32));
        break;
    case MATRIX2x4:
        NWCopyMtx24WithHeader(reinterpret_cast<f32*>(&command[2]), data, VALUE_HEADER | ((totalCount * REG_COUNT - 1) << SIZE_SHIFT));
        NWForwardCurrentCmdBuffer(sizeof(u32) * 3 + (REG_COUNT * MATRIX2x4) * sizeof(f32));
        break;
    case MATRIX3x4:
        NWCopyMtx34WithHeader(reinterpret_cast<f32*>(&command[2]), data, VALUE_HEADER | ((totalCount * REG_COUNT - 1) << SIZE_SHIFT));
        NWForwardCurrentCmdBuffer(sizeof(u32) * 3 + (REG_COUNT * MATRIX3x4) * sizeof(f32));
        break;
    case MATRIX4x4:
        NWCopyMtx44WithHeader(reinterpret_cast<f32*>(&command[2]), data, VALUE_HEADER | ((totalCount * REG_COUNT - 1) << SIZE_SHIFT));
        NWForwardCurrentCmdBuffer(sizeof(u32) * 3 + (REG_COUNT * MATRIX4x4) * sizeof(f32));
        break;
    default:
        NW_FATAL_ERROR("Not supported yet.\n");
        break;
    }
}

__forceinline void NWSetUniform4fvContinuous(int count, const f32* data){
    enum{ VECTOR4 = 1, MATRIX2x4 = 2, MATRIX3x4 = 3, MATRIX4x4 = 4, REG_COUNT = 4 };

    f32* command = (f32*)NWGetCurrentCmdBuffer();

    switch (count){
    case VECTOR4:   NWCopyVec4Reverse(command, data);  NWForwardCurrentCmdBuffer((REG_COUNT * VECTOR4)   * sizeof(f32)); break;
    case MATRIX2x4: NWCopyMtx24Reverse(command, data); NWForwardCurrentCmdBuffer((REG_COUNT * MATRIX2x4) * sizeof(f32)); break;
    case MATRIX3x4: NWCopyMtx34Reverse(command, data); NWForwardCurrentCmdBuffer((REG_COUNT * MATRIX3x4) * sizeof(f32)); break;
    case MATRIX4x4: NWCopyMtx44Reverse(command, data); NWForwardCurrentCmdBuffer((REG_COUNT * MATRIX4x4) * sizeof(f32)); break;
    default: NW_FATAL_ERROR("Not supported yet.\n"); break;
    }
}

__forceinline void NWSetUniform4fvEnd(){ NWForwardCurrentCmdBuffer(sizeof(f32)); }

__forceinline void NWSetVertexUniform4fv(u32 index, int count, const f32* data)   { NWSetUniform4fv<REG_VERTEX_UNIFORM_FLOAT_INDEX>(index, count, data); }
__forceinline void NWSetVertexUniform3fv(u32 index, int count, const f32* data)   { NWSetUniform3fv<REG_VERTEX_UNIFORM_FLOAT_INDEX>(index, count, data); }
__forceinline void NWSetVertexUniform2fv(u32 index, int count, const f32* data){NWSetUniform2fv<REG_VERTEX_UNIFORM_FLOAT_INDEX>(index, count, data);}
__forceinline void NWSetVertexUniform1fv(u32 index, int count, const f32* data)   { NWSetUniform1fv<REG_VERTEX_UNIFORM_FLOAT_INDEX>(index, count, data); }
__forceinline void NWSetGeometryUniform4fv(u32 index, int count, const f32* data) { NWSetUniform4fv<REG_GEOMETRY_UNIFORM_FLOAT_INDEX>(index, count, data); }
__forceinline void NWSetGeometryUniform3fv(u32 index, int count, const f32* data) { NWSetUniform3fv<REG_GEOMETRY_UNIFORM_FLOAT_INDEX>(index, count, data); }
__forceinline void NWSetGeometryUniform2fv(u32 index, int count, const f32* data) { NWSetUniform2fv<REG_GEOMETRY_UNIFORM_FLOAT_INDEX>(index, count, data); }
__forceinline void NWSetGeometryUniform1fv(u32 index, int count, const f32* data) { NWSetUniform1fv<REG_GEOMETRY_UNIFORM_FLOAT_INDEX>(index, count, data); }

__forceinline void NWSetVertexUniform4fvBegin(u32 index, int totalCount, int count, const f32* data)   { NWSetUniform4fvBegin<REG_VERTEX_UNIFORM_FLOAT_INDEX>(index, totalCount, count, data); }
__forceinline void NWSetVertexUniform4fvContinuous(int count, const f32* data)                         { NWSetUniform4fvContinuous(count, data); }
__forceinline void NWSetVertexUniform4fvEnd()                                                          { NWSetUniform4fvEnd(); }
__forceinline void NWSetGeometryUniform4fvBegin(u32 index, int totalCount, int count, const f32* data) { NWSetUniform4fvBegin<REG_GEOMETRY_UNIFORM_FLOAT_INDEX>(index, totalCount, count, data); }
__forceinline void NWSetGeometryUniform4fvContinuous(int count, const f32* data)                       { NWSetUniform4fvContinuous(count, data); }
__forceinline void NWSetGeometryUniform4fvEnd()                                                        { NWSetUniform4fvEnd(); }

class CommandBufferInfo{
public:
    enum BufferResult{
        RESULT_OK            = 0,
        RESULT_OUT_OF_MEMORY = 1 << 1
    };

    CommandBufferInfo(void* buffer, size_t size){
        mTopAddress     = static_cast<u8*>(buffer);
        mCurrentAddress = mTopAddress;
        mSize           = size;
        mBottomAddress  = reinterpret_cast<u8*>(nw::ut::AddOffsetToPtr(mTopAddress, size));
    }

    CommandBufferInfo(){
        mTopAddress = mCurrentAddress = mBottomAddress = NULL;
        mSize = 0;
    }

    u8*       GetCurrentAddress()       { return (mCurrentAddress != NULL) ? mCurrentAddress : reinterpret_cast<u8*>(__cb_current_command_buffer); }
    const u8* GetCurrentAddress() const { return (mCurrentAddress != NULL) ? mCurrentAddress : reinterpret_cast<const u8*>(__cb_current_command_buffer); }
    size_t    GetCurrentSize()    const { return nw::ut::GetOffsetFromPtr(mTopAddress, mCurrentAddress); }

    Result ForwardCommand(int size){
        if (!this->CheckRestMemory(size)) return Result(RESULT_OUT_OF_MEMORY);
        this->ForwardAddress(size);
        return Result(RESULT_OK);
    }

    Result PutCommand(const void* buffer, int size){
        NW_NULL_ASSERT(buffer);
        NW_ASSERT(size > 0);
        if (!this->CheckRestMemory(size)) return Result(RESULT_OUT_OF_MEMORY);
        nw::os::MemCpy(this->GetCurrentAddress(), buffer, size);
        this->ForwardAddress(size);
        return Result(RESULT_OK);
    }

    template<int size>
    Result PutCommand(const void* buffer){
        NW_NULL_ASSERT(buffer);
        if (!this->CheckRestMemory(size)) return Result(RESULT_OUT_OF_MEMORY);
        internal::FastWordCopy((u32*)this->GetCurrentAddress(), (u32*)buffer, size);
        this->ForwardAddress(size);
    }

private:
    u8*    mTopAddress;
    u8*    mBottomAddress;
    u8*    mCurrentAddress;
    size_t mSize;

    bool CheckRestMemory(size_t size){
        if (mCurrentAddress == NULL)
            return nw::ut::AddOffsetToPtr(__cb_current_command_buffer, size) <= __cb_current_max_command_buffer;
        else
            return nw::ut::AddOffsetToPtr(mCurrentAddress, size) <= mBottomAddress;
    }

    void ForwardAddress(int size){
        if (mCurrentAddress != NULL)
            mCurrentAddress += size;
        else
            NWForwardCurrentCmdBuffer(size);
    }
};

template<>
inline Result CommandBufferInfo::PutCommand<4>(const void* buffer){
    NW_NULL_ASSERT(buffer);
    if (!this->CheckRestMemory(4)) return Result(RESULT_OUT_OF_MEMORY);
    *reinterpret_cast<u32*>(this->GetCurrentAddress()) = *reinterpret_cast<const u32*>(buffer);
    this->ForwardAddress(4);
    return Result(RESULT_OK);
}

template<>
inline Result CommandBufferInfo::PutCommand<8>(const void* buffer){
    NW_NULL_ASSERT(buffer);
    if (!this->CheckRestMemory(8)) return Result(RESULT_OUT_OF_MEMORY);
    *reinterpret_cast<u64*>(this->GetCurrentAddress()) = *reinterpret_cast<const u64*>(buffer);
    this->ForwardAddress(8);
    return Result(RESULT_OK);
}

template<>
inline Result CommandBufferInfo::PutCommand<12>(const void* buffer){
    NW_NULL_ASSERT(buffer);
    if (!this->CheckRestMemory(12)) return Result(RESULT_OUT_OF_MEMORY);
    *reinterpret_cast<u64*>(this->GetCurrentAddress()) = *reinterpret_cast<const u64*>(buffer);
    *reinterpret_cast<u32*>(this->GetCurrentAddress() + 8) = *(reinterpret_cast<const u32*>(buffer) + 2);
    this->ForwardAddress(12);
    return Result(RESULT_OK);
}

template<>
inline Result CommandBufferInfo::PutCommand<16>(const void* buffer){
    NW_NULL_ASSERT(buffer);
    if (!this->CheckRestMemory(16)) return Result(RESULT_OUT_OF_MEMORY);
    *reinterpret_cast<u64*>(this->GetCurrentAddress()) = *reinterpret_cast<const u64*>(buffer);
    *reinterpret_cast<u64*>(this->GetCurrentAddress() + 8) = *(reinterpret_cast<const u64*>(buffer) + 1);
    this->ForwardAddress(16);
    return Result(RESULT_OK);
}

class CommandCacheBuilder{
public:
    CommandCacheBuilder(): mStartAddr(NULL), mEndAddr(NULL) {}

    void Begin(){ mStartAddr = NWGetCurrentCmdBuffer(); }
    void End(){ if (mStartAddr) mEndAddr = NWGetCurrentCmdBuffer(); }

    void Rollback(){
        void* current = NWGetCurrentCmdBuffer();
        NWBackwardCurrentCmdBuffer(nw::ut::GetOffsetFromPtr(mStartAddr, current));
    }

    void Reset(){ mStartAddr = mEndAddr = NULL; }

    s32 GetSize(){
        if (mStartAddr && mEndAddr)
            return nw::ut::GetOffsetFromPtr(mStartAddr, mEndAddr);
        return 0;
    }

    void* AllocAndCopy(nw::os::IAllocator* allocator = NULL){
        s32 size = this->GetSize();
        if (size == 0) return NULL;
        void* buffer = (allocator != NULL) ? allocator->Alloc(size, 4) : CommandCacheManager::Allocate(size);
        if (!buffer) return NULL;
        nw::os::MemCpy(buffer, mStartAddr, size);
        return buffer;
    }

private:
    void* mStartAddr;
    void* mEndAddr;
};

} // namespace internal
} // namespace gfx
} // namespace nw