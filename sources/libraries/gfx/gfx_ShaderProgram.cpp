#include <nw/gfx/gfx_ShaderProgram.h>

#include <nw/os/os_Memory.h>
#include <nw/ut/ut_Foreach.h>
#include <nw/gfx/gfx_ShaderBinaryInfo.h>
#include <nw/gfx/gfx_ActivateCommand.h>

namespace nw{
namespace gfx{

ShaderProgram::ShaderProgram(os::IAllocator* allocator): 
    GfxObject(allocator),
    m_ProgramObject(0),
    m_GeometryShaderMode(GEOMETRY_SHADER_MODE_UNINITIALIZED)
    {
    const u32 REG_VERTEX_UNIFORM_BOOL = 0x2B0;
    const u32 REG_GEOMETRY_UNIFORM_BOOL = 0x280;
    const u32 REG_GEOMETRY_DISABLE = 0x244;
    
    const u32 HEADER_VERTEX   = internal::MakeCommandHeader(REG_VERTEX_UNIFORM_BOOL, 5, true, 0xF);
    const u32 HEADER_GEOMETRY = internal::MakeCommandHeader(REG_GEOMETRY_UNIFORM_BOOL, 5, true, 0xF);
    const u32 HEADER_DISABLE_GEOMETRY = internal::MakeCommandHeader(REG_GEOMETRY_DISABLE, 1, false, 0x1);
    
    m_VertexIntUniforms[0] = 0x7fff0000; // b0-b15
    m_VertexIntUniforms[1] = HEADER_VERTEX,
    m_VertexIntUniforms[2] = 0x00000000; // i0
    m_VertexIntUniforms[3] = 0x00000000; // i1
    m_VertexIntUniforms[4] = 0x00000000; // i2
    m_VertexIntUniforms[5] = 0x00000000; // i3

    m_GeometryIntUniforms[0] = 0x7fff0000; // b0-b15
    m_GeometryIntUniforms[1] = HEADER_GEOMETRY,
    m_GeometryIntUniforms[2] = 0x00000000; // i0
    m_GeometryIntUniforms[3] = 0x00000000; // i1
    m_GeometryIntUniforms[4] = 0x00000000; // i2
    m_GeometryIntUniforms[5] = 0x00000000; // i3
    
    m_DisableGeometry[0] = 0;
    m_DisableGeometry[1] = HEADER_DISABLE_GEOMETRY;
}

ShaderProgram::~ShaderProgram() { }

void ShaderProgram::ActivateDescription(ResShaderProgramDescription shaderProgramDescription)
{
    ResBinaryShader shader = ResBinaryShader( shaderProgramDescription.GetOwnerShaderData() );
    bool hasGeometry = (shaderProgramDescription.GetGeometryShaderIndex() >= 0);
    
    bool skipShaderBinary = false;
    bool skipShaderMode   = false;
    
    if (this->m_Description.IsValid())
    {
        bool hasGeometryPrev = (this->m_Description.GetGeometryShaderIndex() >= 0);
        if (hasGeometry == hasGeometryPrev)
        {
            skipShaderMode = true;
        }
        
        ResBinaryShader shaderPrev = ResBinaryShader(this->m_Description.GetOwnerShaderData());
        if (shader == shaderPrev)
        {
            skipShaderBinary = true;
        }
    }
    
    if (!skipShaderMode)
    {
        ActivateShaderProgramMode(hasGeometry);
    }
    
    if (!skipShaderBinary)
    {
        NW_NULL_ASSERT(shader.ref().m_CommandCache);
        internal::NWUseCmdlist(shader.ref().m_CommandCache, shader.ref().m_CommandCacheSize);
    }
    
    NW_NULL_ASSERT(shaderProgramDescription.ref().m_CommandCache );
    internal::NWUseCmdlist(shaderProgramDescription.ref().m_CommandCache, shaderProgramDescription.ref().m_CommandCacheSize);
    
    const ShaderBinaryInfo* shaderBinaryInfo = shader.GetShaderBinaryInfo();

    m_VertexIntUniforms[0]       |= shaderBinaryInfo->GetBoolConstant(shaderProgramDescription.GetVertexShaderIndex());
    
    if (hasGeometry)
    {
        m_GeometryIntUniforms[0] |= shaderBinaryInfo->GetBoolConstant(shaderProgramDescription.GetGeometryShaderIndex());
    }
    
    m_Description = shaderProgramDescription;
}

void ShaderProgram::ActivateShaderProgramMode(bool useGeometry)
{
    gfx::internal::SetupShaderProgramMode(useGeometry);
    
    m_GeometryShaderMode = useGeometry ? GEOMETRY_SHADER_MODE_USED : GEOMETRY_SHADER_MODE_NONE;
    m_DisableGeometry[0] = useGeometry ? 1 : 0;
}

}
}