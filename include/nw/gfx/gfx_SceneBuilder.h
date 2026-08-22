#pragma once

#include <nw/gfx/gfx_SceneObject.h>

namespace nw{
namespace os{
    class IAllocator;
}
namespace gfx{

class SceneNode;

class SceneBuilder{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(SceneBuilder);

public:
    SceneBuilder():
        mIsFixedSizeMemory(true),
        mMaxChildren(SceneObject::DEFAULT_MAX_CHILDREN),
        mMaxCallbacks(SceneObject::DEFAULT_MAX_CALLBACKS),
        mBufferOption(0),
        mSharedMaterialModel(NULL),
        mMaxAnimObjectsPerGroup(1),
        mParticleSetMarginCount(0),
        mIsAnimationEnabled(true)
    {}

    ~SceneBuilder() {}

    SceneBuilder& Resource(ResSceneObject resource) { mResource = resource; return *this; }

    SceneBuilder& IsFixedSizeMemory(bool isFixedSizeMemory){
        mIsFixedSizeMemory = isFixedSizeMemory;
        return *this;
    }

    SceneBuilder& MaxChildren(int maxChildren) { mMaxChildren = maxChildren; return *this; }

    SceneBuilder& MaxCallbacks(int maxCallbacks) { mMaxCallbacks = maxCallbacks; return *this; }

    SceneBuilder& BufferOption(bit32 bufferOption) { mBufferOption = bufferOption; return *this; }

    SceneBuilder& SharedMaterialModel(Model* model) { mSharedMaterialModel = model; return *this; }

    SceneBuilder& MaxAnimObjectsPerGroup(s32 maxAnimObjects) { mMaxAnimObjectsPerGroup = maxAnimObjects; return *this; }

    SceneBuilder& ParticleSetMarginCount(int marginCount) { mParticleSetMarginCount = marginCount; return *this; }

    SceneBuilder& IsAnimationEnabled(bool isAnimationEnabled) { mIsAnimationEnabled = isAnimationEnabled; return *this; }

    SceneObject* CreateObject(nw::os::IAllocator* allocator, nw::os::IAllocator* deviceAllocator);

    SceneObject* CreateTree(nw::os::IAllocator* allocator, nw::os::IAllocator* deviceAllocator);

    size_t GetMemorySize(size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT) const{
        nw::os::MemorySizeCalculator size(alignment);
        BuildSceneObject(&size, NULL, NULL, mResource, NULL, NULL, false, true);
        return size.GetSizeWithPadding(alignment);
    }

    size_t GetDeviceMemorySize(size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT) const{
        nw::os::MemorySizeCalculator size(alignment);
        BuildSceneObject(NULL, &size, NULL, mResource, NULL, NULL, false, true);
        return size.GetSizeWithPadding(alignment);
    }

    size_t GetMemorySizeTree(size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT) const{
        nw::os::MemorySizeCalculator size(alignment);
        BuildSceneObject(&size, NULL, NULL, mResource, NULL, NULL, true, true);
        return size.GetSizeWithPadding(alignment);
    }

    size_t GetDeviceMemorySizeTree(size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT) const{
        nw::os::MemorySizeCalculator size(alignment);
        BuildSceneObject(NULL, &size, NULL, mResource, NULL, NULL, true, true);
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

    ResSceneObject mResource;
    bool mIsFixedSizeMemory;
    int mMaxChildren;
    int mMaxCallbacks;
    bit32 mBufferOption;
    Model* mSharedMaterialModel;

    int mMaxAnimObjectsPerGroup;
    int mParticleSetMarginCount;

    bool mIsAnimationEnabled;
};

}
}