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

int GetUniformLocation(GLuint programId,const char* name){
    int result = glGetUniformLocation(programId, name);
    return result;
}

}

TextWriterResource::TextWriterResource():
    mPosZ(0.f),
    mIsInitialized(false){
    this->ResetLoadingTexture();
}

TextWriterResource::~TextWriterResource(){
    this->Finalize();
}

void TextWriterResource::InitResource(const void* shaderBinary,u32 size){
    this->mProgramId = glCreateProgram();

    GLuint shaderHandle = glCreateShader(GL_VERTEX_SHADER);

    glShaderBinary(1, &shaderHandle, GL_PLATFORM_BINARY_DMP, shaderBinary, size);
    glAttachShader(this->mProgramId, shaderHandle);
    glDeleteShader(shaderHandle);
    glAttachShader(this->mProgramId, GL_DMP_FRAGMENT_SHADER_DMP);
    glBindAttribLocation(this->mProgramId, internal::VERTEX_ATTR_POS,      "aPosition");
    glBindAttribLocation(this->mProgramId, internal::VERTEX_ATTR_POS_Z,    "aPositionZ");
    glBindAttribLocation(this->mProgramId, internal::VERTEX_ATTR_COLOR,    "aColor");
    glBindAttribLocation(this->mProgramId, internal::VERTEX_ATTR_TEXCOORD, "aTexCoord");
    glLinkProgram(this->mProgramId);
    glGenTextures(1, &this->mTextureId);

    /* Uniform Location*/ 
    mUniformLocations[internal::LOC_PROJECTION] =GetUniformLocation(this->mProgramId, "uProjection");
    mUniformLocations[internal::LOC_MODELVIEW] = GetUniformLocation(this->mProgramId, "uModelView");
    mUniformLocations[internal::LOC_FRAGMENTLIGHTING_ENABLED] = GetUniformLocation(this->mProgramId, "dmp_FragmentLighting.enabled"),
    mUniformLocations[internal::LOC_TEXTURE0_SAMPLERTYPE] = GetUniformLocation(this->mProgramId, "dmp_Texture[0].samplerType"),
    mUniformLocations[internal::LOC_FOG_MODE] = GetUniformLocation(this->mProgramId, "dmp_Fog.mode");
    mUniformLocations[internal::LOC_FRAGOPERATION_ENABLEALPHATEST] = GetUniformLocation(this->mProgramId, "dmp_FragOperation.enableAlphaTest");

    /* Texture Env Uniforms */

    /* Tex Env 3*/
    mTexEnvUniformLocations[internal::TEXENV_3][internal::TCLOC_SRCRGB      ] = GetUniformLocation(this->mProgramId, NW_FONT_TEXENV3 ".srcRgb"      );
    mTexEnvUniformLocations[internal::TEXENV_3][internal::TCLOC_SRCALPHA    ] = GetUniformLocation(this->mProgramId, NW_FONT_TEXENV3 ".srcAlpha"    );
    mTexEnvUniformLocations[internal::TEXENV_3][internal::TCLOC_OPERANDRGB  ] = GetUniformLocation(this->mProgramId, NW_FONT_TEXENV3 ".operandRgb"  );
    mTexEnvUniformLocations[internal::TEXENV_3][internal::TCLOC_OPERANDALPHA] = GetUniformLocation(this->mProgramId, NW_FONT_TEXENV3 ".operandAlpha");
    mTexEnvUniformLocations[internal::TEXENV_3][internal::TCLOC_COMBINERGB  ] = GetUniformLocation(this->mProgramId, NW_FONT_TEXENV3 ".combineRgb"  );
    mTexEnvUniformLocations[internal::TEXENV_3][internal::TCLOC_COMBINEALPHA] = GetUniformLocation(this->mProgramId, NW_FONT_TEXENV3 ".combineAlpha");
    mTexEnvUniformLocations[internal::TEXENV_3][internal::TCLOC_SCALERGB    ] = GetUniformLocation(this->mProgramId, NW_FONT_TEXENV3 ".scaleRgb"    );
    mTexEnvUniformLocations[internal::TEXENV_3][internal::TCLOC_SCALEALPHA  ] = GetUniformLocation(this->mProgramId, NW_FONT_TEXENV3 ".scaleAlpha"  );
    mTexEnvUniformLocations[internal::TEXENV_3][internal::TCLOC_CONSTRGBA   ] = GetUniformLocation(this->mProgramId, NW_FONT_TEXENV3 ".constRgba"   );


    /* Tex Env 4*/
    mTexEnvUniformLocations[internal::TEXENV_4][internal::TCLOC_SRCRGB      ] =GetUniformLocation(this->mProgramId, NW_FONT_TEXENV4 ".srcRgb"      );
    mTexEnvUniformLocations[internal::TEXENV_4][internal::TCLOC_SRCALPHA    ] =GetUniformLocation(this->mProgramId, NW_FONT_TEXENV4 ".srcAlpha"    );
    mTexEnvUniformLocations[internal::TEXENV_4][internal::TCLOC_OPERANDRGB  ] =GetUniformLocation(this->mProgramId, NW_FONT_TEXENV4 ".operandRgb"  );
    mTexEnvUniformLocations[internal::TEXENV_4][internal::TCLOC_OPERANDALPHA] =GetUniformLocation(this->mProgramId, NW_FONT_TEXENV4 ".operandAlpha");
    mTexEnvUniformLocations[internal::TEXENV_4][internal::TCLOC_COMBINERGB  ] =GetUniformLocation(this->mProgramId, NW_FONT_TEXENV4 ".combineRgb"  );
    mTexEnvUniformLocations[internal::TEXENV_4][internal::TCLOC_COMBINEALPHA] =GetUniformLocation(this->mProgramId, NW_FONT_TEXENV4 ".combineAlpha");
    mTexEnvUniformLocations[internal::TEXENV_4][internal::TCLOC_SCALERGB    ] =GetUniformLocation(this->mProgramId, NW_FONT_TEXENV4 ".scaleRgb"    );
    mTexEnvUniformLocations[internal::TEXENV_4][internal::TCLOC_SCALEALPHA  ] =GetUniformLocation(this->mProgramId, NW_FONT_TEXENV4 ".scaleAlpha"  );
    mTexEnvUniformLocations[internal::TEXENV_4][internal::TCLOC_CONSTRGBA   ] =GetUniformLocation(this->mProgramId, NW_FONT_TEXENV4 ".constRgba"   );

    mTexEnvUniformLocations[internal::TEXENV_5][internal::TCLOC_SRCRGB      ] =GetUniformLocation(this->mProgramId, NW_FONT_TEXENV5 ".srcRgb"      );
    mTexEnvUniformLocations[internal::TEXENV_5][internal::TCLOC_SRCALPHA    ] =GetUniformLocation(this->mProgramId, NW_FONT_TEXENV5 ".srcAlpha"    );
    mTexEnvUniformLocations[internal::TEXENV_5][internal::TCLOC_OPERANDRGB  ] =GetUniformLocation(this->mProgramId, NW_FONT_TEXENV5 ".operandRgb"  );
    mTexEnvUniformLocations[internal::TEXENV_5][internal::TCLOC_OPERANDALPHA] =GetUniformLocation(this->mProgramId, NW_FONT_TEXENV5 ".operandAlpha");
    mTexEnvUniformLocations[internal::TEXENV_5][internal::TCLOC_COMBINERGB  ] =GetUniformLocation(this->mProgramId, NW_FONT_TEXENV5 ".combineRgb"  );
    mTexEnvUniformLocations[internal::TEXENV_5][internal::TCLOC_COMBINEALPHA] =GetUniformLocation(this->mProgramId, NW_FONT_TEXENV5 ".combineAlpha");
    mTexEnvUniformLocations[internal::TEXENV_5][internal::TCLOC_SCALERGB    ] =GetUniformLocation(this->mProgramId, NW_FONT_TEXENV5 ".scaleRgb"    );
    mTexEnvUniformLocations[internal::TEXENV_5][internal::TCLOC_SCALEALPHA  ] =GetUniformLocation(this->mProgramId, NW_FONT_TEXENV5 ".scaleAlpha"  );
    mTexEnvUniformLocations[internal::TEXENV_5][internal::TCLOC_CONSTRGBA   ] =GetUniformLocation(this->mProgramId, NW_FONT_TEXENV5 ".constRgba"   );

    this->mIsInitialized = true;
}

void TextWriterResource::FinalizeGX(){
    for (int i = 0; i < internal::VERTEX_ATTR_NUM; ++i){
        glDisableVertexAttribArray(i);
    }
}


void TextWriterResource::Finalize(){
    if (this->mIsInitialized){
        FinalizeGX();
        glDeleteTextures(1, &this->mTextureId);
        this->mIsInitialized = false;

        glUseProgram(0);
        glDeleteProgram(this->mProgramId);
    }
}

void TextWriterResource::SetProjectionMtx(const nn::math::MTX44& mtx) const{
    glUniform4fv(this->mUniformLocations[internal::LOC_PROJECTION], 4, mtx.a);
}

void TextWriterResource::SetPosZ(f32 posZ){
    this->mPosZ = posZ;
    glVertexAttrib1f(internal::VERTEX_ATTR_POS_Z, this->mPosZ);
}

}
}