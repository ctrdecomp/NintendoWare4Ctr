#include <nw/os/os_Memory.h>
#include <nw/gfx/gfx_ActivateCommand.h>
//#include <nw/gfx/gfx_DisplayList.h>
#include <nw/gfx/res/gfx_ResVertex.h>
#include <nw/gfx/res/gfx_ResShape.h>
#include <nw/gfx/gfx_ShaderBinaryInfo.h>

namespace nw{
namespace gfx{

nw::os::IAllocator* CommandCacheManager::sAllocator = NULL;

namespace internal{
namespace{

const u32 CLEAR_VERTEX_COMMAND[] ={
    0xBFFF0000,
    internal::MakeCommandHeader(0x202, 1, false, 0xF),0,
    internal::MakeCommandHeader(0x205, 1, false, 0xF),0,
    internal::MakeCommandHeader(0x205 + 3, 1, false, 0xF),0,
    internal::MakeCommandHeader(0x205 + 3 * 2, 1, false, 0xF),0,
    internal::MakeCommandHeader(0x205 + 3 * 3, 1, false, 0xF),0,
    internal::MakeCommandHeader(0x205 + 3 * 4, 1, false, 0xF),0,
    internal::MakeCommandHeader(0x205 + 3 * 5, 1, false, 0xF),0,
    internal::MakeCommandHeader(0x205 + 3 * 6, 1, false, 0xF),0,
    internal::MakeCommandHeader(0x205 + 3 * 7, 1, false, 0xF),0,
    internal::MakeCommandHeader(0x205 + 3 * 8, 1, false, 0xF),0,
    internal::MakeCommandHeader(0x205 + 3 * 9, 1, false, 0xF),0,
    internal::MakeCommandHeader(0x205 + 3 * 10, 1, false, 0xF),0,
    internal::MakeCommandHeader(0x205 + 3 * 11, 1, false, 0xF),1,
    internal::MakeCommandHeader(0x232, 4, true, 0xF),0,0,0,0,2,
    internal::MakeCommandHeader(0x232, 4, true, 0xF),0,0,0,0,3,
    internal::MakeCommandHeader(0x232, 4, true, 0xF),0,0,0,0,4,
    internal::MakeCommandHeader(0x232, 4, true, 0xF),0,0,0,0,5,
    internal::MakeCommandHeader(0x232, 4, true, 0xF),0,0,0,0,6,
    internal::MakeCommandHeader(0x232, 4, true, 0xF),0,0,0,0,7,
    internal::MakeCommandHeader(0x232, 4, true, 0xF),0,0,0,0,8,
    internal::MakeCommandHeader(0x232, 4, true, 0xF),0,0,0,0,9,
    internal::MakeCommandHeader(0x232, 4, true, 0xF),0,0,0,0,10,
    internal::MakeCommandHeader(0x232, 4, true, 0xF),0,0,0,0,11,
    internal::MakeCommandHeader(0x232, 4, true, 0xF),0,0,0,0
};

template <typename TShape>
s32 CalcSetupActivateVertexAttributeCommandSize_(TShape shape, ResShaderProgramDescription shaderProgramDesc){
    s32 commandSize = 12;

    s32 vtxAttrNum = shape.GetVertexAttributesCount();

    for (s32 i = 0; i < vtxAttrNum; ++i){
        ResVertexAttribute attribute = shape.GetVertexAttributes(i);

        if (attribute.GetFlags() & ResVertexAttributeData::FLAG_VERTEX_PARAM){
            commandSize += 6;
        }
        else{
            commandSize += 4;
        }
    }

    if (shaderProgramDesc.GetGeometryShaderIndex() >= 0){
        commandSize += 4;
    }

    return sizeof(u32) * commandSize;
}

template <typename TShape>
s32 CalcSetupDeactivateVertexAttributeCommandSize_(TShape shape, ResShaderProgramDescription shaderProgramDesc){
    NW_UNUSED_VARIABLE(shaderProgramDesc);

    int inputIndex = 0;
    s32 vtxAttrNum = shape.GetVertexAttributesCount();
    s32 commandIndex = 0;

    for (s32 i = 0; i < vtxAttrNum; ++i){
        ResVertexAttribute attribute = shape.GetVertexAttributes(i);

        if (attribute.GetFlags() & ResVertexAttributeData::FLAG_VERTEX_PARAM){
            ++inputIndex;
            continue;
        }

        if (attribute.GetFlags() & ResVertexAttributeData::FLAG_INTERLEAVE){
            ResInterleavedVertexStream interleave = ResStaticCast<ResInterleavedVertexStream>(attribute);
            s32 streamCount = interleave.GetVertexStreamsCount();
            inputIndex += streamCount;
        }
        else{
            ++inputIndex;
        }

        commandIndex += 2;
    }

    commandIndex += 6;

    for (int i = 1; i < inputIndex; ++i){
        commandIndex += 6;
    }

    return sizeof(u32) * commandIndex;
}

inline s32 GetAttributeIndexFromUsage(ResShaderProgramDescription shaderProgramDesc, s32 usage){
    return shaderProgramDesc.GetAttributeIndices(usage);
}

} // namespace

void ClearVertexAttribute(){
    NWUseCmdlist(CLEAR_VERTEX_COMMAND, sizeof(CLEAR_VERTEX_COMMAND));
}

template <typename TShape>
static s32 SetupActivateVertexAttributeCommand_(CommandBufferInfo& bufferInfo, TShape shape, ResShaderProgramDescription shaderProgramDesc){
    enum{
        MAX_ATTRIBUTES_NUM = 12,

        REG_VTX_SHADER_ATTR_NUM = 0x2b9,
        REG_VTX_SHADER_ATTR_NUM_2 = 0x242,
        REG_VTX_MAP_0 = 0x2bb,
        REG_VTX_MAP_1 = 0x2bc,
        REG_VTX_STREAM_BASE = 0x200,
        REG_VTX_ARRAY_OFFSET = 0x203,
        REG_VTX_PARAM_INDEX = 0x232,
        REG_GEOM_MAP_0 = 0x28b,
        REG_GEOM_MAP_1 = 0x28c
    };

    const ShaderBinaryInfo* shaderInfo = shaderProgramDesc.GetShaderBinaryInfo();
    NW_NULL_ASSERT(shaderInfo);

    u32 shaderVtxAttrNum = shaderInfo->GetInputRegisterNum(shaderProgramDesc.GetVertexShaderIndex());

    u32 baseAddr = nngxGetPhysicalAddr(nn::gx::CTR::GetVramStartAddr(nn::gx::CTR::MEM_VRAMA));
    shape.ref().mBaseAddress = baseAddr;

    s32 vtxAttrNum = shape.GetVertexAttributesCount();
    s32 inputRegNum = shaderVtxAttrNum;

    u32* command = reinterpret_cast<u32*>(bufferInfo.GetCurrentAddress());

    command[0] = (inputRegNum - 1) | 0xa0000000;
    command[1] = internal::MakeCommandHeader(REG_VTX_SHADER_ATTR_NUM, 1, false, 0xb);

    command[2] = (inputRegNum - 1);
    command[3] = internal::MakeCommandHeader(REG_VTX_SHADER_ATTR_NUM_2, 1, false, 0x1);

    command[4] = 0;
    command[5] = internal::MakeCommandHeader(REG_VTX_MAP_0, 2, true, 0xF);
    command[6] = 0;
    command[7] = 0;

    command[8] = baseAddr >> 3;
    command[9] = internal::MakeCommandHeader(REG_VTX_STREAM_BASE, 3, true, 0xF);
    command[10] = 0;
    command[11] = static_cast<u32>(inputRegNum - 1) << 28;

    u32* inputMapTable = &command[4];
    u32* inputFormat = &command[10];
    u32* vertexParamMask = &command[11];

    int inputIndex = 0;
    int arrayIndex = 0;
    u32 commandIndex = 12;

    u32 usedFlag = 0;

    for (s32 i = 0; i < vtxAttrNum; ++i){
        ResVertexAttribute attribute = shape.GetVertexAttributes(i);

        if (attribute.GetFlags() & ResVertexAttributeData::FLAG_VERTEX_PARAM){
            continue;
        }

        if (attribute.GetFlags() & ResVertexAttributeData::FLAG_INTERLEAVE){
            ResInterleavedVertexStream interleave = ResStaticCast<ResInterleavedVertexStream>(attribute);

            u32 bufferAddr = nngxGetPhysicalAddr(interleave.GetImageAddress());
            s32 streamCount = interleave.GetVertexStreamsCount();

            command[commandIndex] = bufferAddr - baseAddr;
            command[commandIndex + 1] = internal::MakeCommandHeader(REG_VTX_ARRAY_OFFSET + 3 * arrayIndex, 3, true, 0xF);
            command[commandIndex + 2] = 0;
            command[commandIndex + 3] = (interleave.GetStride() << 16) | (streamCount << 28);

            u32* arraySetting = &command[commandIndex + 2];
            commandIndex += 4;

            for (s32 streamIdx = 0; streamIdx < streamCount; ++streamIdx){
                ResVertexStream stream = interleave.GetVertexStreams(streamIdx);

                s32 usage = stream.GetUsage();
                s32 usageIndex = GetAttributeIndexFromUsage(shaderProgramDesc, usage);
                NW_ASSERT(0 <= usageIndex && usageIndex < MAX_ATTRIBUTES_NUM);

                inputMapTable[(inputIndex / 8) * 2] |= (usageIndex & 0xf) << (4 * (inputIndex % 8));
                usedFlag |= 0x1 << usageIndex;

                u32 format = stream.GetFormatType();
                u32 dimension = stream.GetDimension();

                inputFormat[inputIndex / 8] |= CommandCacheHelper::GetVertexFormat(dimension, format) << ((inputIndex % 8) * 4);

                if (streamIdx < 8){
                    arraySetting[0] |= inputIndex << (streamIdx * 4);
                }
                else{
                    arraySetting[1] |= inputIndex << ((streamIdx - 8) * 4);
                }

                ++inputIndex;
            }
        }
        else{
            s32 usage = attribute.GetUsage();
            s32 usageIndex = GetAttributeIndexFromUsage(shaderProgramDesc, usage);

            NW_ASSERT(0 <= usageIndex && usageIndex < MAX_ATTRIBUTES_NUM);
            inputMapTable[(inputIndex / 8) * 2] |= (usageIndex & 0xF) << (4 * (inputIndex % 8));
            usedFlag |= 0x1 << usageIndex;

            ResVertexStream stream = ResStaticCast<ResVertexStream>(attribute);

            u32 format = stream.GetFormatType();
            u32 dimension = stream.GetDimension();

            inputFormat[inputIndex / 8] |= CommandCacheHelper::GetVertexFormat(dimension, format) << ((inputIndex % 8) * 4);

            u32 bufferAddr = nngxGetPhysicalAddr(stream.GetImageAddress());

            command[commandIndex] = bufferAddr - baseAddr;
            command[commandIndex + 1] = internal::MakeCommandHeader(REG_VTX_ARRAY_OFFSET + 3 * arrayIndex, 3, true, 0xF);
            command[commandIndex + 2] = inputIndex;
            command[commandIndex + 3] = (CommandCacheHelper::GetVertexSize(dimension, format) << 16) + (1 << 28);

            ++inputIndex;
            commandIndex += 4;
        }

        ++arrayIndex;
    }

    const u32 HEADER_VTX_PARAM_INDEX = internal::MakeCommandHeader(REG_VTX_PARAM_INDEX, 4, true, 0xF);

    for (s32 i = 0; i < vtxAttrNum; ++i){
        ResVertexAttribute attribute = shape.GetVertexAttributes(i);

        if (attribute.GetFlags() & ResVertexAttributeData::FLAG_VERTEX_PARAM){
            ResVertexParamAttribute param = ResStaticCast<ResVertexParamAttribute>(attribute);

            s32 usage = param.GetUsage();
            s32 usageIndex = GetAttributeIndexFromUsage(shaderProgramDesc, usage);
            NW_ASSERT(0 <= usageIndex && usageIndex < 12);
            inputMapTable[(inputIndex / 8) * 2] |= (usageIndex & 0xF) << (4 * (inputIndex % 8));
            usedFlag |= 0x1 << usageIndex;

            u32 data[4] = { 0, 0, 0, 0 };

            int count = param.GetAttributeCount();
            f32* fdata = param.GetAttribute();

            for (int j = 0; j < count; ++j){
                data[j] = ut::Float24::Float32ToBits24(fdata[j]);
            }

            command[commandIndex] = inputIndex;
            command[commandIndex + 1] = HEADER_VTX_PARAM_INDEX;
            command[commandIndex + 2] = (data[3] << 8) | (data[2] >> 16);
            command[commandIndex + 3] = (data[2] << 16) | (data[1] >> 8);
            command[commandIndex + 4] = (data[1] << 24) | (data[0]);
            command[commandIndex + 5] = 0;

            vertexParamMask[0] |= 1 << (16 + inputIndex);

            ++inputIndex;
            commandIndex += 6;
        }
    }

    while (inputIndex < shaderVtxAttrNum){
        s32 usage = shaderVtxAttrNum - 1;
        s32 usageIndex = GetAttributeIndexFromUsage(shaderProgramDesc, usage);
        while (usedFlag & (0x1 << usageIndex)){
            --usageIndex;
        }
        inputMapTable[(inputIndex / 8) * 2] |= (usageIndex & 0xF) << (4 * (inputIndex % 8));
        usedFlag |= 0x1 << usageIndex;

        command[commandIndex] = inputIndex;
        command[commandIndex + 1] = HEADER_VTX_PARAM_INDEX;
        command[commandIndex + 2] = (ut::Float24::Float32ToBits24(1.0f) << 8);
        command[commandIndex + 3] = 0;
        command[commandIndex + 4] = 0;
        command[commandIndex + 5] = 0;
        commandIndex += 6;

        vertexParamMask[0] |= 1 << (16 + inputIndex);
        ++inputIndex;
    }

    const u32 HEADER_GEOM_MAP_0 = internal::MakeCommandHeader(REG_GEOM_MAP_0, 1, false, 0xF);
    const u32 HEADER_GEOM_MAP_1 = internal::MakeCommandHeader(REG_GEOM_MAP_1, 1, false, 0xF);

    if (shaderProgramDesc.GetGeometryShaderIndex() >= 0){
        command[commandIndex + 0] = 0x76543210;
        command[commandIndex + 1] = HEADER_GEOM_MAP_0;
        command[commandIndex + 2] = 0xfedcba98;
        command[commandIndex + 3] = HEADER_GEOM_MAP_1;
        commandIndex += 4;
    }

    return static_cast<s32>(commandIndex * sizeof(u32));
}

template <typename TShape>
static s32 SetupDeactivateVertexAttributeCommand_(CommandBufferInfo& bufferInfo, TShape shape, ResShaderProgramDescription shaderProgramDesc){
    NW_UNUSED_VARIABLE(shaderProgramDesc);

    enum{
        REG_VTX_ARRAY_VTXMASK = 0x202,
        REG_VTX_ARRAY_OFFSET = 0x203,
        REG_VTX_PARAM_INDEX = 0x232,
        REG_VTX_MAP_0 = 0x2bb,
        REG_VTX_MAP_1 = 0x2bc
    };

    s32 vtxAttrNum = shape.GetVertexAttributesCount();

    u32* command = reinterpret_cast<u32*>(bufferInfo.GetCurrentAddress());

    int inputIndex = 0;
    int arrayIndex = 0;
    u32 commandIndex = 0;

    for (s32 i = 0; i < vtxAttrNum; ++i){
        ResVertexAttribute attribute = shape.GetVertexAttributes(i);

        if (attribute.GetFlags() & ResVertexAttributeData::FLAG_VERTEX_PARAM){
            ++inputIndex;
            continue;
        }

        if (attribute.GetFlags() & ResVertexAttributeData::FLAG_INTERLEAVE){
            ResInterleavedVertexStream interleave = ResStaticCast<ResInterleavedVertexStream>(attribute);

            command[commandIndex] = 0;
            command[commandIndex + 1] = internal::MakeCommandHeader(REG_VTX_ARRAY_OFFSET + 2 + 3 * arrayIndex, 1, false, 0xF);
            commandIndex += 2;

            s32 streamCount = interleave.GetVertexStreamsCount();
            inputIndex += streamCount;
        }
        else{
            command[commandIndex] = 0;
            command[commandIndex + 1] = internal::MakeCommandHeader(REG_VTX_ARRAY_OFFSET + 2 + 3 * arrayIndex, 1, false, 0xF);
            commandIndex += 2;

            ++inputIndex;
        }

        ++arrayIndex;
    }

    {
        const u32 INPUT_MAP0 = 0x76543210;
        const u32 INPUT_MAP1 = 0x0000ba98;

        command[commandIndex++] = INPUT_MAP0;
        command[commandIndex++] = internal::MakeCommandHeader(REG_VTX_MAP_0, 2, true, 0xF);
        command[commandIndex++] = INPUT_MAP1;
        command[commandIndex++] = 0;
    }

    command[commandIndex++] = (static_cast<u32>(inputIndex - 1) << 28) | (((0x1 << inputIndex) - 2) << 16);
    command[commandIndex++] = internal::MakeCommandHeader(REG_VTX_ARRAY_VTXMASK, 1, false, 0xF);

    const u32 HEADER_VTX_PARAM_INDEX = internal::MakeCommandHeader(REG_VTX_PARAM_INDEX, 4, true, 0xF);

    for (int i = 1; i < inputIndex; ++i){
        command[commandIndex] = i;
        command[commandIndex + 1] = HEADER_VTX_PARAM_INDEX;
        command[commandIndex + 2] = 0;
        command[commandIndex + 3] = 0;
        command[commandIndex + 4] = 0;
        command[commandIndex + 5] = 0;
        commandIndex += 6;
    }

    return static_cast<s32>(commandIndex * sizeof(u32));
}

s32 SetupVertexAttributeCommand(CommandBufferInfo& bufferInfo, ResSeparateDataShape shape, ResShaderProgramDescription shaderProgramDesc){
    return SetupActivateVertexAttributeCommand_(bufferInfo, shape, shaderProgramDesc);
}

s32 SetupVertexAttributeCommand(CommandBufferInfo& bufferInfo, ResParticleShape shape, ResShaderProgramDescription shaderProgramDesc){
    return SetupActivateVertexAttributeCommand_(bufferInfo, shape, shaderProgramDesc);
}

s32 SetupDeactivateVertexAttributeCommand(CommandBufferInfo& bufferInfo, ResSeparateDataShape shape, ResShaderProgramDescription shaderProgramDesc){
    return SetupDeactivateVertexAttributeCommand_(bufferInfo, shape, shaderProgramDesc);
}

s32 SetupDeactivateVertexAttributeCommand(CommandBufferInfo& bufferInfo, ResParticleShape shape, ResShaderProgramDescription shaderProgramDesc){
    return SetupDeactivateVertexAttributeCommand_(bufferInfo, shape, shaderProgramDesc);
}

s32 CalcSetupActivateVertexAttributeCommandSize(ResSeparateDataShape shape, ResShaderProgramDescription shaderProgramDesc){
    return CalcSetupActivateVertexAttributeCommandSize_(shape, shaderProgramDesc);
}

s32 CalcSetupActivateVertexAttributeCommandSize_(ResParticleShape shape, ResShaderProgramDescription shaderProgramDesc){
    return CalcSetupActivateVertexAttributeCommandSize_(shape, shaderProgramDesc);
}

s32 CalcSetupDeactivateVertexAttributeCommandSize(ResSeparateDataShape shape, ResShaderProgramDescription shaderProgramDesc){
    return CalcSetupDeactivateVertexAttributeCommandSize_(shape, shaderProgramDesc);
}

s32 CalcSetupDeactivateVertexAttributeCommandSize_(ResParticleShape shape, ResShaderProgramDescription shaderProgramDesc){
    return CalcSetupDeactivateVertexAttributeCommandSize_(shape, shaderProgramDesc);
}

s32 SetupShaderProgramMode(bool useGeometry){
    static u32 USE_GEOMETRY_COMMAND[] ={
        0x00000000, 0x00900251, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x01d00200, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,

        0x0, 0x00010229,

        0x00000000, 0x01d00200, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
    };

    const u32 IDX_REG_229 = 44;

    if (useGeometry){
        USE_GEOMETRY_COMMAND[IDX_REG_229] = 0x00000002;
    }
    else{
        USE_GEOMETRY_COMMAND[IDX_REG_229] = 0x0;
    }

    NWUseCmdlist<sizeof(USE_GEOMETRY_COMMAND)>(&USE_GEOMETRY_COMMAND[0]);

    return sizeof(USE_GEOMETRY_COMMAND);
}

static inline GLuint ToPrimitiveModeGL(u8 mode, bool isGeometryShaderEnabled){
    static const GLuint PRIM_MODE_TABLE[] ={
        GL_TRIANGLES,
        GL_TRIANGLE_STRIP,
        GL_TRIANGLE_FAN
    };

    NW_ASSERT(mode < (sizeof(PRIM_MODE_TABLE) / sizeof(GLuint)));

    GLuint glMode = PRIM_MODE_TABLE[mode];
    if (isGeometryShaderEnabled){
        glMode = GL_GEOMETRY_PRIMITIVE_DMP;
    }

    return glMode;
}

s32 CalcSetupDrawIndexStreamCommand(ResIndexStream indexStream){
    NW_UNUSED_VARIABLE(indexStream);

    return 28 * sizeof(u32);
}

s32 SetupDrawIndexStreamCommand(CommandBufferInfo& bufferInfo, ResIndexStream indexStream, bool hasGeometryShader){
    enum{
        REG_INDEX_STREAM_OFFSET = 0x227,
        REG_INDEX_STREAM_COUNT = 0x228,
        REG_ELEMENTS_MODE = 0x229,
        REG_ELEMENTS_MODE_2 = 0x253,
        REG_ELEMENTS_MODE_3 = 0x25e,
        REG_TRIANGLE_INDEX_RESET = 0x25f,
        REG_DRAW_READY = 0x245,
        REG_DRAW_KICK = 0x22f,
        REG_VERTEX_CACHE_CLEAR = 0x231,
        REG_COLOR_DEPTH_CACHE_CLEAR = 0x110,
        REG_COLOR_DEPTH_CACHE_FLUSH = 0x111
    };

    GLuint mode = ToPrimitiveModeGL(indexStream.GetPrimitiveMode(), hasGeometryShader);

    const size_t commandCount = 28;
    u32* command = reinterpret_cast<u32*>(bufferInfo.GetCurrentAddress());

    u32 baseAddr = nngxGetPhysicalAddr(nn::gx::CTR::GetVramStartAddr(nn::gx::CTR::MEM_VRAMA));
    u32 bufferAddr = nngxGetPhysicalAddr(indexStream.GetImageAddress());

    NW_ASSERT((bufferAddr - baseAddr) < 0x10000000);

    const u32 HEADER_INDEX_STREAM_OFFSET = internal::MakeCommandHeader(REG_INDEX_STREAM_OFFSET, 1, false, 0xF);
    const u32 HEADER_INDEX_STREAM_COUNT = internal::MakeCommandHeader(REG_INDEX_STREAM_COUNT, 1, false, 0xF);

    command[0] = (bufferAddr - baseAddr);
    command[1] = HEADER_INDEX_STREAM_OFFSET;
    command[2] = indexStream.GetStreamCount();
    command[3] = HEADER_INDEX_STREAM_COUNT;

    if (indexStream.GetFormatType() == GL_UNSIGNED_SHORT){
        command[0] |= 0x80000000;
        command[2] /= 2;
    }

    const u32 HEADER_ELEMENTS_MODE = internal::MakeCommandHeader(REG_ELEMENTS_MODE, 1, false, 0x2);
    const u32 HEADER_ELEMENTS_MODE_2 = internal::MakeCommandHeader(REG_ELEMENTS_MODE_2, 1, false, 0x2);

    if (mode == GL_TRIANGLES){
        command[4] = 1 << 8;
        command[5] = HEADER_ELEMENTS_MODE;
        command[6] = 1 << 8;
        command[7] = HEADER_ELEMENTS_MODE_2;
    }
    else{
        command[4] = 0;
        command[5] = HEADER_ELEMENTS_MODE;
        command[6] = 0;
        command[7] = HEADER_ELEMENTS_MODE_2;
    }

    switch (mode){
    case GL_TRIANGLES:              command[8] = 3 << 8; break;
    case GL_TRIANGLE_STRIP:         command[8] = 1 << 8; break;
    case GL_TRIANGLE_FAN:           command[8] = 2 << 8; break;
    case GL_GEOMETRY_PRIMITIVE_DMP: command[8] = 3 << 8; break;
    }

    u32 commandIndex = 9;

    command[commandIndex++] = internal::MakeCommandHeader(REG_ELEMENTS_MODE_3, 1, false, 0x2);
    command[commandIndex++] = 1;
    command[commandIndex++] = internal::MakeCommandHeader(REG_TRIANGLE_INDEX_RESET, 1, false, 0xF);
    command[commandIndex++] = 0;
    command[commandIndex++] = internal::MakeCommandHeader(REG_DRAW_READY, 1, false, 0xF);
    command[commandIndex++] = 1;
    command[commandIndex++] = internal::MakeCommandHeader(REG_DRAW_KICK, 1, false, 0xF);
    command[commandIndex++] = 1;
    command[commandIndex++] = internal::MakeCommandHeader(REG_DRAW_READY, 1, false, 0xF);
    command[commandIndex++] = 1;
    command[commandIndex++] = internal::MakeCommandHeader(REG_VERTEX_CACHE_CLEAR, 1, false, 0xF);
    command[commandIndex++] = 0;
    command[commandIndex++] = internal::MakeCommandHeader(REG_ELEMENTS_MODE_3, 1, false, 0x8);
    command[commandIndex++] = 0;
    command[commandIndex++] = internal::MakeCommandHeader(REG_ELEMENTS_MODE_3, 1, false, 0x8);
    command[commandIndex++] = 1;
    command[commandIndex++] = internal::MakeCommandHeader(REG_COLOR_DEPTH_CACHE_FLUSH, 1, false, 0xF);
    command[commandIndex++] = 1;
    command[commandIndex++] = internal::MakeCommandHeader(REG_COLOR_DEPTH_CACHE_CLEAR, 1, false, 0xF);

    return commandIndex * sizeof(u32);
}

}

}
}