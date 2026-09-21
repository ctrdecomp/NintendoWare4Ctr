#pragma once

#include <GLES2/gl2.h>
#include <GLES2/gl2extern.h>

#include <nw/math/math_Types.h>
#include <nw/font/font_WideTextWriter.h>
#include <nw/ut/ut_Rect.h>

#include <nw/lyt/lyt_Types.h>

namespace nw{
namespace lyt{

class Layout;

class GraphicsResource
{
public:
    enum Uniform
    {
        UNIFORM_uProjection,
        UNIFORM_uModelView,
        UNIFORM_uTexMtx0,
        UNIFORM_uTexMtx1,
        UNIFORM_uTexMtx2,
        UNIFORM_uColor,
        UNIFORM_uTransform,
        UNIFORM_uFrameSpec,
        UNIFORM_uVertexColor,
        UNIFORM_uVertexTexCoord0,
        UNIFORM_uVertexTexCoord1,
        UNIFORM_uVertexTexCoord2,
        UNIFORM_uRcpTexSize0,
        UNIFORM_dmp_Texture0_samplerType,
        UNIFORM_dmp_Texture1_samplerType,
        UNIFORM_dmp_Texture2_samplerType,
        UNIFORM_dmp_Texture3_samplerType,
        UNIFORM_dmp_TexEnv0_combineRgb,
        UNIFORM_dmp_TexEnv1_combineRgb,
        UNIFORM_dmp_TexEnv2_combineRgb,
        UNIFORM_dmp_TexEnv3_combineRgb,
        UNIFORM_dmp_TexEnv4_combineRgb,
        UNIFORM_dmp_TexEnv5_combineRgb,
        UNIFORM_dmp_TexEnv0_combineAlpha,
        UNIFORM_dmp_TexEnv1_combineAlpha,
        UNIFORM_dmp_TexEnv2_combineAlpha,
        UNIFORM_dmp_TexEnv3_combineAlpha,
        UNIFORM_dmp_TexEnv4_combineAlpha,
        UNIFORM_dmp_TexEnv5_combineAlpha,
        UNIFORM_dmp_TexEnv0_srcRgb,
        UNIFORM_dmp_TexEnv1_srcRgb,
        UNIFORM_dmp_TexEnv2_srcRgb,
        UNIFORM_dmp_TexEnv3_srcRgb,
        UNIFORM_dmp_TexEnv4_srcRgb,
        UNIFORM_dmp_TexEnv5_srcRgb,
        UNIFORM_dmp_TexEnv0_srcAlpha,
        UNIFORM_dmp_TexEnv1_srcAlpha,
        UNIFORM_dmp_TexEnv2_srcAlpha,
        UNIFORM_dmp_TexEnv3_srcAlpha,
        UNIFORM_dmp_TexEnv4_srcAlpha,
        UNIFORM_dmp_TexEnv5_srcAlpha,
        UNIFORM_dmp_TexEnv0_operandRgb,
        UNIFORM_dmp_TexEnv1_operandRgb,
        UNIFORM_dmp_TexEnv2_operandRgb,
        UNIFORM_dmp_TexEnv3_operandRgb,
        UNIFORM_dmp_TexEnv4_operandRgb,
        UNIFORM_dmp_TexEnv5_operandRgb,
        UNIFORM_dmp_TexEnv0_operandAlpha,
        UNIFORM_dmp_TexEnv1_operandAlpha,
        UNIFORM_dmp_TexEnv2_operandAlpha,
        UNIFORM_dmp_TexEnv3_operandAlpha,
        UNIFORM_dmp_TexEnv4_operandAlpha,
        UNIFORM_dmp_TexEnv5_operandAlpha,
        UNIFORM_dmp_TexEnv0_scaleRgb,
        UNIFORM_dmp_TexEnv1_scaleRgb,
        UNIFORM_dmp_TexEnv2_scaleRgb,
        UNIFORM_dmp_TexEnv3_scaleRgb,
        UNIFORM_dmp_TexEnv4_scaleRgb,
        UNIFORM_dmp_TexEnv5_scaleRgb,
        UNIFORM_dmp_TexEnv0_scaleAlpha,
        UNIFORM_dmp_TexEnv1_scaleAlpha,
        UNIFORM_dmp_TexEnv2_scaleAlpha,
        UNIFORM_dmp_TexEnv3_scaleAlpha,
        UNIFORM_dmp_TexEnv4_scaleAlpha,
        UNIFORM_dmp_TexEnv5_scaleAlpha,
        UNIFORM_dmp_TexEnv0_constRgba,
        UNIFORM_dmp_TexEnv1_constRgba,
        UNIFORM_dmp_TexEnv2_constRgba,
        UNIFORM_dmp_TexEnv3_constRgba,
        UNIFORM_dmp_TexEnv4_constRgba,
        UNIFORM_dmp_TexEnv5_constRgba,
        UNIFORM_dmp_TexEnv0_bufferColor,
        UNIFORM_dmp_TexEnv1_bufferInput,
        UNIFORM_dmp_TexEnv2_bufferInput,
        UNIFORM_dmp_TexEnv3_bufferInput,
        UNIFORM_dmp_TexEnv4_bufferInput,
        UNIFORM_dmp_FragOperation_enableAlphaTest,
        UNIFORM_dmp_FragOperation_alphaRefValue,
        UNIFORM_dmp_FragOperation_alphaTestFunc,
        UNIFORM_MAX
    };

    enum VBO
    {
        VBO_ELEMENT,
        VBO_VERTEX_INDEX,
        VBO_MAX
    };

    GraphicsResource();
    ~GraphicsResource();
    void StartSetup();
    const wchar_t* GetResourcePath(int index);
    void SetResource(int index, void* content, u32 fileSize, bool bFree = true);
    bool FinishSetup();
    void Finalize();

    bool Initialized() const {return m_Initialized;}
    const void* GetRectShaderBinary() const {return m_pRectShaderBinary;}
    u32 GetRectShaderBinarySize() const {return m_RectShaderBinarySize;}
    font::WideTextWriter& GetTextWriter() {return m_TextWriter;}
    const font::WideTextWriter& GetTextWriter() const {return m_TextWriter;}
    font::TextWriterResource& GetTextWriterResource() {return m_TextWriterResource;}
    const font::TextWriterResource& GetTextWriterResource() const {return m_TextWriterResource;}
    const math::MTX34&  GetMtxModelView() const {return m_MtxModelView;}
    void SetMtxModelView(const math::MTX34 m)
    {
        m_MtxModelView = m;
        m_MtxModelViewLoaded = false;
    }
    int GetTexCoordSrc(int index) const {return m_TexCoordSrc[index];}
    void SetTexCoordSrc(int index, int src) {m_TexCoordSrc[index] = src;}

    void SetProjectionMtx(const nw::math::MTX44& mtx);

    GLint GetUniformLocation(int uniformName) const {return m_UniformLocation[uniformName];}
    u32 GetNumTexEnv() const {return m_NumTexEnv;}

    void ResetGlProgramState();
    void ResetGlState();

    void SetNumTexEnv(u32 numTexEnv) {m_NumTexEnv = static_cast<u8>(numTexEnv);}
    bool GetTexEnvAuto() const {return m_TexEnvAuto;}
    u8 GetNumTexMap() const {return m_NumTexMap;}
    void SetTexEnvAuto(bool value, u8 numTexMap = 0)
    {
        m_TexEnvAuto = value;
        m_NumTexMap = numTexMap;
    }

    GLuint GetGlProgram() const {return m_GlProgram;}
    GLuint GetGlProgramDebug() const {return m_GlProgramDebug;}
 
    void LoadMtxModelView()
    {
        if (!this->m_MtxModelViewLoaded)
        {
            this->m_MtxModelViewLoaded = true;
            GLint loc = this->GetUniformLocation(this->UNIFORM_uModelView);
            glUniform4fv(loc, 3, &this->m_MtxModelView.matrix[0][0]);
        }
    }

    void UseProgram(GLuint program) { glUseProgram(program); }

public:
    GLuint GetVBO(int index) const {return m_GlVertexBufferObject[index];}
    void SetupProgram()
    {
        if (this->m_FirstDraw)
        {
            this->m_FirstDraw = false;
            this->ActiveVBO();
        }
    }

protected:
    void InitVBO();
    void ActiveVBO();

protected:
    void* m_pRectShaderBinary;
    u32 m_RectShaderBinarySize;
    font::TextWriterResource m_TextWriterResource;
    font::WideTextWriter m_TextWriter;
    GLuint m_GlProgram;
    GLuint m_GlProgramDebug;
    math::MTX34 m_MtxModelView;
    s8 m_TexCoordSrc[TexMapMax];
    u8 m_NumTexEnv;
    u8 m_NumTexMap;
    GLuint m_GlVertexBufferObject[VBO_MAX];
    GLint m_UniformLocation[UNIFORM_MAX];
    bool m_TexEnvAuto;
    bool m_MtxModelViewLoaded;
    bool m_FirstDraw;
    bool m_Initialized;
};

}
}