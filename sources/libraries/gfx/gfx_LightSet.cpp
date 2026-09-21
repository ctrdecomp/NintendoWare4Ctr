

#include <nw/gfx/gfx_LightSet.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_ROOT_DEFINITION(LightSet);

LightSet* LightSet::DynamicBuilder::Create(os::IAllocator* allocator)
{
    NW_NULL_ASSERT(allocator);

    void* memory = allocator->Alloc(sizeof(LightSet));
    NW_NULL_ASSERT(memory);
    LightSet* lightSet = new(memory) LightSet(allocator,ResLightSet(NULL),m_Description);
    
    if (m_Description.isFixedSizeMemory && m_Description.maxVertexLights != 0)
    {
        void* memoryArray = allocator->Alloc(sizeof(VertexLight*) * m_Description.maxVertexLights);
        lightSet->m_VertexLights = VertexLightArray(memoryArray, m_Description.maxVertexLights, allocator);
    }
    else{
        lightSet->m_VertexLights = VertexLightArray(allocator);
    }

    return lightSet;
}

LightSet* LightSet::Create(ResLightSet resource,os::IAllocator* allocator)
{
    NW_NULL_ASSERT(allocator);
    NW_ASSERT(resource.IsValid());
    
    void* memory = allocator->Alloc(sizeof(LightSet));
    NW_NULL_ASSERT(memory);

    LightSet::Description description;
    description.maxVertexLights = resource.GetLightsCount();

    LightSet* lightSet = new(memory) LightSet(allocator,resource,description);

    if (description.isFixedSizeMemory && description.maxVertexLights != 0)
    {
        void* memoryArray = allocator->Alloc(sizeof(VertexLight*) * description.maxVertexLights);
        lightSet->m_VertexLights = VertexLightArray(memoryArray, description.maxVertexLights, allocator);
    }
    else{
        lightSet->m_VertexLights = VertexLightArray(allocator);
    }

    return lightSet;
}

}
}