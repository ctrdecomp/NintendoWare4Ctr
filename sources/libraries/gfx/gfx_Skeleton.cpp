// Filename: gfx_Skeleton.cpp
//
// Project: NintendoWare4Ctr

#include <nw/gfx/gfx_Skeleton.h>

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(Skeleton, SceneObject);

void Skeleton::CreateCallbacks(nw::os::IAllocator* allocator, int maxCallbacks, bool isFixedSizeMemory)
{
    if (isFixedSizeMemory)
    {
        if (maxCallbacks == 0)
        {
            m_PreCalculateMatrixSignal = CalculateMatrixSignal::CreateInvalidateSignal(allocator);
            m_PostCalculateMatrixSignal = CalculateMatrixSignal::CreateInvalidateSignal(allocator);
        }
        else
        {
            m_PreCalculateMatrixSignal = CalculateMatrixSignal::CreateFixedSizedSignal(maxCallbacks, allocator);
            m_PostCalculateMatrixSignal = CalculateMatrixSignal::CreateFixedSizedSignal(maxCallbacks, allocator);
        }
    }
    else{
        m_PreCalculateMatrixSignal = CalculateMatrixSignal::CreateVariableSizeSignal(allocator);
        m_PostCalculateMatrixSignal = CalculateMatrixSignal::CreateVariableSizeSignal(allocator);
    }
}

}
}