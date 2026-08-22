#pragma once

#include <GLES2/gl2.h>

#include <nn/types.h>
#include <nn/util/util_NonCopyable.h>
#include <nw/ut/ut_Preprocessor.h>
#include <nw/font/font_Types.h>
#include <nw/math/math_Types.h>

namespace nw {
namespace font {
class TextWriterResource : private nn::util::NonCopyable<TextWriterResource>{
    friend class CharWriter;
public:
    typedef const int   (*TexEnvUniformLocationSquareArray)[internal::TCLOC_MAX];
    typedef const int   (*TexEnvUniformLocationSquareArray)[internal::TCLOC_MAX];

    TextWriterResource();
    ~TextWriterResource();
    void Finalize();
    static void FinalizeGX();
    void DeleteResource(){ this->Finalize(); }
    void InitResource(const void* shaderBinary,u32 size);
    void SetPosZ(f32 posZ);
    void UpdatePosZ(f32 posZ){
        if (this->mPosZ != posZ){
            SetPosZ(posZ);
        }
    }
    void ActiveGlProgram() const{
        glUseProgram(this->mProgramId);
    }

    bool SetLoadingTexture(const void* pTexture){
        const bool isChanged = this->mLoadingTexture != pTexture;
        this->mLoadingTexture = pTexture;
        return isChanged;
    }
    void ActiveGlProgram() const{
        glUseProgram(mProgramId);
    }
    internal::VertexAttribute* GetVertexAttributeArray()  { return mVtxAttrs; }
    TexEnvUniformLocationSquareArray GetTexEnvUniformLocations() const   { return mTexEnvUniformLocations; }
    void ResetLoadingTexture(){ this->mLoadingTexture = NULL; }

    void SetViewMtx(const nn::math::MTX34& mtx) const;

    void SetProjectionMtx(const nn::math::MTX44& mtx) const;

    u32 mProgramId;
    GLuint mTextureId;
    internal::VertexAttribute mVtxAttrs[internal::TRIFAN_VTX_MAX];
    int mUniformLocations[internal::LOC_MAX];
    int mTexEnvUniformLocations[internal::TEXENV_MAX][internal::TCLOC_MAX];
    const void* mLoadingTexture;
    f32 mPosZ;
    bool mIsInitialized;
};

}
}