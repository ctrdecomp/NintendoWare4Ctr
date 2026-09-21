// Filename: gfx_ParticleContext.cpp
//
// Project: NintendoWare4Ctr

#include <nw/gfx/gfx_ParticleContext.h>

namespace nw{
namespace gfx{

ParticleContext* ParticleContext::Builder::Create(nw::os::IAllocator* allocator)
{
    NW_NULL_ASSERT(allocator);

    VEC3Array emissionPositions(allocator);
    U16Array emissionParents(allocator);
    F32Array particleWorkF32(allocator);
    VEC3Array prevTranslate(allocator);

    if (m_IsFixedSizeMemory)
    {
        emissionPositions = VEC3Array(this->m_MaxEmission, allocator);
        emissionParents = U16Array(this->m_MaxEmission, allocator);
        particleWorkF32 = F32Array(this->m_MaxStreamLength, allocator);
        if (this->m_UseDoubleBuffer)
        {
            prevTranslate = VEC3Array(this->m_MaxStreamLength, allocator);
        }
    }
    else
    {
#ifndef NW_MOVE_ARRAY_VARIABILITY_ENABLED
        NW_FATAL_ERROR("Can't disable IsFixedSizeMemory without NW_MOVE_ARRAY_VARIABILITY_ENABLED");
#endif
    }

    void* memory = allocator->Alloc(sizeof(ParticleContext));
    NW_NULL_ASSERT(memory);

    ParticleContext* context = new(memory) ParticleContext(allocator, emissionPositions, emissionParents, particleWorkF32, prevTranslate);

    return context;
}

}
}