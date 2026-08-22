#include <nw/os/os_Memory.h>

#include <nw/gfx/gfx_VertexLight.h>
#include <nw/gfx/gfx_ISceneVisitor.h>
#include <nw/gfx/res/gfx_ResVertex.h>

#include <cstring>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(VertexLight, Light);

VertexLight* VertexLight::DynamicBuilder::Create(nw::os::IAllocator* allocator){
    NW_NULL_ASSERT(allocator);

    ResPtr resource(CreateResVertexLight(allocator),ResVertexLightDataDestroyer(allocator));

    void* memory = allocator->Alloc(sizeof(VertexLight));
    NW_NULL_ASSERT(memory);
    VertexLight* light = new(memory) VertexLight(allocator,resource,mDescription);
    
    Result result = light->Initialize(allocator);
    NW_ASSERT(result.IsSuccess());

    return light;
}

size_t VertexLight::DynamicBuilder::GetMemorySize( size_t alignment ) const{
    NW_ASSERT(this->mDescription.isFixedSizeMemory);

    os::MemorySizeCalculator size(alignment);

    size += sizeof(ResVertexLightData);

    size += sizeof(VertexLight);

    TransformNode::GetMemorySizeForInitialize(&size,ResTransformNode(),this->mDescription);

    size += sizeof(ResVertexLightData);

    return size.GetSizeWithPadding(alignment);
}

VertexLight* VertexLight::Create(SceneNode* parent,ResSceneObject resource,const VertexLight::Description& description,os::IAllocator* allocator){
    NW_NULL_ASSERT(allocator);
    
    ResVertexLight resNode = ResDynamicCast<ResVertexLight>(resource);
    NW_ASSERT(resNode.IsValid());
    NW_ASSERT( internal::ResCheckRevision( resNode ) );
    
    void* memory = allocator->Alloc(sizeof(VertexLight));
    NW_NULL_ASSERT(memory);    

    VertexLight* light = new(memory) VertexLight(
        allocator,
        resNode,
        description);

    Result result = light->Initialize(allocator);
    NW_ASSERT(result.IsSuccess());
    
    if (parent){
        bool isAttached = parent->AttachChild(light);
        NW_ASSERT(isAttached);
    }

    return light;
}

void VertexLight::UpdateDirection(){
    ResVertexLight resLight = this->GetResVertexLight();

    if (nw::ut::CheckFlag(resLight.GetFlags(), ResFragmentLightData::FLAG_IS_INHERITING_DIRECTION_ROTATE)){
        this->CalcInheritingDiretion(this->Direction(), resLight.GetDirection());
    }
    else{
        this->Direction() = resLight.GetDirection();
    }
}

void VertexLight::Accept(ISceneVisitor* visitor){
    visitor->VisitVertexLight(this);
    AcceptChildren(visitor);
}


ResVertexLightData* VertexLight::CreateResVertexLight(os::IAllocator* allocator, const char* name /* = NULL */){
    const int MAX_NAME_LENGTH = 256;

    ResVertexLightData* resVertexLight =AllocateAndFillN<ResVertexLightData>(allocator, sizeof(ResVertexLightData), 0);
    
    resVertexLight->typeInfo = ResVertexLight::TYPE_INFO;
    resVertexLight->mHeader.revision = ResLight::BINARY_REVISION;
    resVertexLight->mHeader.signature = ResLight::SIGNATURE;
    
    resVertexLight->mUserDataDicCount = 0;
    resVertexLight->toUserDataDic.set_ptr( NULL );
    
    resVertexLight->toName.set_ptr(AllocateAndCopyString(name, allocator, MAX_NAME_LENGTH));

    resVertexLight->mChildrenTableCount = 0;
    resVertexLight->toChildrenTable.set_ptr( NULL );
    resVertexLight->mAnimGroupsDicCount = 0;
    resVertexLight->toAnimGroupsDic.set_ptr( NULL );

    const math::VEC3 scale(1.0f, 1.0f, 1.0f);
    const math::VEC3 rotate(0.0f, 0.0f, 0.0f);
    const math::VEC3 translate(0.0f, 0.0f, 0.0f);
    resVertexLight->mTransform = math::Transform3(scale, rotate, translate);
    resVertexLight->mWorldMatrix = math::MTX34::Identity();
    ResTransformNode(resVertexLight).SetBranchVisible(true);

    return resVertexLight;
}
void VertexLight::DestroyResVertexLight(os::IAllocator* allocator, ResVertexLightData* resVertexLight){
    NW_NULL_ASSERT( allocator );
    NW_NULL_ASSERT( resVertexLight );
    
    if (resVertexLight->toName.to_ptr() != NULL){
        allocator->Free( const_cast<char*>( resVertexLight->toName.to_ptr() ) );
    }
    allocator->Free( resVertexLight );
}

Result VertexLight::CreateOriginalValue(os::IAllocator* allocator){
    Result result = INITIALIZE_RESULT_OK;

    void* buffer = allocator->Alloc(sizeof(ResVertexLightData));
    NW_NULL_ASSERT(buffer);

    ResVertexLightData* originalValue = new(buffer) ResVertexLightData(GetResVertexLight().ref());
    mOriginalValue = ResVertexLight(originalValue);

    mOriginalTransform = this->GetResTransformNode().GetTransform();

    return result;
}

Result VertexLight::Initialize(os::IAllocator* allocator){
    Result result = INITIALIZE_RESULT_OK;

    result |= TransformNode::Initialize(allocator);
    NW_ENSURE_AND_RETURN(result);
    
    result |= CreateOriginalValue(allocator);
    NW_ENSURE_AND_RETURN(result);

    result |= CreateAnimGroup(allocator);
    NW_ENSURE_AND_RETURN(result);

    return result;
}

void VertexLight::GetMemorySizeInternal( os::MemorySizeCalculator* pSize,ResVertexLight resVertexLight,Description description){
    NW_ASSERT(description.isFixedSizeMemory);

    os::MemorySizeCalculator& size = *pSize;

    size += sizeof(VertexLight);

    TransformNode::GetMemorySizeForInitialize(&size,resVertexLight,description);

    size += sizeof(ResVertexLightData);

    if (description.isAnimationEnabled &&
        resVertexLight.GetAnimGroupsCount() > 0){
        AnimGroup::Builder()
            .ResAnimGroup(resVertexLight.GetAnimGroups(0))
            .UseOriginalValue(true)
            .GetMemorySizeInternal(&size);
    }
}

}
}