// Filename: gfx_FragmentLight.cpp
//
// Project: NintendoWare4Ctr

#include <nw/os/os_Memory.h>

#include <nw/gfx/gfx_FragmentLight.h>
#include <nw/gfx/gfx_ISceneVisitor.h>

#include <cstring>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(FragmentLight, Light);

FragmentLight* FragmentLight::DynamicBuilder::Create(nw::os::IAllocator* allocator)
{
    NW_NULL_ASSERT(allocator);

    ResPtr resource(CreateResFragmentLight(allocator), ResFragmentLightDataDestroyer(allocator));

    void* memory = allocator->Alloc(sizeof(FragmentLight));
    NW_NULL_ASSERT(memory);
    FragmentLight* light = new(memory) FragmentLight(allocator, resource, m_Description);

    Result result = light->Initialize(allocator);
    NW_ASSERT(result.IsSuccess());

    return light;
}

size_t FragmentLight::DynamicBuilder::GetMemorySize(size_t alignment) const
{
    NW_ASSERT(m_Description.isFixedSizeMemory);

    nw::os::MemorySizeCalculator size(alignment);

    size += sizeof(ResFragmentLightData);
    size += sizeof(ResReferenceLookupTable);
    size += sizeof(ResLightingLookupTable);
    size += sizeof(ResReferenceLookupTable);

    size += sizeof(FragmentLight);

    TransformNode::GetMemorySizeForInitialize(&size, ResTransformNode(), m_Description);

    size += sizeof(ResFragmentLightData);

    return size.GetSizeWithPadding(alignment);
}

FragmentLight* FragmentLight::Create(SceneNode* parent, ResSceneObject resource, const FragmentLight::Description& description, nw::os::IAllocator* allocator)
{
    NW_NULL_ASSERT(allocator);

    ResFragmentLight resNode = ResDynamicCast<ResFragmentLight>(resource);
    NW_ASSERT(resNode.IsValid());
    NW_ASSERT(internal::ResCheckRevision(resNode));

    void* memory = allocator->Alloc(sizeof(FragmentLight));
    NW_NULL_ASSERT(memory);

    FragmentLight* light = new(memory) FragmentLight(allocator, resNode, description);

    Result result = light->Initialize(allocator);
    NW_ASSERT(result.IsSuccess());

    if (parent)
    {
        bool isAttached = parent->AttachChild(light);
        NW_ASSERT(isAttached);
    }

    return light;
}

void FragmentLight::UpdateDirection()
{
    ResFragmentLight resLight = GetResFragmentLight();

    if (ut::CheckFlag(resLight.GetFlags(), ResFragmentLightData::FLAG_IS_INHERITING_DIRECTION_ROTATE))
    {
        CalcInheritingDiretion(Direction(), resLight.GetDirection());
    }
    else{
        Direction() = resLight.GetDirection();
    }
}

void FragmentLight::Accept(ISceneVisitor* visitor)
{
    visitor->VisitFragmentLight(this);
    AcceptChildren(visitor);
}

ResFragmentLightData* FragmentLight::CreateResFragmentLight(nw::os::IAllocator* allocator, const char* name /* = NULL */)
{
    ResFragmentLightData* resFragmentLight = AllocateAndFillN<ResFragmentLightData>(allocator, sizeof(ResFragmentLightData), 0);

    resFragmentLight->typeInfo = ResFragmentLight::TYPE_INFO;
    resFragmentLight->m_Header.revision = ResLight::BINARY_REVISION;
    resFragmentLight->m_Header.signature = ResLight::SIGNATURE;

    resFragmentLight->m_UserDataDicCount = 0;
    resFragmentLight->toUserDataDic.set_ptr(NULL);

    resFragmentLight->toName.set_ptr(AllocateAndCopyString(name, allocator, MAX_NAME_LENGTH));

    resFragmentLight->m_ChildrenTableCount = 0;
    resFragmentLight->toChildrenTable.set_ptr(NULL);
    resFragmentLight->m_AnimGroupsDicCount = 0;
    resFragmentLight->toAnimGroupsDic.set_ptr(NULL);

    const math::VEC3 scale(1.0f, 1.0f, 1.0f);
    const math::VEC3 rotate(0.0f, 0.0f, 0.0f);
    const math::VEC3 translate(0.0f, 0.0f, 0.0f);
    resFragmentLight->m_Transform = math::Transform3(scale, rotate, translate);
    resFragmentLight->m_WorldMatrix = math::MTX34::Identity();
    ResTransformNode(resFragmentLight).SetBranchVisible(true);

    ResReferenceLookupTableData* distanceSampler = AllocateAndFill<ResReferenceLookupTableData>(allocator, 0);

    distanceSampler->typeInfo = ResReferenceLookupTable_TYPE_INFO;
    distanceSampler->toTargetLut.set_ptr(NULL);
    distanceSampler->toPath.set_ptr(NULL);

    ResLightingLookupTableData* angleSampler = AllocateAndFill<ResLightingLookupTableData>(allocator, 0);

    ResReferenceLookupTableData* referenceAngleSampler = AllocateAndFill<ResReferenceLookupTableData>(allocator, 0);

    referenceAngleSampler->typeInfo = ResReferenceLookupTable_TYPE_INFO;
    referenceAngleSampler->toPath.set_ptr(NULL);
    referenceAngleSampler->toTargetLut.set_ptr(NULL);

    angleSampler->toSampler.set_ptr(referenceAngleSampler);
    angleSampler->m_Input = ResLightingLookupTable::INPUT_NH;
    angleSampler->m_Scale = ResLightingLookupTable::SCALE_1;

    resFragmentLight->toDistanceSampler.set_ptr(distanceSampler);
    resFragmentLight->toAngleSampler.set_ptr(angleSampler);

    return resFragmentLight;
}

ResFragmentLightData* FragmentLight::CloneResFragmentLight(ResFragmentLight resource, nw::os::IAllocator* allocator)
{
    ResFragmentLightData* resFragmentLight = AllocateAndFillN<ResFragmentLightData>(allocator, sizeof(ResFragmentLightData), 0);
    ResFragmentLightData* source = resource.ptr();

    resFragmentLight->typeInfo = source->typeInfo;
    resFragmentLight->m_Header = source->m_Header;

    resFragmentLight->toName.set_ptr(NULL);

    resFragmentLight->m_UserDataDicCount = 0;
    resFragmentLight->toUserDataDic.set_ptr(NULL);

    resFragmentLight->m_Flags = source->m_Flags;
    resFragmentLight->m_IsBranchVisible = source->m_IsBranchVisible;
    resFragmentLight->m_ChildrenTableCount = 0;
    resFragmentLight->toChildrenTable.set_ptr(NULL);
    resFragmentLight->m_AnimGroupsDicCount = 0;
    resFragmentLight->toAnimGroupsDic.set_ptr(NULL);

    resFragmentLight->m_Transform = source->m_Transform;
    resFragmentLight->m_LocalMatrix = source->m_LocalMatrix;
    resFragmentLight->m_WorldMatrix = source->m_WorldMatrix;

    resFragmentLight->m_IsLightEnabled = source->m_IsLightEnabled;

    ResLookupTable srcDistanceSampler = resource.GetDistanceSampler();

    ResReferenceLookupTableData* distanceSampler = AllocateAndFill<ResReferenceLookupTableData>(allocator, 0);

    distanceSampler->typeInfo = ResReferenceLookupTable_TYPE_INFO;
    if (srcDistanceSampler.IsValid())
    {
        distanceSampler->toTargetLut.set_ptr(srcDistanceSampler.Dereference().ptr());
    }
    else{
        distanceSampler->toTargetLut.set_ptr(NULL);
    }
    distanceSampler->toTableName.set_ptr(NULL);
    distanceSampler->toPath.set_ptr(NULL);

    resFragmentLight->toDistanceSampler.set_ptr(distanceSampler);

    ResLightingLookupTable srcAngleSampler = resource.GetAngleSampler();

    ResLightingLookupTableData* angleSampler = AllocateAndFill<ResLightingLookupTableData>(allocator, 0);

    if (srcAngleSampler.IsValid())
    {
        ResLookupTable srcReferenceAngleSampler = srcAngleSampler.GetSampler();

        ResReferenceLookupTableData* referenceAngleSampler = AllocateAndFill<ResReferenceLookupTableData>(allocator, 0);

        referenceAngleSampler->typeInfo = ResReferenceLookupTable_TYPE_INFO;
        if (srcReferenceAngleSampler.IsValid())
        {
            referenceAngleSampler->toTargetLut.set_ptr(srcReferenceAngleSampler.Dereference().ptr());
        }
        else{
            referenceAngleSampler->toTargetLut.set_ptr(NULL);
        }
        referenceAngleSampler->toTableName.set_ptr(NULL);
        referenceAngleSampler->toPath.set_ptr(NULL);

        angleSampler->toSampler.set_ptr(referenceAngleSampler);
        angleSampler->m_Input = srcAngleSampler.GetInput();
        angleSampler->m_Scale = srcAngleSampler.GetScale();
    }
    else{
        angleSampler->toSampler.set_ptr(NULL);
    }

    resFragmentLight->toAngleSampler.set_ptr(angleSampler);

    resFragmentLight->m_LightKind = source->m_LightKind;
    resFragmentLight->m_Ambient = source->m_Ambient;
    resFragmentLight->m_Diffuse = source->m_Diffuse;
    resFragmentLight->m_Specular0 = source->m_Specular0;
    resFragmentLight->m_Specular1 = source->m_Specular1;
    resFragmentLight->m_AmbientU32 = source->m_AmbientU32;
    resFragmentLight->m_DiffuseU32 = source->m_DiffuseU32;
    resFragmentLight->m_Specular0U32 = source->m_Specular0U32;
    resFragmentLight->m_Specular1U32 = source->m_Specular1U32;
    resFragmentLight->m_Direction = source->m_Direction;
    resFragmentLight->m_DistanceAttenuationStart = source->m_DistanceAttenuationStart;
    resFragmentLight->m_DistanceAttenuationEnd = source->m_DistanceAttenuationEnd;
    resFragmentLight->m_DistanceAttenuationScale = source->m_DistanceAttenuationScale;
    resFragmentLight->m_DistanceAttenuationBias = source->m_DistanceAttenuationBias;

    return resFragmentLight;
}

void FragmentLight::DestroyResFragmentLight(nw::os::IAllocator* allocator, ResFragmentLightData* resFragmentLight)
{
    NW_NULL_ASSERT(allocator);
    NW_NULL_ASSERT(resFragmentLight);

    allocator->Free(resFragmentLight->toDistanceSampler.to_ptr());

    ResLightingLookupTableData* angleSampler = reinterpret_cast<ResLightingLookupTableData*>(resFragmentLight->toAngleSampler.to_ptr());
    if (angleSampler->toSampler.to_ptr() != NULL)
    {
        allocator->Free(angleSampler->toSampler.to_ptr());
    }
    allocator->Free(angleSampler);

    if (resFragmentLight->toName.to_ptr() != NULL)
    {
        allocator->Free(const_cast<char*>(resFragmentLight->toName.to_ptr()));
    }

    allocator->Free(resFragmentLight);
}

Result FragmentLight::CreateOriginalValue(nw::os::IAllocator* allocator)
{
    Result result = INITIALIZE_RESULT_OK;

    void* buffer = allocator->Alloc(sizeof(ResFragmentLightData));
    NW_NULL_ASSERT(buffer);

    ResFragmentLightData* originalValue = new(buffer) ResFragmentLightData(GetResFragmentLight().ref());
    m_OriginalValue = ResFragmentLight(originalValue);

    m_OriginalTransform = GetResTransformNode().GetTransform();

    return result;
}

Result FragmentLight::Initialize(nw::os::IAllocator* allocator)
{
    Result result = INITIALIZE_RESULT_OK;

    result |= TransformNode::Initialize(allocator);
    NW_ENSURE_AND_RETURN(result);

    result |= CreateOriginalValue(allocator);
    NW_ENSURE_AND_RETURN(result);

    result |= CreateAnimGroup(allocator);
    NW_ENSURE_AND_RETURN(result);

    return result;
}

void FragmentLight::GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize, ResFragmentLight resFragmentLight, Description description)
{
    NW_ASSERT(description.isFixedSizeMemory);

    nw::os::MemorySizeCalculator& size = *pSize;

    size += sizeof(FragmentLight);

    TransformNode::GetMemorySizeForInitialize(&size, resFragmentLight, description);

    size += sizeof(ResFragmentLightData);

    if (description.isAnimationEnabled && resFragmentLight.GetAnimGroupsCount() > 0)
    {
        AnimGroup::Builder().ResAnimGroup(resFragmentLight.GetAnimGroups(0)).UseOriginalValue(true).GetMemorySizeInternal(&size);
    }
}

}
}