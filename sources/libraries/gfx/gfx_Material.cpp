#include <nw/gfx/gfx_Material.h>
#include <nw/gfx/gfx_Model.h>
#include <nw/gfx/res/gfx_ResMaterial.h>
#include <nw/gfx/res/gfx_ResShader.h>
#include <nw/gfx/gfx_AnimObject.h>

namespace{
nw::gfx::ResMaterial GetAvailableResMaterial(nw::gfx::Material* material, nw::gfx::Model::BufferOption bufferOption)
{
    return nw::ut::CheckFlag(material->GetOwnerModel()->GetBufferOption(), bufferOption) ? material->GetBuffer() : material->GetOriginal();
}
}

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(Material, SceneObject);

Material::Material(nw::os::IAllocator* allocator, ResMaterial resMaterial, s32 bufferCount, Model* owner):
    SceneObject(allocator, resMaterial),
    m_Owner(owner),
    m_BufferCount(bufferCount)
    {
    ResBinaryShader resShader = resMaterial.GetShader().Dereference();

    NW_ASSERT(resShader.IsValid());
    m_ProgramDescription = resShader.GetDescriptions(resMaterial.GetShaderProgramDescriptionIndex());
}

Material* Material::Create(ResMaterial resource, s32 bufferCount, Model* parent, nw::os::IAllocator* allocator)
{
    NW_NULL_ASSERT(allocator);
    NW_ASSERT(resource.IsValid());

    NW_ASSERTMSG(bufferCount <= 1, "Material Buffer Count must be 0 or 1.");

    void* memory = allocator->Alloc(sizeof(Material));
    if (memory == NULL)
    {
        return NULL;
    }

    Material* material = new(memory) Material(allocator, resource, bufferCount, parent);

    Result result = material->Initialize(allocator);
    if (result.IsSuccess())
    {
        material->m_ShaderParameterResMaterial = GetAvailableResMaterial(material, Model::FLAG_BUFFER_SHADER_PARAMETER);
        material->m_ShadingParameterResMaterial = GetAvailableResMaterial(material, Model::FLAG_BUFFER_SHADING_PARAMETER);
        material->m_MaterialColorResMaterial = GetAvailableResMaterial(material, Model::FLAG_BUFFER_MATERIAL_COLOR);
        material->m_RasterizationResMaterial = GetAvailableResMaterial(material, Model::FLAG_BUFFER_RASTERIZATION);
        material->m_TextureCoordinatorResMaterial = GetAvailableResMaterial(material, Model::FLAG_BUFFER_TEXTURE_COORDINATOR);
        material->m_TextureMapperResMaterial = GetAvailableResMaterial(material, Model::FLAG_BUFFER_TEXTURE_MAPPER);
        material->m_FragmentLightingResMaterial = GetAvailableResMaterial(material, Model::FLAG_BUFFER_FRAGMENT_LIGHTING);
        material->m_FragmentLightingTableResMaterial = GetAvailableResMaterial(material, Model::FLAG_BUFFER_FRAGMENT_LIGHTING_TABLE);
        material->m_TextureCombinerResMaterial = GetAvailableResMaterial(material, Model::FLAG_BUFFER_TEXTURE_COMBINER);
        material->m_AlphaTestResMaterial = GetAvailableResMaterial(material, Model::FLAG_BUFFER_ALPHA_TEST);
        material->m_FragmentOperationResMaterial = GetAvailableResMaterial(material, Model::FLAG_BUFFER_FRAGMENT_OPERATION);
        material->m_SceneEnvironmentResMaterial = GetAvailableResMaterial(material, Model::FLAG_BUFFER_SCENE_ENVIRONMENT);
        return material;
    }
    else{
        SafeDestroy(material);
        return NULL;
    }
}

void Material::GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize, ResMaterial resMaterial, s32 bufferCount, bit32 bufferOption)
{
    nw::os::MemorySizeCalculator& size = *pSize;

    size += sizeof(Material);

    size += sizeof(ResMaterial) * bufferCount;
    if (bufferCount > 0)
    {
        nw::os::MemorySizeCalculator bufferSize(size.GetAlignment());

        bufferSize += sizeof(ResMaterialData);

        if (ut::CheckFlag(bufferOption, Model::FLAG_BUFFER_SHADER_PARAMETER))
        {
            bufferSize += sizeof(ut::Offset) * resMaterial.GetShaderParametersCount();

            for (int i = 0; i < resMaterial.GetShaderParametersCount(); ++i)
            {
                const int parameterLength = resMaterial.GetShaderParameters(i).GetParameterLength();

                bufferSize += sizeof(ResShaderParameterData) + sizeof(f32) * (parameterLength - 1);
            }
        }

        if (ut::CheckFlag(bufferOption, Model::FLAG_BUFFER_TEXTURE_MAPPER))
        {
            for (int i = 0; i < resMaterial.GetTextureMappersCount(); i++)
            {
                ResPixelBasedTextureMapper resTextureMapper = resMaterial.GetTextureMappers(i);
                if (resTextureMapper.IsValid())
                {
                    resTextureMapper.GetMemorySizeForCloneInternal(&bufferSize);
                }
            }
        }

        if (ut::CheckFlag(bufferOption, Model::FLAG_BUFFER_PROCEDURAL_TEXTURE_MAPPER))
        {
            ResProceduralTextureMapper resTextureMapper = resMaterial.GetProceduralTextureMapper();
            if (resTextureMapper.IsValid())
            {
                resTextureMapper.GetMemorySizeForCloneInternal(&bufferSize);
            }
        }

        if (ut::CheckFlagOr(bufferOption, Model::MULTI_FLAG_BUFFER_FRAGMENT_SHADER))
        {
            if (resMaterial.GetFragmentShader().IsValid())
            {
                bufferSize += sizeof(ResFragmentShaderData);

                if (ut::CheckFlag(bufferOption, Model::FLAG_BUFFER_FRAGMENT_LIGHTING_TABLE))
                {
                    ResFragmentShader resFragmentShader = resMaterial.GetFragmentShader();

                    if (resFragmentShader.GetFragmentLightingTable().IsValid())
                    {
                        ResFragmentLightingTable resFragmentLightingTable = resFragmentShader.GetFragmentLightingTable();

                        bufferSize += sizeof(ResFragmentLightingTableData);
                        if (resFragmentLightingTable.GetReflectanceRSampler().IsValid())
                        {
                            bufferSize += sizeof(ResLightingLookupTableData);
                            bufferSize += sizeof(ResReferenceLookupTableData);
                        }
                        if (resFragmentLightingTable.GetReflectanceGSampler().IsValid())
                        {
                            bufferSize += sizeof(ResLightingLookupTableData);
                            bufferSize += sizeof(ResReferenceLookupTableData);
                        }
                        if (resFragmentLightingTable.GetReflectanceBSampler().IsValid())
                        {
                            bufferSize += sizeof(ResLightingLookupTableData);
                            bufferSize += sizeof(ResReferenceLookupTableData);
                        }
                        if (resFragmentLightingTable.GetDistribution0Sampler().IsValid())
                        {
                            bufferSize += sizeof(ResLightingLookupTableData);
                            bufferSize += sizeof(ResReferenceLookupTableData);
                        }
                        if (resFragmentLightingTable.GetDistribution1Sampler().IsValid())
                        {
                            bufferSize += sizeof(ResLightingLookupTableData);
                            bufferSize += sizeof(ResReferenceLookupTableData);
                        }
                        if (resFragmentLightingTable.GetFresnelSampler().IsValid())
                        {
                            bufferSize += sizeof(ResLightingLookupTableData);
                            bufferSize += sizeof(ResReferenceLookupTableData);
                        }
                    }
                }
            }
        }

        bufferSize *= bufferCount;
        size += bufferSize.GetSizeWithPadding(size.GetAlignment());
    }
}

Material::~Material()
{
    std::for_each(m_Buffers.begin(), m_Buffers.end(), ResMaterialDestroyer(&this->GetAllocator()));
}

bool Material::CanUseBuffer(u32 objectType) const
{
    if (0 < m_BufferCount)
    {
        bit32 option = GetOwnerModel()->GetBufferOption();
        switch (objectType)
        {
        case anim::ResAnimGroupMember::OBJECT_TYPE_MATERIAL_COLOR:
            return ut::CheckFlag(option, Model::FLAG_BUFFER_MATERIAL_COLOR);
        case anim::ResAnimGroupMember::OBJECT_TYPE_TEXTURE_SAMPLER:
            return ut::CheckFlag(option, Model::FLAG_BUFFER_TEXTURE_MAPPER);
        case anim::ResAnimGroupMember::OBJECT_TYPE_TEXTURE_MAPPER:
            return ut::CheckFlag(option, Model::FLAG_BUFFER_TEXTURE_MAPPER);
        case anim::ResAnimGroupMember::OBJECT_TYPE_BLEND_OPERATION:
            return ut::CheckFlag(option, Model::FLAG_BUFFER_FRAGMENT_OPERATION);
        case anim::ResAnimGroupMember::OBJECT_TYPE_TEXTURE_COORDINATOR:
            return ut::CheckFlag(option, Model::FLAG_BUFFER_TEXTURE_COORDINATOR);
        default:
            NW_ASSERT(false);
        }
    }
    return false;
}

Result Material::CreateBuffers(nw::os::IAllocator* allocator)
{
    Result result = INITIALIZE_RESULT_OK;

    NW_NULL_ASSERT(m_Owner);
    bit32 bufferOption = m_Owner->GetBufferOption();

    if (m_BufferCount != 0)
    {
        void* memory = allocator->Alloc(sizeof(ResMaterial) * m_BufferCount);
        if (memory == NULL)
        {
            result |= Result::MASK_FAIL_BIT;
        }

        NW_ENSURE_AND_RETURN(result);

        m_Buffers = ResMaterialArray(memory, m_BufferCount, allocator);

        for (int bufferIndex = 0; bufferIndex < m_BufferCount; bufferIndex++)
        {
            NW_ENSURE_AND_RETURN(result);
            ::std::pair<ResMaterial, Result> copyResult = CopyResMaterial(allocator, bufferOption);

            if (copyResult.second.IsSuccess())
            {
                this->m_Buffers.PushBackFast(copyResult.first);
            }
            else{
                DestroyResMaterial(allocator, copyResult.first);
                result |= Result::MASK_FAIL_BIT;
            }
        }
    }

    return result;
}

::std::pair<ResMaterial, Result> Material::CopyResMaterial(nw::os::IAllocator* allocator, bit32 bufferOption)
{
    Result result = INITIALIZE_RESULT_OK;

    ResMaterial resMaterial = GetOriginal();

    void* materialMemory = allocator->Alloc(sizeof(ResMaterialData));
    if (materialMemory == NULL)
    {
        result |= Result::MASK_FAIL_BIT;
        return ::std::make_pair(ResMaterial(NULL), result);
    }

    ResMaterialData* buffer = new(materialMemory) ResMaterialData(GetOriginal().ref());

    ResMaterial copyMaterial = ResMaterial(buffer);

    buffer->toName.set_ptr(NULL);
    buffer->toShader.set_ptr(NULL);
    buffer->m_ShaderParametersTableCount = 0;
    buffer->toShaderParametersTable.set_ptr(NULL);
    buffer->toTextureMappers[0].set_ptr(NULL);
    buffer->toTextureMappers[1].set_ptr(NULL);
    buffer->toTextureMappers[2].set_ptr(NULL);
    buffer->toProceduralTextureMapper.set_ptr(NULL);
    buffer->toFragmentShader.set_ptr(NULL);

    buffer->m_AlphaTestHash = 0x0;
    buffer->m_FragmentLightingHash = 0x0;
    buffer->m_FragmentLightingTableHash = 0x0;
    buffer->m_FragmentLightingTableParametersHash = 0x0;
    buffer->m_FragmentOperationHash = 0x0;
    buffer->m_MaterialColorHash = 0x0;
    buffer->m_RasterizationHash = 0x0;
    buffer->m_ShaderParametersHash = 0x0;
    buffer->m_ShadingParameterHash = 0x0;
    buffer->m_TextureCombinersHash = 0x0;
    buffer->m_TextureCoordinatorsHash = 0x0;
    buffer->m_TextureMappersHash = 0x0;
    buffer->m_TextureSamplersHash = 0x0;

    if (ut::CheckFlag(bufferOption, Model::FLAG_BUFFER_SHADER_PARAMETER))
    {
        buffer->m_ShaderParametersTableCount = resMaterial.GetShaderParametersCount();

        ut::Offset* offsets = NULL;
        if (buffer->m_ShaderParametersTableCount != 0)
        {
            offsets = allocator->AllocAndConstruct<ut::Offset>(resMaterial.GetShaderParametersCount());

            if (offsets == NULL)
            {
                result |= Result::MASK_FAIL_BIT;
                return ::std::make_pair(copyMaterial, result);
            }

            buffer->toShaderParametersTable.set_ptr(offsets);
        }

        for (int i = 0; i < resMaterial.GetShaderParametersCount(); i++)
        {
            offsets[i].set_ptr(NULL);
        }

        for (int i = 0; i < resMaterial.GetShaderParametersCount(); i++)
        {
            ResShaderParameter resShaderParameter = resMaterial.GetShaderParameters(i);
            ResShaderParameterValue resShaderParameterValue = resShaderParameter.GetParameter();

            const int parameterLength = resShaderParameter.GetParameterLength();

            void* parameterMemory = allocator->Alloc(sizeof(ResShaderParameterData) + sizeof(f32) * (parameterLength - 1));
            if (parameterMemory == NULL)
            {
                result |= Result::MASK_FAIL_BIT;
                return ::std::make_pair(copyMaterial, result);
            }

            ResShaderParameterData* shaderParameter = static_cast<ResShaderParameterData*>(parameterMemory);
            *shaderParameter = ResShaderParameterData(resShaderParameter.ref());

            for (int j = 0; j < parameterLength; ++j)
            {
                shaderParameter->m_Parameter.m_Value[j] = resShaderParameterValue.ref().m_Value[j];
            }

            offsets[i].set_ptr(shaderParameter);
        }
    }

    if (ut::CheckFlag(bufferOption, Model::FLAG_BUFFER_TEXTURE_MAPPER))
    {
        for (int i = 0; i < resMaterial.GetTextureMappersCount(); i++)
        {
            ResPixelBasedTextureMapper resTextureMapper = resMaterial.GetTextureMappers(i);
            if (resTextureMapper.IsValid())
            {
                ResTextureMapper textureMapper = resTextureMapper.CloneDynamic(allocator);

                if (!textureMapper.IsValid())
                {
                    result |= Result::MASK_FAIL_BIT;
                    return ::std::make_pair(copyMaterial, result);
                }

                buffer->toTextureMappers[i].set_ptr(textureMapper.ptr());
            }
        }
    }

    if (ut::CheckFlag(bufferOption, Model::FLAG_BUFFER_PROCEDURAL_TEXTURE_MAPPER))
    {
        ResProceduralTextureMapper resTextureMapper = resMaterial.GetProceduralTextureMapper();
        if (resTextureMapper.IsValid())
        {
            ResTextureMapper textureMapper = resTextureMapper.CloneDynamic(allocator);

            if (!textureMapper.IsValid())
            {
                result |= Result::MASK_FAIL_BIT;
                return ::std::make_pair(copyMaterial, result);
            }

            buffer->toProceduralTextureMapper.set_ptr(textureMapper.ptr());
        }
    }

    if (ut::CheckFlagOr(bufferOption, Model::MULTI_FLAG_BUFFER_FRAGMENT_SHADER))
    {
        ResFragmentShader resFragmentShader = resMaterial.GetFragmentShader();
        if (resFragmentShader.IsValid())
        {
            void* shaderMemory = allocator->Alloc(sizeof(ResFragmentShaderData));

            if (shaderMemory == NULL)
            {
                result |= Result::MASK_FAIL_BIT;
                return ::std::make_pair(copyMaterial, result);
            }

            ResFragmentShaderData* fragmentShader = new(shaderMemory) ResFragmentShaderData(resFragmentShader.ref());

            buffer->toFragmentShader.set_ptr(fragmentShader);

            fragmentShader->toFragmentLightingTable.set_ptr(NULL);

            if (ut::CheckFlag(bufferOption, Model::FLAG_BUFFER_FRAGMENT_LIGHTING_TABLE))
            {
                ResFragmentLightingTable resFragmentLightingTable = resFragmentShader.GetFragmentLightingTable();
                if (resFragmentLightingTable.IsValid())
                {
                    ::std::pair<ResFragmentLightingTable, Result> copyResult =
                        CopyResFragmentLightingTable(allocator, resFragmentLightingTable);

                    fragmentShader->toFragmentLightingTable.set_ptr(copyResult.first.ptr());

                    if (copyResult.second.IsFailure())
                    {
                        result |= Result::MASK_FAIL_BIT;
                        return ::std::make_pair(copyMaterial, result);
                    }
                }
            }
        }
    }

    return ::std::make_pair(copyMaterial, result);
}

void Material::DestroyResMaterial(nw::os::IAllocator* allocator, ResMaterial resMaterial)
{
    NW_NULL_ASSERT(allocator);
    if (resMaterial.IsValid())
    {
        resMaterial.Cleanup();

        DestroyResFragmentShader(allocator, resMaterial.GetFragmentShader());

        if (resMaterial.GetProceduralTextureMapper().IsValid())
        {
            resMaterial.GetProceduralTextureMapper().DestroyDynamic();
        }

        for (int i = 0; i < resMaterial.GetTextureMappersCount(); i++)
        {
            if (resMaterial.GetTextureMappers(i).IsValid())
            {
                resMaterial.GetTextureMappers(i).DestroyDynamic();
            }
        }

        if (resMaterial.ref().toShaderParametersTable.to_ptr() != NULL)
        {
            for (int i = 0; i < resMaterial.GetShaderParametersCount(); i++)
            {
                DestroyResShaderParameter(allocator, resMaterial.GetShaderParameters(i));
            }
            allocator->Free(resMaterial.ref().toShaderParametersTable.to_ptr());
        }

        allocator->Free(resMaterial.ptr());
    }
}

void Material::DestroyResFragmentShader(nw::os::IAllocator* allocator, ResFragmentShader resFragmentShader)
{
    if (resFragmentShader.IsValid())
    {
        ResFragmentLightingTable resFragmentLightingTable = resFragmentShader.GetFragmentLightingTable();

        if (resFragmentLightingTable.IsValid())
        {
            if (resFragmentLightingTable.GetReflectanceRSampler().IsValid())
            {
                allocator->Free(resFragmentLightingTable.GetReflectanceRSampler().GetSampler().ptr());
                allocator->Free(resFragmentLightingTable.GetReflectanceRSampler().ptr());
            }

            if (resFragmentLightingTable.GetReflectanceGSampler().IsValid())
            {
                allocator->Free(resFragmentLightingTable.GetReflectanceGSampler().GetSampler().ptr());
                allocator->Free(resFragmentLightingTable.GetReflectanceGSampler().ptr());
            }

            if (resFragmentLightingTable.GetReflectanceBSampler().IsValid())
            {
                allocator->Free(resFragmentLightingTable.GetReflectanceBSampler().GetSampler().ptr());
                allocator->Free(resFragmentLightingTable.GetReflectanceBSampler().ptr());
            }

            if (resFragmentLightingTable.GetDistribution0Sampler().IsValid())
            {
                allocator->Free(resFragmentLightingTable.GetDistribution0Sampler().GetSampler().ptr());
                allocator->Free(resFragmentLightingTable.GetDistribution0Sampler().ptr());
            }

            if (resFragmentLightingTable.GetDistribution1Sampler().IsValid())
            {
                allocator->Free(resFragmentLightingTable.GetDistribution1Sampler().GetSampler().ptr());
                allocator->Free(resFragmentLightingTable.GetDistribution1Sampler().ptr());
            }

            if (resFragmentLightingTable.GetFresnelSampler().IsValid())
            {
                allocator->Free(resFragmentLightingTable.GetFresnelSampler().GetSampler().ptr());
                allocator->Free(resFragmentLightingTable.GetFresnelSampler().ptr());
            }

            allocator->Free(resFragmentLightingTable.ptr());
        }

        allocator->Free(resFragmentShader.ptr());
    }
}

void Material::DestroyResShaderParameter(nw::os::IAllocator* allocator, ResShaderParameter resShaderParameter)
{
    if (resShaderParameter.IsValid())
    {
        allocator->Free(resShaderParameter.ptr());
    }
}

::std::pair<ResFragmentLightingTable, Result> Material::CopyResFragmentLightingTable(nw::os::IAllocator* allocator,ResFragmentLightingTable resFragmentLightingTable)
{
    Result result = INITIALIZE_RESULT_OK;

    void* tableMemory = allocator->Alloc(sizeof(ResFragmentLightingTableData));
    if (tableMemory == NULL)
    {
        result |= Result::MASK_FAIL_BIT;
        return ::std::make_pair(ResFragmentLightingTable(NULL), result);
    }

    ResFragmentLightingTableData* fragmentLightingTable = new(tableMemory) ResFragmentLightingTableData(resFragmentLightingTable.ref());

    fragmentLightingTable->toReflectanceRSampler.set_ptr(NULL);
    fragmentLightingTable->toReflectanceGSampler.set_ptr(NULL);
    fragmentLightingTable->toReflectanceBSampler.set_ptr(NULL);
    fragmentLightingTable->toDistribution0Sampler.set_ptr(NULL);
    fragmentLightingTable->toDistribution1Sampler.set_ptr(NULL);
    fragmentLightingTable->toFresnelSampler.set_ptr(NULL);

    if (resFragmentLightingTable.GetReflectanceRSampler().IsValid() && result.IsSuccess())
    {
        ::std::pair<ResLightingLookupTable, Result> copyResult =
            CopyResLightingLookupTable(allocator, resFragmentLightingTable.GetReflectanceRSampler());

        fragmentLightingTable->toReflectanceRSampler.set_ptr(copyResult.first.ptr());

        result |= copyResult.second;
    }

    if (resFragmentLightingTable.GetReflectanceGSampler().IsValid() && result.IsSuccess())
    {
        ::std::pair<ResLightingLookupTable, Result> copyResult =
            CopyResLightingLookupTable(allocator, resFragmentLightingTable.GetReflectanceGSampler());

        fragmentLightingTable->toReflectanceGSampler.set_ptr(copyResult.first.ptr());

        result |= copyResult.second;
    }

    if (resFragmentLightingTable.GetReflectanceBSampler().IsValid() && result.IsSuccess())
    {
        ::std::pair<ResLightingLookupTable, Result> copyResult =
            CopyResLightingLookupTable(allocator, resFragmentLightingTable.GetReflectanceBSampler());

        fragmentLightingTable->toReflectanceBSampler.set_ptr(copyResult.first.ptr());

        result |= copyResult.second;
    }

    if (resFragmentLightingTable.GetDistribution0Sampler().IsValid() && result.IsSuccess())
    {
        ::std::pair<ResLightingLookupTable, Result> copyResult =
            CopyResLightingLookupTable(allocator, resFragmentLightingTable.GetDistribution0Sampler());

        fragmentLightingTable->toDistribution0Sampler.set_ptr(copyResult.first.ptr());

        result |= copyResult.second;
    }

    if (resFragmentLightingTable.GetDistribution1Sampler().IsValid() && result.IsSuccess())
    {
        ::std::pair<ResLightingLookupTable, Result> copyResult =
            CopyResLightingLookupTable(allocator, resFragmentLightingTable.GetDistribution1Sampler());

        fragmentLightingTable->toDistribution1Sampler.set_ptr(copyResult.first.ptr());

        result |= copyResult.second;
    }

    if (resFragmentLightingTable.GetFresnelSampler().IsValid() && result.IsSuccess())
    {
        ::std::pair<ResLightingLookupTable, Result> copyResult =
            CopyResLightingLookupTable(allocator, resFragmentLightingTable.GetFresnelSampler());

        fragmentLightingTable->toFresnelSampler.set_ptr(copyResult.first.ptr());

        result |= copyResult.second;
    }

    return ::std::make_pair(ResFragmentLightingTable(fragmentLightingTable), result);
}

::std::pair<ResLightingLookupTable, Result> Material::CopyResLightingLookupTable(nw::os::IAllocator* allocator, ResLightingLookupTable resLightingLookupTable)
{
    Result result = INITIALIZE_RESULT_OK;

    void* tableMemory = allocator->Alloc(sizeof(ResLightingLookupTableData));

    if (tableMemory == NULL)
    {
        result |= Result::MASK_FAIL_BIT;
        return ::std::make_pair(ResLightingLookupTable(NULL), result);
    }

    ResLightingLookupTableData* lightingLookupTable = new(tableMemory) ResLightingLookupTableData(resLightingLookupTable.ref());

    void* referenceTableMemory = allocator->Alloc(sizeof(ResReferenceLookupTableData));

    if (referenceTableMemory == NULL)
    {
        result |= Result::MASK_FAIL_BIT;
        allocator->Free(lightingLookupTable);
        return ::std::make_pair(ResLightingLookupTable(NULL), result);
    }

    ResReferenceLookupTable sampler = ut::ResStaticCast<ResReferenceLookupTable>(resLightingLookupTable.GetSampler());

    ResReferenceLookupTableData* referenceLookupTable = new(referenceTableMemory) ResReferenceLookupTableData(sampler.ref());

    referenceLookupTable->toTargetLut.set_ptr(sampler.GetTargetLut().ptr());
    referenceLookupTable->toPath.set_ptr(sampler.GetPath());
    referenceLookupTable->toTableName.set_ptr(sampler.GetTableName());

    lightingLookupTable->m_Input = resLightingLookupTable.GetInput();
    lightingLookupTable->m_Scale = resLightingLookupTable.GetScale();
    lightingLookupTable->toSampler.set_ptr(referenceLookupTable);

    return ::std::make_pair(ResLightingLookupTable(lightingLookupTable), result);
}

void* Material::GetAnimTargetObject(const anim::ResAnimGroupMember& anim, const ResMaterial resMaterial)
{
    u32 objectType = anim.GetObjectType();
    ResMaterial mat = resMaterial;

    switch (objectType)
    {
    case anim::ResAnimGroupMember::OBJECT_TYPE_MATERIAL_COLOR:{
            anim::ResMaterialColorMember member = ResStaticCast<anim::ResMaterialColorMember>(anim);
            const char* matName = member.GetMaterialName();

            member.SetResMaterialPtr(NW_ANY_TO_PTR_VALUE(mat.ptr()));
            ResMaterialColorData* ptr = mat.GetMaterialColor().ptr();
            return ptr;
        }

    case anim::ResAnimGroupMember::OBJECT_TYPE_TEXTURE_SAMPLER:{
            anim::ResTextureSamplerMember member = ResStaticCast<anim::ResTextureSamplerMember>(anim);
            const char* matName = member.GetMaterialName();
            int mapperIndex = member.GetTextureMapperIndex();

            member.SetResMaterialPtr(NW_ANY_TO_PTR_VALUE(mat.ptr()));

            ResPixelBasedTextureMapperData* ptr = mat.GetTextureMappers(mapperIndex).ptr();
            return ptr;
        }

    case anim::ResAnimGroupMember::OBJECT_TYPE_TEXTURE_MAPPER:{
            anim::ResTextureMapperMember member = ResStaticCast<anim::ResTextureMapperMember>(anim);
            const char* matName = member.GetMaterialName();
            int mapperIndex = member.GetTextureMapperIndex();

            member.SetResMaterialPtr(NW_ANY_TO_PTR_VALUE(mat.ptr()));
            ResPixelBasedTextureMapperData* ptr = mat.GetTextureMappers(mapperIndex).ptr();
            return ptr;
        }

    case anim::ResAnimGroupMember::OBJECT_TYPE_BLEND_OPERATION:{
            anim::ResBlendOperationMember member = ResStaticCast<anim::ResBlendOperationMember>(anim);
            const char* matName = member.GetMaterialName();

            member.SetResMaterialPtr(NW_ANY_TO_PTR_VALUE(mat.ptr()));
            ResBlendOperationData* ptr = mat.GetFragmentOperation().GetBlendOperation().ptr();
            return ptr;
        }

    case anim::ResAnimGroupMember::OBJECT_TYPE_TEXTURE_COORDINATOR:{
            anim::ResTextureCoordinatorMember member = ResStaticCast<anim::ResTextureCoordinatorMember>(anim);
            const char* matName = member.GetMaterialName();
            int coordinatorIndex = member.GetTextureCoordinatorIndex();

            member.SetResMaterialPtr(NW_ANY_TO_PTR_VALUE(mat.ptr()));
            ResTextureCoordinatorData* ptr = mat.GetTextureCoordinators(coordinatorIndex).ptr();
            return ptr;
        }

    default:
        NW_ASSERT(false);
        return NULL;
    }
}

Result Material::Initialize(nw::os::IAllocator* allocator)
{
    Result result = INITIALIZE_RESULT_OK;

    result |= CreateBuffers(allocator);
    NW_ENSURE_AND_RETURN(result);

    return result;
}

}
}