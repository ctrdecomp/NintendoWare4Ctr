#include <nw/os/os_Memory.h>

#include <nw/gfx/gfx_FragmentLight.h>
#include <nw/gfx/gfx_ISceneVisitor.h>

#include <cstring>

namespace adsl{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(FragmentLight, Light);

FragmentLight* FragmentLight::DynamicBuilder::Create(nw::os::IAllocator* allocator){
    NW_NULL_ASSERT(allocator);

    ResPtr resource(CreateResFragmentLight(allocator), ResFragmentLightDataDestroyer(allocator));

    void* memory = allocator->Alloc(sizeof(FragmentLight));
    NW_NULL_ASSERT(memory);
    FragmentLight* light = new(memory) FragmentLight(allocator, resource, mDescription);

    Result result = light->Initialize(allocator);
    NW_ASSERT(result.IsSuccess());

    return light;
}

size_t FragmentLight::DynamicBuilder::GetMemorySize(size_t alignment) const{
    NW_ASSERT(mDescription.isFixedSizeMemory);

    nw::os::MemorySizeCalculator size(alignment);

    size += sizeof(ResFragmentLightData);
    size += sizeof(ResReferenceLookupTable);
    size += sizeof(ResLightingLookupTable);
    size += sizeof(ResReferenceLookupTable);

    size += sizeof(FragmentLight);

    TransformNode::GetMemorySizeForInitialize(&size, ResTransformNode(), mDescription);

    size += sizeof(ResFragmentLightData);

    return size.GetSizeWithPadding(alignment);
}

FragmentLight* FragmentLight::Create(SceneNode* parent, ResSceneObject resource, const FragmentLight::Description& description, nw::os::IAllocator* allocator){
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

void FragmentLight::UpdateDirection(){
    ResFragmentLight resLight = GetResFragmentLight();

    if (ut::CheckFlag(resLight.GetFlags(), ResFragmentLightData::FLAG_IS_INHERITING_DIRECTION_ROTATE)){
        CalcInheritingDiretion(Direction(), resLight.GetDirection());
    }
    else{
        Direction() = resLight.GetDirection();
    }
}

void FragmentLight::Accept(ISceneVisitor* visitor){
    visitor->VisitFragmentLight(this);
    AcceptChildren(visitor);
}

ResFragmentLightData* FragmentLight::CreateResFragmentLight(nw::os::IAllocator* allocator, const char* name /* = NULL */){
    ResFragmentLightData* resFragmentLight = AllocateAndFillN<ResFragmentLightData>(allocator, sizeof(ResFragmentLightData), 0);

    resFragmentLight->typeInfo = ResFragmentLight::TYPE_INFO;
    resFragmentLight->mHeader.revision = ResLight::BINARY_REVISION;
    resFragmentLight->mHeader.signature = ResLight::SIGNATURE;

    resFragmentLight->mUserDataDicCount = 0;
    resFragmentLight->toUserDataDic.set_ptr(NULL);

    resFragmentLight->toName.set_ptr(AllocateAndCopyString(name, allocator, MAX_NAME_LENGTH));

    resFragmentLight->mChildrenTableCount = 0;
    resFragmentLight->toChildrenTable.set_ptr(NULL);
    resFragmentLight->mAnimGroupsDicCount = 0;
    resFragmentLight->toAnimGroupsDic.set_ptr(NULL);

    const math::VEC3 scale(1.0f, 1.0f, 1.0f);
    const math::VEC3 rotate(0.0f, 0.0f, 0.0f);
    const math::VEC3 translate(0.0f, 0.0f, 0.0f);
    resFragmentLight->mTransform = math::Transform3(scale, rotate, translate);
    resFragmentLight->mWorldMatrix = math::MTX34::Identity();
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
    angleSampler->mInput = ResLightingLookupTable::INPUT_NH;
    angleSampler->mScale = ResLightingLookupTable::SCALE_1;

    resFragmentLight->toDistanceSampler.set_ptr(distanceSampler);
    resFragmentLight->toAngleSampler.set_ptr(angleSampler);

    return resFragmentLight;
}

ResFragmentLightData* FragmentLight::CloneResFragmentLight(ResFragmentLight resource, nw::os::IAllocator* allocator){
    ResFragmentLightData* resFragmentLight = AllocateAndFillN<ResFragmentLightData>(allocator, sizeof(ResFragmentLightData), 0);
    ResFragmentLightData* source = resource.ptr();

    resFragmentLight->typeInfo = source->typeInfo;
    resFragmentLight->mHeader = source->mHeader;

    resFragmentLight->toName.set_ptr(NULL);

    resFragmentLight->mUserDataDicCount = 0;
    resFragmentLight->toUserDataDic.set_ptr(NULL);

    resFragmentLight->mFlags = source->mFlags;
    resFragmentLight->mIsBranchVisible = source->mIsBranchVisible;
    resFragmentLight->mChildrenTableCount = 0;
    resFragmentLight->toChildrenTable.set_ptr(NULL);
    resFragmentLight->mAnimGroupsDicCount = 0;
    resFragmentLight->toAnimGroupsDic.set_ptr(NULL);

    resFragmentLight->mTransform = source->mTransform;
    resFragmentLight->mLocalMatrix = source->mLocalMatrix;
    resFragmentLight->mWorldMatrix = source->mWorldMatrix;

    resFragmentLight->mIsLightEnabled = source->mIsLightEnabled;

    ResLookupTable srcDistanceSampler = resource.GetDistanceSampler();

    ResReferenceLookupTableData* distanceSampler = AllocateAndFill<ResReferenceLookupTableData>(allocator, 0);

    distanceSampler->typeInfo = ResReferenceLookupTable_TYPE_INFO;
    if (srcDistanceSampler.IsValid()){
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

    if (srcAngleSampler.IsValid()){
        ResLookupTable srcReferenceAngleSampler = srcAngleSampler.GetSampler();

        ResReferenceLookupTableData* referenceAngleSampler = AllocateAndFill<ResReferenceLookupTableData>(allocator, 0);

        referenceAngleSampler->typeInfo = ResReferenceLookupTable_TYPE_INFO;
        if (srcReferenceAngleSampler.IsValid()){
            referenceAngleSampler->toTargetLut.set_ptr(srcReferenceAngleSampler.Dereference().ptr());
        }
        else{
            referenceAngleSampler->toTargetLut.set_ptr(NULL);
        }
        referenceAngleSampler->toTableName.set_ptr(NULL);
        referenceAngleSampler->toPath.set_ptr(NULL);

        angleSampler->toSampler.set_ptr(referenceAngleSampler);
        angleSampler->mInput = srcAngleSampler.GetInput();
        angleSampler->mScale = srcAngleSampler.GetScale();
    }
    else{
        angleSampler->toSampler.set_ptr(NULL);
    }

    resFragmentLight->toAngleSampler.set_ptr(angleSampler);

    resFragmentLight->mLightKind = source->mLightKind;
    resFragmentLight->mAmbient = source->mAmbient;
    resFragmentLight->mDiffuse = source->mDiffuse;
    resFragmentLight->mSpecular0 = source->mSpecular0;
    resFragmentLight->mSpecular1 = source->mSpecular1;
    resFragmentLight->mAmbientU32 = source->mAmbientU32;
    resFragmentLight->mDiffuseU32 = source->mDiffuseU32;
    resFragmentLight->mSpecular0U32 = source->mSpecular0U32;
    resFragmentLight->mSpecular1U32 = source->mSpecular1U32;
    resFragmentLight->mDirection = source->mDirection;
    resFragmentLight->mDistanceAttenuationStart = source->mDistanceAttenuationStart;
    resFragmentLight->mDistanceAttenuationEnd = source->mDistanceAttenuationEnd;
    resFragmentLight->mDistanceAttenuationScale = source->mDistanceAttenuationScale;
    resFragmentLight->mDistanceAttenuationBias = source->mDistanceAttenuationBias;

    return resFragmentLight;
}

void FragmentLight::DestroyResFragmentLight(nw::os::IAllocator* allocator, ResFragmentLightData* resFragmentLight){
    NW_NULL_ASSERT(allocator);
    NW_NULL_ASSERT(resFragmentLight);

    allocator->Free(resFragmentLight->toDistanceSampler.to_ptr());

    ResLightingLookupTableData* angleSampler = reinterpret_cast<ResLightingLookupTableData*>(resFragmentLight->toAngleSampler.to_ptr());
    if (angleSampler->toSampler.to_ptr() != NULL){
        allocator->Free(angleSampler->toSampler.to_ptr());
    }
    allocator->Free(angleSampler);

    if (resFragmentLight->toName.to_ptr() != NULL){
        allocator->Free(const_cast<char*>(resFragmentLight->toName.to_ptr()));
    }

    allocator->Free(resFragmentLight);
}

Result FragmentLight::CreateOriginalValue(nw::os::IAllocator* allocator){
    Result result = INITIALIZE_RESULT_OK;

    void* buffer = allocator->Alloc(sizeof(ResFragmentLightData));
    NW_NULL_ASSERT(buffer);

    ResFragmentLightData* originalValue = new(buffer) ResFragmentLightData(GetResFragmentLight().ref());
    mOriginalValue = ResFragmentLight(originalValue);

    mOriginalTransform = GetResTransformNode().GetTransform();

    return result;
}

Result FragmentLight::Initialize(nw::os::IAllocator* allocator){
    Result result = INITIALIZE_RESULT_OK;

    result |= TransformNode::Initialize(allocator);
    NW_ENSURE_AND_RETURN(result);

    result |= CreateOriginalValue(allocator);
    NW_ENSURE_AND_RETURN(result);

    result |= CreateAnimGroup(allocator);
    NW_ENSURE_AND_RETURN(result);

    return result;
}

void FragmentLight::GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize, ResFragmentLight resFragmentLight, Description description){
    NW_ASSERT(description.isFixedSizeMemory);

    nw::os::MemorySizeCalculator& size = *pSize;

    size += sizeof(FragmentLight);

    TransformNode::GetMemorySizeForInitialize(&size, resFragmentLight, description);

    size += sizeof(ResFragmentLightData);

    if (description.isAnimationEnabled && resFragmentLight.GetAnimGroupsCount() > 0){
        AnimGroup::Builder().ResAnimGroup(resFragmentLight.GetAnimGroups(0)).UseOriginalValue(true).GetMemorySizeInternal(&size);
    }
}

}
}