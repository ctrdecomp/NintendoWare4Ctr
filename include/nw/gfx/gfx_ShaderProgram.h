#pragma once

#include <nw/gfx/gfx_GfxObject.h>
#include <nw/gfx/gfx_ShaderUniforms.h>
#include <nw/gfx/res/gfx_ResShader.h>
#include <nw/gfx/res/gfx_ResVertex.h>

#include <nw/ut/ut_MoveArray.h>

#include <nw/types.h>
#include <nw/gfx/gfx_CommandUtil.h>
#include <nw/gfx/gfx_ActivateCommand.h>

namespace nw{
namespace os{
    class IAllocator;
}
namespace gfx{

class RenderContext;

enum GeometryShaderMode
{
    GEOMETRY_SHADER_MODE_UNINITIALIZED,
    GEOMETRY_SHADER_MODE_NONE,
    GEOMETRY_SHADER_MODE_USED,
    GEOMETRY_SHADER_MODE_COUNT
};

class ShaderProgram : public GfxObject
{
public:
    void ActivateDescription(ResShaderProgramDescription shaderProgramDescription);

    void ActivateShaderProgramMode(bool useGeometry);

    void DeactivateDescription()
    {
        m_GeometryShaderMode = GEOMETRY_SHADER_MODE_UNINITIALIZED;

        m_ProgramObject = 0;
        m_Description = ResShaderProgramDescription(0);
    }

    const ResShaderProgramDescription GetActiveDescription() const { return m_Description; }


    bool IsParticleShader() const
    {
        return m_Description.GetGeometryShaderIndex() > 0;
    }

    inline int GetVertexAttributeIndex(ResVertexAttribute::VertexAttributeUsage usage) const;

    int GetVertexAttributeIndex(u32 usage) const
    {
        return GetVertexAttributeIndex(static_cast<ResVertexAttribute::VertexAttributeUsage>(usage));
    }

    void SetProjection(const math::MTX44& projection) const
    {
        internal::NWSetVertexUniform4fv(VERTEX_SHADER_UNIFORM_PROJMTX_INDEX, 4, projection);
    }

    void SetViewMatrix(const math::MTX34& view) const
    {
        internal::NWSetVertexUniform4fv(VERTEX_SHADER_UNIFORM_VIEWMTX_INDEX, 3, view);
    }

    void SetWorldMatrix(const math::MTX34& world) const
    {
        internal::NWSetVertexUniform4fv(VERTEX_SHADER_UNIFORM_WRLDMTX_INDEX, 3, world);
    }

    void SetModelNormalMatrix(const math::MTX34& normalMatrix) const
    {
        internal::NWSetVertexUniform4fv(VERTEX_SHADER_UNIFORM_NORMMTX_INDEX, 3, normalMatrix);
    }

    void SetUniversal(int index, const math::VEC4& vec) const
    {
        internal::NWSetVertexUniform4fv(VERTEX_SHADER_UNIFORM_UNIVREG_INDEX + index, 1, vec);
    }

    void SetUniversal(int index, const math::MTX34& mtx) const
    {
        internal::NWSetVertexUniform4fv(VERTEX_SHADER_UNIFORM_UNIVREG_INDEX + index, 3, mtx);
    }

    void SetUniversal(int index, const math::MTX44& mtx) const
    {
        internal::NWSetVertexUniform4fv(VERTEX_SHADER_UNIFORM_UNIVREG_INDEX + index, 4, mtx);
    }

    void SetUserVertexUniform(s32 index, ResShaderParameterValue parameter) const
    {
        if (parameter.GetUniformType() == ResShaderParameterValue::TYPE_BOOL1)
        {
            SetVertexUniformBool(index, parameter.GetValueBool());
        }
        else if (parameter.GetUniformType() == ResShaderParameterValue::TYPE_FLOAT1)
        {
            internal::NWSetVertexUniform1fv(index, 1, parameter.GetValueF32());
        }
        else if (parameter.GetUniformType() == ResShaderParameterValue::TYPE_FLOAT2)
        {
            internal::NWSetVertexUniform2fv(index, 1, parameter.GetValueF32());
        }
        else if (parameter.GetUniformType() == ResShaderParameterValue::TYPE_FLOAT3)
        {
            internal::NWSetVertexUniform3fv(index, 1, parameter.GetValueF32());
        }
        else if (parameter.GetUniformType() == ResShaderParameterValue::TYPE_FLOAT4)
        {
            internal::NWSetVertexUniform4fv(index, 1, parameter.GetValueF32());
        }
    }

    void SetUserGeometryUniform(s32 index, ResShaderParameterValue parameter) const
    {
        if (parameter.GetUniformType() == ResShaderParameterValue::TYPE_BOOL1)
        {
            SetGeometryUniformBool(index, parameter.GetValueBool());
        }
        else if (parameter.GetUniformType() == ResShaderParameterValue::TYPE_FLOAT1)
        {
            internal::NWSetGeometryUniform1fv(index, 1, parameter.GetValueF32());
        }
        else if (parameter.GetUniformType() == ResShaderParameterValue::TYPE_FLOAT2)
        {
            internal::NWSetGeometryUniform2fv(index, 1, parameter.GetValueF32());
        }
        else if (parameter.GetUniformType() == ResShaderParameterValue::TYPE_FLOAT3)
        {
            internal::NWSetGeometryUniform3fv(index, 1, parameter.GetValueF32());
        }
        else if (parameter.GetUniformType() == ResShaderParameterValue::TYPE_FLOAT4)
        {
            internal::NWSetGeometryUniform4fv(index, 1, parameter.GetValueF32());
        }
    }

    void SetVertexUniformBool(int index, bool value) const
    {
        NW_ASSERT(0 <= index && index < 16);
        if (value)
        {
            m_VertexIntUniforms[0] |= 0x1 << index;
        }
        else{
            m_VertexIntUniforms[0] &= ~(0x1 << index);
        }
    }

    void SetGeometryUniformBool(int index, bool value) const
    {
        NW_ASSERT(0 <= index && index < 16);
        if (value)
        {
            m_GeometryIntUniforms[0] |= 0x1 << index;
        }
        else{
            m_GeometryIntUniforms[0] &= ~(0x1 << index);
        }
    }

    void SetVertexUniformInt(int index, s32 value) const
    {
        NW_ASSERT(0 <= index && index < 4);
        m_VertexIntUniforms[2 + index] = value & 0xFF;
    }

    void SetGeometryUniformInt(int index, s32 value) const
    {
        NW_ASSERT(0 <= index && index < 4);
        m_GeometryIntUniforms[2 + index] = value & 0xFF;
    }

    void SetVertexUniformInt(int index, s32 x, s32 y) const
    {
        NW_ASSERT(0 <= index && index < 4);
        m_VertexIntUniforms[2 + index] = (x & 0xFF) | ((y & 0xFF) << 8);
    }

    void SetGeometryUniformInt(int index, s32 x, s32 y) const
    {
        NW_ASSERT(0 <= index && index < 4);
        m_GeometryIntUniforms[2 + index] = (x & 0xFF) | ((y & 0xFF) << 8);
    }

    void SetVertexUniformInt(int index, s32 x, s32 y, s32 z) const
    {
        NW_ASSERT(0 <= index && index < 4);
        m_VertexIntUniforms[2 + index] = (x & 0xFF) | ((y & 0xFF) << 8) | ((z & 0xFF) << 16);
    }

    void SetGeometryUniformInt(int index, s32 x, s32 y, s32 z) const
    {
        NW_ASSERT(0 <= index && index < 4);
        m_GeometryIntUniforms[2 + index] = (x & 0xFF) | ((y & 0xFF) << 8) | ((z & 0xFF) << 16);
    }

    void FlushUniform() const
    {
        internal::NWUseCmdlist<sizeof(m_VertexIntUniforms)>(&m_VertexIntUniforms[0]);
        if (m_Description.GetGeometryShaderIndex() >= 0)
        {
            internal::NWUseCmdlist<sizeof(m_GeometryIntUniforms)>(&m_GeometryIntUniforms[0]);
        }
    }

    ShaderProgram(nw::os::IAllocator* allocator);

private:
    virtual ~ShaderProgram();

    ResShaderProgramDescription m_Description;
    GLuint m_ProgramObject;

    GeometryShaderMode m_GeometryShaderMode;

    mutable u32 m_VertexIntUniforms[6];
    mutable u32 m_GeometryIntUniforms[6];
    mutable u32 m_DisableGeometry[2];
};

inline int ShaderProgram::GetVertexAttributeIndex(ResVertexAttribute::VertexAttributeUsage usage) const
{
    ResShaderProgramDescription description = GetActiveDescription();
    NW_ASSERT(description.IsValid());

    return description.GetAttributeIndices(usage);
}

}
}