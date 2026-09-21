// Filename: gfx_ResShader.cpp
//
// Project: NintendoWare4Ctr

#include <nw/gfx/res/gfx_ResShader.h>
#include <nw/gfx/res/gfx_ResGraphicsFile.h>
#include <nw/gfx/gfx_ShaderUniforms.h>
#include <nw/ut/ut_Foreach.h>
#include <nw/ut/ut_MoveArray.h>
#include <nw/gfx/gfx_ShaderBinaryInfo.h>

namespace nw {
namespace gfx {
namespace res {

typedef Result (*SetupFunc)(os::IAllocator* allocator, ResShader resShader);

static Result ResBinaryShader_Setup(os::IAllocator* allocator, ResShader resShader);
static Result ResReferenceShader_Setup(os::IAllocator* allocator, ResShader resShader);

static SetupFunc s_ShaderSetupTable[] = 
{
    ResBinaryShader_Setup,
    ResReferenceShader_Setup
};

ResBinaryShader ResShader::Dereference()
{
    NW_ASSERT(this->IsValid());
    
    switch (this->ref().typeInfo)
    {
    case ResBinaryShader::TYPE_INFO:
        {
            return ResStaticCast<ResBinaryShader>(*this);
        }
        
    case ResReferenceShader::TYPE_INFO:
        {
            ResReferenceShader resRefShader = ResStaticCast<ResReferenceShader>(*this);
            
            NW_ASSERT(resRefShader.GetTargetShader().IsValid());
            return resRefShader.GetTargetShader().Dereference();
        }
        
    default:
        {
            return ResBinaryShader( NULL );
        }
    }
}

const ResBinaryShader ResShader::Dereference() const
{
    NW_ASSERT(this->IsValid());
    
    switch (this->ref().typeInfo)
    {
    case ResBinaryShader::TYPE_INFO:
        {
            return ResStaticCast<ResBinaryShader>(*this);
        }
        
    case ResReferenceShader::TYPE_INFO:
        {
            ResReferenceShader resRefShader = ResStaticCast<ResReferenceShader>(*this);
            
            NW_ASSERT(resRefShader.GetTargetShader().IsValid());
            return resRefShader.GetTargetShader().Dereference();
        }
        
    default:
        {
            return ResBinaryShader( NULL );
        }
    }
}

Result ResShader::Setup(os::IAllocator* allocator, ResGraphicsFile graphicsFile)
{
    NW_UNUSED_VARIABLE(graphicsFile);

    NW_ASSERT(internal::ResCheckRevision(*this));
    
    Result result = RESOURCE_RESULT_OK;
    switch (this->ref().typeInfo)
    {
    case ResBinaryShader::TYPE_INFO:
        {
            result |= s_ShaderSetupTable[0](allocator, *this);
        }
        break;
    case ResReferenceShader::TYPE_INFO:
        {
            result |= s_ShaderSetupTable[1](allocator, *this);
        }
        break;
    default:
        {
        }
    }
    return result;
}

Result ResShader::Setup(os::IAllocator* allocator)
{
    return this->Setup(allocator, ResGraphicsFile(NULL));
}

static Result ResBinaryShader_Setup(os::IAllocator* allocator, ResShader resShader)
{
    Result result = RESOURCE_RESULT_OK;

    ResBinaryShader resBinaryShader = ResDynamicCast<ResBinaryShader>(resShader);

    NW_ASSERT(resBinaryShader.GetShaderObjectsCount() != 0);

    u32 shaderObjects = resBinaryShader.GetShaderObjects(0);

    if (shaderObjects != NULL)
    {
        return result;
    }
    
    if (allocator == NULL) 
    { 
        allocator = CommandCacheManager::GetAllocator(); 
    }
    resBinaryShader.ref().m_CommandAllocator = allocator;

    for (int i = 0; i < resBinaryShader.GetShaderKindsCount(); i++)
    {
        u32 shader = reinterpret_cast<u32>(resShader.ptr());

        resBinaryShader.SetShaderObjects(i, shader);
    }

    void* binaryAnalyzerBuffer = allocator->Alloc(sizeof(ShaderBinaryInfo), 4);
    
    resBinaryShader.ref().m_ShaderBinaryInfo = new(binaryAnalyzerBuffer) ShaderBinaryInfo(resBinaryShader.GetBinaryData());
    
    ShaderBinaryInfo* shaderInfo = resBinaryShader.GetShaderBinaryInfo();
    shaderInfo->AnalyzeBinary();
    
    s32 commandSize = shaderInfo->GetCommonCommandSize();
    
    u32* buffer = reinterpret_cast<u32*>(allocator->Alloc(commandSize, 4));

    s32 writtenSize = shaderInfo->BuildCommonCommand(buffer, static_cast<u32>(commandSize));
    NW_ASSERT(writtenSize == commandSize);
    
    resBinaryShader.ref().m_CommandCache     = buffer;
    resBinaryShader.ref().m_CommandCacheSize = writtenSize;

    for (int i = 0; i < resBinaryShader.GetDescriptionsCount(); i++)
    {
        ResShaderProgramDescription description = resBinaryShader.GetDescriptions(i);
        GLuint vertexShader = 0;
        s32 vertexIndex = resBinaryShader.GetDescriptions(i).GetVertexShaderIndex();
        NW_MINMAXLT_ASSERT(vertexIndex, 0, resBinaryShader.GetShaderObjectsCount());
        vertexShader = resBinaryShader.GetShaderObjects(vertexIndex);
        
        GLuint geometryShader = 0;
        s32 geometryIndex = resBinaryShader.GetDescriptions(i).GetGeometryShaderIndex();

        if (0 <= geometryIndex && geometryIndex < resBinaryShader.GetShaderObjectsCount())
        {
            geometryShader = resBinaryShader.GetShaderObjects(geometryIndex);
        }

        resBinaryShader.GetDescriptions(i).SetVertexShaderObject(vertexShader);
        resBinaryShader.GetDescriptions(i).SetGeometryShaderObject(geometryShader);
        resBinaryShader.GetDescriptions(i).Setup(allocator);
    }
    
    return result;
}

static Result ResReferenceShader_Setup(os::IAllocator* allocator, ResShader resShader)
{
    Result result = RESOURCE_RESULT_OK;
    ResReferenceShader resRefShader = ResDynamicCast<ResReferenceShader>( resShader );
    
    NW_ASSERT(resRefShader.IsValid());
    NW_ASSERT(resRefShader.GetTargetShader().IsValid());
    
    ResBinaryShader binaryShader = ResDynamicCast<ResBinaryShader>(resRefShader.GetTargetShader());
    
    if (binaryShader.IsValid())
    {
        ResBinaryShader_Setup(allocator, binaryShader);
    }
    return result;
}

void ResShader::Cleanup()
{
    ResBinaryShader resBinaryShader = ResDynamicCast<ResBinaryShader>(*this);
    
    if (resBinaryShader.IsValid())
    {
        os::IAllocator* allocator = resBinaryShader.ref().m_CommandAllocator;
        
        if (resBinaryShader.ref().m_CommandCache != NULL)
        {
            resBinaryShader.ref().m_CommandAllocator->Free(resBinaryShader.ref().m_CommandCache);
            resBinaryShader.ref().m_CommandCache = NULL;
            resBinaryShader.ref().m_CommandCacheSize = 0;
        }
        
        if (resBinaryShader.ref().m_ShaderBinaryInfo != NULL)
        {
            ShaderBinaryInfo* shaderInfo = resBinaryShader.GetShaderBinaryInfo();
            shaderInfo->~ShaderBinaryInfo();
            resBinaryShader.ref().m_CommandAllocator->Free(shaderInfo);
            
            resBinaryShader.ref().m_ShaderBinaryInfo = NULL;
        }
        
        ut::SafeCleanupAll(resBinaryShader.GetDescriptions());

        for (int i = 0; i < resBinaryShader.GetShaderObjectsCount(); i++)
        {
            u32 shaderObject = resBinaryShader.GetShaderObjects(i);
            if (shaderObject)
            {
                resBinaryShader.SetShaderObjects(i, 0);
            }
        }

        resBinaryShader.ref().m_CommandAllocator = NULL;
    }
}

Result ResShaderProgramDescription::Setup(os::IAllocator* allocator)
{
    if (allocator == NULL) 
    {
        allocator = CommandCacheManager::GetAllocator(); 
    }
    
    NW_ASSERT(allocator == ResBinaryShader(this->GetOwnerShaderData()).ref().m_CommandAllocator);
    
    NW_UNUSED_VARIABLE(allocator);
    Result result = RESOURCE_RESULT_OK;
    
    s32 symbolsCount = this->GetSymbolsCount();
    for (int i = 0; i < symbolsCount; ++i)
    {
        ResShaderSymbol resShaderSymbol = this->GetSymbols(i);
        NW_ASSERT(resShaderSymbol.IsValid());

        if (resShaderSymbol.GetLocation() == -1)
        {
            s32 regIndex = this->GetVertexUniformIndex(resShaderSymbol.GetName(), NULL);
            
            if (regIndex >= 0)
            {
                resShaderSymbol.SetGeometryUniform(false);
                resShaderSymbol.SetLocation(regIndex);
            }
            else
            {
                regIndex = this->GetGeometryUniformIndex(resShaderSymbol.GetName(), NULL);
                
                if (regIndex >= 0)
                {
                    resShaderSymbol.SetGeometryUniform(true);
                    resShaderSymbol.SetLocation(regIndex);
                }
                else
                {
                    resShaderSymbol.SetLocation(-1);
                    result |= RESOURCE_RESULT_IRRELEVANT_LOCATION_SHADER_SYMBOL;
                }
            }
        }
    }

    if (ref().m_CommandCache)
    {
        return result;
    }

    ResBinaryShader ownerShader = ResBinaryShader(this->GetOwnerShaderData());
    
    ShaderBinaryInfo* shaderInfo = ownerShader.GetShaderBinaryInfo();
    
    s32 vertexShaderIndex = this->GetVertexShaderIndex();
    s32 geometryShaderIndex = this->GetGeometryShaderIndex();
        
    s32 commandSize = shaderInfo->GetShaderProgramCommandSize(vertexShaderIndex, geometryShaderIndex);
    u32* buffer = reinterpret_cast<u32*>(allocator->Alloc(commandSize, 4));
    
    s32 writtenSize = shaderInfo->BuildShaderProgramCommand(vertexShaderIndex, geometryShaderIndex, buffer, commandSize);
    NW_ASSERT(writtenSize == commandSize);
    
    ref().m_CommandCache     = buffer;
    ref().m_CommandCacheSize = writtenSize;
    
    return result;
}

void ResShaderProgramDescription::Cleanup()
{
    s32 symbolsCount = this->GetSymbolsCount();
    for (int i = 0; i < symbolsCount; ++i)
    {
        ResShaderSymbol resShaderSymbol = this->GetSymbols(i);
        NW_ASSERT(resShaderSymbol.IsValid());

        resShaderSymbol.SetLocation(-1);
        resShaderSymbol.SetGeometryUniform(false);
    }
    
    this->ref().m_ProgramObject = 0;
    
    if (this->ref().m_CommandCache)
    {
        os::IAllocator* allocator = ResBinaryShader(this->GetOwnerShaderData()).ref().m_CommandAllocator;
        
        allocator->Free(this->ref().m_CommandCache);
        this->ref().m_CommandCache = NULL;
        this->ref().m_CommandCacheSize = 0;
    }
}

s32 ResShaderProgramDescription::GetVertexUniformIndex(const char* name, ShaderBinaryInfo::SymbolType* pSymbolType) const
{
    const ShaderBinaryInfo* shaderInfo = this->GetShaderBinaryInfo();
    NW_NULL_ASSERT(shaderInfo);
    
    ::std::pair<s32, ShaderBinaryInfo::SymbolType> uniformInfo = 
        shaderInfo->SearchUniformIndex(this->GetVertexShaderIndex(), name);
    
    if (pSymbolType)
    {
        *pSymbolType = uniformInfo.second;
    }
    
    return uniformInfo.first;
}

s32 ResShaderProgramDescription::GetGeometryUniformIndex(const char* name, ShaderBinaryInfo::SymbolType* pSymbolType) const
{
    const ShaderBinaryInfo* shaderInfo = this->GetShaderBinaryInfo();
    NW_NULL_ASSERT(shaderInfo);
    
    ::std::pair<s32, ShaderBinaryInfo::SymbolType> uniformInfo = 
        shaderInfo->SearchUniformIndex(this->GetGeometryShaderIndex(), name);
    
    if (pSymbolType)
    {
        *pSymbolType = uniformInfo.second;
    }
    
    return uniformInfo.first;
}

} // namespace res
} // namespace gfx
} // namespace nw