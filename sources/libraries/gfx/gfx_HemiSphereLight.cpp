// Filename: gfx_HemiSphereLight.cpp
//
// Project: NintendoWare4Ctr

#include <nw/os/os_Memory.h>

#include <nw/gfx/gfx_HemiSphereLight.h>
#include <nw/gfx/gfx_ISceneVisitor.h>

#include <cstring>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(HemiSphereLight, Light);

HemiSphereLight* HemiSphereLight::DynamicBuilder::Create(nw::os::IAllocator* allocator)
{
    NW_NULL_ASSERT(allocator);

    ResPtr resource(CreateResHemiSphereLight(allocator), ResHemiSphereLightDataDestroyer(allocator));

    void* memory = allocator->Alloc(sizeof(HemiSphereLight));
    NW_NULL_ASSERT(memory);
    HemiSphereLight* light = new(memory) HemiSphereLight(allocator,resource,this->m_Description);
    
    Result result = light->Initialize(allocator);
    NW_ASSERT(result.IsSuccess());

    return light;
}

size_t HemiSphereLight::DynamicBuilder::GetMemorySize(size_t alignment) const
{
    NW_ASSERT(this->m_Description.isFixedSizeMemory);

    os::MemorySizeCalculator size(alignment);

    size += sizeof(ResHemiSphereLightData);
    size += sizeof(HemiSphereLight);

    TransformNode::GetMemorySizeForInitialize(&size,ResTransformNode(),this->m_Description);

    size += sizeof(ResHemiSphereLightData);

    return size.GetSizeWithPadding(alignment);
}

HemiSphereLight* HemiSphereLight::Create(SceneNode* parent,ResSceneObject resource,const HemiSphereLight::Description& description,os::IAllocator* allocator)
{
    NW_NULL_ASSERT(allocator);
    
    ResHemiSphereLight resNode = ResDynamicCast<ResHemiSphereLight>(resource);
    NW_ASSERT(resNode.IsValid());
    NW_ASSERT(internal::ResCheckRevision(resNode));
    
    void* memory = allocator->Alloc(sizeof(HemiSphereLight));
    NW_NULL_ASSERT(memory);    

    HemiSphereLight* light = new(memory) HemiSphereLight(allocator,resNode,description);
    
    Result result = light->Initialize(allocator);
    NW_ASSERT(result.IsSuccess());

    if (parent)
    {
        bool isAttached = parent->AttachChild(light);
        NW_ASSERT(isAttached);
    }

    return light;
}

void HemiSphereLight::Accept(ISceneVisitor* visitor)
{
    visitor->VisitHemiSphereLight(this);
    AcceptChildren(visitor);
}

ResHemiSphereLightData* HemiSphereLight::CreateResHemiSphereLight(os::IAllocator* allocator, const char* name)
{
    ResHemiSphereLightData* resHemiSphereLight = AllocateAndFillN<ResHemiSphereLightData>(allocator, sizeof(ResHemiSphereLightData), 0);

    resHemiSphereLight->typeInfo = ResHemiSphereLight::TYPE_INFO;
    resHemiSphereLight->m_Header.revision = ResLight::BINARY_REVISION;
    resHemiSphereLight->m_Header.signature = ResLight::SIGNATURE;
    
    resHemiSphereLight->m_UserDataDicCount = 0;
    resHemiSphereLight->toUserDataDic.set_ptr( NULL );
    
    resHemiSphereLight->toName.set_ptr(AllocateAndCopyString(name, allocator, MAX_NAME_LENGTH));

    resHemiSphereLight->m_ChildrenTableCount = 0;
    resHemiSphereLight->toChildrenTable.set_ptr(NULL);
    resHemiSphereLight->m_AnimGroupsDicCount = 0;
    resHemiSphereLight->toAnimGroupsDic.set_ptr(NULL);

    const math::VEC3 scale(1.0f, 1.0f, 1.0f);
    const math::VEC3 rotate(0.0f, 0.0f, 0.0f);
    const math::VEC3 translate(0.0f, 0.0f, 0.0f);
    resHemiSphereLight->m_Transform = math::Transform3(scale, rotate, translate);
    resHemiSphereLight->m_WorldMatrix = math::MTX34::Identity();
    ResTransformNode(resHemiSphereLight).SetBranchVisible(true);

    return resHemiSphereLight;
}

void HemiSphereLight::DestroyResHemiSphereLight(os::IAllocator* allocator, ResHemiSphereLightData* resHemiSphereLight)
{
    NW_NULL_ASSERT(allocator);
    NW_NULL_ASSERT(resHemiSphereLight);
    
    if (resHemiSphereLight->toName.to_ptr() != NULL)
    {
        allocator->Free(const_cast<char*>( resHemiSphereLight->toName.to_ptr()));
    }
    allocator->Free(resHemiSphereLight);
}

Result HemiSphereLight::CreateOriginalValue(os::IAllocator* allocator)
{
    Result result = INITIALIZE_RESULT_OK;

    void* buffer = allocator->Alloc(sizeof(ResHemiSphereLightData));
    NW_NULL_ASSERT(buffer);

    ResHemiSphereLightData* originalValue = new(buffer) ResHemiSphereLightData(GetResHemiSphereLight().ref());
    m_OriginalValue = ResHemiSphereLight(originalValue);

    m_OriginalTransform = this->GetResTransformNode().GetTransform();

    return result;
}

Result HemiSphereLight::Initialize(os::IAllocator* allocator)
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

void HemiSphereLight::GetMemorySizeInternal( os::MemorySizeCalculator* pSize,ResHemiSphereLight resHemiSphereLight,Description description)
{
    NW_ASSERT(description.isFixedSizeMemory);

    os::MemorySizeCalculator& size = *pSize;

    size += sizeof(HemiSphereLight);

    TransformNode::GetMemorySizeForInitialize(&size,resHemiSphereLight,description);

    size += sizeof(ResHemiSphereLightData);

    if (description.isAnimationEnabled && resHemiSphereLight.GetAnimGroupsCount() > 0)
    {
        AnimGroup::Builder().ResAnimGroup(resHemiSphereLight.GetAnimGroups(0)).UseOriginalValue(true).GetMemorySizeInternal(&size);
    }
}

}
}