#include <nw/gfx/gfx_Skeleton.h>

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(Skeleton, SceneObject);

void Skeleton::CreateCallbacks(nw::os::IAllocator* allocator, int maxCallbacks, bool isFixedSizeMemory){
    if (isFixedSizeMemory){
        if (maxCallbacks == 0){
            mPreCalculateMatrixSignal = CalculateMatrixSignal::CreateInvalidateSignal(allocator);
            mPostCalculateMatrixSignal = CalculateMatrixSignal::CreateInvalidateSignal(allocator);
        }
        else{
            mPreCalculateMatrixSignal = CalculateMatrixSignal::CreateFixedSizedSignal(maxCallbacks, allocator);
            mPostCalculateMatrixSignal = CalculateMatrixSignal::CreateFixedSizedSignal(maxCallbacks, allocator);
        }
    }
    else{
        mPreCalculateMatrixSignal = CalculateMatrixSignal::CreateVariableSizeSignal(allocator);
        mPostCalculateMatrixSignal = CalculateMatrixSignal::CreateVariableSizeSignal(allocator);
    }
}

}
}