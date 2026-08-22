#include <nw/gfx/gfx_AimTargetViewUpdater.h>
#include <nw/os/os_Memory.h>
#include <nw/math/inlines/math_Matrix34.ipp>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(AimTargetViewUpdater, CameraViewUpdater);

AimTargetViewUpdater* AimTargetViewUpdater::Create(os::IAllocator* allocator){
    NW_NULL_ASSERT(allocator);
    
    void* updaterMemory = allocator->Alloc(sizeof(AimTargetViewUpdater));
    NW_NULL_ASSERT(updaterMemory);

    void* dataMemory
        = AllocateAndFill<ResAimTargetViewUpdaterData>(allocator, 0);

    ResAimTargetViewUpdaterData* buffer =
            new(dataMemory) ResAimTargetViewUpdaterData();

    buffer->typeInfo         = ResAimTargetViewUpdater::TYPE_INFO;
    buffer->mTargetPosition = VIEW_TARGET_POSITION;
    buffer->mTwist          = VIEW_TWIST;
    buffer->mFlags          = 0x0;

    ResAimTargetViewUpdater resUpdater = ResAimTargetViewUpdater(buffer);

    return new(updaterMemory) AimTargetViewUpdater(allocator, true, resUpdater);
}

AimTargetViewUpdater* AimTargetViewUpdater::Create(os::IAllocator* allocator,ResAimTargetViewUpdater resUpdater){
    NW_NULL_ASSERT(allocator);
    
    void* updaterMemory = allocator->Alloc(sizeof(AimTargetViewUpdater));
    NW_NULL_ASSERT(updaterMemory);

    return new(updaterMemory) AimTargetViewUpdater(allocator, false, resUpdater);
}

AimTargetViewUpdater::AimTargetViewUpdater(os::IAllocator* allocator,bool isDynamic,ResAimTargetViewUpdater resUpdater): 
    CameraViewUpdater(allocator, isDynamic),
    mResource(resUpdater)
{}

AimTargetViewUpdater::~AimTargetViewUpdater(){
    if (this->IsDynamic() && this->mResource.IsValid()){
        this->GetAllocator().Free(this->mResource.ptr());
    }
}

void  AimTargetViewUpdater::Update(math::MTX34* viewMatrix,const math::MTX34& worldMatrix,const math::VEC3& cameraPosition){
    NW_ASSERT(this->mResource.IsValid());
    u32 flags = this->mResource.GetFlags();

    if (ut::CheckFlagOr(
        flags,
        ResAimTargetViewUpdaterData::FLAG_INHERITING_TARGET_TRANSLATE |
        ResAimTargetViewUpdaterData::FLAG_INHERITING_TARGET_ROTATE)){
        math::VEC3 lookReverse(
            cameraPosition.x - this->mResource.GetTargetPosition().x,
            cameraPosition.y - this->mResource.GetTargetPosition().y,
            cameraPosition.z - this->mResource.GetTargetPosition().z);

        if ((lookReverse.x == 0.0f) && (lookReverse.z == 0.0f)){
            math::MTX34LookAtRad(viewMatrix, &cameraPosition, this->mResource.GetTwist(), &this->mResource.GetTargetPosition());
        }
        else{
            math::MTX33 rotateMatrix;
            math::MTX34ToMTX33(&rotateMatrix, &worldMatrix);

            math::VEC3 r(lookReverse.z, 0.0f, -lookReverse.x);

            math::VEC3Normalize(&lookReverse, &lookReverse);
            math::VEC3Normalize(&r, &r);

            math::VEC3 u;
            math::VEC3Cross(&u, &lookReverse, &r);

            f32 st, ct;
            math::SinCosRad(&st, &ct, this->mResource.GetTwist());
            math::VEC3 up;

            up.x    = ct * u.x - st * r.x;
            up.y    = ct * u.y;
            up.z    = ct * u.z - st * r.z;

            math::VEC3 targetPosition(this->mResource.GetTargetPosition());

            if (ut::CheckFlag(flags, ResAimTargetViewUpdaterData::FLAG_INHERITING_TARGET_ROTATE)){
                math::VEC3Transform(&targetPosition, &rotateMatrix, &targetPosition);
                math::VEC3Transform(&up, &rotateMatrix, &up);
            }
            if (ut::CheckFlag(flags, ResAimTargetViewUpdaterData::FLAG_INHERITING_TARGET_TRANSLATE)){
                math::VEC3Add(&targetPosition, &targetPosition, &cameraPosition);
            }

            NW_ASSERT(cameraPosition != targetPosition);

            math::MTX34LookAt(viewMatrix,&cameraPosition,&up,&targetPosition);
        }
    }
    else{
        math::MTX34LookAtRad(viewMatrix, &cameraPosition, this->mResource.GetTwist(), &this->mResource.GetTargetPosition());
    }
}

}
}