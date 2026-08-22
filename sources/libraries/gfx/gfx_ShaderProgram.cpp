#include <nw/gfx/gfx_ShaderProgram.h>

#include <nw/os/os_Memory.h>
#include <nw/ut/ut_Foreach.h>
#include <nw/gfx/gfx_ShaderBinaryInfo.h>
#include <nw/gfx/gfx_ActivateCommand.h>

namespace nw{
namespace gfx{

ShaderProgram::ShaderProgram(os::IAllocator* allocator): 
    GfxObject(allocator),
    mProgramObject(0),
    mGeometryShaderMode(GEOMETRY_SHADER_MODE_UNINITIALIZED){
    const u32 REG_VERTEX_UNIFORM_BOOL = 0x2B0;
    const u32 REG_GEOMETRY_UNIFORM_BOOL = 0x280;
    const u32 REG_GEOMETRY_DISABLE = 0x244;
    
    const u32 HEADER_VERTEX   = internal::MakeCommandHeader(REG_VERTEX_UNIFORM_BOOL, 5, true, 0xF);
    const u32 HEADER_GEOMETRY = internal::MakeCommandHeader(REG_GEOMETRY_UNIFORM_BOOL, 5, true, 0xF);
    const u32 HEADER_DISABLE_GEOMETRY = internal::MakeCommandHeader(REG_GEOMETRY_DISABLE, 1, false, 0x1);
    
    mVertexIntUniforms[0] = 0x7fff0000; // b0-b15
    mVertexIntUniforms[1] = HEADER_VERTEX,
    mVertexIntUniforms[2] = 0x00000000; // i0
    mVertexIntUniforms[3] = 0x00000000; // i1
    mVertexIntUniforms[4] = 0x00000000; // i2
    mVertexIntUniforms[5] = 0x00000000; // i3

    mGeometryIntUniforms[0] = 0x7fff0000; // b0-b15
    mGeometryIntUniforms[1] = HEADER_GEOMETRY,
    mGeometryIntUniforms[2] = 0x00000000; // i0
    mGeometryIntUniforms[3] = 0x00000000; // i1
    mGeometryIntUniforms[4] = 0x00000000; // i2
    mGeometryIntUniforms[5] = 0x00000000; // i3
    
    mDisableGeometry[0] = 0;
    mDisableGeometry[1] = HEADER_DISABLE_GEOMETRY;
}

ShaderProgram::~ShaderProgram(){ }

void ShaderProgram::ActivateDescription(ResShaderProgramDescription shaderProgramDescription){
    ResBinaryShader shader = ResBinaryShader( shaderProgramDescription.GetOwnerShaderData() );
    bool hasGeometry = (shaderProgramDescription.GetGeometryShaderIndex() >= 0);
    
    bool skipShaderBinary = false;
    bool skipShaderMode   = false;
    
    if (this->mDescription.IsValid())
    {
        bool hasGeometryPrev = (this->mDescription.GetGeometryShaderIndex() >= 0);
        if (hasGeometry == hasGeometryPrev){
            skipShaderMode = true;
        }
        
        ResBinaryShader shaderPrev = ResBinaryShader(this->mDescription.GetOwnerShaderData());
        if (shader == shaderPrev){
            skipShaderBinary = true;
        }
    }
    
    if (!skipShaderMode){
        ActivateShaderProgramMode(hasGeometry);
    }
    
    if (!skipShaderBinary){
        NW_NULL_ASSERT(shader.ref().mCommandCache);
        internal::NWUseCmdlist(shader.ref().mCommandCache, shader.ref().mCommandCacheSize);
    }
    
    NW_NULL_ASSERT(shaderProgramDescription.ref().mCommandCache );
    internal::NWUseCmdlist(shaderProgramDescription.ref().mCommandCache, shaderProgramDescription.ref().mCommandCacheSize);
    
    const ShaderBinaryInfo* shaderBinaryInfo = shader.GetShaderBinaryInfo();

    mVertexIntUniforms[0]       |= shaderBinaryInfo->GetBoolConstant(shaderProgramDescription.GetVertexShaderIndex());
    
    if (hasGeometry){
        mGeometryIntUniforms[0] |= shaderBinaryInfo->GetBoolConstant(shaderProgramDescription.GetGeometryShaderIndex());
    }
    
    mDescription = shaderProgramDescription;
}

void ShaderProgram::ActivateShaderProgramMode(bool useGeometry){
    gfx::internal::SetupShaderProgramMode(useGeometry);
    
    mGeometryShaderMode = useGeometry ? GEOMETRY_SHADER_MODE_USED : GEOMETRY_SHADER_MODE_NONE;
    mDisableGeometry[0] = useGeometry ? 1 : 0;
}

}
}