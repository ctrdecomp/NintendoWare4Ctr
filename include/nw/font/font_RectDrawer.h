#pragma once

#include <nw/ut/ut_CmdCache.h>
#include <nw/font/font_CharWriter.h>
#include <nn/gx/CTR/gx_CommandAccess.h>
#include <nw/font/font_RectDrawerCommand.h>


#define NW_FONT_COMMAND_SET_BLEND_FUNC(eq, src, dst) \
    PICA_CMD_DATA_COLOR_OPERATION( \
        PICA_DATA_FRAGOP_MODE_DMP, \
        PICA_DATA_ENABLE_BLEND), \
    PICA_CMD_HEADER_SINGLE(PICA_REG_COLOR_OPERATION), \
    PICA_CMD_DATA_BLEND_FUNC(eq, src, dst), \
    PICA_CMD_HEADER_SINGLE(PICA_REG_BLEND_FUNC)

#define NW_FONT_COMMAND_SET_BLEND_DEFAULT \
    NW_FONT_COMMAND_SET_BLEND_FUNC( \
        PICA_DATA_BLEND_EQUATION_ADD, \
        PICA_DATA_BLEND_FUNC_SRC_ALPHA, \
        PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_ALPHA )

namespace nw {
namespace font {
namespace internal{
inline u32* SetVertexNumCmd(u32** pCmdPtr,u32 addrOffset,u32 vtxNum){
    NW_FONT_ADD_SINGLE_COMMAND_PTR(*pCmdPtr,PICA_CMD_HEADER_SINGLE( PICA_REG_INDEX_ARRAY_ADDR_OFFSET ),PICA_CMD_DATA_INDEX_ARRAY_ADDR_OFFSET(addrOffset, 1 ));

    NW_FONT_ADD_SINGLE_COMMAND_PTR(*pCmdPtr,PICA_CMD_HEADER_SINGLE(PICA_REG_DRAW_VERTEX_NUM),PICA_CMD_DATA_DRAW_VERTEX_NUM(vtxNum));

    return *pCmdPtr;
}
}

class RectDrawer{
public:
    enum{
        DONT_USE_SETUP_COMMAND = 0x1
    };
    static u32 GetVertexBufferCommandBufferSize(const void* shaderBinary,u32 size);
    static void* GetVertexBufferData();
    static u32 GetVertexBufferSize();
    static u32 GetCommandBufferSize(const void* shaderBinary,u32 size);

    RectDrawer();
    void AddUniformMtx();
    void Initialize(void* vtxBufCmdBuf,const void* shaderBinary,u32 size);
    void Initialize(void* vertexBuffer,void* commandBuffer,const void* shaderBinary,u32 size);
    void SetProjectionMtx(const nn::math::MTX44& mtx);
    void SetViewMtxForText(const nn::math::MTX34& mtx);
    void SetParallax(f32 parallax,f32 dLevel,f32 addDist = 0.0f);
    void BuildTextCommand(CharWriter* pCharWriter);
    void UniformAndDrawText(DispStringBuffer* pDispStringBuffer);
    void UseBeginCommand();
    void UseEndCommand();
    void InitUniformBuffer(u32* __restrict buf,u32 addr);

    void DisableTextures();

    virtual ~RectDrawer();
    virtual void Finalize();
    virtual void DrawBegin(u32 flag = 0);
    virtual void DrawEnd(u32 flag = 0);
protected:
    u32 GetVertexIndexAddressOffset(u32 vtxNum) const;
    const u32* GetDrawCommands() const;
    u32 GetDrawCommandSize() const;

private:
    static void InitializeStatic();
    void InitializeCMD(void* vertexBuffer,void* commandBuffer,const void* shaderBinary,u32 size,bool initVertexBuffer);
protected:
    static u32 SetUniformCommand(u32* buffer,u32 num){
        #if defined(NW_PLATFORM_CTR)
                buffer[2] = buffer[3];
        #else
                (void)buffer;
        #endif
                return sizeof(u32) * (4 + 4 * num);
    }
    enum{
        UNIFORM_MTX_USE_MAX     =  3,
        UNIFORM_DATA_USE_MAX    =  1,

        UNIFORM_TEXT_MTX_MAX    =  3,

        UNIFORM_PROJ_NUM        =  4,

        UNIFORM_PROJ_START      =  0,
        UNIFORM_ULCDDATA_START  = UNIFORM_PROJ_START + UNIFORM_PROJ_NUM,
        UNIFORM_CONST_START,
        UNIFORM_ADDR_START,
        UNIFORM_MTX_START       = 32,
        UNIFORM_DATA_START      = 64,

        UNIFORM_TEXTCOLOR_START = UNIFORM_MTX_START + UNIFORM_TEXT_MTX_MAX,

        UNIFORM_MAX             = 96
    };

    enum{
        UNIFORM_ADDR_NUM = UNIFORM_MTX_START - UNIFORM_ADDR_START,
        UNIFORM_MTX_NUM = UNIFORM_DATA_START - UNIFORM_MTX_START,
        UNIFORM_DATA_NUM = UNIFORM_MAX - UNIFORM_DATA_START,

        UNIFORM_TEXTCOLOR_NUM = (UNIFORM_DATA_START - UNIFORM_TEXTCOLOR_START) & ~((1 << 2) - 1),

        DRAW_VTX_NUM = 6,
        BIND_ATTRIB_LOCATION = 0
    };

    struct VertexBuffer{
        f32 position[UNIFORM_ADDR_NUM][DRAW_VTX_NUM][4];
    };
    
    ut::internal::CmdCache mCmdCacheDrawBegin;
    ut::internal::CmdCache mCmdCacheDrawEnd;
    u8 mUniformAddrIndex;
    u8 mUniformMtxIndex;
    u8 mUniformDataIndex;
    u8 mUniformTextColorIndex;
    u32 mUniformProjBuffer[4 + 4 * (UNIFORM_PROJ_NUM + 1)];
    u32 mUniformAddrBuffer[4 + 4 * UNIFORM_ADDR_NUM];

    union{
        u32 mUniformMtxBuffer[ 4 + 4 * UNIFORM_MTX_NUM  ];
        struct{
            u32 mUniformTextMtxBuffer  [ 4 + 4 * UNIFORM_TEXT_MTX_MAX ];
            u32 mUniformTextColorBuffer[ 4 + 4 * UNIFORM_TEXTCOLOR_NUM ];
        };
    };
    u32 mUniformDataBuffer[ 4 + 4 * UNIFORM_DATA_NUM ];
    struct UniformAddress{
        UniformAddress():   
            addrMtx(0.f),
            addrSizeAndVtx(0.f),
            addrTexCoord(0.f),
            addrColor(0.f)
        {}
        f32 addrMtx;
        f32 addrSizeAndVtx; 
        f32 addrTexCoord;
        f32 addrColor;
    };
    UniformAddress mUniformAddress;
    UniformAddress *const mUniformAddr;
    math::VEC4 *const  mUniformMtx;
    math::VEC4 *const  mUniformData;
    math::VEC4 *const  mUniformTextColor;
    void* mCommandBuffer;
    void* mVertexBufferArray;
    u32 mVertexBufferBaseAddr;
    bool mIsColorDirty;

    static bool sStaticInitialized;
    static VertexBuffer sVertexBuffer;
};

}
}