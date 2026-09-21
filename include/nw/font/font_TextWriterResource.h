#pragma once

#include <GLES2/gl2.h>

#include <nn/types.h>
#include <nn/util/util_NonCopyable.h>
#include <nw/ut/ut_Preprocessor.h>
#include <nw/font/font_Types.h>
#include <nw/math/math_Types.h>

namespace nw {
namespace font {
class TextWriterResource : private nn::util::NonCopyable<TextWriterResource>
{
    friend class CharWriter;
public:
    typedef const int   (*TexEnvUniformLocationSquareArray)[internal::TCLOC_MAX];
    typedef const int   (*TexEnvUniformLocationSquareArray)[internal::TCLOC_MAX];

    TextWriterResource();
    ~TextWriterResource();
    void Finalize();
    static void FinalizeGX();
    void DeleteResource() { this->Finalize(); }
    void InitResource(const void* shaderBinary,u32 size);
    void SetPosZ(f32 posZ);
    void UpdatePosZ(f32 posZ)
    {
        if (this->m_PosZ != posZ)
        {
            SetPosZ(posZ);
        }
    }
    void ActiveGlProgram() const
    {
        glUseProgram(this->m_ProgramId);
    }

    bool SetLoadingTexture(const void* pTexture)
    {
        const bool isChanged = this->m_LoadingTexture != pTexture;
        this->m_LoadingTexture = pTexture;
        return isChanged;
    }

    internal::VertexAttribute* GetVertexAttributeArray() { return m_VtxAttrs; }
    TexEnvUniformLocationSquareArray GetTexEnvUniformLocations() const { return m_TexEnvUniformLocations; }
    void ResetLoadingTexture() { this->m_LoadingTexture = NULL; }

    void SetViewMtx(const nn::math::MTX34& mtx) const;

    void SetProjectionMtx(const nn::math::MTX44& mtx) const;

    u32 m_ProgramId;
    GLuint m_TextureId;
    internal::VertexAttribute m_VtxAttrs[internal::TRIFAN_VTX_MAX];
    int m_UniformLocations[internal::LOC_MAX];
    int m_TexEnvUniformLocations[internal::TEXENV_MAX][internal::TCLOC_MAX];
    const void* m_LoadingTexture;
    f32 m_PosZ;
    bool m_IsInitialized;
};

}
}