#pragma once

#include <nw/gfx/gfx_TransformNode.h>
#include <nw/gfx/res/gfx_ResModel.h>
#include <nw/ut/ut_Flag.h>
#include <nw/ut/ut_MoveArray.h>
#include <nw/gfx/gfx_Material.h>
#include <nw/gfx/gfx_MaterialActivator.h>
#include <nw/gfx/gfx_SimpleMaterialActivator.h>

namespace nw{
namespace gfx{

class Model : public TransformNode
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(Model);

public:
    NW_UT_RUNTIME_TYPEINFO;

    typedef nw::ut::Signal3<void, Model*, ResMesh, RenderContext*> RenderSignal;

    typedef RenderSignal::SlotType RenderSlot;

    enum BufferOption
    {
        FLAG_BUFFER_SHADER_PARAMETER_SHIFT,

        FLAG_BUFFER_SHADING_PARAMETER_SHIFT,

        FLAG_BUFFER_MATERIAL_COLOR_SHIFT,

        FLAG_BUFFER_RASTERIZATION_SHIFT,

        FLAG_BUFFER_TEXTURE_COORDINATOR_SHIFT,

        FLAG_BUFFER_TEXTURE_MAPPER_SHIFT,

        FLAG_BUFFER_PROCEDURAL_TEXTURE_MAPPER_SHIFT,

        FLAG_BUFFER_FRAGMENT_LIGHTING_SHIFT,

        FLAG_BUFFER_FRAGMENT_LIGHTING_TABLE_SHIFT,

        FLAG_BUFFER_TEXTURE_COMBINER_SHIFT,

        FLAG_BUFFER_ALPHA_TEST_SHIFT,

        FLAG_BUFFER_FRAGMENT_OPERATION_SHIFT,

        FLAG_BUFFER_SCENE_ENVIRONMENT_SHIFT,

        FLAG_BUFFER_SHADER_PARAMETER                = 0x1 << FLAG_BUFFER_SHADER_PARAMETER_SHIFT,
        FLAG_BUFFER_SHADING_PARAMETER               = 0x1 << FLAG_BUFFER_SHADING_PARAMETER_SHIFT,
        FLAG_BUFFER_MATERIAL_COLOR                  = 0x1 << FLAG_BUFFER_MATERIAL_COLOR_SHIFT,
        FLAG_BUFFER_RASTERIZATION                   = 0x1 << FLAG_BUFFER_RASTERIZATION_SHIFT,
        FLAG_BUFFER_TEXTURE_COORDINATOR             = 0x1 << FLAG_BUFFER_TEXTURE_COORDINATOR_SHIFT,
        FLAG_BUFFER_TEXTURE_MAPPER                  = 0x1 << FLAG_BUFFER_TEXTURE_MAPPER_SHIFT,
        FLAG_BUFFER_PROCEDURAL_TEXTURE_MAPPER       = 0x1 << FLAG_BUFFER_PROCEDURAL_TEXTURE_MAPPER_SHIFT,
        FLAG_BUFFER_FRAGMENT_LIGHTING               = 0x1 << FLAG_BUFFER_FRAGMENT_LIGHTING_SHIFT,
        FLAG_BUFFER_FRAGMENT_LIGHTING_TABLE         = 0x1 << FLAG_BUFFER_FRAGMENT_LIGHTING_TABLE_SHIFT,
        FLAG_BUFFER_TEXTURE_COMBINER                = 0x1 << FLAG_BUFFER_TEXTURE_COMBINER_SHIFT,
        FLAG_BUFFER_ALPHA_TEST                      = 0x1 << FLAG_BUFFER_ALPHA_TEST_SHIFT,
        FLAG_BUFFER_FRAGMENT_OPERATION              = 0x1 << FLAG_BUFFER_FRAGMENT_OPERATION_SHIFT,
        FLAG_BUFFER_SCENE_ENVIRONMENT               = 0x1 << FLAG_BUFFER_SCENE_ENVIRONMENT_SHIFT,

        FLAG_BUFFER_NOT_USE = 0,

        MULTI_FLAG_BUFFER_FRAGMENT_SHADER =
            FLAG_BUFFER_FRAGMENT_LIGHTING         |
            FLAG_BUFFER_FRAGMENT_LIGHTING_TABLE   |
            FLAG_BUFFER_TEXTURE_COMBINER          |
            FLAG_BUFFER_ALPHA_TEST,

        MULTI_FLAG_BUFFER_MATERIAL =
            FLAG_BUFFER_SHADER_PARAMETER          |
            FLAG_BUFFER_SHADING_PARAMETER         |
            FLAG_BUFFER_MATERIAL_COLOR            |
            FLAG_BUFFER_RASTERIZATION             |
            FLAG_BUFFER_TEXTURE_COORDINATOR       |
            FLAG_BUFFER_TEXTURE_MAPPER            |
            FLAG_BUFFER_PROCEDURAL_TEXTURE_MAPPER |
            FLAG_BUFFER_FRAGMENT_LIGHTING         |
            FLAG_BUFFER_FRAGMENT_LIGHTING_TABLE   |
            FLAG_BUFFER_TEXTURE_COMBINER          |
            FLAG_BUFFER_ALPHA_TEST                |
            FLAG_BUFFER_FRAGMENT_OPERATION        |
            FLAG_BUFFER_SCENE_ENVIRONMENT,

        MULTI_FLAG_ANIMATABLE_MATERIAL =
            FLAG_BUFFER_MATERIAL_COLOR      |
            FLAG_BUFFER_TEXTURE_COORDINATOR |
            FLAG_BUFFER_TEXTURE_MAPPER      |
            FLAG_BUFFER_FRAGMENT_OPERATION,

        MULTI_FLAG_LIGHTING_MATERIAL =
            FLAG_BUFFER_SHADING_PARAMETER       |
            FLAG_BUFFER_MATERIAL_COLOR          |
            FLAG_BUFFER_FRAGMENT_LIGHTING       |
            FLAG_BUFFER_FRAGMENT_LIGHTING_TABLE |
            FLAG_BUFFER_SCENE_ENVIRONMENT
    };

    struct Description : public TransformNode::Description
    {
        bit32 bufferOption;
        Model* sharedMaterialModel;

        Description() :
            bufferOption(0),
            sharedMaterialModel(NULL) {}
    };

    struct IsVisibleModelDefaultFunctor
    {
        bool IsVisible(const gfx::Model* model)
        {
            return model->IsVisible() && model->IsEnabledResults(SceneNode::FLAG_IS_VISIBLE);
        }
    };

    static Model* Create(SceneNode* parent,ResSceneObject resource,const Model::Description& description,nw::os::IAllocator* allocator);

    static size_t GetMemorySize(ResModel resModel,Description description, size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT)
    {
        nw::os::MemorySizeCalculator size(alignment);

        GetMemorySizeInternal(&size, resModel, description);

        return size.GetSizeWithPadding(alignment);
    }

    static void GetMemorySizeInternal( nw::os::MemorySizeCalculator* pSize,ResModel resModel,Description description)
    {
        nw::os::MemorySizeCalculator& size = *pSize;

        size += sizeof(Model);
        GetMemorySizeForInitialize(pSize, resModel, description);
    }

    virtual void Accept(ISceneVisitor* visitor);

    ResModel GetResModel() 
    {
        return ResModel(this->GetResSceneObject().ptr());
    }

    const ResModel GetResModel() const 
    {
        return ResModel(this->GetResSceneObject().ptr());
    }

    ResMeshArray GetResMeshes()
    {
        if (this->m_MeshBuffers.empty())
        {
            ResModel model = this->GetResModel();
            NW_ASSERT(model.IsValid());
            return model.GetMeshes();
        }
        return m_MeshBuffers;
    }

    ResMeshNodeVisibility GetResMeshNodeVisibilities(int idx)
    {
        if (this->m_MeshNodeVisibilityBuffers.empty())
        {
            ResModel model = this->GetResModel();
            NW_ASSERT(model.IsValid());
            return model.GetMeshNodeVisibilities(idx);
        }
        return ResMeshNodeVisibility(&this->m_MeshNodeVisibilityBuffers[idx] );
    }

    AnimGroup* GetMaterialAnimGroup() { return m_MaterialAnimGroup; }

    const AnimGroup* GetMaterialAnimGroup() const { return m_MaterialAnimGroup; }

    int GetMaterialAnimBindingIndex() const { return m_MaterialAnimBindingIndex; }

    const AnimObject* GetMaterialAnimObject(int objectIndex = 0) const
    {
        NW_NULL_ASSERT(m_AnimBinding);
        return this->m_AnimBinding->GetAnimObject(this->m_MaterialAnimBindingIndex, objectIndex);
    }

    AnimObject* GetMaterialAnimObject(int objectIndex = 0)
    {
        NW_NULL_ASSERT(m_AnimBinding);
        return this->m_AnimBinding->GetAnimObject(this->m_MaterialAnimBindingIndex, objectIndex);
    }

    void SetMaterialAnimObject(AnimObject* animObject, int objectIndex = 0)
    {
        NW_NULL_ASSERT(m_AnimBinding);
        this->m_AnimBinding->SetAnimObject(this->m_MaterialAnimBindingIndex, animObject, objectIndex);
    }

    AnimGroup* GetVisibilityAnimGroup() { return m_VisibilityAnimGroup; }

    const AnimGroup* GetVisibilityAnimGroup() const { return m_VisibilityAnimGroup; }

    int GetVisibilityAnimBindingIndex() const { return m_VisibilityAnimBindingIndex; }

    const AnimObject* GetVisibilityAnimObject(int objectIndex = 0) const
    {
        NW_NULL_ASSERT(m_AnimBinding);
        return this->m_AnimBinding->GetAnimObject(this->m_VisibilityAnimBindingIndex, objectIndex);
    }

    AnimObject* GetVisibilityAnimObject(int objectIndex = 0)
    {
        NW_NULL_ASSERT(m_AnimBinding);
        return this->m_AnimBinding->GetAnimObject(this->m_VisibilityAnimBindingIndex, objectIndex);
    }

   void SetVisibilityAnimObject(AnimObject* animObject, int objectIndex = 0)
    {
        NW_NULL_ASSERT(m_AnimBinding);
        this->m_AnimBinding->SetAnimObject(this->m_VisibilityAnimBindingIndex, animObject, objectIndex);
    }

    RenderSignal& PreRenderSignal() { return *this->m_PreRenderSignal; }

    const RenderSignal& PreRenderSignal() const { return *this->m_PreRenderSignal; }

    RenderSignal& PostRenderSignal() { return *this->m_PostRenderSignal; }

    const RenderSignal& PostRenderSignal() const { return *this->m_PostRenderSignal; }

    typedef gfx::MaterialArray MaterialArray;

    typedef std::pair<gfx::MaterialArray::iterator,gfx::MaterialArray::iterator> MaterialRange;

    MaterialRange GetMaterials()
    {
        return std::make_pair(this->m_Materials.begin(),this->m_Materials.end());
    }

    int GetMaterialCount() const { return this->m_Materials.size(); }

    Material* GetMaterial(int index)
    {
        return m_Materials[index];
    }

    const Material* GetMaterial(int index) const
    {
        return m_Materials[index];
    }

    const IMaterialActivator* GetMaterialActivator() const
    {
        return this->m_MaterialActivator.Get();
    }

    IMaterialActivator* GetMaterialActivator()
    {
        return this->m_MaterialActivator.Get();
    }

    void SetMaterialActivator(IMaterialActivator* materialActivator)
    {
        return this->m_MaterialActivator.Reset(materialActivator);
    }

    void SetSharedMaterialActivator(IMaterialActivator* materialActivator)
    {
        return this->m_MaterialActivator.Reset(materialActivator, false);
    }

    bool CheckBufferOption(bit32 bufferOption) const
    {
        return ut::CheckFlag(this->m_BufferOption, bufferOption);
    }

    bit32 GetBufferOption() const
    {
        return this->m_BufferOption;
    }

    nw::math::MTX34& ModelViewMatrix()
    {
        return this->m_ModelViewMatrix;
    }

    const nw::math::MTX34& ModelViewMatrix() const
    {
        return this->m_ModelViewMatrix;
    }

    nw::math::MTX34& NormalMatrix()
    {
        return this->m_NormalMatrix;
    }

    const nw::math::MTX34& NormalMatrix() const
    {
        return this->m_NormalMatrix;
    }

    inline void UpdateNormalMatrix(const math::MTX34& viewMatrix, bool isModelCoordinate);

    u8 GetLayerId() const { return m_LayerId; }

    void SetLayerId(u8 layerId) { m_LayerId = layerId; }

    ResMaterial::TranslucencyKind GetRenderLayerId(ResMesh mesh)
    {
        NW_ASSERT(mesh.IsValid());
        Material* material = this->GetMaterial(mesh.GetMaterialIndex());
        
        ResMaterial shadingParametersResMaterial = material->GetShadingParameterResMaterial();
        NW_ASSERT(shadingParametersResMaterial.IsValid());
        
        return shadingParametersResMaterial.GetTranslucencyKind();
    }

    bool IsVisible() const { return m_Visible; }

    void SetVisible(bool visible) { m_Visible = visible; }

    bool IsMeshVisible(ResMesh mesh)
    {
        int index = mesh.GetMeshNodeVisibilityIndex();
        if (index < 0)
        {
            return mesh.IsVisible();
        }
        else{
            ResMeshNodeVisibility visibility = this->GetResMeshNodeVisibilities(index);
            NW_ASSERT(visibility.IsValid());
            {
                return visibility.IsVisible() && mesh.IsVisible();
            }
        }
    }

    void InvalidateRenderKeyCache();

    template<typename Type>
    Type GetUserParameter() const
    {
        return *reinterpret_cast<const Type*>(&m_UserParameter);
    }

    template<typename Type>
    void SetUserParameter(Type parameter)
    {
        m_UserParameter = *reinterpret_cast<u32*>(&parameter);
    }

protected:
    virtual Result Initialize(nw::os::IAllocator* allocator);

    Model(nw::os::IAllocator* allocator,ResTransformNode resource,const Model::Description& description): 
        TransformNode(allocator,resource,description),
        m_MaterialAnimGroup(NULL),
        m_MaterialAnimBindingIndex(0),
        m_VisibilityAnimGroup(NULL),
        m_VisibilityAnimBindingIndex(0),
        m_PreRenderSignal(NULL),
        m_PostRenderSignal(NULL),
        m_MeshBuffers(NULL, NULL),
        m_BufferOption(description.bufferOption),
        m_ModelViewMatrix(nw::math::MTX34::Identity()),
        m_NormalMatrix(nw::math::MTX34::Identity()),
        m_LayerId(0),
        m_OriginalVisibility(true),
        m_Visible(true),
        m_SharingMaterial(description.sharedMaterialModel != NULL),
        m_Description(description),
        m_UserParameter(0)
        {
        this->SetVisible(ResStaticCast<ResModel>(resource).IsVisible());
    }

    virtual ~Model()
    {
        DestroyResMeshes(&GetAllocator(), m_MeshBuffers);
        SafeDestroy(this->m_PreRenderSignal);
        SafeDestroy(this->m_PostRenderSignal);
        
        if (!m_SharingMaterial)
        {
            SafeDestroyAll(this->m_Materials);
        }

        SafeDestroy(this->m_MaterialAnimGroup);
        SafeDestroy(this->m_VisibilityAnimGroup);
    }

    static void GetMemorySizeForInitialize(nw::os::MemorySizeCalculator* pSize,ResModel resModel,Description description)
    {
        NW_ASSERT(description.isFixedSizeMemory);

        os::MemorySizeCalculator& size = *pSize;

        TransformNode::GetMemorySizeForInitialize(pSize, resModel, description);

        const s32 meshesCount = resModel.GetMeshesCount();
        const s32 visibilitiesConut = resModel.GetMeshNodeVisibilitiesCount();

        size += sizeof(ut::Offset) * meshesCount;
        size += sizeof(ResMeshData) * meshesCount;
        size += sizeof(bool) * meshesCount;

        size += sizeof(ResMeshNodeVisibilityData) * visibilitiesConut;
        size += sizeof(bool) * visibilitiesConut;

        if (description.sharedMaterialModel != NULL)
        {
            const int materialCount = description.sharedMaterialModel->GetMaterialCount();
            size += sizeof(Material*) * materialCount;
        }
        else{
            const s32 bufferCount = (description.bufferOption == 0x0) ? 0 : 1;
            const s32 materialCount = resModel.GetMaterialsCount();

            size += sizeof(Material*) * materialCount;

            for (int i=0; i < materialCount ; i++)
            {
                Material::GetMemorySizeInternal(
                    pSize,
                    resModel.GetMaterials(i),
                    bufferCount,
                    description.bufferOption);
            }
        }

        if (description.maxCallbacks == 0)
        {
            RenderSignal::GetMemorySizeForInvalidateSignalInternal(pSize);
            RenderSignal::GetMemorySizeForInvalidateSignalInternal(pSize);
        }
        else{
            RenderSignal::GetMemorySizeForFixedSizedSignalInternal(pSize, description.maxCallbacks);
            RenderSignal::GetMemorySizeForFixedSizedSignalInternal(pSize, description.maxCallbacks);
        }

        if (description.isAnimationEnabled)
        {
            const int animGroupCount = resModel.GetAnimGroupsCount();
            for (int animGroupIdx = 0; animGroupIdx < animGroupCount; ++animGroupIdx)
            {
                anim::ResAnimGroup resAnimGroup = resModel.GetAnimGroups(animGroupIdx);

                if (
                    resAnimGroup.GetTargetType() == anim::ResGraphicsAnimGroup::TARGET_TYPE_MATERIAL ||
                    resAnimGroup.GetTargetType() == anim::ResGraphicsAnimGroup::TARGET_TYPE_VISIBILITY) 
                    {
                    AnimGroup::Builder()
                        .ResAnimGroup(resAnimGroup)
                        .UseOriginalValue(true)
                        .GetMemorySizeInternal(pSize);
                }
            }
        }

        if (description.sharedMaterialModel == NULL)
        {
            if (description.bufferOption == 0 || description.bufferOption == FLAG_BUFFER_SCENE_ENVIRONMENT)
            {
                SimpleMaterialActivator::GetMemorySizeInternal(pSize);
            }
            else{
                MaterialActivator::GetMemorySizeInternal(pSize);
            }
        }
    }

private:
    void BindMaterialAnim(AnimGroup* animGroup);

    void BindVisibilityAnim(AnimGroup* animGroup);

    Result CreateResMeshes(nw::os::IAllocator* allocator);

    void DestroyResMeshes(nw::os::IAllocator* allocator, ResMeshArray resMeshes);

    Result CreateResMeshNodeVisibilities(nw::os::IAllocator* allocator);

    Result CreateMaterials(nw::os::IAllocator* allocator);

    Result CreateAnimGroups(nw::os::IAllocator* allocator);

    Result CreateCallbacks(nw::os::IAllocator* allocator);

    Result CreateMaterialActivator(os::IAllocator* allocator);

    void* GetAnimTargetObject(const anim::ResAnimGroupMember& anim);

    void* GetMaterialAnimTargetPtr(Material* material, const anim::ResAnimGroupMember& anim, bool isOriginalValue);

    int GetMaterialIndex(const anim::ResAnimGroupMember& anim) const;

    AnimGroup* m_MaterialAnimGroup;
    s32 m_MaterialAnimBindingIndex;

    AnimGroup* m_VisibilityAnimGroup;
    s32 m_VisibilityAnimBindingIndex;

    RenderSignal* m_PreRenderSignal;
    RenderSignal* m_PostRenderSignal;

    gfx::MaterialArray m_Materials;
    ResMeshArray m_MeshBuffers;
    nw::ut::MoveArray<ResMeshNodeVisibilityData> m_MeshNodeVisibilityBuffers;
    bit32 m_BufferOption;
    nw::math::MTX34 m_ModelViewMatrix;
    nw::math::MTX34 m_NormalMatrix;
    GfxPtr<IMaterialActivator> m_MaterialActivator;

    u8 m_LayerId;
    bool m_OriginalVisibility;
    bool m_Visible;
    bool m_SharingMaterial;

    nw::ut::MoveArray<bool> m_MeshOriginalVisibilities;
    nw::ut::MoveArray<bool> m_MeshNodeOriginalVisibilities;

    const Model::Description& m_Description;
    u32 m_UserParameter;
};

inline void Model::UpdateNormalMatrix(const math::MTX34& viewMatrix,bool isModelCoordinate)
{
    if (isModelCoordinate)
    {
        math::MTX34 worldInvTranspose;
        math::MTX34InvTranspose(&worldInvTranspose, &this->WorldMatrix());
        math::MTX34Mult(&this->NormalMatrix(), &viewMatrix, &worldInvTranspose);
    }
    else{
        if (this->WorldTransform().IsEnabledFlags(CalculatedTransform::FLAG_IS_SCALE_ONE))
        {
            math::MTX34Copy(&this->NormalMatrix(), &viewMatrix);
        }
        else{
            math::MTX34 worldInv;
            math::MTX34 worldTranspose;
            math::MTX34 worldInvTranspose;
            math::MTX34Inverse(&worldInv, this->WorldMatrix());
            math::MTX34Transpose(&worldTranspose, &worldInv);
            math::MTX34Mult(&worldInvTranspose, &worldInv, &worldTranspose);
            math::MTX34Mult(&this->NormalMatrix(), &viewMatrix, &worldInvTranspose);
        }
    }
}

}
}