// Filename: gfx_Model.cpp
//
// Project: NintendoWare4Ctr

#include <nw/gfx/gfx_Model.h>
#include <nw/gfx/gfx_Material.h>
#include <nw/gfx/gfx_ISceneVisitor.h>

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/ut/ut_Foreach.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(Model, TransformNode);

Model* Model::Create(SceneNode* parent, ResSceneObject resource, const Model::Description& description, nw::os::IAllocator* allocator)
{
    NW_NULL_ASSERT(allocator);

    ResModel resNode = ResDynamicCast<ResModel>(resource);
    NW_ASSERT(resNode.IsValid());

    void* memory = allocator->Alloc(sizeof(Model));
    NW_NULL_ASSERT(memory);

    Model* node = new(memory) Model(allocator, resNode, description);

    Result result = node->Initialize(allocator);

    NW_ASSERT(result.IsSuccess());

    if (parent)
    {
        bool isAttached = parent->AttachChild(node);
        NW_ASSERT(isAttached);
    }
    return node;
}

Result Model::Initialize(nw::os::IAllocator* allocator)
{
    Result result = INITIALIZE_RESULT_OK;

    result |= TransformNode::Initialize(allocator);
    NW_ENSURE_AND_RETURN(result);

    result |= CreateResMeshes(allocator);
    NW_ENSURE_AND_RETURN(result);

    result |= CreateResMeshNodeVisibilities(allocator);
    NW_ENSURE_AND_RETURN(result);

    result |= CreateMaterials(allocator);
    NW_ENSURE_AND_RETURN(result);

    result |= CreateCallbacks(allocator);
    NW_ENSURE_AND_RETURN(result);

    result |= CreateAnimGroups(allocator);
    NW_ENSURE_AND_RETURN(result);

    result |= CreateMaterialActivator(allocator);
    NW_ENSURE_AND_RETURN(result);

    return result;
}

void Model::Accept(ISceneVisitor* visitor)
{
    visitor->VisitModel(this);
    AcceptChildren(visitor);
}

void Model::BindMaterialAnim(AnimGroup* animGroup)
{
    const int animMemberCount = animGroup->GetMemberCount();
    for (int memberIdx = 0; memberIdx < animMemberCount; ++memberIdx)
    {
        anim::ResAnimGroupMember resAnimGroupMember = animGroup->GetResAnimGroupMember(memberIdx);
        const int matIdx = GetMaterialIndex(resAnimGroupMember);

        Material* pMat = GetMaterial(matIdx);

        animGroup->SetTargetObjectIndex(memberIdx, matIdx);
        animGroup->SetTargetPtr(memberIdx, GetMaterialAnimTargetPtr(pMat, resAnimGroupMember, false));

        void* object = pMat->GetAnimTargetObject(resAnimGroupMember, pMat->GetActiveResource(resAnimGroupMember.GetObjectType()));
        animGroup->SetTargetObject(memberIdx, object);

        bool useOriginalValue = pMat->CanUseBuffer(resAnimGroupMember.GetObjectType());
        if (useOriginalValue)
        {
            animGroup->SetOriginalValue(memberIdx, GetMaterialAnimTargetPtr(pMat, resAnimGroupMember, true));
        }
    }
}

void Model::BindVisibilityAnim(AnimGroup* animGroup)
{
    using namespace anim;

    const int animMemberCount = animGroup->GetMemberCount();
    for (int memberIdx = 0; memberIdx < animMemberCount; ++memberIdx)
    {
        anim::ResAnimGroupMember resAnimGroupMember = animGroup->GetResAnimGroupMember(memberIdx);

        switch (resAnimGroupMember.GetObjectType())
        {
        case anim::ResAnimGroupMember::OBJECT_TYPE_MODEL:{
                u8* target = NULL;
                if (resAnimGroupMember.GetMemberType() == ResModelMember::MEMBER_TYPE_VISIBLE)
                {
                    target = reinterpret_cast<u8*>(&m_Visible);
                }
                else if (resAnimGroupMember.GetMemberType() == ResModelMember::MEMBER_TYPE_BRANCH_VISIBLE)
                {
                    target = reinterpret_cast<u8*>(&m_BranchVisible);
                }
                else
                {
                    NW_FATAL_ERROR("");
                }

                animGroup->SetTargetObjectIndex(memberIdx, -1);
                animGroup->SetTargetPtr(memberIdx, target);
                animGroup->SetOriginalValue(memberIdx, &m_OriginalVisibility);
            }
            break;
        case anim::ResAnimGroupMember::OBJECT_TYPE_MESH:{
                ResMeshMember meshMember = ResDynamicCast<ResMeshMember>(resAnimGroupMember);
                NW_ASSERT(meshMember.IsValid());

                const int meshIndex = meshMember.GetMeshIndex();
                u8* target = reinterpret_cast<u8*>(GetResMeshes()[meshIndex].ptr());
                target += resAnimGroupMember.GetMemberOffset();

                animGroup->SetTargetObjectIndex(memberIdx, meshIndex);
                animGroup->SetTargetPtr(memberIdx, target);
                animGroup->SetOriginalValue(memberIdx, &m_MeshOriginalVisibilities[meshIndex]);
            }
            break;
        case anim::ResAnimGroupMember::OBJECT_TYPE_MESH_NODE_VISIBILITY:{
                ResMeshNodeVisibilityMember nodeVisibilityMember = ResDynamicCast<ResMeshNodeVisibilityMember>(resAnimGroupMember);
                NW_ASSERT(nodeVisibilityMember.IsValid());

                const char* nodeName = nodeVisibilityMember.GetNodeName();
                const int visibilityIndex = GetResModel().GetMeshNodeVisibilitiesIndex(nodeName);
                NW_ASSERT(visibilityIndex >= 0);

                u8* target = reinterpret_cast<u8*>(GetResMeshNodeVisibilities(visibilityIndex).ptr());
                target += resAnimGroupMember.GetMemberOffset();

                animGroup->SetTargetObjectIndex(memberIdx, visibilityIndex);
                animGroup->SetTargetPtr(memberIdx, target);
                animGroup->SetOriginalValue(memberIdx, &m_MeshNodeOriginalVisibilities[visibilityIndex]);
            }
            break;
        default:{
                NW_FATAL_ERROR("Unknown animation member type");
            }
            break;
        }

        void* object = GetAnimTargetObject(resAnimGroupMember);
        animGroup->SetTargetObject(memberIdx, object);
    }
}

Result Model::CreateAnimGroups(nw::os::IAllocator* allocator)
{
    Result result = INITIALIZE_RESULT_OK;

    AnimBinding* animBinding = GetAnimBinding();
    if (animBinding == NULL)
    {
        return result;
    }

    ResModel resModel = GetResModel();
    NW_ASSERT(resModel.IsValid());

    const int animGroupCount = resModel.GetAnimGroupsCount();
    for (int animGroupIdx = 0; animGroupIdx < animGroupCount; ++animGroupIdx)
    {
        anim::ResAnimGroup resAnimGroup = resModel.GetAnimGroups(animGroupIdx);
        const int targetType = resAnimGroup.GetTargetType();

        if (targetType == anim::ResGraphicsAnimGroup::TARGET_TYPE_MATERIAL && m_SharingMaterial)
        {
            continue;
        }

        if (targetType == anim::ResGraphicsAnimGroup::TARGET_TYPE_MATERIAL ||
            targetType == anim::ResGraphicsAnimGroup::TARGET_TYPE_VISIBILITY)
            {
            AnimGroup* animGroup = AnimGroup::Builder()
                .ResAnimGroup(resAnimGroup)
                .SetSceneNode(this)
                .UseOriginalValue(true)
                .Create(allocator);

            if (animGroup == NULL)
            {
                result |= Result::MASK_FAIL_BIT;
            }

            NW_ENSURE_AND_RETURN(result);

            switch (targetType)
            {
            case anim::ResGraphicsAnimGroup::TARGET_TYPE_MATERIAL:{
                    BindMaterialAnim(animGroup);

                    animBinding->SetAnimGroup(animGroupIdx, animGroup);
                    m_MaterialAnimGroup = animGroup;
                    m_MaterialAnimBindingIndex = animGroupIdx;
                }
                break;

            case anim::ResGraphicsAnimGroup::TARGET_TYPE_VISIBILITY:{
                    BindVisibilityAnim(animGroup);

                    animBinding->SetAnimGroup(animGroupIdx, animGroup);
                    m_VisibilityAnimGroup = animGroup;
                    m_VisibilityAnimBindingIndex = animGroupIdx;
                }
                break;

            default:
                NW_ASSERT(false);
            }
        }
    }

    return result;
}

Result Model::CreateMaterials(nw::os::IAllocator* allocator)
{
    Result result = INITIALIZE_RESULT_OK;

    if (m_SharingMaterial)
    {
        NW_ASSERTMSG(m_BufferOption == 0, "In the case of using a shared material, it can not use buffer option.");
        m_BufferOption = m_Description.sharedMaterialModel->GetBufferOption();

        int materialCount = m_Description.sharedMaterialModel->GetMaterialCount();

        NW_ASSERT(materialCount != 0);

        void* memory = allocator->Alloc(sizeof(Material*) * materialCount);

        if (memory == NULL)
        {
            result |= Result::MASK_FAIL_BIT;
        }
        NW_ENSURE_AND_RETURN(result);

        m_Materials.Reset(memory, materialCount, allocator);

        for (int i = 0; i < materialCount; i++)
        {
            m_Materials.PushBackFast(m_Description.sharedMaterialModel->GetMaterial(i));
        }
    }
    else{
        ResModel resModel = GetResModel();
        NW_ASSERT(resModel.IsValid());

        int materialCount = resModel.GetMaterialsCount();

        if (materialCount != 0)
        {
            void* memory = allocator->Alloc(sizeof(Material*) * materialCount);

            if (memory == NULL)
            {
                result |= Result::MASK_FAIL_BIT;
            }
            NW_ENSURE_AND_RETURN(result);

            m_Materials.Reset(memory, materialCount, allocator);
            s32 bufferCount = (m_BufferOption == 0x0) ? 0 : 1;

            for (int i = 0; i < materialCount; i++)
            {
                Material* material = Material::Create(resModel.GetMaterials(i), bufferCount, this, allocator);

                if (material == NULL)
                {
                    result |= Result::MASK_FAIL_BIT;
                }

                NW_ENSURE_AND_RETURN(result);

                m_Materials.PushBackFast(material);
            }
        }
    }

    return result;
}

Result Model::CreateResMeshes(nw::os::IAllocator* allocator)
{
    Result result = INITIALIZE_RESULT_OK;

    ResModel resModel = GetResModel();
    NW_ASSERT(resModel.IsValid());

    s32 meshesCount = resModel.GetMeshesCount();

    if (meshesCount != 0)
    {
        ut::Offset* meshOffsets = NULL;
        meshOffsets = allocator->AllocAndFill<ut::Offset>(meshesCount, ut::Offset());
        ResMeshData* buffer = allocator->Alloc<ResMeshData>(meshesCount);

        if (meshOffsets == NULL || buffer == NULL)
        {
            if (meshOffsets != NULL)
            {
                allocator->Free(meshOffsets);
            }

            if (buffer != NULL)
            {
                allocator->Free(buffer);
            }

            result |= Result::MASK_FAIL_BIT;
        }

        NW_ENSURE_AND_RETURN(result);

        for (int i = 0; i < meshesCount; ++i)
        {
            buffer[i] = *(resModel.GetMeshes(i).ptr());
            buffer[i].toOwnerModel.set_ptr(GetResModel().ptr());
            meshOffsets[i].set_ptr(&buffer[i]);
        }
        m_MeshBuffers = ResMeshArray(meshOffsets, meshesCount);

        m_OriginalVisibility = resModel.IsVisible();

        void* memory = allocator->Alloc(sizeof(bool) * meshesCount);

        if (memory == NULL)
        {
            result |= Result::MASK_FAIL_BIT;
        }
        NW_ENSURE_AND_RETURN(result);

        m_MeshOriginalVisibilities.Reset(memory, meshesCount, allocator);

        for (int i = 0; i < meshesCount; ++i)
        {
            m_MeshOriginalVisibilities.PushBackFast(m_MeshBuffers[i].IsVisible());
        }
    }

    return result;
}

Result Model::CreateResMeshNodeVisibilities(nw::os::IAllocator* allocator)
{
    Result result = INITIALIZE_RESULT_OK;

    ResModel resModel = GetResModel();
    NW_ASSERT(resModel.IsValid());

    s32 visibilitiesConut = resModel.GetMeshNodeVisibilitiesCount();

    if (visibilitiesConut != 0)
    {
        ResMeshNodeVisibilityData* buffer = allocator->Alloc<ResMeshNodeVisibilityData>(visibilitiesConut);

        if (buffer == NULL)
        {
            result |= Result::MASK_FAIL_BIT;
        }

        NW_ENSURE_AND_RETURN(result);

        m_MeshNodeVisibilityBuffers.Reset(buffer, visibilitiesConut, allocator);

        for (int i = 0; i < visibilitiesConut; ++i)
        {
            m_MeshNodeVisibilityBuffers.PushBackFast(*(resModel.GetMeshNodeVisibilities(i).ptr()));
        }

        void* memory = allocator->Alloc(sizeof(bool) * visibilitiesConut);

        if (memory == NULL)
        {
            result |= Result::MASK_FAIL_BIT;
        }
        NW_ENSURE_AND_RETURN(result);

        m_MeshNodeOriginalVisibilities.Reset(memory, visibilitiesConut, allocator);

        for (int i = 0; i < visibilitiesConut; ++i)
        {
            m_MeshNodeOriginalVisibilities.PushBackFast(resModel.GetMeshNodeVisibilities(i).IsVisible());
        }
    }

    return result;
}

void Model::DestroyResMeshes(nw::os::IAllocator* allocator, ResMeshArray resMeshes)
{
    NW_NULL_ASSERT(allocator);

    if (resMeshes.empty())
    {
        return;
    }

    allocator->Free((*resMeshes.begin()).ptr());
    allocator->Free(static_cast<ResMeshArray::pointer>(resMeshes));
}

Result Model::CreateCallbacks(nw::os::IAllocator* allocator)
{
    Result result = INITIALIZE_RESULT_OK;

    if (m_Description.isFixedSizeMemory)
    {
        if (m_Description.maxCallbacks == 0)
        {
            m_PreRenderSignal = RenderSignal::CreateInvalidateSignal(allocator);
            m_PostRenderSignal = RenderSignal::CreateInvalidateSignal(allocator);
        }
        else{
            m_PreRenderSignal = RenderSignal::CreateFixedSizedSignal(m_Description.maxCallbacks, allocator);
            m_PostRenderSignal = RenderSignal::CreateFixedSizedSignal(m_Description.maxCallbacks, allocator);
        }
    }
    else{
        m_PreRenderSignal = RenderSignal::CreateVariableSizeSignal(allocator);
        m_PostRenderSignal = RenderSignal::CreateVariableSizeSignal(allocator);
    }

    if (m_PreRenderSignal == NULL || m_PostRenderSignal == NULL)
    {
        result |= Result::MASK_FAIL_BIT;
    }

    return result;
}

Result Model::CreateMaterialActivator(nw::os::IAllocator* allocator)
{
    Result result = INITIALIZE_RESULT_OK;

    if (!m_SharingMaterial)
    {
        IMaterialActivator* activator =
            (m_Description.bufferOption == 0 || m_Description.bufferOption == FLAG_BUFFER_SCENE_ENVIRONMENT) ?
            static_cast<IMaterialActivator*>(SimpleMaterialActivator::Create(allocator)) :
            static_cast<IMaterialActivator*>(MaterialActivator::Create(allocator));

        if (activator != NULL)
        {
            m_MaterialActivator.Reset(activator);
        }
        else{
            result |= Result::MASK_FAIL_BIT;
        }
    }

    return result;
}

void* Model::GetAnimTargetObject(const anim::ResAnimGroupMember& anim)
{
    switch (anim.GetObjectType())
    {
    case anim::ResAnimGroupMember::OBJECT_TYPE_MODEL:{
            return this;
        }

    case anim::ResAnimGroupMember::OBJECT_TYPE_MESH:{
            anim::ResMeshMember member = ResStaticCast<anim::ResMeshMember>(anim);
            int meshIndex = member.GetMeshIndex();

            ResMeshData* ptr = GetResMeshes()[meshIndex].ptr();
            return ptr;
        }

    case anim::ResAnimGroupMember::OBJECT_TYPE_MESH_NODE_VISIBILITY:{
            anim::ResMeshNodeVisibilityMember member = ResStaticCast<anim::ResMeshNodeVisibilityMember>(anim);
            const char* nodeName = member.GetNodeName();

            s32 visibilityIndex = GetResModel().GetMeshNodeVisibilitiesIndex(nodeName);
            ResMeshNodeVisibilityData* ptr = GetResMeshNodeVisibilities(visibilityIndex).ptr();
            return ptr;
        }

    default:
        NW_ASSERT(false);
        return NULL;
    }
}

void* Model::GetMaterialAnimTargetPtr(Material* material, const anim::ResAnimGroupMember& anim, bool isOriginalValue)
{
    ResMaterial resMat = isOriginalValue ? material->GetOriginal() : material->GetActiveResource(anim.GetObjectType());

    void* object = material->GetAnimTargetObject(anim, resMat);

    if (anim.GetObjectType() == anim::ResAnimGroupMember::OBJECT_TYPE_TEXTURE_SAMPLER)
    {
        ResPixelBasedTextureMapper mapper(object);
        object = mapper.ref().toSampler.to_ptr();
    }

    return ut::AddOffsetToPtr(object, anim.GetMemberOffset());
}

int Model::GetMaterialIndex(const anim::ResAnimGroupMember& anim) const
{
    const char* matName = NULL;

    switch (anim.GetObjectType())
    {
    case anim::ResAnimGroupMember::OBJECT_TYPE_MATERIAL_COLOR:{
            anim::ResMaterialColorMember member = ResStaticCast<anim::ResMaterialColorMember>(anim);
            matName = member.GetMaterialName();
            break;
        }

    case anim::ResAnimGroupMember::OBJECT_TYPE_TEXTURE_SAMPLER:{
            anim::ResTextureSamplerMember member = ResStaticCast<anim::ResTextureSamplerMember>(anim);
            matName = member.GetMaterialName();
            break;
        }

    case anim::ResAnimGroupMember::OBJECT_TYPE_TEXTURE_MAPPER:{
            anim::ResTextureMapperMember member = ResStaticCast<anim::ResTextureMapperMember>(anim);
            matName = member.GetMaterialName();
            break;
        }

    case anim::ResAnimGroupMember::OBJECT_TYPE_BLEND_OPERATION:
    {
            anim::ResBlendOperationMember member = ResStaticCast<anim::ResBlendOperationMember>(anim);
            matName = member.GetMaterialName();
            break;
        }

    case anim::ResAnimGroupMember::OBJECT_TYPE_TEXTURE_COORDINATOR:{
            anim::ResTextureCoordinatorMember member = ResStaticCast<anim::ResTextureCoordinatorMember>(anim);
            matName = member.GetMaterialName();
            break;
        }

    default:
        NW_ASSERT(false);
        return 0;
    }

    int matIdx = GetResModel().GetMaterialsIndex(matName);
    NW_ASSERT(matIdx != -1);

    return matIdx;
}

void Model::InvalidateRenderKeyCache()
{
    ResMeshArray::iterator end = m_MeshBuffers.end();
    for (ResMeshArray::iterator mesh = m_MeshBuffers.begin(); mesh != end; ++mesh)
    {
        (*mesh).SetFlags(ut::DisableFlag<u32, u32>((*mesh).GetFlags(), ResMesh::FLAG_VALID_RENDER_KEY_CACHE));
    }
}

}
}