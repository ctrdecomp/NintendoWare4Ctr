// Filename: font_TextWriterResource.cpp
//
// Project: NintendoWare4Ctr

#include <GLES2/gl2extern.h>

#include <nw/font/font_TextWriterResource.h>
#include <nw/font/font_DispStringBuffer.h>
#include <nw/font/font_RectDrawer.h>

#define NW_FONT_TEXENV3   "dmp_TexEnv[3]"
#define NW_FONT_TEXENV4   "dmp_TexEnv[4]"
#define NW_FONT_TEXENV5   "dmp_TexEnv[5]"

namespace nw {
namespace font {

namespace{

int GetUniformLocation(GLuint programId,const char* name)
{
    int result = glGetUniformLocation(programId, name);
    return result;
}

}

TextWriterResource::TextWriterResource():
    m_PosZ(0.f),
    m_IsInitialized(false)
    {
    this->ResetLoadingTexture();
}

TextWriterResource::~TextWriterResource()
{
    this->Finalize();
}

void TextWriterResource::InitResource(const void* shaderBinary,u32 size)
{
    this->m_ProgramId = glCreateProgram();

    GLuint shaderHandle = glCreateShader(GL_VERTEX_SHADER);

    glShaderBinary(1, &shaderHandle, GL_PLATFORM_BINARY_DMP, shaderBinary, size);
    glAttachShader(this->m_ProgramId, shaderHandle);
    glDeleteShader(shaderHandle);
    glAttachShader(this->m_ProgramId, GL_DMP_FRAGMENT_SHADER_DMP);
    glBindAttribLocation(this->m_ProgramId, internal::VERTEX_ATTR_POS,      "aPosition");
    glBindAttribLocation(this->m_ProgramId, internal::VERTEX_ATTR_POS_Z,    "aPositionZ");
    glBindAttribLocation(this->m_ProgramId, internal::VERTEX_ATTR_COLOR,    "aColor");
    glBindAttribLocation(this->m_ProgramId, internal::VERTEX_ATTR_TEXCOORD, "aTexCoord");
    glLinkProgram(this->m_ProgramId);
    glGenTextures(1, &this->m_TextureId);

    /* Uniform Location*/ 
    m_UniformLocations[internal::LOC_PROJECTION] =GetUniformLocation(this->m_ProgramId, "uProjection");
    m_UniformLocations[internal::LOC_MODELVIEW] = GetUniformLocation(this->m_ProgramId, "uModelView");
    m_UniformLocations[internal::LOC_FRAGMENTLIGHTING_ENABLED] = GetUniformLocation(this->m_ProgramId, "dmp_FragmentLighting.enabled"),
    m_UniformLocations[internal::LOC_TEXTURE0_SAMPLERTYPE] = GetUniformLocation(this->m_ProgramId, "dmp_Texture[0].samplerType"),
    m_UniformLocations[internal::LOC_FOG_MODE] = GetUniformLocation(this->m_ProgramId, "dmp_Fog.mode");
    m_UniformLocations[internal::LOC_FRAGOPERATION_ENABLEALPHATEST] = GetUniformLocation(this->m_ProgramId, "dmp_FragOperation.enableAlphaTest");

    /* Texture Env Uniforms */

    /* Tex Env 3*/
    m_TexEnvUniformLocations[internal::TEXENV_3][internal::TCLOC_SRCRGB      ] = GetUniformLocation(this->m_ProgramId, NW_FONT_TEXENV3 ".srcRgb"      );
    m_TexEnvUniformLocations[internal::TEXENV_3][internal::TCLOC_SRCALPHA    ] = GetUniformLocation(this->m_ProgramId, NW_FONT_TEXENV3 ".srcAlpha"    );
    m_TexEnvUniformLocations[internal::TEXENV_3][internal::TCLOC_OPERANDRGB  ] = GetUniformLocation(this->m_ProgramId, NW_FONT_TEXENV3 ".operandRgb"  );
    m_TexEnvUniformLocations[internal::TEXENV_3][internal::TCLOC_OPERANDALPHA] = GetUniformLocation(this->m_ProgramId, NW_FONT_TEXENV3 ".operandAlpha");
    m_TexEnvUniformLocations[internal::TEXENV_3][internal::TCLOC_COMBINERGB  ] = GetUniformLocation(this->m_ProgramId, NW_FONT_TEXENV3 ".combineRgb"  );
    m_TexEnvUniformLocations[internal::TEXENV_3][internal::TCLOC_COMBINEALPHA] = GetUniformLocation(this->m_ProgramId, NW_FONT_TEXENV3 ".combineAlpha");
    m_TexEnvUniformLocations[internal::TEXENV_3][internal::TCLOC_SCALERGB    ] = GetUniformLocation(this->m_ProgramId, NW_FONT_TEXENV3 ".scaleRgb"    );
    m_TexEnvUniformLocations[internal::TEXENV_3][internal::TCLOC_SCALEALPHA  ] = GetUniformLocation(this->m_ProgramId, NW_FONT_TEXENV3 ".scaleAlpha"  );
    m_TexEnvUniformLocations[internal::TEXENV_3][internal::TCLOC_CONSTRGBA   ] = GetUniformLocation(this->m_ProgramId, NW_FONT_TEXENV3 ".constRgba"   );


    /* Tex Env 4*/
    m_TexEnvUniformLocations[internal::TEXENV_4][internal::TCLOC_SRCRGB      ] =GetUniformLocation(this->m_ProgramId, NW_FONT_TEXENV4 ".srcRgb"      );
    m_TexEnvUniformLocations[internal::TEXENV_4][internal::TCLOC_SRCALPHA    ] =GetUniformLocation(this->m_ProgramId, NW_FONT_TEXENV4 ".srcAlpha"    );
    m_TexEnvUniformLocations[internal::TEXENV_4][internal::TCLOC_OPERANDRGB  ] =GetUniformLocation(this->m_ProgramId, NW_FONT_TEXENV4 ".operandRgb"  );
    m_TexEnvUniformLocations[internal::TEXENV_4][internal::TCLOC_OPERANDALPHA] =GetUniformLocation(this->m_ProgramId, NW_FONT_TEXENV4 ".operandAlpha");
    m_TexEnvUniformLocations[internal::TEXENV_4][internal::TCLOC_COMBINERGB  ] =GetUniformLocation(this->m_ProgramId, NW_FONT_TEXENV4 ".combineRgb"  );
    m_TexEnvUniformLocations[internal::TEXENV_4][internal::TCLOC_COMBINEALPHA] =GetUniformLocation(this->m_ProgramId, NW_FONT_TEXENV4 ".combineAlpha");
    m_TexEnvUniformLocations[internal::TEXENV_4][internal::TCLOC_SCALERGB    ] =GetUniformLocation(this->m_ProgramId, NW_FONT_TEXENV4 ".scaleRgb"    );
    m_TexEnvUniformLocations[internal::TEXENV_4][internal::TCLOC_SCALEALPHA  ] =GetUniformLocation(this->m_ProgramId, NW_FONT_TEXENV4 ".scaleAlpha"  );
    m_TexEnvUniformLocations[internal::TEXENV_4][internal::TCLOC_CONSTRGBA   ] =GetUniformLocation(this->m_ProgramId, NW_FONT_TEXENV4 ".constRgba"   );

    m_TexEnvUniformLocations[internal::TEXENV_5][internal::TCLOC_SRCRGB      ] =GetUniformLocation(this->m_ProgramId, NW_FONT_TEXENV5 ".srcRgb"      );
    m_TexEnvUniformLocations[internal::TEXENV_5][internal::TCLOC_SRCALPHA    ] =GetUniformLocation(this->m_ProgramId, NW_FONT_TEXENV5 ".srcAlpha"    );
    m_TexEnvUniformLocations[internal::TEXENV_5][internal::TCLOC_OPERANDRGB  ] =GetUniformLocation(this->m_ProgramId, NW_FONT_TEXENV5 ".operandRgb"  );
    m_TexEnvUniformLocations[internal::TEXENV_5][internal::TCLOC_OPERANDALPHA] =GetUniformLocation(this->m_ProgramId, NW_FONT_TEXENV5 ".operandAlpha");
    m_TexEnvUniformLocations[internal::TEXENV_5][internal::TCLOC_COMBINERGB  ] =GetUniformLocation(this->m_ProgramId, NW_FONT_TEXENV5 ".combineRgb"  );
    m_TexEnvUniformLocations[internal::TEXENV_5][internal::TCLOC_COMBINEALPHA] =GetUniformLocation(this->m_ProgramId, NW_FONT_TEXENV5 ".combineAlpha");
    m_TexEnvUniformLocations[internal::TEXENV_5][internal::TCLOC_SCALERGB    ] =GetUniformLocation(this->m_ProgramId, NW_FONT_TEXENV5 ".scaleRgb"    );
    m_TexEnvUniformLocations[internal::TEXENV_5][internal::TCLOC_SCALEALPHA  ] =GetUniformLocation(this->m_ProgramId, NW_FONT_TEXENV5 ".scaleAlpha"  );
    m_TexEnvUniformLocations[internal::TEXENV_5][internal::TCLOC_CONSTRGBA   ] =GetUniformLocation(this->m_ProgramId, NW_FONT_TEXENV5 ".constRgba"   );

    this->m_IsInitialized = true;
}

void TextWriterResource::FinalizeGX()
{
    for (int i = 0; i < internal::VERTEX_ATTR_NUM; ++i)
    {
        glDisableVertexAttribArray(i);
    }
}


void TextWriterResource::Finalize()
{
    if (this->m_IsInitialized)
    {
        FinalizeGX();
        glDeleteTextures(1, &this->m_TextureId);
        this->m_IsInitialized = false;

        glUseProgram(0);
        glDeleteProgram(this->m_ProgramId);
    }
}

void TextWriterResource::SetProjectionMtx(const nn::math::MTX44& mtx) const
{
    glUniform4fv(this->m_UniformLocations[internal::LOC_PROJECTION], 4, mtx.a);
}

void TextWriterResource::SetPosZ(f32 posZ)
{
    this->m_PosZ = posZ;
    glVertexAttrib1f(internal::VERTEX_ATTR_POS_Z, this->m_PosZ);
}

}
}