#pragma once

#include <nw/font/font_RectDrawer.h>

#if 0
#define NW_LYT_DRAWER_ALIGN_32 __attribute__((aligned(32)))
#else
#define NW_LYT_DRAWER_ALIGN_32
#endif

#include <GLES2/gl2.h>
#include <GLES2/gl2extern.h>
#include <nw/lyt/lyt_Types.h>
#include <nw/ut/ut_Color.h>

namespace nw { 
namespace lyt {

class Layout;
class Pane;
class Material;
class DrawInfo;
class Material;
class TexMap;
class TextBox;
class GraphicsResource;

class Drawer : public font::RectDrawer{
    typedef RectDrawer  Base;
public:
    Drawer();
    explicit Drawer(GraphicsResource& graphicsResource);
    void Initialize(GraphicsResource& graphicsResource,void* vertexBuffer = NULL);
protected:
    void Initialize(void* vtxBufCmdBuf,const void* shaderBinary,u32 size){
        Base::Initialize(vtxBufCmdBuf, shaderBinary, size);
    }

    void Initialize(void* vertexBuffer,void* commandBuffer,const void* shaderBinary,u32 size){
        Base::Initialize(vertexBuffer, commandBuffer, shaderBinary, size);
    }

public:
    virtual void        Finalize();
public:
    void DrawBegin(const DrawInfo& drawInfo);
    virtual void DrawBegin(u32 flag = 0);

    void Draw(const Layout* pLayout, DrawInfo& drawInfo);
    void Draw(const Pane* pPane, DrawInfo& drawInfo);

    void DrawEnd( const DrawInfo& drawInfo ){
        NW_UNUSED_VARIABLE(drawInfo);
        u32 flag = 0;
        DrawEnd(flag);
    }
    virtual void DrawEnd(u32 flag = 0);
public:
    void DumpCmdList();
    void SetUpTexEnv(const Material* pMaterial );
    void SetUpTextures(const Material* pMaterial, bool addDisableCommand = true);
    void SetUpTextureCoords(const nw::math::VEC4 texCoords[TexMapMax * 2],const int texCoordNum);
    void SetUpVtxColors(const nw::ut::Color8 vtxColors[VERTEX_MAX],const u8 globalAlpha);
    void SetUpTextBox(const TextBox* pTextBox,const Material* pMaterial,const DrawInfo& drawInfo);
    void SetUpMtx(const nw::math::MTX34& mtx);
    void SetUpQuad(const nw::lyt::Size& size,const nw::math::VEC2& basePt);
    void SetUniformDataEnd();
    void SetCurrentUniformAlpha( const f32 alpha );
    u8 CalcTextureCoords(const Material* pMaterial,const TexCoordQuad* pTexCoordQuad,nw::math::VEC4 texCoords[TexMapMax * 2]) const;

    void FlushBuffer(){
        if (mUniformAddrIndex > 0){
            UniformAndDraw();
        }
    }

protected:
    void UniformAndDraw();
    void SetUpTexEnvType2(const Material* pMaterial);
    void SetUpTexEnvType3(const Material* pMaterial);
    void SetUpGLTexEnvUser(const Material* pMaterial);
    void SetUpBlendMode(const Material* pMaterial);
    void SetUpAlphaTest(const Material* pMaterial);

protected:
    GLuint mPrevTexObj;
    int mActiveTexureNum;
    int mTexCoordNum;
    bool mIsBlendDefault;
    bool mAlphaTestEnable;

protected:
    enum TEX_ENV_TYPE{
        TEX_ENV_TYPE_0_TEX,
        TEX_ENV_TYPE_1_TEX,
        TEX_ENV_TYPE_2_TEX,
        TEX_ENV_TYPE_3_TEX,
        TEX_ENV_TYPE_TEXT,
        TEX_ENV_TYPE_USER,
        TEX_ENV_TYPE_NUM
    };

    TEX_ENV_TYPE mCurrentTexEnvType;

#if ! defined(NW_RELEASE)
    TEX_ENV_TYPE mPreviousTexEnvType;
#endif

    enum{
        TEX_ENV_UNIFORM_NUM = 8,
        TEX_ENV_STAGE_NUM = 3,
        TEX_ENV_UNIFORM_VALUE_NUM = 16
    };
};

}
}