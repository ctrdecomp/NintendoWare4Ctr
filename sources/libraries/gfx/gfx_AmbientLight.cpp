#include <nw/os/os_Memory.h>

#include <nw/gfx/gfx_AmbientLight.h>
#include <nw/gfx/gfx_ISceneVisitor.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(AmbientLight, Light);

AmbientLight* AmbientLight::DynamicBuilder::Create(os::IAllocator* allocator){
    NW_NULL_ASSERT(allocator);


    ResPtr resource(CreateResAmbientLight(allocator),ResAmbientLightDataDestroyer(allocator));

    void* memory = allocator->Alloc(sizeof(AmbientLight));
    NW_NULL_ASSERT(memory);
    AmbientLight* light = new(memory) AmbientLight(allocator,resource,this->mDescription);
    
    Result result = light->Initialize(allocator);
    NW_ASSERT(result.IsSuccess());

    return light;
}

size_t AmbientLight::DynamicBuilder::GetMemorySize( size_t alignment ) const{
    NW_ASSERT(this->mDescription.isFixedSizeMemory);

    os::MemorySizeCalculator size(alignment);

    size += sizeof(ResAmbientLightData);

    size += sizeof(AmbientLight);

    TransformNode::GetMemorySizeForInitialize(&size,ResTransformNode(),this->mDescription);

    size += sizeof(ResAmbientLightData);

    return size.GetSizeWithPadding(alignment);
}

AmbientLight* AmbientLight::Create(SceneNode* parent,ResSceneObject resource,const AmbientLight::Description& description,os::IAllocator* allocator){
    NW_NULL_ASSERT(allocator);
    
    ResAmbientLight resNode = ResDynamicCast<ResAmbientLight>(resource);
    NW_ASSERT(resNode.IsValid());
    NW_ASSERT( internal::ResCheckRevision( resNode ) );
    
    void* memory = allocator->Alloc(sizeof(AmbientLight));
    NW_NULL_ASSERT(memory);    

    AmbientLight* light = new(memory) AmbientLight(allocator,resNode,description);
    
    Result result = light->Initialize(allocator);
    NW_ASSERT(result.IsSuccess());

    if (parent){
        bool isAttached = parent->AttachChild(light);
        NW_ASSERT(isAttached);
    }

    return light;
}

void AmbientLight::Accept(ISceneVisitor* visitor){
    visitor->VisitAmbientLight(this);
    AcceptChildren(visitor);
}

ResAmbientLightData* AmbientLight::CreateResAmbientLight(os::IAllocator* allocator, const char* name){    
    ResAmbientLightData* resAmbientLight = AllocateAndFillN<ResAmbientLightData>(allocator, sizeof(ResAmbientLightData), 0);

    resAmbientLight->typeInfo = ResAmbientLight::TYPE_INFO;
    resAmbientLight->mHeader.revision = ResLight::BINARY_REVISION;
    resAmbientLight->mHeader.signature = ResLight::SIGNATURE;
    
    resAmbientLight->mUserDataDicCount = 0;
    resAmbientLight->toUserDataDic.set_ptr( NULL );

    resAmbientLight->toName.set_ptr(AllocateAndCopyString(name, allocator, MAX_NAME_LENGTH));

    resAmbientLight->mChildrenTableCount = 0;
    resAmbientLight->toChildrenTable.set_ptr( NULL );
    resAmbientLight->mAnimGroupsDicCount = NULL;
    resAmbientLight->toAnimGroupsDic.set_ptr( NULL );

    const math::VEC3 scale(1.0f, 1.0f, 1.0f);
    const math::VEC3 rotate(0.0f, 0.0f, 0.0f);
    const math::VEC3 translate(0.0f, 0.0f, 0.0f);
    resAmbientLight->mTransform = math::Transform3(scale, rotate, translate);
    resAmbientLight->mWorldMatrix = math::MTX34::Identity();
    ResTransformNode(resAmbientLight).SetBranchVisible(true);

    return resAmbientLight;
}

void AmbientLight::DestroyResAmbientLight(os::IAllocator* allocator, ResAmbientLightData* resAmbientLight){
    NW_NULL_ASSERT( allocator );
    NW_NULL_ASSERT( resAmbientLight );
    
    if (resAmbientLight->toName.to_ptr() != NULL){
        allocator->Free(const_cast<char*>(resAmbientLight->toName.to_ptr()));
    }
    allocator->Free( resAmbientLight );
}

Result AmbientLight::CreateOriginalValue(os::IAllocator* allocator){
    Result result = INITIALIZE_RESULT_OK;

    void* buffer = allocator->Alloc(sizeof(ResAmbientLightData));
    NW_NULL_ASSERT(buffer);

    ResAmbientLightData* originalValue = new(buffer) ResAmbientLightData(GetResAmbientLight().ref());
    mOriginalValue = ResAmbientLight(originalValue);

    mOriginalTransform = this->GetResTransformNode().GetTransform();

    return result;
}

Result AmbientLight::Initialize(os::IAllocator* allocator){
    Result result = INITIALIZE_RESULT_OK;

    result |= TransformNode::Initialize(allocator);
    NW_ENSURE_AND_RETURN(result);
    
    result |= CreateOriginalValue(allocator);
    NW_ENSURE_AND_RETURN(result);

    result |= CreateAnimGroup(allocator);
    NW_ENSURE_AND_RETURN(result);

    return result;
}

void AmbientLight::GetMemorySizeInternal( os::MemorySizeCalculator* pSize,ResAmbientLight resAmbientLight,Description description){
    NW_ASSERT(description.isFixedSizeMemory);

    os::MemorySizeCalculator& size = *pSize;

    size += sizeof(AmbientLight);

    TransformNode::GetMemorySizeForInitialize(&size,resAmbientLight,description);

    size += sizeof(ResAmbientLightData);

    if (description.isAnimationEnabled && resAmbientLight.GetAnimGroupsCount() > 0){
        AnimGroup::Builder()
            .ResAnimGroup(resAmbientLight.GetAnimGroups(0))
            .UseOriginalValue(true)
            .GetMemorySizeInternal(&size);
    }
}

}
}