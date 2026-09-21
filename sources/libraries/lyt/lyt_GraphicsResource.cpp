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

enum ResouceFileID
{
    RESOURCEFILEID_RECTDRAWERSHADER,
    RESOURCEFILEID_FONTSHADER,
    RESOURCEFILEID_PANESHADER,
    RESOURCEFILEID_CONSTCOLORSHADER,
    RESOURCEFILEID_MAX
};

const wchar_t* s_ResourceFiles[] =
{
    L"/shaders/nwfont_RectDrawerShader.shbin",
    L"/shaders/nwfont_TextWriterShader.shbin",
    L"/shaders/nwlyt_PaneShader.shbin",
    L"/shaders/nwlyt_ConstColorShader.shbin",
};

#define FEATURE_FINAL(x) NULL

const char* s_UniformNames[] =
{
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

wchar_t* StrCopy(wchar_t* dst, const wchar_t* src)
{
    size_t i = 0;
    for (; src[i] != L'\0'; ++i)
    {
        dst[i] = src[i];
    }

    dst[i] = src[i];

    return dst + i;
}

wchar_t s_ResourcePaths[RESOURCEFILEID_MAX][FILENAME_MAX];
bool s_ResourcePathsInitialized = false;

}

GraphicsResource::GraphicsResource(): 
    mpRectShaderBinary(NULL), 
    m_RectShaderBinarySize(0), 
    m_GlProgram(0), 
    m_GlProgramDebug(0), 
    m_Initialized(false)
    {
}

GraphicsResource::~GraphicsResource()
{
    this->Finalize();
}

void GraphicsResource::Finalize()
{
    if (!m_Initialized)
    {
        return;
    }

    m_Initialized = false;

    glUseProgram(0);

    glDeleteProgram(this->m_GlProgram);
    m_GlProgram = 0;

    glDeleteProgram(this->m_GlProgramDebug);
    m_GlProgramDebug = 0;

    glDeleteBuffers(this->VBO_MAX, this->m_GlVertexBufferObject);

    if (NULL != mpRectShaderBinary)
    {
        Layout::FreeMemory(this->mpRectShaderBinary);
    }
    mpRectShaderBinary = NULL;
    m_RectShaderBinarySize = 0;

    this->m_TextWriter.SetTextWriterResource(0);
    this->m_TextWriterResource.DeleteResource();
}

const wchar_t* GraphicsResource::GetResourcePath(int index)
{
    if (!s_ResourcePathsInitialized)
    {
        static const wchar_t* pResourceRoot = L"rom:";
        for (int i = 0; i < RESOURCEFILEID_MAX; ++i)
        {
            wchar_t* buff = s_ResourcePaths[i];
            buff = StrCopy(buff, pResourceRoot);
            buff = StrCopy(buff, s_ResourceFiles[i]);
        }

        s_ResourcePathsInitialized = true;
    }

    if (0 <= index && index < RESOURCEFILEID_MAX)
    {
        return s_ResourcePaths[index];
    }
    else
    {
        return NULL;
    }
}

void GraphicsResource::SetResource(int index, void* content, u32 fileSize, bool bFree)
{
    switch (index)
    {
    case RESOURCEFILEID_RECTDRAWERSHADER:
    {
            mpRectShaderBinary = Layout::AllocMemory(fileSize);
            m_RectShaderBinarySize = fileSize;
            std::memcpy(this->mpRectShaderBinary, content, fileSize);
            if (bFree)
            {
                Layout::FreeMemory(content);
            }
            return;
        }
    case RESOURCEFILEID_FONTSHADER:
    {
            m_TextWriterResource.InitResource(static_cast<u8*>(content), fileSize);
            m_TextWriter.SetTextWriterResource(&this->m_TextWriterResource);

            if (bFree)
            {
                Layout::FreeMemory(content);
            }
            return;
        }
    case RESOURCEFILEID_PANESHADER:
    {
            m_GlProgram = glCreateProgram();

            GLuint shader = glCreateShader(GL_VERTEX_SHADER);

            glShaderBinary(1, &shader, GL_PLATFORM_BINARY_DMP, content, fileSize);

            glAttachShader(this->m_GlProgram, shader);
            glAttachShader(this->m_GlProgram, GL_DMP_FRAGMENT_SHADER_DMP);

            glDeleteShader(shader);

            glBindAttribLocation(this->m_GlProgram, VERTEXATTR_VERTEX_INDEX, "aVertexIndex");

            glLinkProgram(this->m_GlProgram);

            glUseProgram(this->m_GlProgram);

            glUniform1i(glGetUniformLocation(this->m_GlProgram, "dmp_FragOperation.mode"), GL_FRAGOP_MODE_GL_DMP);

            glUniform1i(glGetUniformLocation(this->m_GlProgram, "dmp_FragmentLighting.enabled"), GL_FALSE);

            if (bFree)
            {
                Layout::FreeMemory(content);
            }
            return;
        }

    case RESOURCEFILEID_CONSTCOLORSHADER:
    {
            m_GlProgramDebug = glCreateProgram();

            GLuint shader = glCreateShader(GL_VERTEX_SHADER);

            glShaderBinary(1, &shader, GL_PLATFORM_BINARY_DMP, content, fileSize);

            glAttachShader(this->m_GlProgramDebug, shader);
            glAttachShader(this->m_GlProgramDebug, GL_DMP_FRAGMENT_SHADER_DMP);

            glDeleteShader(shader);

            glBindAttribLocation(this->m_GlProgramDebug, VERTEXATTR_POS, "aPosition");

            glLinkProgram(this->m_GlProgramDebug);

            if (bFree)
            {
                Layout::FreeMemory(content);
            }
            return;
        }
    default:
    }
}

void GraphicsResource::StartSetup()
{
    NW_ASSERT(!this->m_Initialized);
}

bool GraphicsResource::FinishSetup()
{
    math::MTX34Identity(&this->m_MtxModelView);

    this->InitVBO();

    for (int i = 0; i < this->UNIFORM_MAX; ++i)
    {
        if (s_UniformNames[i] == NULL)
        {
            m_UniformLocation[i] = 0;
        }
        else
        {
            m_UniformLocation[i] = glGetUniformLocation(m_GlProgram, s_UniformNames[i]);
        }
    }
    m_Initialized = true;

    return true;
}

void GraphicsResource::SetProjectionMtx(const nw::math::MTX44& mtx)
{
    GLuint program = 0;

    program = this->GetGlProgram();
    glUseProgram(program);
    glUniformMatrix4fv(this->GetUniformLocation(this->UNIFORM_uProjection), 1, GL_TRUE, mtx.a);

    program = this->GetGlProgramDebug();
    glUseProgram(program);
    glUniformMatrix4fv(glGetUniformLocation(program, "uProjection"), 1, GL_TRUE, mtx.a);

    this->m_TextWriterResource.ActiveGlProgram();
    this->m_TextWriterResource.SetProjectionMtx(mtx);
}

void GraphicsResource::ResetGlProgramState()
{
    this->SetNumTexEnv(internal::TexEnvUnitMax);
    this->SetTexEnvAuto(false);
}

void GraphicsResource::ResetGlState()
{
    m_FirstDraw = true;
}

void GraphicsResource::ActiveVBO()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->GetVBO(this->VBO_ELEMENT));

    glBindBuffer(GL_ARRAY_BUFFER, this->GetVBO(this->VBO_VERTEX_INDEX));
    glEnableVertexAttribArray(VERTEXATTR_VERTEX_INDEX);
    glVertexAttribPointer(VERTEXATTR_VERTEX_INDEX, VERTEXATTRSIZE_INDEX, GL_SHORT, GL_FALSE, 0, NULL);
}

}
}