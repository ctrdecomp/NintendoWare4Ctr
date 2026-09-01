// Filename: gfx_ResMaterial.cpp
//
// Project: NintendoWare4Ctr

#include <nw/gfx/res/gfx_ResUtil.h>
#include <nw/gfx/res/gfx_ResMaterial.h>
#include <nw/gfx/res/gfx_ResGraphicsFile.h>

namespace nw {
namespace gfx {
namespace res {

static Result SetupTextureCoordinators(ResMaterial resMaterial);

Result ResMaterial::Setup(os::IAllocator* allocator, ResGraphicsFile graphicsFile){
    NW_ASSERT(internal::ResCheckRevision(graphicsFile));
    NW_ASSERT(internal::ResCheckRevision(*this));
    
    NW_ASSERT(this->IsValid());

    Result result = RESOURCE_RESULT_OK;

    if(!ut::CheckFlag(this->GetFlags(), ResMaterialData::FLAG_HAS_BEEN_SETUP_TEXTURE)){
        result |= this->SetupTextures(allocator, *this, graphicsFile);
    }

    if (!ut::CheckFlag(this->GetFlags(), ResMaterialData::FLAG_HAS_BEEN_SETUP_SHADER)){
        result |= this->SetupShader(allocator, this->GetShader(), graphicsFile);
    }

    if (!ut::CheckFlag(this->GetFlags(), ResMaterialData::FLAG_HAS_BEEN_SETUP_FRAGMENTSHADER)){
        ResFragmentShader resFragmentShader = this->GetFragmentShader();
        NW_ASSERT(resFragmentShader.GetFragmentLightingTable().IsValid());
        Result resultFragmentShader = resFragmentShader.Setup(allocator, graphicsFile);
        result |= resultFragmentShader;

        if (resultFragmentShader.IsSuccess()){
            this->EnableFlags(ResMaterialData::FLAG_HAS_BEEN_SETUP_FRAGMENTSHADER);
            this->CalcFragmentLightingTableHash();
        }
    }

    if (result.IsSuccess()){
        this->EnableFlags(ResMaterialData::FLAG_HAS_BEEN_SETUP);
    }

    return result;
}

void ResMaterial::Cleanup(){
    this->DisableFlags(ResMaterialData::FLAG_HAS_BEEN_SETUP);

    this->DisableFlags(ResMaterialData::FLAG_HAS_BEEN_SETUP_FRAGMENTSHADER);
    ut::SafeCleanup(this->GetFragmentShader());

    this->DisableFlags(ResMaterialData::FLAG_HAS_BEEN_SETUP_SHADER);

    ResReferenceShader resShader = ResDynamicCast<ResReferenceShader>(this->GetShader());
    if (resShader.IsValid()){
        resShader.ref().toTargetShader.set_ptr(NULL);
    }
    
    this->DisableFlags((ResMaterialData::FLAG_HAS_BEEN_SETUP_TEXTURE));
    int textureMapperNum = this->GetTextureMappersCount();
    for (int i = 0; i < textureMapperNum; ++i){
        ResPixelBasedTextureMapper textureMapper = this->GetTextureMappers( i );
        if (textureMapper.IsValid()){
            textureMapper.Cleanup();
        }
    }

    if (this->GetProceduralTextureMapper().IsValid()){
        this->GetProceduralTextureMapper().Cleanup();
    }
}

Result ResMaterial::SetupTextures(os::IAllocator* allocator, ResMaterial resMaterial, ResGraphicsFile graphicsFile){
    Result result = RESOURCE_RESULT_OK;

    result |= SetupTextureCoordinators(resMaterial);

    s32 textureMapperNum = resMaterial.GetTextureMappersCount();
    u32 textureSamplersHash = resMaterial.GetTextureSamplersHash();

    for (int i = 0; i < textureMapperNum; ++i){
        ResPixelBasedTextureMapper resTextureMapper = resMaterial.GetTextureMappers( i );
        if (!resTextureMapper.IsValid()){
            continue;
        }

        if (!resTextureMapper.GetTexture().IsValid()){
            continue;
        }

        result |= resTextureMapper.Setup(allocator, graphicsFile);

        enum {
            SAMPLER_TYPE_SHIFT = 28,
            SAMPLER_TYPE_MASK = 0x7 << SAMPLER_TYPE_SHIFT,
            SAMPLER_TYPE_2D = 0,
            SAMPLER_TYPE_CUBE_MAP = 1,
            SAMPLER_TYPE_SHADOW_2D = 2,
            SAMPLER_TYPE_PROJECTION = 3,
            SAMPLER_TYPE_SHADOW_CUBE = 4
        };
        u32* command = resTextureMapper.GetCommandCache();
        NW_NULL_ASSERT(command);

        command[5] &= ~(SAMPLER_TYPE_MASK);

        if (i == 0){
            switch (resMaterial.GetTextureCoordinators(0).GetMappingMethod()){
            case ResTextureCoordinator::MAPPINGMETHOD_UV_COORDINATE:{
                    command[5] |= SAMPLER_TYPE_2D << SAMPLER_TYPE_SHIFT;
                }
                break;
            case ResTextureCoordinator::MAPPINGMETHOD_PROJECTION:{
                    command[5] |= SAMPLER_TYPE_PROJECTION << SAMPLER_TYPE_SHIFT;
                }
                break;
            case ResTextureCoordinator::MAPPINGMETHOD_CAMERA_CUBE_ENV:{
                    command[5] |= SAMPLER_TYPE_CUBE_MAP << SAMPLER_TYPE_SHIFT;
                }
                break;
            case ResTextureCoordinator::MAPPINGMETHOD_CAMERA_SPHERE_ENV:{
                    command[5] |= SAMPLER_TYPE_2D << SAMPLER_TYPE_SHIFT;
                }
                break;
            case ResTextureCoordinator::MAPPINGMETHOD_SHADOW:{
                    command[5] |= SAMPLER_TYPE_SHADOW_2D << SAMPLER_TYPE_SHIFT;
                }
                break;
            case ResTextureCoordinator::MAPPINGMETHOD_SHADOW_CUBE:{
                    command[5] |= SAMPLER_TYPE_SHADOW_CUBE << SAMPLER_TYPE_SHIFT;
                }
                break;
            default:
                NW_FATAL_ERROR("Unsupported mapping method.");
                break;
            }
        }

        if (result.IsSuccess()){
            if (resMaterial.GetTextureCoordinators(0).GetMappingMethod() == ResTextureCoordinator::MAPPINGMETHOD_CAMERA_CUBE_ENV && resTextureMapper.GetTexture().Dereference().GetTypeInfo() != ResCubeTexture::TYPE_INFO){
                result |= RESOURCE_RESULT_IRRELEVANT_TEXTURE_MAPPING_METHOD;
            }

            ResTexture resImageTexture = resTextureMapper.GetTexture().Dereference();
            u32 hash = 0;
            if (i == 0){
                hash = (u32)resImageTexture.ptr();
            }
            else{
                hash = ((u32)resImageTexture.ptr() << i * 8) | ((u32)resImageTexture.ptr() >> (32 - i * 8));
            }

            textureSamplersHash ^= hash;
        }
    }

    resMaterial.SetTextureMappersHash(textureSamplersHash);

    if (resMaterial.GetProceduralTextureMapper().IsValid()){
        result |= resMaterial.GetProceduralTextureMapper().Setup(allocator, graphicsFile);
    }

    if (result.IsSuccess()){
        this->EnableFlags(ResMaterialData::FLAG_HAS_BEEN_SETUP_TEXTURE);
    }

    return result;
}

static Result SetupTextureCoordinators(ResMaterial resMaterial){
    Result result = RESOURCE_RESULT_OK;

    s32 textureCoordinatorNum = resMaterial.GetTextureCoordinatorsCount();
    ResMaterial::TextureCoordinateConfig config = resMaterial.GetTextureCoordinateConfig();
    bool isProceduralTextureEnabled = resMaterial.GetProceduralTextureMapper().IsValid();
    for (int i = 0; i < textureCoordinatorNum; ++i){
        ResTextureCoordinator resTextureCoordinator = resMaterial.GetTextureCoordinators(i);
        bool isTextureMapperEnabled = resMaterial.GetTextureMappers(i).IsValid();
        bool isTextureCoordinatorEnabled = false;

        switch(i){
        case 0:{
                if (isTextureMapperEnabled){
                    isTextureCoordinatorEnabled = true;
                }
                else if (config == ResMaterial::CONFIG_0120 || config == ResMaterial::CONFIG_0110){
                    if (isProceduralTextureEnabled){
                        isTextureCoordinatorEnabled = true;
                    }
                }
            }
            break;
        case 1:{
                if (isTextureMapperEnabled){
                    isTextureCoordinatorEnabled = true;
                }
                else{
                    if (config == ResMaterial::CONFIG_0111){
                        if (resMaterial.GetTextureMappers(2).IsValid() || isProceduralTextureEnabled){
                            isTextureCoordinatorEnabled = true;
                        }
                    }
                    else if (config == ResMaterial::CONFIG_0110 || config == ResMaterial::CONFIG_0112){
                        if (resMaterial.GetTextureMappers(2).IsValid()){
                            isTextureCoordinatorEnabled = true;
                        }
                    }
                    else if (config == ResMaterial::CONFIG_0121){
                        if (isProceduralTextureEnabled){
                            isTextureCoordinatorEnabled = true;
                        }
                    }
                }
            }
            break;
        case 2:{
                if (config == ResMaterial::CONFIG_0122){
                    if (isTextureMapperEnabled || isProceduralTextureEnabled){
                        isTextureCoordinatorEnabled = true;
                    }
                }
                else if (config == ResMaterial::CONFIG_0120 || config == ResMaterial::CONFIG_0121){
                    if (isTextureMapperEnabled){
                        isTextureCoordinatorEnabled = true;
                    }
                }
                else if (config == ResMaterial::CONFIG_0112){
                    if (isProceduralTextureEnabled){
                        isTextureCoordinatorEnabled = true;
                    }
                }
            }
            break;
        default:{
                NW_FATAL_ERROR("Invalid coordinator number.");
            }
        }

        resTextureCoordinator.SetEnabled(isTextureCoordinatorEnabled);
    }
    return result;
}

Result ResMaterial::SetupShader(os::IAllocator* allocator,ResShader resShader,ResGraphicsFile graphicsFile){
    Result result = RESOURCE_RESULT_OK;
    ResShader setupShader;

    if (resShader.IsValid()){
        bool existShader = false;
        ResReferenceShader refer = ResDynamicCast<ResReferenceShader>(resShader);
        if (refer.IsValid()){
            if (refer.GetTargetShader().IsValid()){
                setupShader = refer.GetTargetShader();
                existShader = true;
            }
            else{
                ::std::pair<ResShader, bool> referenceResult;
                referenceResult = GetReferenceShaderTarget(refer, graphicsFile);
                if (referenceResult.second){
                    setupShader = referenceResult.first;
                    existShader = true;
                }
                else{
                    result |= Result::MASK_FAIL_BIT;
                    result |= RESOURCE_RESULT_NOT_FOUND_SHADER;
                }
            }
        }
        else{
            setupShader = resShader;
            existShader = true;
        }

        if (existShader){
            result |= setupShader.Setup(allocator, graphicsFile);
        }
    }

    bool isSetup = resShader.IsValid() && result.IsSuccess();
    if (isSetup){
        this->EnableFlags(ResMaterialData::FLAG_HAS_BEEN_SETUP_SHADER);

        ResBinaryShader resBinaryShader = resShader.Dereference();
        NW_MINMAXLT_ASSERT(this->GetShaderProgramDescriptionIndex(), 0, resBinaryShader.GetDescriptionsCount());

        ResShaderProgramDescription description = resBinaryShader.GetDescriptions(this->GetShaderProgramDescriptionIndex());
        this->CacheUserUniformIndex(description.GetSymbols());
    }

    return result;
}

void ResMaterial::CacheUserUniformIndex(ResShaderSymbolArray symbols){
    for (int parameterIndex = 0; parameterIndex < this->GetShaderParametersCount(); ++parameterIndex){
        ResShaderParameter parameter = this->GetShaderParameters(parameterIndex);
        if (parameter.IsValid()){
            const char* name = parameter.GetName();

            if (name == NULL) { continue; }

            for (int symbolIndex = 0; symbolIndex < symbols.size(); ++symbolIndex){
                ResShaderSymbol shaderSymbol = symbols[symbolIndex];
                if (::std::strcmp(name, shaderSymbol.GetName()) == 0){
                    parameter.SetSymbolIndex(symbolIndex);
                    break;
                }
            }
        }
    }
}

void ResMaterial::CalcFragmentLightingTableHash(){
    enum {
        D0_SHIFT = 0,
        D1_SHIFT = 4,
        RR_SHIFT = 8,
        RG_SHIFT = 12,
        RB_SHIFT = 16,
        FR_SHIFT = 20,
        MAX_BITS = 32
    };

    ResFragmentShader resFragmentShader = this->GetFragmentShader();
    ResFragmentLightingTable resFragmentLightingTable = resFragmentShader.GetFragmentLightingTable();
    NW_ASSERT(resFragmentLightingTable.IsValid());

    u32 fragmentLightingTableParametersHash = this->GetFragmentLightingTableParametersHash();

    if(resFragmentLightingTable.GetDistribution0Sampler().IsValid()){
        ResReferenceLookupTable refLut = ResDynamicCast<ResReferenceLookupTable>(resFragmentLightingTable.GetDistribution0Sampler().GetSampler());
        if (refLut.IsValid()){
            ResImageLookupTable imageLut = refLut.Dereference();
            fragmentLightingTableParametersHash ^= ((u32)imageLut.ptr()) << D0_SHIFT;
        }
    }

    if (resFragmentLightingTable.GetDistribution1Sampler().IsValid()){
        ResReferenceLookupTable refLut = ResDynamicCast<ResReferenceLookupTable>(resFragmentLightingTable.GetDistribution1Sampler().GetSampler());
        if (refLut.IsValid()){
            ResImageLookupTable imageLut = refLut.Dereference();
            fragmentLightingTableParametersHash ^= ((u32)imageLut.ptr() << D1_SHIFT) | ((u32)imageLut.ptr() >> (MAX_BITS - D1_SHIFT));
        }
    }

    if (resFragmentLightingTable.GetReflectanceRSampler().IsValid()){
        ResReferenceLookupTable refLut = ResDynamicCast<ResReferenceLookupTable>(resFragmentLightingTable.GetReflectanceRSampler().GetSampler());
        if (refLut.IsValid()){
            ResImageLookupTable imageLut = refLut.Dereference();
            fragmentLightingTableParametersHash ^= ((u32)imageLut.ptr() << RR_SHIFT) | ((u32)imageLut.ptr() >> (MAX_BITS - RR_SHIFT));
        }
    }

    if (resFragmentLightingTable.GetReflectanceGSampler().IsValid()){
        ResReferenceLookupTable refLut = ResDynamicCast<ResReferenceLookupTable>(resFragmentLightingTable.GetReflectanceGSampler().GetSampler());
        if (refLut.IsValid()){
            ResImageLookupTable imageLut = refLut.Dereference();
            fragmentLightingTableParametersHash ^= ((u32)imageLut.ptr() << RG_SHIFT) | ((u32)imageLut.ptr() >> (MAX_BITS - RG_SHIFT));
        }
    }

    if (resFragmentLightingTable.GetReflectanceBSampler().IsValid()){
        ResReferenceLookupTable refLut = ResDynamicCast<ResReferenceLookupTable>(resFragmentLightingTable.GetReflectanceBSampler().GetSampler());
        if (refLut.IsValid()){
            ResImageLookupTable imageLut = refLut.Dereference();
            fragmentLightingTableParametersHash ^= ((u32)imageLut.ptr() << RB_SHIFT) | ((u32)imageLut.ptr() >> (MAX_BITS - RB_SHIFT));
        }
    }

    if (resFragmentLightingTable.GetFresnelSampler().IsValid()){
        ResReferenceLookupTable refLut = ResDynamicCast<ResReferenceLookupTable>(resFragmentLightingTable.GetFresnelSampler().GetSampler());
        if (refLut.IsValid()){
            ResImageLookupTable imageLut = refLut.Dereference();
            fragmentLightingTableParametersHash ^= ((u32)imageLut.ptr() << FR_SHIFT) | ((u32)imageLut.ptr() >> (MAX_BITS - FR_SHIFT));
        }
    }

    this->SetFragmentLightingTableHash(fragmentLightingTableParametersHash);
}

}
}
}