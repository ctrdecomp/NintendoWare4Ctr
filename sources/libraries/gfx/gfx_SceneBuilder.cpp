#include <nw/gfx/gfx_SceneBuilder.h>

#include <nw/gfx/gfx_SceneNode.h>
#include <nw/gfx/gfx_TransformNode.h>
#include <nw/gfx/gfx_Model.h>
#include <nw/gfx/gfx_SkeletalModel.h>
#include <nw/gfx/gfx_ParticleModel.h>
#include <nw/gfx/gfx_ParticleEmitter.h>
#include <nw/gfx/gfx_ParticleSet.h>
#include <nw/gfx/gfx_FragmentLight.h>
#include <nw/gfx/gfx_VertexLight.h>
#include <nw/gfx/gfx_AmbientLight.h>
#include <nw/gfx/gfx_HemiSphereLight.h>
#include <nw/gfx/gfx_Fog.h>
#include <nw/gfx/gfx_Camera.h>
#include <nw/gfx/gfx_SceneEnvironmentSetting.h>
#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>

namespace nw {
namespace gfx {

SceneObject* SceneBuilder::CreateObject(os::IAllocator* allocator,os::IAllocator* deviceAllocator)
{
    NW_NULL_ASSERT(allocator);
    NW_ASSERT(this->m_Resource.IsValid());

    SceneObject* root = BuildSceneObject(NULL, NULL, 0, this->m_Resource, allocator, deviceAllocator, false, false);
    
    return root;
}

SceneObject* SceneBuilder::CreateTree(os::IAllocator* allocator,os::IAllocator* deviceAllocator)
{
    NW_NULL_ASSERT(allocator);
    NW_ASSERT(this->m_Resource.IsValid());

    SceneObject* root = BuildSceneObject(NULL, NULL, 0, this->m_Resource, allocator, deviceAllocator, true, false);
    
    return root;
}

SceneObject* SceneBuilder::BuildSceneObject(os::MemorySizeCalculator* pSize,os::MemorySizeCalculator* pDeviceSize,SceneNode* parent,ResSceneObject resource,os::IAllocator* allocator,os::IAllocator* deviceAllocator,bool isRecursive,bool isCalculation) const
{
    NW_ASSERT( allocator != NULL || isCalculation );

    if (!resource.IsValid()) { return 0; }
    
    SceneObject* object = 0;
        
    switch (resource.GetTypeInfo())
    {
    case ResSceneNode::TYPE_INFO:{
            SceneNode::Description description;
            description.isFixedSizeMemory = m_IsFixedSizeMemory;
            description.maxCallbacks = m_MaxCallbacks;
            description.maxChildren = m_MaxChildren;
            description.maxAnimObjectsPerGroup = m_MaxAnimObjectsPerGroup;
            description.isAnimationEnabled = m_IsAnimationEnabled;

            if (isCalculation)
            {
                if (pSize)
                {
                    SceneNode::GetMemorySizeInternal(pSize, ResStaticCast<ResSceneNode>(resource), description);
                }
                if (pDeviceSize)
                {
                    SceneNode::GetDeviceMemorySizeInternal(pDeviceSize, ResStaticCast<ResSceneNode>(resource), description);
                }
            }
            else{
                SceneNode* node = SceneNode::Create(
                    parent, resource, description, allocator);
                object = node;
            }
        }
        break;
        
    case ResTransformNode::TYPE_INFO:{
            TransformNode::Description description;
            description.isFixedSizeMemory = m_IsFixedSizeMemory;
            description.maxCallbacks = m_MaxCallbacks;
            description.maxChildren = m_MaxChildren;
            description.maxAnimObjectsPerGroup = m_MaxAnimObjectsPerGroup;
            description.isAnimationEnabled = m_IsAnimationEnabled;

            if (isCalculation)
            {
                if (pSize)
                {
                    TransformNode::GetMemorySizeInternal(pSize, ResStaticCast<ResTransformNode>(resource), description);
                }
                if (pDeviceSize)
                {
                    TransformNode::GetDeviceMemorySizeInternal(pDeviceSize, ResStaticCast<ResTransformNode>(resource), description);
                }
            }
            else{
                TransformNode* node = TransformNode::Create(parent,resource,description,allocator);
                object = node;
            }
        }
        break;
        
    case ResModel::TYPE_INFO:{
            Model::Description description;
            description.isFixedSizeMemory = m_IsFixedSizeMemory;
            description.maxCallbacks = m_MaxCallbacks;
            description.maxChildren = m_MaxChildren;
            description.bufferOption = m_BufferOption;
            description.sharedMaterialModel = m_SharedMaterialModel;
            description.isAnimationEnabled = m_IsAnimationEnabled;

            description.maxAnimObjectsPerGroup = m_MaxAnimObjectsPerGroup;

            if (isCalculation)
            {
                if (pSize)
                {
                    Model::GetMemorySizeInternal(pSize, ResStaticCast<ResModel>(resource), description);
                }
                if (pDeviceSize)
                {
                    Model::GetDeviceMemorySizeInternal(pDeviceSize, ResStaticCast<ResModel>(resource), description);
                }
            }
            else{
                Model* node = Model::Create(parent, resource, description, allocator);
                object = node;
            }
        }
        break;
        
    case ResSkeletalModel::TYPE_INFO:{
            SkeletalModel::Builder builder;
            builder
                .IsFixedSizeMemory(this->m_IsFixedSizeMemory)
                .MaxCallbacks(this->m_MaxCallbacks)
                .MaxChildren(this->m_MaxChildren)
                .BufferOption(this->m_BufferOption)
                .SharedMaterialModel(this->m_SharedMaterialModel)
                .MaxAnimObjectsPerGroup(this->m_MaxAnimObjectsPerGroup)
                .IsAnimationEnabled(this->m_IsAnimationEnabled);
            if (isCalculation)
            {
                if (pSize)
                {
                    builder.GetMemorySizeInternal(pSize, ResStaticCast<ResSkeletalModel>(resource));
                }
                if (pDeviceSize)
                {
                    builder.GetDeviceMemorySizeInternal(pDeviceSize, ResStaticCast<ResSkeletalModel>(resource));
                }
            }
            else{
                SkeletalModel* node = builder.Create(parent, resource, allocator);
                object = node;
            }
        }
        break;
        
    case ResParticleModel::TYPE_INFO:{
            ResParticleModel resNode = ResDynamicCast<ResParticleModel>(resource);
            NW_ASSERT(resNode.IsValid());

            ParticleModel::Description description;
            description.isFixedSizeMemory = m_IsFixedSizeMemory;
            description.maxCallbacks = m_MaxCallbacks;
            description.bufferOption = m_BufferOption;
            description.sharedMaterialModel = m_SharedMaterialModel;
            description.maxChildren = resNode.GetChildrenCount() + resNode.GetParticleSetsCount() + m_ParticleSetMarginCount;
            description.particleSetCount = resNode.GetParticleSetsCount() + m_ParticleSetMarginCount;
            description.isAnimationEnabled = m_IsAnimationEnabled;

            if (isCalculation)
{ 
                if (pSize)
                {
                    ParticleModel::GetMemorySizeInternal(pSize, resNode, description);
                }
                if (pDeviceSize)
                {
                    ParticleModel::GetDeviceMemorySizeInternal(pDeviceSize, resNode, description);
                }
            }
            else{
                ParticleModel* node = ParticleModel::Create(parent, resource, description, allocator, deviceAllocator);
                object = node;
            }
        }
        break;
        
    case ResParticleEmitter::TYPE_INFO:{
            ParticleEmitter::Description description;
            description.isFixedSizeMemory = m_IsFixedSizeMemory;
            description.maxCallbacks = m_MaxCallbacks;
            description.maxChildren = m_MaxChildren;
            description.isAnimationEnabled = m_IsAnimationEnabled;

            if (isCalculation)
            {
                if (pSize)
                {
                    ParticleEmitter::GetMemorySizeInternal(pSize, ResStaticCast<ResParticleEmitter>(resource), description);
                }
                if (pDeviceSize)
                {
                    ParticleEmitter::GetDeviceMemorySizeInternal(pDeviceSize, ResStaticCast<ResParticleEmitter>(resource), description);
                }
            }
            else{
                ParticleEmitter* node = ParticleEmitter::Create(parent, resource, description, allocator);
                object = node;
            }
        }
        break;
        
    case ResFragmentLight::TYPE_INFO:{
            FragmentLight::Description description;
            description.isFixedSizeMemory = m_IsFixedSizeMemory;
            description.maxCallbacks = m_MaxCallbacks;
            description.maxChildren = m_MaxChildren;
            description.maxAnimObjectsPerGroup = m_MaxAnimObjectsPerGroup;
            description.isAnimationEnabled = m_IsAnimationEnabled;

            if (isCalculation)
            {
                if (pSize)
                {
                    FragmentLight::GetMemorySizeInternal(pSize, ResStaticCast<ResFragmentLight>(resource), description);
                }
                if (pDeviceSize)
                {
                    FragmentLight::GetDeviceMemorySizeInternal(pDeviceSize, ResStaticCast<ResFragmentLight>(resource), description);
                }
            }
            else{
                FragmentLight* node = FragmentLight::Create(parent, resource, description, allocator);
                object = node;
            }
        }
        break;

    case ResVertexLight::TYPE_INFO:{
            VertexLight::Description description;
            description.isFixedSizeMemory = m_IsFixedSizeMemory;
            description.maxCallbacks = m_MaxCallbacks;
            description.maxChildren = m_MaxChildren;
            description.maxAnimObjectsPerGroup = m_MaxAnimObjectsPerGroup;
            description.isAnimationEnabled = m_IsAnimationEnabled;

            if (isCalculation)
            {
                if (pSize)
                {
                    VertexLight::GetMemorySizeInternal(pSize, ResStaticCast<ResVertexLight>(resource), description);
                }
                if (pDeviceSize)
                {
                    VertexLight::GetDeviceMemorySizeInternal(pDeviceSize, ResStaticCast<ResVertexLight>(resource), description);
                }
            }
            else{
                VertexLight* node = VertexLight::Create(parent, resource, description, allocator);
                object = node;
            }
        }
        break;
    case ResAmbientLight::TYPE_INFO:{
            AmbientLight::Description description;
            description.isFixedSizeMemory = m_IsFixedSizeMemory;
            description.maxCallbacks = m_MaxCallbacks;
            description.maxChildren = m_MaxChildren;
            description.maxAnimObjectsPerGroup = m_MaxAnimObjectsPerGroup;
            description.isAnimationEnabled = m_IsAnimationEnabled;

            if (isCalculation)
            {
                if (pSize)
                {
                    AmbientLight::GetMemorySizeInternal(pSize, ResStaticCast<ResAmbientLight>(resource), description);
                }
                if (pDeviceSize)
                {
                    AmbientLight::GetDeviceMemorySizeInternal(pDeviceSize, ResStaticCast<ResAmbientLight>(resource), description);
                }
            }
            else{
                AmbientLight* node = AmbientLight::Create(parent, resource, description, allocator);
                object = node;
            }
        }
        break;

    case ResHemiSphereLight::TYPE_INFO:{
            HemiSphereLight::Description description;
            description.isFixedSizeMemory = m_IsFixedSizeMemory;
            description.maxCallbacks = m_MaxCallbacks;
            description.maxChildren = m_MaxChildren;
            description.maxAnimObjectsPerGroup = m_MaxAnimObjectsPerGroup;
            description.isAnimationEnabled = m_IsAnimationEnabled;

            if (isCalculation)
            {
                if (pSize)
                {
                    HemiSphereLight::GetMemorySizeInternal(pSize, ResStaticCast<ResHemiSphereLight>(resource), description);
                }
                if (pDeviceSize)
                {
                    HemiSphereLight::GetDeviceMemorySizeInternal(pDeviceSize, ResStaticCast<ResHemiSphereLight>(resource), description);
                }
            }
            else{
                HemiSphereLight* node = HemiSphereLight::Create(parent, resource, description, allocator);
                object = node;
            }
        }
        break;

    case ResFog::TYPE_INFO:{
            Fog::Description description;
            description.isFixedSizeMemory = m_IsFixedSizeMemory;
            description.maxCallbacks = m_MaxCallbacks;
            description.maxChildren = m_MaxChildren;
            description.isAnimationEnabled = m_IsAnimationEnabled;

            if (isCalculation)
            {
                if (pSize)
                {
                    Fog::GetMemorySizeInternal(pSize, ResStaticCast<ResFog>(resource), description);
                }
                if (pDeviceSize)
                {
                    Fog::GetDeviceMemorySizeInternal(pDeviceSize, ResStaticCast<ResFog>(resource), description);
                }
            }
            else{
                Fog* node = Fog::Create(parent, resource, description, allocator);
                object = node;
            }
        }
        break;

    case ResCamera::TYPE_INFO:{
            Camera::Description description;
            description.isFixedSizeMemory = m_IsFixedSizeMemory;
            description.maxCallbacks = m_MaxCallbacks;
            description.maxChildren = m_MaxChildren;
            description.maxAnimObjectsPerGroup = m_MaxAnimObjectsPerGroup;
            description.isAnimationEnabled = m_IsAnimationEnabled;

            if (isCalculation)
            {
                if (pSize)
                {
                    Camera::GetMemorySizeInternal(pSize, ResStaticCast<ResCamera>(resource), description);
                }
                if (pDeviceSize)
                {
                    Camera::GetDeviceMemorySizeInternal(pDeviceSize, ResStaticCast<ResCamera>(resource), description);
                }
            }
            else{
                Camera* node = Camera::Create(parent, resource, description, allocator);
                object = node;
            }
        }
        break;

    case ResSceneEnvironmentSetting::TYPE_INFO:{
            SceneEnvironmentSetting::Description description;

            if (isCalculation)
            {
                if (pSize)
                {
                    SceneEnvironmentSetting::GetMemorySizeInternal(pSize, ResStaticCast<ResSceneEnvironmentSetting>(resource), description);
                }
                if (pDeviceSize) {}
            }
            else{
                SceneEnvironmentSetting* node = SceneEnvironmentSetting::Create(resource, description, allocator);
                object = node;
            }
        }
        break;
        
    default: NW_FATAL_ERROR("Unknown resource type."); break;
    }

    if (isCalculation)
    {
        if (isRecursive)
        {
            BuildChildren(pSize, pDeviceSize, NULL, resource, NULL, NULL, true);
        }
    }
    else
    {
        SceneNode* sceneNode = nw::ut::DynamicCast<SceneNode*>(object);

        if (sceneNode != NULL && isRecursive)
        {
            BuildChildren(NULL, NULL, sceneNode, resource, allocator, deviceAllocator, false);
        }
    }

    return object;
}

void SceneBuilder::BuildChildren(os::MemorySizeCalculator* pSize,os::MemorySizeCalculator* pDeviceSize,SceneNode* parent,ResSceneObject resource,os::IAllocator* allocator,os::IAllocator* deviceAllocator,bool isCalculation) const
{
    NW_NULL_ASSERT(allocator);
    
    ResSceneNode resNode = ResSceneNode(resource.ptr());
    if (!resource.IsValid()) { return; }

    typedef nw::ut::ResArrayClass<ResSceneObject>::type ResSceneObjectArray;
    for (ResSceneObjectArray::iterator i = resNode.GetChildren().begin(); i != resNode.GetChildren().end(); ++i)
    {
        if (isCalculation)
        {
            BuildSceneObject(pSize, pDeviceSize, NULL, resource, NULL, NULL, true, true);
        }
        else{
            SceneObject* child = BuildSceneObject(NULL, NULL, parent, *i, allocator, deviceAllocator, true, false);
        }
    }
}


}
}