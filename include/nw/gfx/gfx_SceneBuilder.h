#pragma once

#include <nw/gfx/gfx_SceneObject.h>

namespace nw{
namespace os{
    class IAllocator;
}
namespace gfx{

class SceneNode;

class SceneBuilder
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(SceneBuilder);

public:
    SceneBuilder():
        m_IsFixedSizeMemory(true),
        m_MaxChildren(SceneObject::DEFAULT_MAX_CHILDREN),
        m_MaxCallbacks(SceneObject::DEFAULT_MAX_CALLBACKS),
        m_BufferOption(0),
        m_SharedMaterialModel(NULL),
        m_MaxAnimObjectsPerGroup(1),
        m_ParticleSetMarginCount(0),
        m_IsAnimationEnabled(true) {}

    ~SceneBuilder() {}

    SceneBuilder& Resource(ResSceneObject resource) { m_Resource = resource; return *this; }

    SceneBuilder& IsFixedSizeMemory(bool isFixedSizeMemory)
    {
        m_IsFixedSizeMemory = isFixedSizeMemory;
        return *this;
    }

    SceneBuilder& MaxChildren(int maxChildren) { m_MaxChildren = maxChildren; return *this; }

    SceneBuilder& MaxCallbacks(int maxCallbacks) { m_MaxCallbacks = maxCallbacks; return *this; }

    SceneBuilder& BufferOption(bit32 bufferOption) { m_BufferOption = bufferOption; return *this; }

    SceneBuilder& SharedMaterialModel(Model* model) { m_SharedMaterialModel = model; return *this; }

    SceneBuilder& MaxAnimObjectsPerGroup(s32 maxAnimObjects) { m_MaxAnimObjectsPerGroup = maxAnimObjects; return *this; }

    SceneBuilder& ParticleSetMarginCount(int marginCount) { m_ParticleSetMarginCount = marginCount; return *this; }

    SceneBuilder& IsAnimationEnabled(bool isAnimationEnabled) { m_IsAnimationEnabled = isAnimationEnabled; return *this; }

    SceneObject* CreateObject(nw::os::IAllocator* allocator, nw::os::IAllocator* deviceAllocator);

    SceneObject* CreateTree(nw::os::IAllocator* allocator, nw::os::IAllocator* deviceAllocator);

    size_t GetMemorySize(size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT) const
    {
        nw::os::MemorySizeCalculator size(alignment);
        BuildSceneObject(&size, NULL, NULL, m_Resource, NULL, NULL, false, true);
        return size.GetSizeWithPadding(alignment);
    }

    size_t GetDeviceMemorySize(size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT) const
    {
        nw::os::MemorySizeCalculator size(alignment);
        BuildSceneObject(NULL, &size, NULL, m_Resource, NULL, NULL, false, true);
        return size.GetSizeWithPadding(alignment);
    }

    size_t GetMemorySizeTree(size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT) const
    {
        nw::os::MemorySizeCalculator size(alignment);
        BuildSceneObject(&size, NULL, NULL, m_Resource, NULL, NULL, true, true);
        return size.GetSizeWithPadding(alignment);
    }

    size_t GetDeviceMemorySizeTree(size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT) const
    {
        nw::os::MemorySizeCalculator size(alignment);
        BuildSceneObject(NULL, &size, NULL, m_Resource, NULL, NULL, true, true);
        return size.GetSizeWithPadding(alignment);
    }

private:
    SceneObject* BuildSceneObject(
        nw::os::MemorySizeCalculator* pSize,
        nw::os::MemorySizeCalculator* pDeviceSize,
        SceneNode* parent,
        ResSceneObject resource,
        nw::os::IAllocator* allocator,
        nw::os::IAllocator* deviceAllocator,
        bool isRecursive,
        bool isCalculation) const;

    void BuildChildren(
        nw::os::MemorySizeCalculator* pSize,
        nw::os::MemorySizeCalculator* pDeviceSize,
        SceneNode* parent,
        ResSceneObject resource,
        nw::os::IAllocator* allocator,
        nw::os::IAllocator* deviceAllocator,
        bool isCalculation) const;

    ResSceneObject m_Resource;
    bool m_IsFixedSizeMemory;
    int m_MaxChildren;
    int m_MaxCallbacks;
    bit32 m_BufferOption;
    Model* m_SharedMaterialModel;

    int m_MaxAnimObjectsPerGroup;
    int m_ParticleSetMarginCount;

    bool m_IsAnimationEnabled;
};

}
}