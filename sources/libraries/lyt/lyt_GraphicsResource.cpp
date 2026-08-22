// Filename: lyt_GraphicsResource.cpp
//
// Project: NintendoWare4Ctr

#include <nw/lyt/lyt_GraphicsResource.h>
#include <nw/lyt/lyt_Common.h>
#include <nw/lyt/lyt_Layout.h>

#include    <cstdlib>

#define ARRAY_LENGTH(a)   (sizeof(a) / sizeof((a)[0]))

namespace nw{
namespace lyt{
namespace{

enum ResouceFileID{
    RESOURCEFILEID_RECTDRAWERSHADER,
    RESOURCEFILEID_FONTSHADER,
    RESOURCEFILEID_PANESHADER,
    RESOURCEFILEID_CONSTCOLORSHADER,
    RESOURCEFILEID_MAX
};

const wchar_t* sResourceFiles[] ={
    L"/shaders/nwfont_RectDrawerShader.shbin",
    L"/shaders/nwfont_TextWriterShader.shbin",
    L"/shaders/nwlyt_PaneShader.shbin",
    L"/shaders/nwlyt_ConstColorShader.shbin",
};

#define FEATURE_FINAL(x) NULL

const char* sUniformNames[] ={
    "uProjection",
    "uModelView",
    "uTexMtx0",
    "uTexMtx1",
    "uTexMtx2",
    "uColor",
    "uTransform",
    "uFrameSpec",
    "uVertexColor",
    "uVertexTexCoord0",
    "uVertexTexCoord1",
    "uVertexTexCoord2",
    "uRcpTexSize0",
    "dmp_Texture[0].samplerType",
    "dmp_Texture[1].samplerType",
    "dmp_Texture[2].samplerType",
    "dmp_Texture[3].samplerType",
    "dmp_TexEnv[0].combineRgb",
    "dmp_TexEnv[1].combineRgb",
    "dmp_TexEnv[2].combineRgb",
    FEATURE_FINAL("dmp_TexEnv[3].combineRgb"),
    FEATURE_FINAL("dmp_TexEnv[4].combineRgb"),
    FEATURE_FINAL("dmp_TexEnv[5].combineRgb"),
    "dmp_TexEnv[0].combineAlpha",
    "dmp_TexEnv[1].combineAlpha",
    "dmp_TexEnv[2].combineAlpha",
    FEATURE_FINAL("dmp_TexEnv[3].combineAlpha"),
    FEATURE_FINAL("dmp_TexEnv[4].combineAlpha"),
    FEATURE_FINAL("dmp_TexEnv[5].combineAlpha"),
    "dmp_TexEnv[0].srcRgb",
    "dmp_TexEnv[1].srcRgb",
    "dmp_TexEnv[2].srcRgb",
    FEATURE_FINAL("dmp_TexEnv[3].srcRgb"),
    FEATURE_FINAL("dmp_TexEnv[4].srcRgb"),
    FEATURE_FINAL("dmp_TexEnv[5].srcRgb"),
    "dmp_TexEnv[0].srcAlpha",
    "dmp_TexEnv[1].srcAlpha",
    "dmp_TexEnv[2].srcAlpha",
    FEATURE_FINAL("dmp_TexEnv[3].srcAlpha"),
    FEATURE_FINAL("dmp_TexEnv[4].srcAlpha"),
    FEATURE_FINAL("dmp_TexEnv[5].srcAlpha"),
    "dmp_TexEnv[0].operandRgb",
    "dmp_TexEnv[1].operandRgb",
    "dmp_TexEnv[2].operandRgb",
    FEATURE_FINAL("dmp_TexEnv[3].operandRgb"),
    FEATURE_FINAL("dmp_TexEnv[4].operandRgb"),
    FEATURE_FINAL("dmp_TexEnv[5].operandRgb"),
    "dmp_TexEnv[0].operandAlpha",
    "dmp_TexEnv[1].operandAlpha",
    "dmp_TexEnv[2].operandAlpha",
    FEATURE_FINAL("dmp_TexEnv[3].operandAlpha"),
    FEATURE_FINAL("dmp_TexEnv[4].operandAlpha"),
    FEATURE_FINAL("dmp_TexEnv[5].operandAlpha"),
    "dmp_TexEnv[0].scaleRgb",
    "dmp_TexEnv[1].scaleRgb",
    "dmp_TexEnv[2].scaleRgb",
    FEATURE_FINAL("dmp_TexEnv[3].scaleRgb"),
    FEATURE_FINAL("dmp_TexEnv[4].scaleRgb"),
    FEATURE_FINAL("dmp_TexEnv[5].scaleRgb"),
    "dmp_TexEnv[0].scaleAlpha",
    "dmp_TexEnv[1].scaleAlpha",
    "dmp_TexEnv[2].scaleAlpha",
    FEATURE_FINAL("dmp_TexEnv[3].scaleAlpha"),
    FEATURE_FINAL("dmp_TexEnv[4].scaleAlpha"),
    FEATURE_FINAL("dmp_TexEnv[5].scaleAlpha"),
    "dmp_TexEnv[0].constRgba",
    "dmp_TexEnv[1].constRgba",
    "dmp_TexEnv[2].constRgba",
    FEATURE_FINAL("dmp_TexEnv[3].constRgba"),
    FEATURE_FINAL("dmp_TexEnv[4].constRgba"),
    FEATURE_FINAL("dmp_TexEnv[5].constRgba"),
    FEATURE_FINAL("dmp_TexEnv[0].bufferColor"),
    FEATURE_FINAL("dmp_TexEnv[1].bufferInput"),
    FEATURE_FINAL("dmp_TexEnv[2].bufferInput"),
    FEATURE_FINAL("dmp_TexEnv[3].bufferInput"),
    FEATURE_FINAL("dmp_TexEnv[4].bufferInput"),
    "dmp_FragOperation.enableAlphaTest",
    "dmp_FragOperation.alphaRefValue",
    "dmp_FragOperation.alphaTestFunc",
};

wchar_t* StrCopy(wchar_t* dst, const wchar_t* src){
    size_t i = 0;
    for (; src[i] != L'\0'; ++i){
        dst[i] = src[i];
    }

    dst[i] = src[i];

    return dst + i;
}

wchar_t sResourcePaths[RESOURCEFILEID_MAX][FILENAME_MAX];
bool sResourcePathsInitialized = false;

}

GraphicsResource::GraphicsResource(): 
    mpRectShaderBinary(NULL), 
    mRectShaderBinarySize(0), 
    mGlProgram(0), 
    mGlProgramDebug(0), 
    mInitialized(false)
{}

GraphicsResource::~GraphicsResource(){
    this->Finalize();
}

void GraphicsResource::Finalize(){
    if (!mInitialized){
        return;
    }

    mInitialized = false;

    glUseProgram(0);

    glDeleteProgram(this->mGlProgram);
    mGlProgram = 0;

    glDeleteProgram(this->mGlProgramDebug);
    mGlProgramDebug = 0;

    glDeleteBuffers(this->VBO_MAX, this->mGlVertexBufferObject);

    if (NULL != mpRectShaderBinary){
        Layout::FreeMemory(this->mpRectShaderBinary);
    }
    mpRectShaderBinary = NULL;
    mRectShaderBinarySize = 0;

    this->mTextWriter.SetTextWriterResource(0);
    this->mTextWriterResource.DeleteResource();
}

const wchar_t* GraphicsResource::GetResourcePath(int index){
    if (!sResourcePathsInitialized){
        static const wchar_t* pResourceRoot = L"rom:";
        for (int i = 0; i < RESOURCEFILEID_MAX; ++i){
            wchar_t* buff = sResourcePaths[i];
            buff = StrCopy(buff, pResourceRoot);
            buff = StrCopy(buff, sResourceFiles[i]);
        }


        sResourcePathsInitialized = true;
    }

    if (0 <= index && index < RESOURCEFILEID_MAX){
        return sResourcePaths[index];
    }
    else{
        return NULL;
    }
}

void GraphicsResource::SetResource(int index, void* content, u32 fileSize, bool bFree){
    switch (index){
    case RESOURCEFILEID_RECTDRAWERSHADER:{
            mpRectShaderBinary = Layout::AllocMemory(fileSize);
            mRectShaderBinarySize = fileSize;
            std::memcpy(this->mpRectShaderBinary, content, fileSize);
            if (bFree){
                Layout::FreeMemory(content);
            }
            return;
        }
    case RESOURCEFILEID_FONTSHADER:{
            mTextWriterResource.InitResource(static_cast<u8*>(content), fileSize);
            mTextWriter.SetTextWriterResource(&this->mTextWriterResource);

            if (bFree){
                Layout::FreeMemory(content);
            }
            return;
        }
    case RESOURCEFILEID_PANESHADER:{
            mGlProgram = glCreateProgram();

            GLuint shader = glCreateShader(GL_VERTEX_SHADER);

            glShaderBinary(1, &shader, GL_PLATFORM_BINARY_DMP, content, fileSize);

            glAttachShader(this->mGlProgram, shader);
            glAttachShader(this->mGlProgram, GL_DMP_FRAGMENT_SHADER_DMP);

            glDeleteShader(shader);

            glBindAttribLocation(this->mGlProgram, VERTEXATTR_VERTEX_INDEX, "aVertexIndex");

            glLinkProgram(this->mGlProgram);

            glUseProgram(this->mGlProgram);

            glUniform1i(glGetUniformLocation(this->mGlProgram, "dmp_FragOperation.mode"), GL_FRAGOP_MODE_GL_DMP);

            glUniform1i(glGetUniformLocation(this->mGlProgram, "dmp_FragmentLighting.enabled"), GL_FALSE);

            if (bFree){
                Layout::FreeMemory(content);
            }
            return;
        }

    case RESOURCEFILEID_CONSTCOLORSHADER:{
            mGlProgramDebug = glCreateProgram();

            GLuint shader = glCreateShader(GL_VERTEX_SHADER);

            glShaderBinary(1, &shader, GL_PLATFORM_BINARY_DMP, content, fileSize);

            glAttachShader(this->mGlProgramDebug, shader);
            glAttachShader(this->mGlProgramDebug, GL_DMP_FRAGMENT_SHADER_DMP);

            glDeleteShader(shader);

            glBindAttribLocation(this->mGlProgramDebug, VERTEXATTR_POS, "aPosition");

            glLinkProgram(this->mGlProgramDebug);

            if (bFree){
                Layout::FreeMemory(content);
            }
            return;
        }
    default:
    }
}

void GraphicsResource::StartSetup(){
    NW_ASSERT(!this->mInitialized);
}

bool GraphicsResource::FinishSetup(){
    math::MTX34Identity(&this->mMtxModelView);

    this->InitVBO();

    for (int i = 0; i < this->UNIFORM_MAX; ++i){
        if (sUniformNames[i] == NULL){
            mUniformLocation[i] = 0;
        }
        else{
            mUniformLocation[i] = glGetUniformLocation(mGlProgram, sUniformNames[i]);
        }
    }
    mInitialized = true;

    return true;
}

void GraphicsResource::SetProjectionMtx(const nw::math::MTX44& mtx){
    GLuint program = 0;

    program = this->GetGlProgram();
    glUseProgram(program);
    glUniformMatrix4fv(this->GetUniformLocation(this->UNIFORM_uProjection), 1, GL_TRUE, mtx.a);

    program = this->GetGlProgramDebug();
    glUseProgram(program);
    glUniformMatrix4fv(glGetUniformLocation(program, "uProjection"), 1, GL_TRUE, mtx.a);

    this->mTextWriterResource.ActiveGlProgram();
    this->mTextWriterResource.SetProjectionMtx(mtx);
}

void GraphicsResource::ResetGlProgramState(){
    this->SetNumTexEnv(internal::TexEnvUnitMax);
    this->SetTexEnvAuto(false);
}

void GraphicsResource::ResetGlState(){
    mFirstDraw = true;
}

void GraphicsResource::ActiveVBO(){
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->GetVBO(this->VBO_ELEMENT));

    glBindBuffer(GL_ARRAY_BUFFER, this->GetVBO(this->VBO_VERTEX_INDEX));
    glEnableVertexAttribArray(VERTEXATTR_VERTEX_INDEX);
    glVertexAttribPointer(VERTEXATTR_VERTEX_INDEX, VERTEXATTRSIZE_INDEX, GL_SHORT, GL_FALSE, 0, NULL);
}

}
}