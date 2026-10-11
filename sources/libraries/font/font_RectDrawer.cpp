// Filename: font_RectDrawer.cpp
//
// Project: NintendoWare4Ctr

#include <GLES2/gl2.h>
#include <GLES2/gl2extern.h>
#include <nw/types.h>
#include <nw/math/math_Types.h>
#include <nw/font/font_RectDrawer.h>
#include <nw/font/font_RectDrawerCommand.h>
#include <nw/font/font_DispStringBuffer.h>
#include <nn/gx/CTR/gx_CommandAccess.h>
#include <nn/gx/CTR/gx_PicaAll.h>

extern int* __cb_current_command_buffer;

namespace nw{
namespace font{
namespace{

const u32 DRAW_MODE2_DEFAULT  = 0;
const u32 DRAW_FUNC_DEFAULT   = 0;
const u32 DRAW_MODE0_DEFAULT  = 0;

const u32 DRAW_MODE0_ELEMENT  = 1;
const u32 DRAW_MODE2_ELEMENT  = PICA_DATA_DRAW_TRIANGLES;

const u32 s_DrawInitCommands[] =
{
    NW_FONT_COMMAND_SET_BLEND_DEFAULT,
    0x0,
    (PICA_REG_FRAGOP_ALPHA_TEST | 0x000F0000),
    (PICA_DATA_FOG_FALSE | (0 << 3) | (0 << 16)),
    (PICA_REG_GAS_FOG_MODE | (0xd << 16)),
    0,
    (PICA_REG_FRAG_LIGHT_EN0 | 0x000F0000),
    1,
    (PICA_REG_FRAG_LIGHT_EN1 | 0x000F0000),
    0x0000000f,
    (PICA_REG_COLOR_BUFFER_READ | 0x000F0000),
    0,
    (PICA_REG_TEXTURE0_LOD | 0x000F0000),
    0,
    (PICA_REG_TEXTURE1_LOD | 0x000F0000),
    0,
    (PICA_REG_TEXTURE2_LOD | 0x000F0000),
    1,
    (PICA_REG_TEXTURE0_SHADOW | 0x000F0000),
    (DRAW_MODE0_ELEMENT << 8),
    (PICA_REG_DRAW_MODE0 | (0x2 << 16)),
    (DRAW_FUNC_DEFAULT | (DRAW_MODE0_ELEMENT << 8)),
    (PICA_REG_DRAW_MODE1 | (0x3 << 16)),
    (DRAW_MODE2_ELEMENT << 8),
    (PICA_REG_VS_OUT_REG_NUM3 | (0x2 << 16)),
    1,
    (PICA_REG_COLOR_DEPTH_BUFFER_CLEAR1 | 0x000F0000),
    1,
    (PICA_REG_COLOR_DEPTH_BUFFER_CLEAR0 | 0x000F0000),
};

void AddVertexAttributeArrayCommand(ut::internal::CmdCache& cmdCache,u32 vboBaseAddr, uptr phyArrayBuffer,u32 vtxAttrNum)
{
    const u32 fixedVtxAttrMask = 0;
    const u32 loadAryBytes = 16;
    const u32 loadAryElementNum = 1;

    const u32 command200[] ={
        (((u32)vboBaseAddr >> 4) << 1),
        (PICA_REG_VTX_ATTR_ARRAYS_BASE_ADDR | (0xF << 16) | (((0x202 - 0x200 + 1) + (0x205 - 0x203 + 1) * 12 - 1) << 20) | 0x80000000),

        (PICA_DATA_SIZE_4_FLOAT | (PICA_DATA_SIZE_1_BYTE << 4) | (PICA_DATA_SIZE_1_BYTE << 8) | (PICA_DATA_SIZE_1_BYTE << 12) |
            (PICA_DATA_SIZE_1_BYTE << 16) | (PICA_DATA_SIZE_1_BYTE << 20) | (PICA_DATA_SIZE_1_BYTE << 24) | (PICA_DATA_SIZE_1_BYTE << 28)),

        PICA_DATA_SIZE_1_BYTE <<  0 | PICA_DATA_SIZE_1_BYTE <<  4
            | PICA_DATA_SIZE_1_BYTE <<  8 | PICA_DATA_SIZE_1_BYTE << 12
            | fixedVtxAttrMask << 16
            | (vtxAttrNum - 1) << 28,

            (phyArrayBuffer - vboBaseAddr) & 0x0FFFFFFF,
            0,
            ((loadAryBytes & 0xFF) << 16) | ((loadAryElementNum & 0x0F) << 28),
    };
    cmdCache.Add(command200, sizeof(command200));

    static const u32 command206[] ={
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,

            0,
        (PICA_REG_VS_ATTR_IN_REG_MAP0 | 0x000F0000),

            0,
        (PICA_REG_VS_ATTR_IN_REG_MAP1 | 0x000F0000),
    };
    cmdCache.Add(command206, sizeof(command206));
}

struct ShaderBinaryFileHeader
{
    u8      signature[4];
    u32     exeCount;
    u32     exeOffsetTop;
};

struct ShaderPackageHeader
{
    u8      signature[4];
    u8      version[2];
    u16     reserved0;
    u32     instOffset;
    u32     instCount;
    u32     swizzleOffset;
    u32     swizzleCount;
    u32     lineOffset;
    u32     lineCount;
    u32     stringOffset;
    u32     stringSize;
};

struct SwizzleInfo
{
    u32     value;
    u16     usedInfo;
    u16     reserve;
};

struct ExeImageHeader
{
    u8      signature[4];

    u8      version[2];
    u8      shaderType;
    u8      mergeOutputMapsDebug;

    u32     mainAddr;
    u32     endAddr;

    u16     inputMask;
    u16     outputMask;

    u8      geometryDataMode;
    u8      startIndex;
    u8      subdPatchSize;
    u8      constVertexNumber;

    u32     setupOffset;
    u32     setupCount;
    u32     labelOffset;
    u32     labelCount;
    u32     outmapOffset;
    u32     outmapCount;
    u32     bsymOffset;
    u32     bsymCount;
    u32     stringOffset;
    u32     stringSize;
};

struct SetupInfo
{
    u16     type;
    u16     index;
    u32     value[4];
};

struct OutmapInfo
{
    u16     type;
    u16     index;
    u16     mask;
    u16     reserve;
};

const u32 DRAWBEGIN_STATICCOMMAND_BYTES  = 656 + sizeof(s_DrawInitCommands) + sizeof(u32) * 2 * 1;

const u32 BURST_MAX = 0x80;

enum
{
    SHADEROUTMAPTYPE_POSITION,
    SHADEROUTMAPTYPE_QUATERNION,
    SHADEROUTMAPTYPE_COLOR,
    SHADEROUTMAPTYPE_TEXCOORD0,
    SHADEROUTMAPTYPE_TEXCOORD0W,
    SHADEROUTMAPTYPE_TEXCOORD1,
    SHADEROUTMAPTYPE_TEXCOORD2,
    SHADEROUTMAPTYPE_VIEW,

    SHADEROUTMAPTYPE_MAX
};

u32 GetColorU32Raw(ut::Color8 color)
{
    return *reinterpret_cast<const u32*>(&color);
}

void EnsureShaderBinaryFileHeader(const void* shaderBinary)
{
    const ShaderBinaryFileHeader& header = *static_cast<const ShaderBinaryFileHeader*>(shaderBinary);
    NN_ASSERT_(header.signature[0] == 'D' && header.signature[1] == 'V' && header.signature[2] == 'L' && header.signature[3] == 'B' );
}

void NormalizeF32Colors(math::VEC4* __restrict dst,const ut::Color8* __restrict src)
{
    register const f32 d = 1.f / 255.f;
    register f32 colors[4];

    for (int i = 0; i < internal::TEXTCOLOR_MAX; ++i)
    {
        colors[0] = src[i].r * d;
        colors[1] = src[i].g * d;
        colors[2] = src[i].b * d;
        colors[3] = src[i].a * d;

        dst[i].x = colors[0];
        dst[i].y = colors[1];
        dst[i].z = colors[2];
        dst[i].w = colors[3];
    }
}

void SetVertexColor(math::VEC4* __restrict dst,const ut::Color8* __restrict src,int mode)
{
    math::VEC4 wkCols[internal::TEXTCOLOR_MAX];
    NormalizeF32Colors(wkCols, src);
    dst[VERTEX_LT] = wkCols[internal::TEXTCOLOR_START];
    dst[VERTEX_RT] = wkCols[
        mode != CharWriter::GRADMODE_H ? internal::TEXTCOLOR_START: internal::TEXTCOLOR_END];
    dst[VERTEX_LB] = wkCols[
        mode != CharWriter::GRADMODE_V ? internal::TEXTCOLOR_START: internal::TEXTCOLOR_END];
    dst[VERTEX_RB] = wkCols[
        mode == CharWriter::GRADMODE_NONE ? internal::TEXTCOLOR_START: internal::TEXTCOLOR_END];
}

const ShaderBinaryFileHeader& GetShaderBinaryFileHeader(const void* shaderBinary)
{
    return *static_cast<const ShaderBinaryFileHeader*>(shaderBinary);
}

const ShaderPackageHeader& GetShaderPackageHeader(const void* shaderBinary)
{
    const ShaderBinaryFileHeader& fileHead = GetShaderBinaryFileHeader(shaderBinary);
    return *static_cast<const ShaderPackageHeader*>(ut::AddOffsetToPtr(shaderBinary,sizeof(fileHead) + (fileHead.exeCount - 1) * sizeof(u32)));
}

void EnsureShaderPackageHeader(const ShaderPackageHeader& header)
{
    (void)header;
}

const ExeImageHeader* GetExeImageHeader(const void* shaderBinary)
{
    const ShaderBinaryFileHeader& fileHead = GetShaderBinaryFileHeader(shaderBinary);
    return static_cast<const ExeImageHeader*>(ut::AddOffsetToPtr(shaderBinary,fileHead.exeOffsetTop));
}

void EnsureExeImageHeader(const ExeImageHeader& header)
{
    (void)header;
}

u32 CalcInstCommandCount(u32 count)
{
    u32 size = (count / BURST_MAX) * math::RoundUp(2 + 1 + BURST_MAX, 2);
    if (0 != count % BURST_MAX)
    {
        size += math::RoundUp(2 + 1 + (count % BURST_MAX), 2);
    }
    return size;
}

u32 CalcDrawBeginCommandBytes(const ShaderPackageHeader& header)
{
    const u32 instBytes = sizeof(u32) * (CalcInstCommandCount(header.instCount) + CalcInstCommandCount(header.swizzleCount));
    return DRAWBEGIN_STATICCOMMAND_BYTES + instBytes;
}

const u32 s_DrawFinalizeCommands[] ={
    (0x0 & 0xf),
    (PICA_REG_VS_FIXED_ATTR | (0xF << 16) | ((4 - 1) << 20) | 0x80000000),
    (u32)(0x3f000000),
    (u32)(0x00000000),
    (u32)(0x00000000),
    0x00000000,

    (DRAW_MODE0_DEFAULT << 8),
    (PICA_REG_DRAW_MODE0 | (0x2 << 16)),
    (0 | (DRAW_MODE0_DEFAULT << 8)),
    (PICA_REG_DRAW_MODE1 | (0x2 << 16)),
    (DRAW_MODE2_DEFAULT << 8),
    (PICA_REG_VS_OUT_REG_NUM3 | (0x2 << 16)),
    1,
    (PICA_REG_COLOR_DEPTH_BUFFER_CLEAR1 | 0x000F0000),
    1,
    (PICA_REG_COLOR_DEPTH_BUFFER_CLEAR0 | 0x000F0000),
};

const u32 DRAWEND_STATICCOMMAND_BYTES = 176 + sizeof(s_DrawFinalizeCommands);

}

bool RectDrawer::s_StaticInitialized = false;
RectDrawer::VertexBuffer RectDrawer::s_VertexBuffer;

void* RectDrawer::GetVertexBufferData()
{
    if (!s_StaticInitialized)
    {
        InitializeStatic();

        s_StaticInitialized = true;
    }

    return &s_VertexBuffer;
}

u32 RectDrawer::GetCommandBufferSize(const void* shaderBinary,u32 /* size */)
{
    const ShaderPackageHeader& header = GetShaderPackageHeader(shaderBinary);
    return  CalcDrawBeginCommandBytes(header) + DRAWEND_STATICCOMMAND_BYTES;
}

u32 RectDrawer::GetVertexBufferCommandBufferSize(const void* shaderBinary,u32 size)
{
    return  GetVertexBufferSize() + GetCommandBufferSize(shaderBinary, size);
}

RectDrawer::RectDrawer():   
    m_UniformAddrIndex(0),
    m_UniformMtxIndex(0),
    m_UniformDataIndex(0),
    m_UniformTextColorIndex(0),
    m_UniformAddr(reinterpret_cast<UniformAddress*>(&m_UniformAddrBuffer[3])),
    m_UniformMtx(reinterpret_cast<math::VEC4*>(&m_UniformMtxBuffer[3])),
    m_UniformData(reinterpret_cast<math::VEC4*>(&m_UniformDataBuffer[3])),
    m_UniformTextColor(reinterpret_cast<math::VEC4*>(&m_UniformTextColorBuffer[3])),
    m_CommandBuffer(NULL),
    m_VertexBufferArray(NULL),
    m_VertexBufferBaseAddr(0),
    m_IsColorDirty(false) {}

RectDrawer::~RectDrawer()
{
    this->Finalize();
}

void RectDrawer::Initialize(void* vtxBufCmdBuf,const void* shaderBinary,u32 size)
{
    const u32 cmdBufSize = GetCommandBufferSize(shaderBinary, size);

    void *const vertexBuffer = ut::AddOffsetToPtr(vtxBufCmdBuf, cmdBufSize);

    this->InitializeCMD(vertexBuffer, vtxBufCmdBuf, shaderBinary, size, true);
}

void RectDrawer::Initialize(void* vertexBuffer,void* commandBuffer,const void* shaderBinary,u32 size)
{
    this->InitializeCMD(vertexBuffer, commandBuffer, shaderBinary, size, false);
}

void RectDrawer::Finalize()
{
    m_CommandBuffer = NULL;
    m_VertexBufferArray = NULL;
}

void RectDrawer::DrawBegin(u32 flag)
{
    if (0 == (flag & DONT_USE_SETUP_COMMAND))
    {
        this->UseBeginCommand();
    }
}

void RectDrawer::UseBeginCommand()
{
    this->m_CmdCacheDrawBegin.Use();
}

void RectDrawer::DrawEnd(u32 flag)
{
    if (0 == (flag & DONT_USE_SETUP_COMMAND))
    {
        this->UseEndCommand();
    }
}

void RectDrawer::SetProjectionMtx(const nn::math::MTX44& mtx)
{
    nn::math::MTX44 *const pDst = reinterpret_cast<nn::math::MTX44*>(&this->m_UniformProjBuffer[3]);
    nn::math::MTX44Copy(pDst, &mtx);

    const u32 size = SetUniformCommand(this->m_UniformProjBuffer, UNIFORM_PROJ_NUM + 1);
    std::memcpy(__cb_current_command_buffer, this->m_UniformProjBuffer, size);
    __cb_current_command_buffer += size >> 2;
}

void RectDrawer::SetViewMtxForText(const nn::math::MTX34& mtx)
{
    nn::math::MTX34 *const pDst = reinterpret_cast<nn::math::MTX34*>(&this->m_UniformMtxBuffer[3]);
    nn::math::MTX34Copy(pDst, &mtx);

    m_UniformMtxIndex = sizeof(mtx) / sizeof(nn::math::VEC4);
    const u32 size = SetUniformCommand(this->m_UniformMtxBuffer, m_UniformMtxIndex);
    std::memcpy(__cb_current_command_buffer, this->m_UniformMtxBuffer, size);
    __cb_current_command_buffer += size >> 2;
    m_UniformMtxIndex = 0;
}

void RectDrawer::UseEndCommand()
{
    this->m_CmdCacheDrawEnd.Use();
}

void RectDrawer::SetParallax(f32 parallax,f32 dLevel,f32 addDist)
{

    nn::math::VEC4 *const pParallaxData = reinterpret_cast<nn::math::VEC4*>(&this->m_UniformProjBuffer[3 + 4 * 4]);
    pParallaxData->Set(parallax, dLevel, addDist, 0.0f);
}

void RectDrawer::InitializeStatic()
{
    static const int vtx[DRAW_VTX_NUM] ={
        VERTEX_RT, VERTEX_LT, VERTEX_RB,
        VERTEX_RB, VERTEX_LT, VERTEX_LB,
    };

    for (int addr = 0; addr < UNIFORM_ADDR_NUM; ++addr)
    {
        for (int vi = 0; vi < DRAW_VTX_NUM; ++vi)
        {
            s_VertexBuffer.position[addr][vi][0] = addr;
            s_VertexBuffer.position[addr][vi][1] = vtx[vi];
            s_VertexBuffer.position[addr][vi][2] = vtx[vi] % 2 ?  1.f : 0.f;
            s_VertexBuffer.position[addr][vi][3] = vtx[vi] / 2 ? -1.f : 0.f;
        }
    }
}

void RectDrawer::AddUniformMtx()
{
    const u32 size = this->SetUniformCommand(this->m_UniformMtxBuffer, this->m_UniformMtxIndex);
    std::memcpy(__cb_current_command_buffer, this->m_UniformMtxBuffer, size);
    __cb_current_command_buffer += size >> 2;

    m_UniformMtxIndex = 0;
}

void RectDrawer::InitializeCMD(void* vertexBuffer,void* commandBuffer,const void* shaderBinary,u32 /* size */,bool initVertexBuffer)
{
    EnsureShaderBinaryFileHeader(shaderBinary);

    const ShaderBinaryFileHeader& fileHead = GetShaderBinaryFileHeader(shaderBinary);

    NN_ASSERT_(fileHead.exeCount == 1);

    const ShaderPackageHeader& pkgHead = GetShaderPackageHeader(shaderBinary);

    EnsureShaderPackageHeader(pkgHead);

    const ExeImageHeader& exeHead = GetExeImageHeader(shaderBinary)[0];

    EnsureExeImageHeader(exeHead);

    NN_ASSERT_(exeHead.shaderType == 0);

    m_VertexBufferArray = vertexBuffer;
    if (initVertexBuffer)
    {
        std::memcpy(vertexBuffer, GetVertexBufferData(), GetVertexBufferSize());
        nngxUpdateBuffer(vertexBuffer, GetVertexBufferSize());
    }

    m_CommandBuffer = commandBuffer;
    const u32 DrawBeginCommandBytes = CalcDrawBeginCommandBytes(pkgHead);
    this->m_CmdCacheDrawBegin.Init(this->m_CommandBuffer, DrawBeginCommandBytes);
    this->m_CmdCacheDrawEnd.Init(ut::AddOffsetToPtr(this->m_CommandBuffer, DrawBeginCommandBytes),DRAWEND_STATICCOMMAND_BYTES);

    const u32 vtxAttrNum = 1;

    const bool useGeometryShader = false;

    const bool useGeometryShaderSubdivision = false;

    {
        static const u32 commands[] ={
            0x0,
            (PICA_REG_VS_OUT_REG_NUM2 | (0x9 << 20)),
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            0x0, 0x0,
            0x0,
            (PICA_REG_VERTEX_ATTR_ARRAYS_BASE_ADDR | (0x1D << 20)),
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            0x0, 0x0,
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            0x0, 0x0,
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            0x0, 0x0,

            ((useGeometryShader ? 2 : 0) | (0 << 8) | (0 << 9) | (false ? 1 << 31 : 0)),

            (PICA_REG_DRAW_MODE0 | (0x1 << 16)),

            0x0,
            (PICA_REG_VERTEX_ATTR_ARRAYS_BASE_ADDR | (0x1D << 20)),
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            0x0, 0x0,
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            0x0, 0x0,
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            0x0, 0x0,

            (useGeometryShader ? 1 : 0),
            (PICA_REG_VS_COM_MODE | (0x1 << 16)),
        };
       this->m_CmdCacheDrawBegin.Add(commands, sizeof(commands));
    }

    {
        const u32* progCodes = static_cast<const u32*>(ut::AddOffsetToPtr(&pkgHead, pkgHead.instOffset));

        for (u32 totalCnt = 0; totalCnt < pkgHead.instCount;)
        {
            const u32 writeCnt = math::Min(BURST_MAX, pkgHead.instCount - totalCnt);
            const u32 commands[] ={
                totalCnt,
                (PICA_REG_VS_PROG_ADDR | 0x000F0000),

                progCodes[totalCnt + 0],
                (PICA_REG_VS_PROG_DATA0 | (0xF << 16) | ((writeCnt - 1) << 20)),
            };
            this->m_CmdCacheDrawBegin.Add(commands, sizeof(commands));
            this->m_CmdCacheDrawBegin.Add(&progCodes[totalCnt + 1], sizeof(u32) * (writeCnt - 1));
            this->m_CmdCacheDrawBegin.RoundUp(8);
            totalCnt += writeCnt;
        }

        {
            static const u32 commands[] ={
                1,
                (PICA_REG_VS_PROG_RENEWAL_END | 0x000F0000),
            };
            this->m_CmdCacheDrawBegin.Add(commands, sizeof(commands));
        }
    }

    {
        const SwizzleInfo *const swDatas = static_cast<const SwizzleInfo*>(ut::AddOffsetToPtr(&pkgHead, pkgHead.swizzleOffset));

        for (u32 totalCnt = 0; totalCnt < pkgHead.swizzleCount; )
        {
            const u32 writeCnt = math::Min(BURST_MAX, pkgHead.swizzleCount - totalCnt);
            const u32 commands[] ={
                totalCnt,
                (PICA_REG_VS_PROG_SWIZZLE_ADDR | 0x000F0000),

                swDatas[totalCnt + 0].value,
                (PICA_REG_VS_PROG_SWIZZLE_DATA0 | (0xF << 16) | ((writeCnt - 1) << 20)),
            };
            this->m_CmdCacheDrawBegin.Add(commands, sizeof(commands));

            for (int i = 1; i < writeCnt; ++i)
            {
                this->m_CmdCacheDrawBegin.Add(&swDatas[totalCnt + i].value, sizeof(u32));
            }

            this->m_CmdCacheDrawBegin.RoundUp(8);
            totalCnt += writeCnt;
        }
    }

    {
        const SetupInfo* setupInfos = static_cast<const SetupInfo*>(ut::AddOffsetToPtr(&exeHead,exeHead.setupOffset));

        for (int i = 0; i < exeHead.setupCount; ++i)
        {
            const SetupInfo& setupInfo = setupInfos[i];

            NN_ASSERT_(setupInfo.type == 2);

            const int dataNum = 3;
            const int writeCount = 1 + dataNum;
            const u32 commands[] ={
                ((setupInfo.index & 0xff) | (PICA_DATA_VS_F24 ? 0x80000000 : 0)),
                (PICA_REG_VS_FLOAT_ADDR | (0xF << 16) | ((writeCount - 1) << 20) | 0x80000000),

                setupInfo.value[3] <<  8 | ((setupInfo.value[2] >> 16) & 0x0000FF),
                setupInfo.value[2] << 16 | ((setupInfo.value[1] >>  8) & 0x00FFFF),
                setupInfo.value[1] << 24 | ((setupInfo.value[0] >>  0) & 0xFFFFFF),
                0,
            };
            m_CmdCacheDrawBegin.Add(commands, sizeof(commands));
        }
    }

    const uptr phyArrayBuffer = nngxGetPhysicalAddr(reinterpret_cast<uptr>(m_VertexBufferArray));

    const u32 vboBaseAddr = math::RoundDown(phyArrayBuffer, 16);

    AddVertexAttributeArrayCommand(this->m_CmdCacheDrawBegin, vboBaseAddr, phyArrayBuffer, vtxAttrNum);

    {
        const u32 commands[] ={
            ((false ? 2 : 0) | (0 << 8) | (0 << 9) | (useGeometryShaderSubdivision ? 1 << 31 : 0)),
            (PICA_REG_DRAW_MODE0 | (0x8 << 16)),

            (((vtxAttrNum - 1) & 0xf) | 0xa0000000),
            (PICA_REG_VS_ATTR_NUM0 | (0xb << 16)),

            (exeHead.mainAddr | 0x7fff0000),
            (PICA_REG_VS_START_ADDR | 0x000F0000),
        };
        this->m_CmdCacheDrawBegin.Add(commands, sizeof(commands));
    }

    {
        const u32 OutRegNum = 7;
        const int CompNum = 4;
        static const u8 outAttrs[SHADEROUTMAPTYPE_MAX] ={
            PICA_DATA_VS_OUT_ATTR_X,
            PICA_DATA_VS_OUT_ATTR_QUART_X,
            PICA_DATA_VS_OUT_ATTR_R,
            PICA_DATA_VS_OUT_ATTR_TEX0_U,
            PICA_DATA_VS_OUT_ATTR_TEX0_W,
            PICA_DATA_VS_OUT_ATTR_TEX1_U,
            PICA_DATA_VS_OUT_ATTR_TEX2_U,
            PICA_DATA_VS_OUT_ATTR_VIEW_X
        };
        u8 outmaps[OutRegNum][CompNum];
        bool useRegs[OutRegNum];
        u8 useOutRegTypes[SHADEROUTMAPTYPE_MAX];

        for (u32 i = 0; i < SHADEROUTMAPTYPE_MAX; ++i)
        {
            useOutRegTypes[i] = 0;
        }

        for (u32 j = 0; j < OutRegNum; ++j)
        {
            useRegs[j] = false;

            for (int i = 0; i < CompNum; ++i)
            {
                outmaps[j][i] = PICA_DATA_VS_OUT_ATTR_INVALID;
            }
        }

        const OutmapInfo *const outmapInfos = static_cast<const OutmapInfo*>(ut::AddOffsetToPtr(&exeHead, exeHead.outmapOffset));
        for (int j = 0; j < math::Min(exeHead.outmapCount, OutRegNum); ++j)
        {
            if (outmapInfos[j].type == 9)
            {
                continue;
            }

            useRegs[outmapInfos[j].index] = true;
            u8* outmap = outmaps[outmapInfos[j].index];
            useOutRegTypes[outmapInfos[j].type] = static_cast<u8>(outmapInfos[j].mask);
            u8 ci = outAttrs[outmapInfos[j].type];
            for (int i = 0; i < CompNum; ++i)
            {
                if (0 != (outmapInfos[j].mask & (1 << i)))
                {
                    outmap[i] = ci;
                    ++ci;
                }
            }
        }

        int outRegUseNum = 0;

        for (int j = 0; j < OutRegNum; ++j)
        {
            if (useRegs[j])
            {
                ++outRegUseNum;
            }
        }

        {
            const u32 commands[] ={
                (exeHead.outputMask & 0xffff),
                (PICA_REG_VS_OUT_REG_MASK | 0x000F0000),

                outRegUseNum - 1,
                (PICA_REG_VS_OUT_REG_NUM2 | 0x000F0000),

                0,
                (PICA_REG_GS_MISC_REG0 | 0x000F0000),

                ((vtxAttrNum - 1) & 0xf),
                (PICA_REG_VS_ATTR_NUM1 | 0x000F0000),

                outRegUseNum - 1,
                (PICA_REG_VS_OUT_REG_NUM1 | 0x000F0000),
                ((outRegUseNum - 1) | (0 << 8)),
                (PICA_REG_VS_OUT_REG_NUM3 | (0x1 << 16)),
                outRegUseNum,
                (PICA_REG_VS_OUT_REG_NUM0 | 0x000F0000),
            };
            this->m_CmdCacheDrawBegin.Add(commands, sizeof(commands));
        }

        {
            const u32 commands[] ={
                *reinterpret_cast<u32*>(outmaps[0]),
                (PICA_REG_VS_OUT_ATTR0 | (0xF << 16) | ((OutRegNum - 1) << 20) | 0x80000000),
            };
            this->m_CmdCacheDrawBegin.Add(commands, sizeof(commands));
            this->m_CmdCacheDrawBegin.Add(reinterpret_cast<u32*>(outmaps[1]), sizeof(u32) * (OutRegNum - 1));
        }

        {
            bool usePosZ = false;
            {
                int outCompNum = 0;
                for (int i = 0; i < 4; ++i)
                {
                    if (useOutRegTypes[SHADEROUTMAPTYPE_POSITION] & (1 << i))
                    {
                        ++outCompNum;
                    }
                }

                usePosZ = outCompNum >= 3;
            }

            const bool useViewQuaternion = 0 != useOutRegTypes[SHADEROUTMAPTYPE_VIEW] ||  0 != useOutRegTypes[SHADEROUTMAPTYPE_QUATERNION];
            const u32 outAttrClk =
                ((usePosZ ? 1 : 0) |
                    ((0 != useOutRegTypes[SHADEROUTMAPTYPE_COLOR] ? 1 : 0) << 1) |
                    ((0 != useOutRegTypes[SHADEROUTMAPTYPE_TEXCOORD0] ? 1 : 0) << 8) |
                    ((0 != useOutRegTypes[SHADEROUTMAPTYPE_TEXCOORD1] ? 1 : 0) << 9) |
                    ((0 != useOutRegTypes[SHADEROUTMAPTYPE_TEXCOORD2] ? 1 : 0) << 10) |
                    ((0 != useOutRegTypes[SHADEROUTMAPTYPE_TEXCOORD0W] ? 1 : 0) << 16) |
                    ((useViewQuaternion ? 1 : 0) << 24));
            const u32 TexEnableBits = (1 << 8) | (1 << 9) | (1 << 10) | (1 << 16);

            const bool useTexCoords = 0 != (outAttrClk & TexEnableBits);
            const u32 outAttrMode = useTexCoords ? 1 : 0;
            const u32 commands[] ={

                outAttrMode,
                (PICA_REG_VS_OUT_ATTR_MODE | 0x000F0000),
                outAttrClk,
                (PICA_REG_VS_OUT_ATTR_CLK | 0x000F0000),
            };
            this->m_CmdCacheDrawBegin.Add(commands, sizeof(commands));
        }
    }

    this->m_CmdCacheDrawBegin.Add(s_DrawInitCommands, sizeof(s_DrawInitCommands));

    this->m_VertexBufferBaseAddr = vboBaseAddr;

    {
        void *const eleArrayBuf = ut::AddOffsetToPtr(this->m_VertexBufferArray, sizeof(s_VertexBuffer.position));
        const uptr phyEleArrayBuf = nngxGetPhysicalAddr(reinterpret_cast<uptr>(eleArrayBuf));
        const u32 indexOffset = phyEleArrayBuf - vboBaseAddr;
        const u32 commands[] ={
            (indexOffset | 0x80000000),
            (PICA_REG_INDEX_ARRAY_ADDR_OFFSET | 0x000F0000),
        };
        this->m_CmdCacheDrawBegin.Add(commands, sizeof(commands));
    }

    AddVertexAttributeArrayCommand(this->m_CmdCacheDrawEnd, vboBaseAddr, phyArrayBuffer, vtxAttrNum);

    this->m_CmdCacheDrawEnd.Add(s_DrawFinalizeCommands, sizeof(s_DrawFinalizeCommands));

    {
        InitUniformBuffer(this->m_UniformProjBuffer, UNIFORM_PROJ_START);

        SetParallax(0.0f, 0.0f, 0.0f);

        InitUniformBuffer(this->m_UniformAddrBuffer, UNIFORM_ADDR_START);
        InitUniformBuffer(this->m_UniformMtxBuffer, UNIFORM_MTX_START);
        InitUniformBuffer(this->m_UniformDataBuffer, UNIFORM_DATA_START);
    }
}

}
}

