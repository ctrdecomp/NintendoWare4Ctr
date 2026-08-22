

#include <nw/gfx/gfx_LookAtTargetViewUpdater.h>
#include <nw/os/os_Memory.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(LookAtTargetViewUpdater, CameraViewUpdater);

LookAtTargetViewUpdater::LookAtTargetViewUpdater(os::IAllocator* allocator,bool isDynamic,ResLookAtTargetViewUpdater resUpdater): 
    CameraViewUpdater(allocator, isDynamic),
    mResource(resUpdater)
{}

LookAtTargetViewUpdater::~LookAtTargetViewUpdater(){
    if (this->IsDynamic() && this->mResource.IsValid()){
        this->GetAllocator().Free(mResource.ptr());
    }
}

LookAtTargetViewUpdater* LookAtTargetViewUpdater::Create(os::IAllocator* allocator){
    NW_NULL_ASSERT(allocator);
    
    void* updaterMemory = allocator->Alloc(sizeof(LookAtTargetViewUpdater));
    NW_NULL_ASSERT(updaterMemory);

    void* dataMemory = AllocateAndFill<ResLookAtTargetViewUpdaterData>(allocator, 0);

    ResLookAtTargetViewUpdaterData* buffer = new(dataMemory) ResLookAtTargetViewUpdaterData();
    
    buffer->typeInfo         = ResLookAtTargetViewUpdater::TYPE_INFO;
    buffer->mTargetPosition = VIEW_TARGET_POSITION;
    buffer->mUpwardVector   = VIEW_UPWARD_VECTOR;
    buffer->mFlags          = 0x0;

    ResLookAtTargetViewUpdater resUpdater = ResLookAtTargetViewUpdater(buffer);

    return new(updaterMemory) LookAtTargetViewUpdater(allocator, true, resUpdater);
}

LookAtTargetViewUpdater* LookAtTargetViewUpdater::Create(os::IAllocator* allocator, ResLookAtTargetViewUpdater resUpdater){
    NW_NULL_ASSERT(allocator);
    
    void* updaterMemory = allocator->Alloc(sizeof(LookAtTargetViewUpdater));
    NW_NULL_ASSERT(updaterMemory);

    return new(updaterMemory) LookAtTargetViewUpdater(allocator, false, resUpdater);
}

void LookAtTargetViewUpdater::Update(math::MTX34* viewMatrix, const math::MTX34& worldMatrixconst math::VEC3& cameraPosition){
    NW_ASSERT(mResource.IsValid());

    u32 flags = mResource.GetFlags();
    math::VEC3 targetPosition(this->mResource.GetTargetPosition());
    math::VEC3 upwardVector(this->mResource.GetUpwardVector());

    if (ut::CheckFlagOr(flags,ResLookAtTargetViewUpdaterData::FLAG_INHERITING_TARGET_ROTATE | ResLookAtTargetViewUpdaterData::FLAG_INHERITING_TARGET_TRANSLATE |
        ResLookAtTargetViewUpdaterData::FLAG_INHERITING_UP_ROTATE)){
        math::MTX33 rotateMatrix;
        math::MTX34ToMTX33(&rotateMatrix, &worldMatrix);

        if (ut::CheckFlag(flags, ResLookAtTargetViewUpdaterData::FLAG_INHERITING_UP_ROTATE)){
            math::VEC3Transform(&upwardVector, &rotateMatrix, &upwardVector);
        }
        if (ut::CheckFlag(flags, ResLookAtTargetViewUpdaterData::FLAG_INHERITING_TARGET_ROTATE)){
            math::VEC3Transform(&targetPosition, &rotateMatrix, &targetPosition);
        }
        if (ut::CheckFlag(flags, ResLookAtTargetViewUpdaterData::FLAG_INHERITING_TARGET_TRANSLATE)){
            math::VEC3Add(&targetPosition, &targetPosition, &cameraPosition);
        }
    }

    NW_ASSERT(cameraPosition != targetPosition);
    NW_ASSERT(!upwardVector.IsZero());

    NW_ASSERT(!math::VEC3().Cross(upwardVector, targetPosition - cameraPosition).IsZero());

    math::MTX34LookAt(viewMatrix,&cameraPosition,&upwardVector,&targetPosition);
}

}
}