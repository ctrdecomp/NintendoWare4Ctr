/*---------------------------------------------------------------------------*
  Project:  NintendoWare

  Copyright (C)Nintendo/HAL Laboratory, Inc.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary
  information of Nintendo and/or its licensed developers and are protected by
  national and international copyright laws. They may not be disclosed to third
  parties or copied or duplicated in any form, in whole or in part, without the
  prior written consent of Nintendo.

  The content herein is highly confidential and should be handled accordingly.
 *---------------------------------------------------------------------------*/


#ifndef NW_GFX_SCENEBUILDER_H_
#define NW_GFX_SCENEBUILDER_H_

#include <nw/gfx/gfx_SceneObject.h>

#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 1301 // padding inserted in struct.
#pragma diag_suppress 2530 // padding added to end of struct.
#endif
namespace nw
{
namespace gfx
{

class SceneNode;

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class SceneBuilder
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(SceneBuilder);

public:
    //
    SceneBuilder()
    : m_IsFixedSizeMemory(true),
      m_MaxChildren(SceneObject::DEFAULT_MAX_CHILDREN),
      m_MaxCallbacks(SceneObject::DEFAULT_MAX_CALLBACKS),
      m_BufferOption(0),
      m_SharedMaterialModel(NULL),
      m_SharedMeshNodeVisibilityModel(NULL),
      m_MaxAnimObjectsPerGroup(1),
      m_ParticleSetMarginCount(0),
      m_IsAnimationEnabled(true)
    {}

    //
    ~SceneBuilder() {}

    //
    SceneBuilder& Resource(ResSceneObject resource) { m_Resource = resource; return *this; }

    //
    //
    //
    //
    //
    SceneBuilder& IsFixedSizeMemory(bool isFixedSizeMemory)
    {
        m_IsFixedSizeMemory = isFixedSizeMemory;
        return *this;
    }

    //
    SceneBuilder& MaxChildren(int maxChildren) { m_MaxChildren = maxChildren; return *this; }

    //
    SceneBuilder& MaxCallbacks(int maxCallbacks) { m_MaxCallbacks = maxCallbacks; return *this; }

    //
    //
    //
    //
    //
    //
    //
    //
    //
    SceneBuilder& BufferOption(bit32 bufferOption) { m_BufferOption = bufferOption; return *this; }

    //
    //
    //
    //
    //
    //
    //
    //
    SceneBuilder& SharedMaterialModel(Model* model) { m_SharedMaterialModel = model; return *this; }

    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    SceneBuilder& SharedMeshNodeVisibiltiyModel(Model* model) { m_SharedMeshNodeVisibilityModel = model; return *this; }

    //
    //
    //
    //
    //
    //
    SceneBuilder& MaxAnimObjectsPerGroup(s32 maxAnimObjects) { m_MaxAnimObjectsPerGroup = maxAnimObjects; return *this; }

    //
    //
    //
    //
    //
    SceneBuilder& ParticleSetMarginCount(int marginCount) { m_ParticleSetMarginCount = marginCount; return *this; }

    //
    //
    //
    //
    //
    //
    SceneBuilder& IsAnimationEnabled(bool isAnimationEnabled) { m_IsAnimationEnabled = isAnimationEnabled; return *this; }

    //
    //
    //
    //
    //
    //
    //
    //
    //
    SceneObject* CreateObject(os::IAllocator* allocator, os::IAllocator* deviceAllocator);

    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    SceneObject* CreateTree(os::IAllocator* allocator, os::IAllocator* deviceAllocator);

    //
    //
    //
    //
    //
    //
    //
    //
    size_t GetMemorySize(
        size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT
    ) const
    {
        os::MemorySizeCalculator size(alignment);

        BuildSceneObject(&size, NULL, NULL, m_Resource, NULL, NULL, false, true);

        return size.GetSizeWithPadding(alignment);
    }

    //
    //
    //
    //
    //
    //
    //
    //
    size_t GetDeviceMemorySize(
        size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT
    ) const
    {
        os::MemorySizeCalculator size(alignment);

        BuildSceneObject(NULL, &size, NULL, m_Resource, NULL, NULL, false, true);

        return size.GetSizeWithPadding(alignment);
    }

    //
    //
    //
    //
    //
    //
    //
    //
    size_t GetMemorySizeTree(
        size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT
    ) const
    {
        os::MemorySizeCalculator size(alignment);

        BuildSceneObject(&size, NULL, NULL, m_Resource, NULL, NULL, true, true);

        return size.GetSizeWithPadding(alignment);
    }

    //
    //
    //
    //
    //
    //
    //
    //
    size_t GetDeviceMemorySizeTree(
        size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT
    ) const
    {
        os::MemorySizeCalculator size(alignment);

        BuildSceneObject(NULL, &size, NULL, m_Resource, NULL, NULL, true, true);

        return size.GetSizeWithPadding(alignment);
    }

private:

    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    SceneObject* BuildSceneObject(
        os::MemorySizeCalculator* pSize,
        os::MemorySizeCalculator* pDeviceSize,
        SceneNode* parent,
        ResSceneObject resource,
        os::IAllocator* allocator,
        os::IAllocator* deviceAllocator,
        bool isRecursive,
        bool isCalculation) const;

    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    void BuildChildren(
        os::MemorySizeCalculator* pSize,
        os::MemorySizeCalculator* pDeviceSize,
        SceneNode* parent,
        ResSceneObject resource,
        os::IAllocator* allocator,
        os::IAllocator* deviceAllocator,
        bool isCalculation) const;

    ResSceneObject m_Resource;
    bool m_IsFixedSizeMemory;
    int m_MaxChildren;
    int m_MaxCallbacks;
    bit32 m_BufferOption;
    Model* m_SharedMaterialModel;
    Model* m_SharedMeshNodeVisibilityModel;

    int m_MaxAnimObjectsPerGroup;
    int m_ParticleSetMarginCount;

    bool m_IsAnimationEnabled;
};

} // namespace gfx
} // namespace nw
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 2530 // padding added to end of struct.
#pragma diag_default 1301 // padding inserted in struct.
#endif

#endif // NW_GFX_SCENEBUILDER_H_
