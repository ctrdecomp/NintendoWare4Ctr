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

class GraphicsResource{
public:
    enum Uniform{
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

    enum VBO{
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

    bool Initialized() const{return mInitialized;}
    const void* GetRectShaderBinary() const{return mpRectShaderBinary;}
    u32 GetRectShaderBinarySize() const{return mRectShaderBinarySize;}
    font::WideTextWriter& GetTextWriter(){return mTextWriter;}
    const font::WideTextWriter& GetTextWriter() const{return mTextWriter;}
    font::TextWriterResource& GetTextWriterResource(){return mTextWriterResource;}
    const font::TextWriterResource& GetTextWriterResource() const{return mTextWriterResource;}
    const math::MTX34&  GetMtxModelView() const{return mMtxModelView;}
    void SetMtxModelView(const math::MTX34 m){
        mMtxModelView = m;
        mMtxModelViewLoaded = false;
    }
    int GetTexCoordSrc(int index) const{return mTexCoordSrc[index];}
    void SetTexCoordSrc(int index, int src){mTexCoordSrc[index] = src;}

    void SetProjectionMtx(const nw::math::MTX44& mtx);

    GLint GetUniformLocation(int uniformName) const{return mUniformLocation[uniformName];}
    u32 GetNumTexEnv() const{return mNumTexEnv;}

    void ResetGlProgramState();
    void ResetGlState();

    void SetNumTexEnv(u32 numTexEnv){mNumTexEnv = static_cast<u8>(numTexEnv);}
    bool GetTexEnvAuto() const{return mTexEnvAuto;}
    u8 GetNumTexMap() const{return mNumTexMap;}
    void SetTexEnvAuto(bool value, u8 numTexMap = 0){
        mTexEnvAuto = value;
        mNumTexMap = numTexMap;
    }

    GLuint GetGlProgram() const{return mGlProgram;}
    GLuint GetGlProgramDebug() const{return mGlProgramDebug;}
 
    void LoadMtxModelView(){
        if (!this->mMtxModelViewLoaded){
            this->mMtxModelViewLoaded = true;
            GLint loc = this->GetUniformLocation(this->UNIFORM_uModelView);
            glUniform4fv(loc, 3, &this->mMtxModelView.matrix[0][0]);
        }
    }

    void UseProgram(GLuint program){ glUseProgram(program); }

public:
    GLuint GetVBO(int index) const{return mGlVertexBufferObject[index];}
    void SetupProgram(){
        if (this->mFirstDraw){
            this->mFirstDraw = false;
            this->ActiveVBO();
        }
    }

protected:
    void InitVBO();
    void ActiveVBO();

protected:
    void* mpRectShaderBinary;
    u32 mRectShaderBinarySize;
    font::TextWriterResource mTextWriterResource;
    font::WideTextWriter mTextWriter;
    GLuint mGlProgram;
    GLuint mGlProgramDebug;
    math::MTX34 mMtxModelView;
    s8 mTexCoordSrc[TexMapMax];
    u8 mNumTexEnv;
    u8 mNumTexMap;
    GLuint mGlVertexBufferObject[VBO_MAX];
    GLint mUniformLocation[UNIFORM_MAX];
    bool mTexEnvAuto;
    bool mMtxModelViewLoaded;
    bool mFirstDraw;
    bool mInitialized;
};

}
}