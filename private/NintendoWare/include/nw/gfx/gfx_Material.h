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

#ifndef NW_GFX_MATERIAL_H_
#define NW_GFX_MATERIAL_H_

#include <nw/gfx/gfx_SceneObject.h>
#include <nw/ut/ut_RuntimeTypeInfo.h>
#include <nw/ut/ut_MoveArray.h>
#include <nw/gfx/gfx_ShaderProgram.h>

namespace nw
{
namespace gfx
{

class Model;
class AnimGroup;

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------
class Material : public SceneObject
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(Material);

public:
    NW_UT_RUNTIME_TYPEINFO;

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    //
    //
    //
    //
    //
    static Material* Create(
        ResMaterial resource,
        s32 bufferCount,
        Model* parent,
        os::IAllocator* allocator);

    //
    //
    //
    //
    //
    //
    static size_t GetMemorySize(
        ResMaterial resource,
        s32 bufferCount,
        bit32 bufferOption,
        size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT)
    {
        os::MemorySizeCalculator size(alignment);

        GetMemorySizeInternal(&size, resource, bufferCount, bufferOption);

        return size.GetSizeWithPadding(alignment);
    }


    //
    static void GetMemorySizeInternal(
        os::MemorySizeCalculator* pSize,
        ResMaterial resource,
        s32 bufferCount,
        bit32 bufferOption);

    //

    //----------------------------------------
    //
    //

    //
    ResMaterial GetOriginal()
    {
        return ResStaticCast<ResMaterial>(this->GetResSceneObject());
    }

    //
    const ResMaterial GetOriginal() const
    {
        return ResStaticCast<ResMaterial>(this->GetResSceneObject());
    }

    //
    //
    //
    //
    Model* GetOwnerModel()
    {
        return this->m_Owner;
    }

    //
    //
    //
    //
    const Model* GetOwnerModel() const
    {
        return this->m_Owner;
    }

    //
    //
    //
    //
    void SetDescription(ResShaderProgramDescription description) { m_ProgramDescription = description; }

    //
    //
    //
    //
    const ResShaderProgramDescription GetDescription() const { return m_ProgramDescription; }

    //

    //----------------------------------------
    //
    //

    //
    int GetBufferCount() const
    {
        return this->m_Buffers.size();
    }

    //
    ResMaterial GetBuffer()
    {
        bool hasBuffer = (0 < m_BufferCount);

        if (hasBuffer)
        {
            return this->m_Buffers[0];
        }
        else
        {
            return ResMaterial(NULL);
        }
    }

    //
    const ResMaterial GetBuffer() const
    {
        bool hasBuffer = (0 < m_BufferCount);

        if (hasBuffer)
        {
            return this->m_Buffers[0];
        }
        else
        {
            return ResMaterial(NULL);
        }
    }

    //
    ResMaterial GetShaderParameterResMaterial() { return m_ShaderParameterResMaterial; }

    //
    const ResMaterial GetShaderParameterResMaterial() const { return m_ShaderParameterResMaterial; }

    //
    ResMaterial GetShadingParameterResMaterial() { return m_ShadingParameterResMaterial; }

    //
    const ResMaterial GetShadingParameterResMaterial() const { return m_ShadingParameterResMaterial; }

    //
    ResMaterial GetMaterialColorResMaterial() { return m_MaterialColorResMaterial; }

    //
    const ResMaterial GetMaterialColorResMaterial() const { return m_MaterialColorResMaterial; }

    //
    ResMaterial GetRasterizationResMaterial() { return m_RasterizationResMaterial; }

    //
    const ResMaterial GetRasterizationResMaterial() const { return m_RasterizationResMaterial; }

    //
    ResMaterial GetTextureCoordinatorResMaterial() { return m_TextureCoordinatorResMaterial; }

    //
    const ResMaterial GetTextureCoordinatorResMaterial() const { return m_TextureCoordinatorResMaterial; }

    //
    ResMaterial GetTextureMapperResMaterial() { return m_TextureMapperResMaterial; }

    //
    const ResMaterial GetTextureMapperResMaterial() const { return m_TextureMapperResMaterial; }

    //
    ResMaterial GetFragmentLightingResMaterial() { return m_FragmentLightingResMaterial; }

    //
    const ResMaterial GetFragmentLightingResMaterial() const { return m_FragmentLightingResMaterial; }

    //
    ResMaterial GetFragmentLightingTableResMaterial() { return m_FragmentLightingTableResMaterial; }

    //
    const ResMaterial GetFragmentLightingTableResMaterial() const { return m_FragmentLightingTableResMaterial; }

    //
    ResMaterial GetTextureCombinerResMaterial() { return m_TextureCombinerResMaterial; }

    //
    const ResMaterial GetTextureCombinerResMaterial() const { return m_TextureCombinerResMaterial; }

    //
    ResMaterial GetAlphaTestResMaterial() { return m_AlphaTestResMaterial; }

    //
    const ResMaterial GetAlphaTestResMaterial() const { return m_AlphaTestResMaterial; }

    //
    ResMaterial GetFragmentOperationResMaterial() { return m_FragmentOperationResMaterial; }

    //
    const ResMaterial GetFragmentOperationResMaterial() const { return m_FragmentOperationResMaterial; }

    //
    ResMaterial GetSceneEnvironmentResMaterial() { return m_SceneEnvironmentResMaterial; }

    //
    const ResMaterial GetSceneEnvironmentResMaterial() const { return m_SceneEnvironmentResMaterial; }

    //

protected:
    //----------------------------------------
    //
    //

    //
    Material(
        os::IAllocator* allocator,
        ResMaterial resMaterial,
        s32 bufferCount,
        Model* parent);

    //
    virtual ~Material();

    //

    struct ResMaterialDestroyer : public std::unary_function<ResMaterial, void>
    {
        ResMaterialDestroyer(os::IAllocator* allocator = 0) : m_Allocator(allocator)
        {}
        result_type operator()(argument_type data)
        {
            DestroyResMaterial(m_Allocator, data);
        }

        os::IAllocator* m_Allocator;
    };

private:

    //
    Result Initialize(os::IAllocator* allocator);

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    Result CreateBuffers(os::IAllocator* allocator);

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    ::std::pair<ResMaterial, Result> CopyResMaterial(
        os::IAllocator* allocator,
        bit32 bufferOption);

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    static void DestroyResMaterial(os::IAllocator* allocator, ResMaterial resMaterial);

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    static void DestroyResFragmentShader(
        os::IAllocator* allocator,
        ResFragmentShader resFragmentShader);

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    static void DestroyResShaderParameter(
        os::IAllocator* allocator,
        ResShaderParameter resShaderParameter);

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    ::std::pair<ResFragmentLightingTable, Result> CopyResFragmentLightingTable(
        os::IAllocator* allocator,
        ResFragmentLightingTable resFragmentLightingTable);

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    ::std::pair<ResLightingLookupTable, Result> CopyResLightingLookupTable(
        os::IAllocator* allocator,
        ResLightingLookupTable resLightingLookupTable);

    //
    //
    //
    //
    //
    ResMaterial GetActiveResource(u32 objectType)
    {
        return (CanUseBuffer(objectType)) ? GetBuffer() : GetOriginal();
    }

    //
    void* GetAnimTargetObject(const anim::ResAnimGroupMember& anim, const ResMaterial resMaterial);

    // Gets whether there is a buffer corresponding to the animation's object type.
    bool CanUseBuffer(u32 objectType) const;

    typedef ut::MoveArray<ResMaterial> ResMaterialArray;

    ResMaterialArray m_Buffers;
    Model* m_Owner;
    ResShaderProgramDescription m_ProgramDescription;
    s32 m_BufferCount;

    ResMaterial m_ShaderParameterResMaterial;
    ResMaterial m_ShadingParameterResMaterial;
    ResMaterial m_MaterialColorResMaterial;
    ResMaterial m_RasterizationResMaterial;
    ResMaterial m_TextureCoordinatorResMaterial;
    ResMaterial m_TextureMapperResMaterial;
    ResMaterial m_FragmentLightingResMaterial;
    ResMaterial m_FragmentLightingTableResMaterial;
    ResMaterial m_TextureCombinerResMaterial;
    ResMaterial m_AlphaTestResMaterial;
    ResMaterial m_FragmentOperationResMaterial;
    ResMaterial m_SceneEnvironmentResMaterial;

    // TODO: Handling this temporarily since CanUseBuffer will be made private.
    friend class Model;
};

//
typedef ut::MoveArray<Material*> MaterialArray;

} // namespace gfx
} // namespace nw

#endif // NW_GFX_MATERIAL_H_
