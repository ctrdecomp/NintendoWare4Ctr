/*---------------------------------------------------------------------------*
  Project:  NintendoWare

  Copyright (C)Nintendo/HAL Laboratory, Inc.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary
  information of Nintendo and/or its licensed developers and are protected by
  national and international copyright laws. They may not be disclosed to third
  parties or copied or duplicated in any form, in whole or in part, without the
  prior written consent of Nintendo.

  The content herein is highly confidential and should be handled accordingly.
 *---------------------------------------------------------------------------*/

#ifndef NW_GFX_SHADERPROGRAM_H_
#define NW_GFX_SHADERPROGRAM_H_

#include <nw/gfx/gfx_GfxObject.h>
#include <nw/gfx/gfx_ShaderUniforms.h>
#include <nw/gfx/res/gfx_ResShader.h>
#include <nw/gfx/res/gfx_ResVertex.h>

#include <nw/ut/ut_MoveArray.h>

#include <nw/types.h>
#include <nw/gfx/gfx_CommandUtil.h>
#include <nw/gfx/gfx_ActivateCommand.h>
#include <nw/gfx/gfx_Config.h>

#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 1301 // padding inserted in struct.
#endif
namespace nw
{
namespace gfx
{

class RenderContext;

//
enum GeometryShaderMode
{
    //
    GEOMETRY_SHADER_MODE_UNINITIALIZED,
    //
    GEOMETRY_SHADER_MODE_NONE,
    //
    GEOMETRY_SHADER_MODE_USED,
    //
    GEOMETRY_SHADER_MODE_COUNT
};

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ShaderProgram : public GfxObject
{
public:
    //----------------------------------------
    //
    //

    //
    //
    //
    //
    void ActivateDescription(ResShaderProgramDescription shaderProgramDescription);

    //
    //
    //
    //
    void ActivateShaderProgramMode(bool useGeometry);

    //
    void DeactivateDescription()
    {
        // Reset the mode settings for the geometry shader.
        m_GeometryShaderMode = GEOMETRY_SHADER_MODE_UNINITIALIZED;

#if defined(NW_GFX_PROGRAM_OBJECT_ENABLED)
        m_UniformLocation = NULL;
#endif
        m_ProgramObject = 0;
        m_Description = ResShaderProgramDescription( 0 );
    }

    //
    //
    //
    //
    const ResShaderProgramDescription GetActiveDescription() const { return m_Description; }

#if defined(NW_GFX_PROGRAM_OBJECT_ENABLED)
    //
    //
    //
    //
    GLuint GetProgramObject() const { return m_ProgramObject; }
#endif

    //
    //
    //
    //
    bool IsParticleShader() const
    {
        return m_Description.GetGeometryShaderIndex() > 0;
    }

    //

    //----------------------------------------
    //
    //


    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    NW_INLINE int GetVertexAttributeIndex(
        ResVertexAttribute::VertexAttributeUsage usage) const;

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    int GetVertexAttributeIndex(u32 usage) const
    {
        return GetVertexAttributeIndex(
            static_cast<ResVertexAttribute::VertexAttributeUsage>(usage));
    }

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    void SetProjection(const math::MTX44& projection) const
    {
        internal::NWSetVertexUniform4fv(VERTEX_SHADER_UNIFORM_PROJMTX_INDEX, 4, projection);
    }

    //
    //
    //
    //
    void SetViewMatrix(const math::MTX34& view) const
    {
        internal::NWSetVertexUniform4fv(VERTEX_SHADER_UNIFORM_VIEWMTX_INDEX, 3, view);
    }

    //
    //
    //
    //
    void SetWorldMatrix(const math::MTX34& world) const
    {
        internal::NWSetVertexUniform4fv(VERTEX_SHADER_UNIFORM_WRLDMTX_INDEX, 3, world);
    }

    //
    //
    //
    //
    void SetModelNormalMatrix(const math::MTX34& normalMatrix) const
    {
        internal::NWSetVertexUniform4fv(VERTEX_SHADER_UNIFORM_NORMMTX_INDEX, 3, normalMatrix);
    }

    //
    //
    //
    //
    //
    void SetUniversal(int index, const math::VEC4& vec) const
    {
        internal::NWSetVertexUniform4fv(VERTEX_SHADER_UNIFORM_UNIVREG_INDEX + index, 1, vec);
    }

    //
    //
    //
    //
    //
    void SetUniversal(int index, const math::MTX34& mtx) const
    {
        internal::NWSetVertexUniform4fv(VERTEX_SHADER_UNIFORM_UNIVREG_INDEX + index, 3, mtx);
    }

    //
    //
    //
    //
    //
    void SetUniversal(int index, const math::MTX44& mtx) const
    {
        internal::NWSetVertexUniform4fv(VERTEX_SHADER_UNIFORM_UNIVREG_INDEX + index, 4, mtx);
    }

#if defined(NW_GFX_PROGRAM_OBJECT_ENABLED)
    //
    //
    //
    //
    //
    void SetUniformBool(ShaderUniform location, bool value) const { glUniform1i(m_UniformLocation->GetUniformLocation(location), value ? 1 : 0); }

    //
    //
    //
    //
    //
    void SetUniformFloat(ShaderUniform location, f32 value) const { glUniform1f(m_UniformLocation->GetUniformLocation(location), value); }

    //
    //
    //
    //
    //
    void SetUniformInt(ShaderUniform location, s32 value) const { glUniform1i(m_UniformLocation->GetUniformLocation(location), value); }

    //
    //
    //
    //
    //
    //
    void SetUniformInt(ShaderUniform location, s32 x, s32 y) const { glUniform2i(m_UniformLocation->GetUniformLocation(location), x, y); }

    //
    //
    //
    //
    //
    //
    //
    void SetUniformInt(ShaderUniform location, s32 x, s32 y, s32 z) const { glUniform3i(m_UniformLocation->GetUniformLocation(location), x, y, z); }

    //
    //
    //
    //
    //
    //
    //
    //
    void SetUniformInt(ShaderUniform location, s32 x, s32 y, s32 z, s32 w) const { glUniform4i(m_UniformLocation->GetUniformLocation(location), x, y, z, w); }

    //
    //
    //
    //
    //
    void SetUniform2(ShaderUniform location, const GLfloat* pointer) const { glUniform2fv(m_UniformLocation->GetUniformLocation(location), 1, pointer); }

    //
    //
    //
    //
    //
    void SetUniform3(ShaderUniform location, const GLfloat* pointer) const { glUniform3fv(m_UniformLocation->GetUniformLocation(location), 1, pointer); }

    //
    //
    //
    //
    //
    void SetUniform4(ShaderUniform location, const GLfloat* pointer) const { glUniform4fv(m_UniformLocation->GetUniformLocation(location), 1, pointer); }

    //
    //
    //
    //
    //
    void SetUniform(ShaderUniform location, const math::VEC2& vec) const { glUniform2fv(m_UniformLocation->GetUniformLocation(location), 1, vec); }

    //
    //
    //
    //
    //
    void SetUniform(ShaderUniform location, const math::VEC3& vec) const { glUniform3fv(m_UniformLocation->GetUniformLocation(location), 1, vec); }

    //
    //
    //
    //
    //
    void SetUniform(ShaderUniform location, const math::VEC4& vec) const { glUniform4fv(m_UniformLocation->GetUniformLocation(location), 1, vec); }

    //
    //
    //
    //
    //
    void SetUniform(ShaderUniform location, const math::MTX22& mtx) const { glUniform2fv(m_UniformLocation->GetUniformLocation(location), 2, mtx); }

    //
    //
    //
    //
    //
    void SetUniform(ShaderUniform location, const math::MTX23& mtx) const { glUniform3fv(m_UniformLocation->GetUniformLocation(location), 2, mtx); }

    //
    //
    //
    //
    //
    void SetUniform(ShaderUniform location, const math::MTX34& mtx) const { glUniform4fv(m_UniformLocation->GetUniformLocation(location), 3, mtx); }

    //
    //
    //
    //
    //
    void SetUniform(ShaderUniform location, const math::MTX44& mtx) const { glUniform4fv(m_UniformLocation->GetUniformLocation(location), 4, mtx); }

    //
    //
    //
    //
    //
    //
    void SetUniforms(ShaderUniform location, const math::VEC2* vec, int count) const { glUniform2fv(m_UniformLocation->GetUniformLocation(location), count, *vec); }

    //
    //
    //
    //
    //
    //
    void SetUniforms(ShaderUniform location, const math::VEC3* vec, int count) const { glUniform3fv(m_UniformLocation->GetUniformLocation(location), count, *vec); }

    //
    //
    //
    //
    //
    //
    void SetUniforms(ShaderUniform location, const math::VEC4* vec, int count) const { glUniform4fv(m_UniformLocation->GetUniformLocation(location), count, *vec); }

    //
    //
    //
    //
    //
    template<typename TValue>
    void SetUniforms(ShaderUniform location, const ut::MoveArray<TValue>* array) const
    {
        NW_NULL_ASSERT(array);
        if (array->empty()) { return; }

        this->SetUniforms(location, array->Elements(), array->size());
    }

    //
    //
    //
    //
    //
    //
    template<typename TValue>
    void SetUniforms(ShaderUniform location, const ut::MoveArray<TValue>* array, s32 size) const
    {
        NW_NULL_ASSERT(array);
        NW_ASSERT(size <= array->size());

        this->SetUniforms(location, array->Elements(), size);
    }

    //
    //
    //
    //
    //
    //
    //
    void SetUserUniform(GLint location, ResShaderParameterValue parameter) const
    {
        if (parameter.GetUniformType() == ResShaderParameterValue::TYPE_BOOL1)
        {
            glUniform1iv(location, 1, reinterpret_cast<GLint*>(parameter.GetValueS32()));
        }
        else if (parameter.GetUniformType() == ResShaderParameterValue::TYPE_FLOAT1)
        {
            glUniform1fv(location, 1, parameter.GetValueF32());
        }
        else if (parameter.GetUniformType() == ResShaderParameterValue::TYPE_FLOAT2)
        {
            glUniform2fv(location, 1, parameter.GetValueF32());
        }
        else if (parameter.GetUniformType() == ResShaderParameterValue::TYPE_FLOAT3)
        {
            glUniform3fv(location, 1, parameter.GetValueF32());
        }
        else if (parameter.GetUniformType() == ResShaderParameterValue::TYPE_FLOAT4)
        {
            glUniform4fv(location, 1, parameter.GetValueF32());
        }
    }
#endif // defined(NW_GFX_PROGRAM_OBJECT_ENABLED)

    //
    //
    //
    //
    //
    void SetUserVertexUniform(s32 index, ResShaderParameterValue parameter) const
    {
        if (parameter.GetUniformType() == ResShaderParameterValue::TYPE_BOOL1)
        {
            this->SetVertexUniformBool(index, parameter.GetValueBool());
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

    //
    //
    //
    //
    //
    void SetUserGeometryUniform(s32 index, ResShaderParameterValue parameter) const
    {
        if (parameter.GetUniformType() == ResShaderParameterValue::TYPE_BOOL1)
        {
            this->SetGeometryUniformBool(index, parameter.GetValueBool());
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

    //
    //
    //
    //
    //
    void SetVertexUniformBool(int index, bool value) const
    {
        NW_ASSERT(0 <= index && index < 16);
        if (value)
        {
            m_VertexIntUniforms[0] |= 0x1 << index;
        }
        else
        {
            m_VertexIntUniforms[0] &= ~(0x1 << index);
        }
    }

    //
    //
    //
    //
    //
    void SetGeometryUniformBool(int index, bool value) const
    {
        NW_ASSERT(0 <= index && index < 16);
        if (value)
        {
            m_GeometryIntUniforms[0] |= 0x1 << index;
        }
        else
        {
            m_GeometryIntUniforms[0] &= ~(0x1 << index);
        }
    }

    //
    //
    //
    //
    //
    void SetVertexUniformInt(int index, s32 value) const
    {
        NW_ASSERT(0 <= index && index < 4);
        m_VertexIntUniforms[2 + index] = value & 0xFF;
    }

    //
    //
    //
    //
    //
    void SetGeometryUniformInt(int index, s32 value) const
    {
        NW_ASSERT(0 <= index && index < 4);
        m_GeometryIntUniforms[2 + index] = value & 0xFF;
    }

    //
    //
    //
    //
    //
    //
    void SetVertexUniformInt(int index, s32 x, s32 y) const
    {
        NW_ASSERT(0 <= index && index < 4);
        m_VertexIntUniforms[2 + index] = (x & 0xFF) | ((y & 0xFF) << 8);
    }

    //
    //
    //
    //
    //
    //
    void SetGeometryUniformInt(int index, s32 x, s32 y) const
    {
        NW_ASSERT(0 <= index && index < 4);
        m_GeometryIntUniforms[2 + index] = (x & 0xFF) | ((y & 0xFF) << 8);
    }

    //
    //
    //
    //
    //
    //
    //
    void SetVertexUniformInt(int index, s32 x, s32 y, s32 z) const
    {
        NW_ASSERT(0 <= index && index < 4);
        m_VertexIntUniforms[2 + index] = (x & 0xFF) | ((y & 0xFF) << 8) | ((z & 0xFF) << 16);
    }

    //
    //
    //
    //
    //
    //
    //
    void SetGeometryUniformInt(int index, s32 x, s32 y, s32 z) const
    {
        NW_ASSERT(0 <= index && index < 4);
        m_GeometryIntUniforms[2 + index] = (x & 0xFF) | ((y & 0xFF) << 8) | ((z & 0xFF) << 16);
    }

    //
    void FlushUniform() const
    {
        internal::NWUseCmdlist<sizeof(m_VertexIntUniforms)>(&m_VertexIntUniforms[0]);
        if (m_Description.GetGeometryShaderIndex() >= 0)
        {
            internal::NWUseCmdlist<sizeof(m_GeometryIntUniforms)>(&m_GeometryIntUniforms[0]);
        }
    }
    //

    //----------------------------------------
    //
    //

    //

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    ShaderProgram(os::IAllocator* allocator);

private:
    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    virtual ~ShaderProgram();

    ResShaderProgramDescription m_Description;
#if defined(NW_GFX_PROGRAM_OBJECT_ENABLED)
    ShaderUniformLocation* m_UniformLocation;
#endif
    GLuint m_ProgramObject;

    GeometryShaderMode m_GeometryShaderMode;

    mutable u32 m_VertexIntUniforms[6];
    mutable u32 m_GeometryIntUniforms[6];
    mutable u32 m_DisableGeometry[2];
};

NW_INLINE int
ShaderProgram::GetVertexAttributeIndex(
    ResVertexAttribute::VertexAttributeUsage usage
) const
{
    ResShaderProgramDescription description = this->GetActiveDescription();
    NW_ASSERT(description.IsValid());

    return description.GetAttributeIndices(usage);
}

} // namespace gfx
} // namespace nw
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 1301 // padding inserted in struct.
#endif

#endif // NW_GFX_SHADERPROGRAM_H_
