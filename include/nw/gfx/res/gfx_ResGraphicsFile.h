#pragma once

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/gfx_Common.h>
#include <nw/gfx/res/gfx_ResRevision.h>
#include <nw/gfx/res/gfx_ResModel.h>
#include <nw/gfx/res/gfx_ResSceneObject.h>
#include <nw/gfx/res/gfx_ResShape.h>
#include <nw/gfx/res/gfx_ResMaterial.h>
#include <nw/gfx/res/gfx_ResParticleEmitter.h>
#include <nw/gfx/res/gfx_ResParticleModel.h>
#include <nw/gfx/res/gfx_ResCamera.h>
#include <nw/gfx/res/gfx_ResLight.h>
#include <nw/gfx/res/gfx_ResFog.h>
#include <nw/gfx/res/gfx_ResSceneEnvironmentSetting.h>
#include <nw/anim/res/anim_ResAnim.h>

namespace nw {
namespace gfx {
namespace res {

struct ResFileHeaderData
{
    nw::ut::ResU32  m_Signature;
    nw::ut::ResU16  m_ByteOrder;
    nw::ut::ResU16  m_HeaderSize;
    nw::ut::ResU32  m_Revision;
    nw::ut::ResU32  m_FileSize;
    nw::ut::ResU16  m_NumBlocks;
    u8              padding_[2];
    nw::ut::BinaryBlockHeader m_BlockHeader;
};

struct ResGraphicsFileData : public ResFileHeaderData
{
    nw::ut::ResS32 m_ModelsDicCount;
    nw::ut::Offset toModelsDic;
    nw::ut::ResS32 m_TexturesDicCount;
    nw::ut::Offset toTexturesDic;
    nw::ut::ResS32 m_LutSetsDicCount;
    nw::ut::Offset toLutSetsDic;
    nw::ut::ResS32 m_MaterialsDicCount;
    nw::ut::Offset toMaterialsDic;
    nw::ut::ResS32 m_ShadersDicCount;
    nw::ut::Offset toShadersDic;
    nw::ut::ResS32 m_CamerasDicCount;
    nw::ut::Offset toCamerasDic;
    nw::ut::ResS32 m_LightsDicCount;
    nw::ut::Offset toLightsDic;
    nw::ut::ResS32 m_FogsDicCount;
    nw::ut::Offset toFogsDic;
    nw::ut::ResS32 m_SceneEnvironmentSettingsDicCount;
    nw::ut::Offset toSceneEnvironmentSettingsDic;
    nw::ut::ResS32 m_SkeletalAnimsDicCount;
    nw::ut::Offset toSkeletalAnimsDic;
    nw::ut::ResS32 m_MaterialAnimsDicCount;
    nw::ut::Offset toMaterialAnimsDic;
    nw::ut::ResS32 m_VisibilityAnimsDicCount;
    nw::ut::Offset toVisibilityAnimsDic;
    nw::ut::ResS32 m_CameraAnimsDicCount;
    nw::ut::Offset toCameraAnimsDic;
    nw::ut::ResS32 m_LightAnimsDicCount;
    nw::ut::Offset toLightAnimsDic;
    nw::ut::ResS32 m_FogAnimsDicCount;
    nw::ut::Offset toFogAnimsDic;
    nw::ut::ResS32 m_EmittersDicCount;
    nw::ut::Offset toEmittersDic;
};

class ResFileHeader : public nw::ut::ResCommon<ResFileHeaderData>
{
public:
    NW_RES_CTOR( ResFileHeader )
    

    u32 GetSignature() const { return static_cast<u32>(ref().m_Signature); }
    void SetSignature(u32 value) { ref().m_Signature = value; }

    u16 GetByteOrder() const { return static_cast<u16>(ref().m_ByteOrder); }
    void SetByteOrder(u16 value) { ref().m_ByteOrder = value; }

    u16 GetHeaderSize() const { return static_cast<u16>(ref().m_HeaderSize); }
    void SetHeaderSize(u16 value) { ref().m_HeaderSize = value; }

    u32 GetRevision() const { return static_cast<u32>(ref().m_Revision); }
    void SetRevision(u32 value) { ref().m_Revision = value; }

    u32 GetFileSize() const { return static_cast<u32>(ref().m_FileSize); }
    void SetFileSize(u32 value) { ref().m_FileSize = value; }

    u16 GetNumBlocks() const { return static_cast<u16>(ref().m_NumBlocks); }
    void SetNumBlocks(u16 value) { ref().m_NumBlocks = value; }

    const nw::ut::BinaryFileHeader& GetFileHeader() const {return *reinterpret_cast<const nw::ut::BinaryFileHeader*>( this->ptr() );}
    const nw::ut::BinaryBlockHeader& GetFirstBlockHeader() const {return ref().m_BlockHeader;}

    void* GetImageBlockData() {return const_cast<void*>(this->GetImageBlockDataImpl());}
    const void* GetImageBlockData() const {return this->GetImageBlockDataImpl();}

    s32 GetImageBlockDataSize() const
    {
        NW_ASSERT(IsValid());
        
        const nw::ut::BinaryFileHeader& header = *reinterpret_cast<const nw::ut::BinaryFileHeader*>( this->ptr() );
        const nw::ut::BinaryBlockHeader* block = &ref().m_BlockHeader;
        
        for (int i = 0; i < header.dataBlocks; ++i)
        {
            if (block->kind == nw::ut::ReverseEndian('IMAG'))
            {
                return block->size - sizeof(nw::ut::BinaryBlockHeader);
            }
            
            block = nw::ut::GetNextBinaryBlockHeader(&header, block);
        }
        
        return 0;
    }

private:
    const void* GetImageBlockDataImpl() const
    {
        NW_ASSERT(IsValid());
        
        const nw::ut::BinaryFileHeader& header = *reinterpret_cast<const nw::ut::BinaryFileHeader*>( this->ptr() );
        const nw::ut::BinaryBlockHeader* block = &ref().m_BlockHeader;
        
        for (int i = 0; i < header.dataBlocks; ++i)
        {
            if (block->kind == nw::ut::ReverseEndian('IMAG'))
            {
                return nw::ut::AddOffsetToPtr(block, sizeof(nw::ut::BinaryBlockHeader));
            }
            
            block = nw::ut::GetNextBinaryBlockHeader( &header, block );
        }
        
        return NULL;
    }

};

class ResGraphicsFile : public ResFileHeader
{
public:
    enum
    {
        BINARY_REVISION = REVISION_RES_GRAPHICS_FILE
    };
    
    NW_RES_CTOR_INHERIT( ResGraphicsFile, ResFileHeader )



    s32 GetModelsCount() const
        { return ref().m_ModelsDicCount; }
    ResModel GetModels(int idx)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toModelsDic.to_ptr());
            return ResModel(dic[idx]);
        }
    ResModel GetModels(const char* key)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toModelsDic.to_ptr());
            return ResModel(dic[key]);
        }
    const ResModel GetModels(int idx) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toModelsDic.to_ptr());
            return ResModel(dic[idx]);
        }
    const ResModel GetModels(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toModelsDic.to_ptr());
            return ResModel(dic[key]);
        }
    s32 GetModelsIndex(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toModelsDic.to_ptr());
            return dic.GetIndex(key);
        }
    nw::ut::internal::ResArray<ResModel, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetModels()
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toModelsDic.to_ptr());
            return nw::ut::internal::ResArray<ResModel, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetModelsCount());
        }
    const nw::ut::internal::ResArray<const ResModel, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetModels() const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toModelsDic.to_ptr());
            return nw::ut::internal::ResArray<const ResModel, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetModelsCount());
        }

    s32 GetTexturesCount() const
        { return ref().m_TexturesDicCount; }
    ResTexture GetTextures(int idx)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toTexturesDic.to_ptr());
            return ResTexture(dic[idx]);
        }
    ResTexture GetTextures(const char* key)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toTexturesDic.to_ptr());
            return ResTexture(dic[key]);
        }
    const ResTexture GetTextures(int idx) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toTexturesDic.to_ptr());
            return ResTexture(dic[idx]);
        }
    const ResTexture GetTextures(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toTexturesDic.to_ptr());
            return ResTexture(dic[key]);
        }
    s32 GetTexturesIndex(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toTexturesDic.to_ptr());
            return dic.GetIndex(key);
        }
    nw::ut::internal::ResArray<ResTexture, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetTextures()
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toTexturesDic.to_ptr());
            return nw::ut::internal::ResArray<ResTexture, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetTexturesCount());
        }
    const nw::ut::internal::ResArray<const ResTexture, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetTextures() const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toTexturesDic.to_ptr());
            return nw::ut::internal::ResArray<const ResTexture, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetTexturesCount());
        }

    s32 GetLutSetsCount() const
        { return ref().m_LutSetsDicCount; }
    ResLookupTableSet GetLutSets(int idx)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toLutSetsDic.to_ptr());
            return ResLookupTableSet(dic[idx]);
        }
    ResLookupTableSet GetLutSets(const char* key)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toLutSetsDic.to_ptr());
            return ResLookupTableSet(dic[key]);
        }
    const ResLookupTableSet GetLutSets(int idx) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toLutSetsDic.to_ptr());
            return ResLookupTableSet(dic[idx]);
        }
    const ResLookupTableSet GetLutSets(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toLutSetsDic.to_ptr());
            return ResLookupTableSet(dic[key]);
        }
    s32 GetLutSetsIndex(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toLutSetsDic.to_ptr());
            return dic.GetIndex(key);
        }
    nw::ut::internal::ResArray<ResLookupTableSet, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetLutSets()
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toLutSetsDic.to_ptr());
            return nw::ut::internal::ResArray<ResLookupTableSet, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetLutSetsCount());
        }
    const nw::ut::internal::ResArray<const ResLookupTableSet, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetLutSets() const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toLutSetsDic.to_ptr());
            return nw::ut::internal::ResArray<const ResLookupTableSet, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetLutSetsCount());
        }

    s32 GetShadersCount() const
        { return ref().m_ShadersDicCount; }
    ResShader GetShaders(int idx)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toShadersDic.to_ptr());
            return ResShader(dic[idx]);
        }
    ResShader GetShaders(const char* key)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toShadersDic.to_ptr());
            return ResShader(dic[key]);
        }
    const ResShader GetShaders(int idx) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toShadersDic.to_ptr());
            return ResShader(dic[idx]);
        }
    const ResShader GetShaders(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toShadersDic.to_ptr());
            return ResShader(dic[key]);
        }
    s32 GetShadersIndex(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toShadersDic.to_ptr());
            return dic.GetIndex(key);
        }
    nw::ut::internal::ResArray<ResShader, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetShaders()
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toShadersDic.to_ptr());
            return nw::ut::internal::ResArray<ResShader, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetShadersCount());
        }
    const nw::ut::internal::ResArray<const ResShader, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetShaders() const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toShadersDic.to_ptr());
            return nw::ut::internal::ResArray<const ResShader, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetShadersCount());
        }

    s32 GetCamerasCount() const
        { return ref().m_CamerasDicCount; }
    ResCamera GetCameras(int idx)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toCamerasDic.to_ptr());
            return ResCamera(dic[idx]);
        }
    ResCamera GetCameras(const char* key)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toCamerasDic.to_ptr());
            return ResCamera(dic[key]);
        }
    const ResCamera GetCameras(int idx) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toCamerasDic.to_ptr());
            return ResCamera(dic[idx]);
        }
    const ResCamera GetCameras(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toCamerasDic.to_ptr());
            return ResCamera(dic[key]);
        }
    s32 GetCamerasIndex(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toCamerasDic.to_ptr());
            return dic.GetIndex(key);
        }
    nw::ut::internal::ResArray<ResCamera, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetCameras()
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toCamerasDic.to_ptr());
            return nw::ut::internal::ResArray<ResCamera, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetCamerasCount());
        }
    const nw::ut::internal::ResArray<const ResCamera, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetCameras() const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toCamerasDic.to_ptr());
            return nw::ut::internal::ResArray<const ResCamera, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetCamerasCount());
        }

    s32 GetLightsCount() const
        { return ref().m_LightsDicCount; }
    ResLight GetLights(int idx)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toLightsDic.to_ptr());
            return ResLight(dic[idx]);
        }
    ResLight GetLights(const char* key)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toLightsDic.to_ptr());
            return ResLight(dic[key]);
        }
    const ResLight GetLights(int idx) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toLightsDic.to_ptr());
            return ResLight(dic[idx]);
        }
    const ResLight GetLights(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toLightsDic.to_ptr());
            return ResLight(dic[key]);
        }
    s32 GetLightsIndex(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toLightsDic.to_ptr());
            return dic.GetIndex(key);
        }
    nw::ut::internal::ResArray<ResLight, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetLights()
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toLightsDic.to_ptr());
            return nw::ut::internal::ResArray<ResLight, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetLightsCount());
        }
    const nw::ut::internal::ResArray<const ResLight, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetLights() const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toLightsDic.to_ptr());
            return nw::ut::internal::ResArray<const ResLight, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetLightsCount());
        }

    s32 GetFogsCount() const
        { return ref().m_FogsDicCount; }
    ResFog GetFogs(int idx)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toFogsDic.to_ptr());
            return ResFog(dic[idx]);
        }
    ResFog GetFogs(const char* key)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toFogsDic.to_ptr());
            return ResFog(dic[key]);
        }
    const ResFog GetFogs(int idx) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toFogsDic.to_ptr());
            return ResFog(dic[idx]);
        }
    const ResFog GetFogs(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toFogsDic.to_ptr());
            return ResFog(dic[key]);
        }
    s32 GetFogsIndex(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toFogsDic.to_ptr());
            return dic.GetIndex(key);
        }
    nw::ut::internal::ResArray<ResFog, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetFogs()
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toFogsDic.to_ptr());
            return nw::ut::internal::ResArray<ResFog, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetFogsCount());
        }
    const nw::ut::internal::ResArray<const ResFog, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetFogs() const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toFogsDic.to_ptr());
            return nw::ut::internal::ResArray<const ResFog, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetFogsCount());
        }

    s32 GetSceneEnvironmentSettingsCount() const
        { return ref().m_SceneEnvironmentSettingsDicCount; }
    ResSceneEnvironmentSetting GetSceneEnvironmentSettings(int idx)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toSceneEnvironmentSettingsDic.to_ptr());
            return ResSceneEnvironmentSetting(dic[idx]);
        }
    ResSceneEnvironmentSetting GetSceneEnvironmentSettings(const char* key)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toSceneEnvironmentSettingsDic.to_ptr());
            return ResSceneEnvironmentSetting(dic[key]);
        }
    const ResSceneEnvironmentSetting GetSceneEnvironmentSettings(int idx) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toSceneEnvironmentSettingsDic.to_ptr());
            return ResSceneEnvironmentSetting(dic[idx]);
        }
    const ResSceneEnvironmentSetting GetSceneEnvironmentSettings(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toSceneEnvironmentSettingsDic.to_ptr());
            return ResSceneEnvironmentSetting(dic[key]);
        }
    s32 GetSceneEnvironmentSettingsIndex(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toSceneEnvironmentSettingsDic.to_ptr());
            return dic.GetIndex(key);
        }
    nw::ut::internal::ResArray<ResSceneEnvironmentSetting, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetSceneEnvironmentSettings()
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toSceneEnvironmentSettingsDic.to_ptr());
            return nw::ut::internal::ResArray<ResSceneEnvironmentSetting, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetSceneEnvironmentSettingsCount());
        }
    const nw::ut::internal::ResArray<const ResSceneEnvironmentSetting, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetSceneEnvironmentSettings() const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toSceneEnvironmentSettingsDic.to_ptr());
            return nw::ut::internal::ResArray<const ResSceneEnvironmentSetting, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetSceneEnvironmentSettingsCount());
        }

    s32 GetSkeletalAnimsCount() const
        { return ref().m_SkeletalAnimsDicCount; }
    anim::res::ResAnim GetSkeletalAnims(int idx)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toSkeletalAnimsDic.to_ptr());
            return anim::res::ResAnim(dic[idx]);
        }
    anim::res::ResAnim GetSkeletalAnims(const char* key)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toSkeletalAnimsDic.to_ptr());
            return anim::res::ResAnim(dic[key]);
        }
    const anim::res::ResAnim GetSkeletalAnims(int idx) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toSkeletalAnimsDic.to_ptr());
            return anim::res::ResAnim(dic[idx]);
        }
    const anim::res::ResAnim GetSkeletalAnims(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toSkeletalAnimsDic.to_ptr());
            return anim::res::ResAnim(dic[key]);
        }
    s32 GetSkeletalAnimsIndex(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toSkeletalAnimsDic.to_ptr());
            return dic.GetIndex(key);
        }
    nw::ut::internal::ResArray<anim::res::ResAnim, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetSkeletalAnims()
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toSkeletalAnimsDic.to_ptr());
            return nw::ut::internal::ResArray<anim::res::ResAnim, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetSkeletalAnimsCount());
        }
    const nw::ut::internal::ResArray<const anim::res::ResAnim, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetSkeletalAnims() const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toSkeletalAnimsDic.to_ptr());
            return nw::ut::internal::ResArray<const anim::res::ResAnim, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetSkeletalAnimsCount());
        }

    s32 GetMaterialAnimsCount() const
        { return ref().m_MaterialAnimsDicCount; }
    anim::res::ResAnim GetMaterialAnims(int idx)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toMaterialAnimsDic.to_ptr());
            return anim::res::ResAnim(dic[idx]);
        }
    anim::res::ResAnim GetMaterialAnims(const char* key)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toMaterialAnimsDic.to_ptr());
            return anim::res::ResAnim(dic[key]);
        }
    const anim::res::ResAnim GetMaterialAnims(int idx) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toMaterialAnimsDic.to_ptr());
            return anim::res::ResAnim(dic[idx]);
        }
    const anim::res::ResAnim GetMaterialAnims(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toMaterialAnimsDic.to_ptr());
            return anim::res::ResAnim(dic[key]);
        }
    s32 GetMaterialAnimsIndex(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toMaterialAnimsDic.to_ptr());
            return dic.GetIndex(key);
        }
    nw::ut::internal::ResArray<anim::res::ResAnim, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetMaterialAnims()
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toMaterialAnimsDic.to_ptr());
            return nw::ut::internal::ResArray<anim::res::ResAnim, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetMaterialAnimsCount());
        }
    const nw::ut::internal::ResArray<const anim::res::ResAnim, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetMaterialAnims() const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toMaterialAnimsDic.to_ptr());
            return nw::ut::internal::ResArray<const anim::res::ResAnim, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetMaterialAnimsCount());
        }

    s32 GetVisibilityAnimsCount() const
        { return ref().m_VisibilityAnimsDicCount; }
    anim::res::ResAnim GetVisibilityAnims(int idx)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toVisibilityAnimsDic.to_ptr());
            return anim::res::ResAnim(dic[idx]);
        }
    anim::res::ResAnim GetVisibilityAnims(const char* key)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toVisibilityAnimsDic.to_ptr());
            return anim::res::ResAnim(dic[key]);
        }
    const anim::res::ResAnim GetVisibilityAnims(int idx) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toVisibilityAnimsDic.to_ptr());
            return anim::res::ResAnim(dic[idx]);
        }
    const anim::res::ResAnim GetVisibilityAnims(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toVisibilityAnimsDic.to_ptr());
            return anim::res::ResAnim(dic[key]);
        }
    s32 GetVisibilityAnimsIndex(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toVisibilityAnimsDic.to_ptr());
            return dic.GetIndex(key);
        }
    nw::ut::internal::ResArray<anim::res::ResAnim, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetVisibilityAnims()
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toVisibilityAnimsDic.to_ptr());
            return nw::ut::internal::ResArray<anim::res::ResAnim, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetVisibilityAnimsCount());
        }
    const nw::ut::internal::ResArray<const anim::res::ResAnim, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetVisibilityAnims() const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toVisibilityAnimsDic.to_ptr());
            return nw::ut::internal::ResArray<const anim::res::ResAnim, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetVisibilityAnimsCount());
        }

    s32 GetCameraAnimsCount() const
        { return ref().m_CameraAnimsDicCount; }
    anim::res::ResCameraAnim GetCameraAnims(int idx)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toCameraAnimsDic.to_ptr());
            return anim::res::ResCameraAnim(dic[idx]);
        }
    anim::res::ResCameraAnim GetCameraAnims(const char* key)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toCameraAnimsDic.to_ptr());
            return anim::res::ResCameraAnim(dic[key]);
        }
    const anim::res::ResCameraAnim GetCameraAnims(int idx) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toCameraAnimsDic.to_ptr());
            return anim::res::ResCameraAnim(dic[idx]);
        }
    const anim::res::ResCameraAnim GetCameraAnims(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toCameraAnimsDic.to_ptr());
            return anim::res::ResCameraAnim(dic[key]);
        }
    s32 GetCameraAnimsIndex(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toCameraAnimsDic.to_ptr());
            return dic.GetIndex(key);
        }
    nw::ut::internal::ResArray<anim::res::ResCameraAnim, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetCameraAnims()
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toCameraAnimsDic.to_ptr());
            return nw::ut::internal::ResArray<anim::res::ResCameraAnim, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetCameraAnimsCount());
        }
    const nw::ut::internal::ResArray<const anim::res::ResCameraAnim, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetCameraAnims() const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toCameraAnimsDic.to_ptr());
            return nw::ut::internal::ResArray<const anim::res::ResCameraAnim, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetCameraAnimsCount());
        }

    s32 GetLightAnimsCount() const
        { return ref().m_LightAnimsDicCount; }
    anim::res::ResLightAnim GetLightAnims(int idx)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toLightAnimsDic.to_ptr());
            return anim::res::ResLightAnim(dic[idx]);
        }
    anim::res::ResLightAnim GetLightAnims(const char* key)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toLightAnimsDic.to_ptr());
            return anim::res::ResLightAnim(dic[key]);
        }
    const anim::res::ResLightAnim GetLightAnims(int idx) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toLightAnimsDic.to_ptr());
            return anim::res::ResLightAnim(dic[idx]);
        }
    const anim::res::ResLightAnim GetLightAnims(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toLightAnimsDic.to_ptr());
            return anim::res::ResLightAnim(dic[key]);
        }
    s32 GetLightAnimsIndex(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toLightAnimsDic.to_ptr());
            return dic.GetIndex(key);
        }
    nw::ut::internal::ResArray<anim::res::ResLightAnim, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetLightAnims()
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toLightAnimsDic.to_ptr());
            return nw::ut::internal::ResArray<anim::res::ResLightAnim, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetLightAnimsCount());
        }
    const nw::ut::internal::ResArray<const anim::res::ResLightAnim, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetLightAnims() const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toLightAnimsDic.to_ptr());
            return nw::ut::internal::ResArray<const anim::res::ResLightAnim, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetLightAnimsCount());
        }

    s32 GetFogAnimsCount() const
        { return ref().m_FogAnimsDicCount; }
    anim::res::ResAnim GetFogAnims(int idx)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toFogAnimsDic.to_ptr());
            return anim::res::ResAnim(dic[idx]);
        }
    anim::res::ResAnim GetFogAnims(const char* key)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toFogAnimsDic.to_ptr());
            return anim::res::ResAnim(dic[key]);
        }
    const anim::res::ResAnim GetFogAnims(int idx) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toFogAnimsDic.to_ptr());
            return anim::res::ResAnim(dic[idx]);
        }
    const anim::res::ResAnim GetFogAnims(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toFogAnimsDic.to_ptr());
            return anim::res::ResAnim(dic[key]);
        }
    s32 GetFogAnimsIndex(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toFogAnimsDic.to_ptr());
            return dic.GetIndex(key);
        }
    nw::ut::internal::ResArray<anim::res::ResAnim, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetFogAnims()
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toFogAnimsDic.to_ptr());
            return nw::ut::internal::ResArray<anim::res::ResAnim, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetFogAnimsCount());
        }
    const nw::ut::internal::ResArray<const anim::res::ResAnim, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetFogAnims() const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toFogAnimsDic.to_ptr());
            return nw::ut::internal::ResArray<const anim::res::ResAnim, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetFogAnimsCount());
        }

    s32 GetEmittersCount() const
        { return ref().m_EmittersDicCount; }
    ResParticleEmitter GetEmitters(int idx)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toEmittersDic.to_ptr());
            return ResParticleEmitter(dic[idx]);
        }
    ResParticleEmitter GetEmitters(const char* key)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toEmittersDic.to_ptr());
            return ResParticleEmitter(dic[key]);
        }
    const ResParticleEmitter GetEmitters(int idx) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toEmittersDic.to_ptr());
            return ResParticleEmitter(dic[idx]);
        }
    const ResParticleEmitter GetEmitters(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toEmittersDic.to_ptr());
            return ResParticleEmitter(dic[key]);
        }
    s32 GetEmittersIndex(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toEmittersDic.to_ptr());
            return dic.GetIndex(key);
        }
    nw::ut::internal::ResArray<ResParticleEmitter, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetEmitters()
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toEmittersDic.to_ptr());
            return nw::ut::internal::ResArray<ResParticleEmitter, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetEmittersCount());
        }
    const nw::ut::internal::ResArray<const ResParticleEmitter, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetEmitters() const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toEmittersDic.to_ptr());
            return nw::ut::internal::ResArray<const ResParticleEmitter, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetEmittersCount());
        }
    
    Result Setup(nw::os::IAllocator* allocator, ResGraphicsFile graphicsFile);

    Result Setup(nw::os::IAllocator* allocator) { return this->Setup( allocator, *this ); }

    void Cleanup();

    template<typename TFunction>
    void ForeachModel(TFunction function)
    {
        ResModelArray::iterator end = this->GetModels().end();
        for (ResModelArray::iterator model = this->GetModels().begin();
            model != end; ++model)
            {
            function(*model);
        }
    }

    template<typename TFunction>
    void ForeachModel(TFunction* function)
    {
        NW_NULL_ASSERT(function);
        this->ForeachModel<TFunction&>(*function);
    }

    template<typename TFunction>
    void ForeachMaterial(TFunction function)
    {
        ResModelArray::iterator end = this->GetModels().end();
        for (ResModelArray::iterator model = this->GetModels().begin();
            model != end; ++model)
            {
            ResMaterialArray::iterator materialEnd = (*model).GetMaterials().end();
            for (ResMaterialArray::iterator material = (*model).GetMaterials().begin();
                material != materialEnd;
                ++material)
                {
                function(*material);
            }
        }
    }

    template<typename TFunction>
    void ForeachMaterial(TFunction* function)
    {
        NW_NULL_ASSERT(function);
        this->ForeachMaterial<TFunction&>(*function);
    }

    template<typename TFunction>
    void ForeachModelMaterial(TFunction function)
    {
        ResModelArray::iterator end = this->GetModels().end();
        for (ResModelArray::iterator model = this->GetModels().begin();
            model != end; ++model)
            {
            ResMaterialArray::iterator materialEnd = (*model).GetMaterials().end();
            for (ResMaterialArray::iterator material = (*model).GetMaterials().begin();
                material != materialEnd;
                ++material)
                {
                function(*model, *material);
            }
        }
    }

    template<typename TFunction>
    void ForeachModelMaterial(TFunction* function)
    {
        NW_NULL_ASSERT(function);
        this->ForeachModelMaterial<TFunction&>(*function);
    }

    template<typename TFunction>
    void ForeachTexture(TFunction function)
    {
        ResTextureArray::iterator end = this->GetTextures().end();
        for (ResTextureArray::iterator texture = this->GetTextures().begin();
            texture != end; ++texture)
            {
            function(*texture);
        }
    }

    template<typename TFunction>
    void ForeachTexture(TFunction* function)
    {
        NW_NULL_ASSERT(function);
        this->ForeachTexture<TFunction&>(*function);
    }

    template<typename TFunction>
    void ForeachIndexStream(TFunction function)
    {
        ResModelArray::iterator modelsEnd = this->GetModels().end();
        for (ResModelArray::iterator model = this->GetModels().begin();
            model != modelsEnd ; ++model)
            {
            ResShapeArray::iterator shapesEnd = (*model).GetShapes().end();
            for (ResShapeArray::iterator shape = (*model).GetShapes().begin();
                shape != shapesEnd; ++shape)
                {
                ResPrimitiveSetArray::iterator primitiveSetsEnd = (*shape).GetPrimitiveSets().end();
                for (ResPrimitiveSetArray::iterator primitiveSet = (*shape).GetPrimitiveSets().begin();
                    primitiveSet != primitiveSetsEnd; ++primitiveSet)
                    {
                    ResPrimitiveArray::iterator primitivesEnd = (*primitiveSet).GetPrimitives().end();
                    for (ResPrimitiveArray::iterator primitive = (*primitiveSet).GetPrimitives().begin();
                        primitive != primitivesEnd; ++primitive)
                        {
                        ResIndexStreamArray::iterator indexStreamsEnd = (*primitive).GetIndexStreams().end();
                        for (ResIndexStreamArray::iterator indexStream = (*primitive).GetIndexStreams().begin();
                            indexStream != indexStreamsEnd; ++indexStream)
                            {
                            function(*indexStream);
                        }
                    }
                }
            }
        }
    }

    template<typename TFunction>
    void ForeachIndexStream(TFunction* function)
    {
        this->ForeachIndexStream<TFunction&>(*function);
    }

    template<typename TFunction>
    void ForeachVertexStream(TFunction function)
    {
        ResModelArray::iterator modelsEnd = this->GetModels().end();
        for (ResModelArray::iterator model = this->GetModels().begin(); model != modelsEnd ; ++model)
        {
            ResShapeArray::iterator shapesEnd = (*model).GetShapes().end();
            for (ResShapeArray::iterator shape = (*model).GetShapes().begin();
                shape != shapesEnd; ++shape)
                {
                ResSeparateDataShape resSeparateDataShape = ResDynamicCast<ResSeparateDataShape>(*shape);

                if (resSeparateDataShape.IsValid())
                {
                    ResVertexAttributeArray::iterator vertexAttributesEnd = resSeparateDataShape.GetVertexAttributes().end();
                    for (ResVertexAttributeArray::iterator vertexAttribute = resSeparateDataShape.GetVertexAttributes().begin();
                        vertexAttribute != vertexAttributesEnd; ++vertexAttribute)
                        {
                        ResVertexStreamBase resVertexStream = ResDynamicCast<ResVertexStreamBase>(*vertexAttribute);

                        if (resVertexStream.IsValid())
                        {
                            function(resVertexStream);
                        }
                    }
                }
            }
        }
    }
    
    template<typename TFunction>
    void ForeachVertexStream(TFunction* function)
    {
        NW_NULL_ASSERT(function);
        this->ForeachVertexStream<TFunction&>(*function);
    }

    template<typename TFunction>
    void ForeachUnresolvedReferenceTexture(TFunction function)
    {
        this->ForeachTexture(CheckUnresolvedTextureFunctor<TFunction>(function));
    }

    template <typename TFunction>
    void ForeachUnresolvedReferenceTexture(TFunction* function)
    {
        this->ForeachUnresolvedReferenceTexture<TFunction&>(*function);
    }

    template <typename TFunction>
    void ForeachUnresolvedReferenceLut(TFunction function)
    {
        this->ForeachMaterial(CheckUnresolvedMaterialLutFunctor<TFunction>(function));

        const int lightCount = this->GetLightsCount();
        for (int i = 0; i < lightCount; ++i)
        {
            ResFragmentLight fragmentLight = 
                ResDynamicCast<ResFragmentLight>(this->GetLights(i));

            if (fragmentLight.IsValid())
            {
                // distanceSampler
                ResReferenceLookupTable distance =
                    ResDynamicCast<ResReferenceLookupTable>(fragmentLight.GetDistanceSampler());
                if (distance.IsValid() && !distance.GetTargetLut().IsValid())
                {
                    function(distance);
                }

                // angleSampler
                ResLightingLookupTable lightingLut = fragmentLight.GetAngleSampler();
                if (lightingLut.IsValid())
                {
                    ResReferenceLookupTable angle = 
                        ResDynamicCast<ResReferenceLookupTable>(lightingLut.GetSampler());
                    if (angle.IsValid() && !angle.GetTargetLut().IsValid())
                    {
                        function(angle);
                    }
                }
            }
        }
    }

    template <typename TFunction>
    void ForeachUnresolvedReferenceLut(TFunction* function)
    {
        NW_NULL_ASSERT(function);
        this->ForeachUnresolvedReferenceLut<TFunction&>(*function);
    }
    
    template <typename TFunction>
    void ForeachUnresolvedReferenceShader(TFunction function)
    {
        const int shaderCount = this->GetShadersCount();
        for (int i = 0; i < shaderCount; ++i)
        {
            ResReferenceShader refer =
                ResDynamicCast<ResReferenceShader>(this->GetShaders(i));
            if (refer.IsValid())
            {
                if (!refer.GetTargetShader().IsValid())
                {
                    function(refer);
                }
            }
        }
    }

    template <typename TFunction>
    void ForeachUnresolvedReferenceShader(TFunction* function)
    {
        NW_NULL_ASSERT(function);
        this->ForeachUnresolvedReferenceShader<TFunction&>(*function);
    }

    template <typename TFunction>
    void ForeachUnresolvedReference(TFunction function)
    {
        this->ForeachUnresolvedReferenceTexture(function);
        this->ForeachUnresolvedReferenceShader(function);
        this->ForeachUnresolvedReferenceLut(function);
    }
    
    template <typename TFunction>
    void ForeachUnresolvedReference(TFunction* function)
    {
        NW_NULL_ASSERT(function);
        this->ForeachUnresolvedReference<TFunction&>(*function);
    }

    bool IsValidRevision() const
    {
        NW_ASSERT(IsValid());
        
        int modelsCount = this->GetModelsCount();
        for (int i = 0 ; i < modelsCount ; ++i)
        {
            if (!internal::ResCheckRevision(this->GetModels(i)))
            {
                return false;
            }
        }
        
        int texturesCount = this->GetTexturesCount();
        for (int i = 0 ; i < texturesCount ; ++i)
        {
            if (!internal::ResCheckRevision(this->GetTextures(i)))
            {
                return false;
            }
        }
        
        int lutSetsCount = this->GetLutSetsCount();
        for (int i = 0 ; i < lutSetsCount ; ++i )
        {
            if (!internal::ResCheckRevision( this->GetLutSets(i)))
            {
                return false;
            }
        }
        
        int shadersCount = this->GetShadersCount();
        for (int i = 0 ; i < shadersCount ; ++i)
        {
            if (!internal::ResCheckRevision(this->GetShaders(i)))
            {
                return false;
            }
        }
        
        int camerasCount = this->GetCamerasCount();
        for (int i = 0 ; i < camerasCount ; ++i)
        {
            if (!internal::ResCheckRevision(this->GetCameras(i) ) )
            {
                return false;
            }
        }
        
        int lightsCount = this->GetLightsCount();
        for (int i = 0 ; i < lightsCount ; ++i )
        {
            if (!internal::ResCheckRevision( this->GetLights(i) ) )
            {
                return false;
            }
        }
        
        int fogsCount = this->GetFogsCount();
        for (int i = 0 ; i < fogsCount ; ++i)
        {
            if (!internal::ResCheckRevision(this->GetFogs(i)))
            {
                return false;
            }
        }
        
        int sceneEnvironmentSettingsCount = this->GetSceneEnvironmentSettingsCount();
        for (int i = 0 ; i < sceneEnvironmentSettingsCount ; ++i)
        {
            if (!internal::ResCheckRevision(this->GetSceneEnvironmentSettings(i)))
            {
                return false;
            }
        }
        
        int skeletalAnimsCount = this->GetSkeletalAnimsCount();
        for (int i = 0 ; i < skeletalAnimsCount ; ++i)
        {
            if (!internal::ResCheckRevision(this->GetSkeletalAnims(i)))
            {
                return false;
            }
        }
        
        int materialAnimsCount = this->GetMaterialAnimsCount();
        for (int i = 0 ; i < materialAnimsCount ; ++i)
        {
            if (!internal::ResCheckRevision(this->GetMaterialAnims(i)))
            {
                return false;
            }
        }
        
        int visibilityAnimsCount = this->GetVisibilityAnimsCount();
        for (int i = 0 ; i < visibilityAnimsCount ; ++i )
        {
            if (!internal::ResCheckRevision( this->GetVisibilityAnims(i)))
            {
                return false;
            }
        }
        
        int cameraAnimsCount = this->GetCameraAnimsCount();
        for (int i = 0 ; i < cameraAnimsCount ; ++i)
        {
            if (!internal::ResCheckRevision(this->GetCameraAnims(i)))
            {
                return false;
            }
        }
        
        int lightAnimsCount = this->GetLightAnimsCount();
        for (int i = 0 ; i < lightAnimsCount ; ++i)
        {
            if (!internal::ResCheckRevision(this->GetLightAnims(i)))
            {
                return false;
            }
        }
        
        int fogAnimsCount = this->GetFogAnimsCount();
        for (int i = 0 ; i < fogAnimsCount ; ++i)
        {
            if (!internal::ResCheckRevision(this->GetFogAnims(i)) )
            {
                return false;
            }
        }
        
        return true;
    }

private:
    template <typename TFunction>
    class CheckUnresolvedTextureFunctor
    {
    public:
        explicit CheckUnresolvedTextureFunctor(TFunction function) :
            m_Function(function) 
        {
        }

        void operator() (ResTexture texture)
        {
            if (texture.ref().typeInfo == ResReferenceTexture::TYPE_INFO)
            {
                ResReferenceTexture refer = ResStaticCast<ResReferenceTexture>(texture);
                if (!refer.GetTargetTexture().IsValid())
                {
                    m_Function(texture);
                }
            }
        }
    private:
        TFunction m_Function;
    };

    template <typename TFunction>
    class CheckUnresolvedMaterialLutFunctor
    {
    public:
        explicit CheckUnresolvedMaterialLutFunctor(TFunction function) : m_Function(function) {}
        void operator() (ResMaterial material)
        {
            NW_ASSERT(material.IsValid());

            if (material.GetFragmentShader().IsValid())
            {
                ResFragmentLightingTable table =
                   material.GetFragmentShader().GetFragmentLightingTable();
                if (table.IsValid())
                {
                    CheckReferenceLutAndExec(table.GetReflectanceRSampler());
                    CheckReferenceLutAndExec(table.GetReflectanceGSampler());
                    CheckReferenceLutAndExec(table.GetReflectanceBSampler());
                    CheckReferenceLutAndExec(table.GetDistribution0Sampler());
                    CheckReferenceLutAndExec(table.GetDistribution1Sampler());
                    CheckReferenceLutAndExec(table.GetFresnelSampler());
                }
            }
        }
    private:
        void CheckReferenceLutAndExec(ResLightingLookupTable lightingLut)
        {
            if (lightingLut.IsValid() && lightingLut.GetSampler().IsValid())
            {
                ResReferenceLookupTable refer = ResDynamicCast<ResReferenceLookupTable>(lightingLut.GetSampler());
                if (refer.IsValid() && !refer.GetTargetLut().IsValid())
                {
                    m_Function(refer);
                }
            }
        }

        TFunction m_Function;
    };
};

class TransferedVramAddressSetter
{
public:
    TransferedVramAddressSetter(void* fcramImageAddress, void* vramImageAddress) : 
        m_FcramImageAddress(fcramImageAddress),
        m_VramImageAddress(vramImageAddress) {}

    void operator()(nw::gfx::ResTexture resTexture) const
    {
        switch (resTexture.ref().typeInfo)
        {
        case nw::gfx::ResImageTexture::TYPE_INFO:{
                nw::gfx::ResImageTexture resImageTexture = nw::gfx::ResStaticCast<nw::gfx::ResImageTexture>( resTexture );

                nw::gfx::ResPixelBasedImage resImage = resImageTexture.GetImage();
                const void* locationAddress = this->GetVramLocation(resImage.GetImageData());
                resImage.SetLocationAddress(locationAddress);
            }
            break;
        case nw::gfx::ResCubeTexture::TYPE_INFO:{
                nw::gfx::ResCubeTexture resCubeTexture = nw::gfx::ResStaticCast<nw::gfx::ResCubeTexture>(resTexture);
                
                for (int face = 0; face <nw::gfx::ResCubeTexture::MAX_CUBE_FACE; ++face)
                {
                    nw::gfx::ResPixelBasedImage resImage = resCubeTexture.GetImage(nw::gfx::ResCubeTexture::CubeFace(face));
                    const void* locationAddress = this->GetVramLocation( resImage.GetImageData() );
                    resImage.SetLocationAddress( locationAddress );
                }
            }
            break;
        default:
            break;
        }
    }

    void operator()(nw::gfx::ResIndexStream resIndexStream) const
    {
        const void* locationAddress = this->GetVramLocation( resIndexStream.GetStream() );
        resIndexStream.SetLocationAddress( locationAddress );
    }

    void operator()(nw::gfx::ResVertexStreamBase resVertexStream) const
    {
        const void* locationAddress = this->GetVramLocation( resVertexStream.GetStream() );
        resVertexStream.SetLocationAddress(locationAddress);
    }
    
private:
    const void* GetVramLocation(const void* fcramLocation) const
    {
        u32 addressOffset = nw::ut::GetOffsetFromPtr(this->m_FcramImageAddress, fcramLocation);
        return nw::ut::AddOffsetToPtr(this->m_VramImageAddress, addressOffset);
    }
    
    void* m_FcramImageAddress;
    void* m_VramImageAddress;
};

class LocationFlagSetter
{
public:
    LocationFlagSetter(u32 flag) : 
        m_Flag(flag) {}

    void operator()(ResTexture resTexture) const
    {
        switch ( resTexture.ref().typeInfo)
        {
        case ResImageTexture::TYPE_INFO:{
                ResImageTexture resImageTexture = ResStaticCast<ResImageTexture>( resTexture );
                resImageTexture.SetLocationFlag(m_Flag);
            }
            break;
        case ResCubeTexture::TYPE_INFO:{
                ResCubeTexture resCubeTexture = ResStaticCast<ResCubeTexture>( resTexture );
                resCubeTexture.SetLocationFlag(m_Flag);
            }
            break;
        default:
            break;
        }
    }

    void operator()(ResIndexStream resIndexStream) const
    {
        resIndexStream.SetLocationFlag(m_Flag);
    }

    void operator()(ResVertexStreamBase resVertexStream) const
    {
        resVertexStream.SetLocationFlag(m_Flag);
    }

private:
    u32 m_Flag;
};

typedef LocationFlagSetter TextureLocationFlagSetter;
typedef LocationFlagSetter IndexStreamLocationFlagSetter;
typedef LocationFlagSetter VertexStreamLocationFlagSetter;


class ParticleMaterialFlagSetter
{
public:
    void operator()(ResModel resModel, ResMaterial resMaterial) const
    {
        NW_ASSERT(resMaterial.IsValid());

        nw::ut::ResTypeInfo resTypeInfo = resModel.GetTypeInfo();
        if (resTypeInfo == nw::gfx::ResParticleModel::TYPE_INFO)
        {
            resMaterial.SetFlags(nw::ut::EnableFlag(resMaterial.GetFlags(), nw::gfx::ResMaterialData::FLAG_PARTICLE_MATERIAL_ENABLED));
        }
    }
};

class DefaultShaderAutoSelector
{
public:
    void operator()(ResModel resModel, ResMaterial resMaterial) const
{
        const char* DEFAULT_SHADER = "DefaultShader";
        NW_ASSERT(resMaterial.IsValid());
        nw::ut::ResTypeInfo resTypeInfo = resModel.GetTypeInfo();
        if (resTypeInfo == ResModel::TYPE_INFO || resTypeInfo == ResSkeletalModel::TYPE_INFO)
        {
            ResReferenceShader referenceShader = ResDynamicCast<ResReferenceShader>(resMaterial.GetShader());

            if (referenceShader.IsValid())
            {
                if (::std::strcmp(referenceShader.GetPath(), DEFAULT_SHADER) == 0)
                {
                    s32 activeCoordinatorsCount = resMaterial.GetActiveTextureCoordinatorsCount();

                    enum
                    { 
                        TEXTURE_COUNT_0 = 3, 
                        TEXTURE_COUNT_1 = 2, 
                        TEXTURE_COUNT_2 = 1, 
                        TEXTURE_COUNT_3 = 0 
                    };
                    const int table[] ={
                        TEXTURE_COUNT_0,
                        TEXTURE_COUNT_1,
                        TEXTURE_COUNT_2,
                        TEXTURE_COUNT_3
                    };

                    resMaterial.SetShaderProgramDescriptionIndex(table[activeCoordinatorsCount]);
                }
            }
        }
    }
};

}
}
}

