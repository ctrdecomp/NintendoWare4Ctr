#include <nw/gfx/gfx_RotateViewUpdater.h>
#include <nw/os/os_Memory.h>

#include <nw/math/inlines/math_Matrix34.ipp>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(RotateViewUpdater, CameraViewUpdater);

RotateViewUpdater* RotateViewUpdater::Create(os::IAllocator* allocator){
    NW_NULL_ASSERT(allocator);
    
    void* updaterMemory = allocator->Alloc(sizeof(RotateViewUpdater));
    NW_NULL_ASSERT(updaterMemory);

    void* dataMemory
        = AllocateAndFill<ResRotateViewUpdaterData>(allocator, 0);

    ResRotateViewUpdaterData* buffer = new(dataMemory) ResRotateViewUpdaterData();

    buffer->typeInfo     = ResRotateViewUpdater::TYPE_INFO;
    buffer->mViewRotate = VIEW_VIEW_ROTATE;
    buffer->mFlags      = 0x0;

    ResRotateViewUpdater resUpdater = ResRotateViewUpdater(buffer);


    return new(updaterMemory) RotateViewUpdater(allocator, true, resUpdater);
}

RotateViewUpdater* RotateViewUpdater::Create(os::IAllocator* allocator, ResRotateViewUpdater resUpdater){
    NW_NULL_ASSERT(allocator);
    
    void* updaterMemory = allocator->Alloc(sizeof(RotateViewUpdater));
    NW_NULL_ASSERT(updaterMemory);

    return new(updaterMemory) RotateViewUpdater(allocator, false, resUpdater);
}

RotateViewUpdater::RotateViewUpdater(os::IAllocator* allocator,bool isDynamic,ResRotateViewUpdater resUpdater): 
    CameraViewUpdater(allocator, isDynamic),
    mResource(resUpdater)
{}

RotateViewUpdater::~RotateViewUpdater(){
    if (this->IsDynamic() && this->mResource.IsValid()){
        this->GetAllocator().Free(this->mResource.ptr());
    }
}

void  RotateViewUpdater::Update(math::MTX34* viewMatrix,const math::MTX34& worldMatrix,const math::VEC3& cameraPosition){
    NW_ASSERT(this->mResource.IsValid());
    u32 flags = this->mResource.GetFlags();
    if (ut::CheckFlag(flags, ResRotateViewUpdaterData::FLAG_INHERITING_ROTATE)){
        math::MTX33 rotateMatrix;
        math::MTX34ToMTX33(&rotateMatrix, &worldMatrix);

        math::VEC3 upwardVector(0.0f,1.0f,0.0f);
        math::VEC3 targetPosition(0.0f,0.0f,-1.0f);

        math::MTX34 transformMatrix;
        math::MTX34 rotateMatrixX;
        math::MTX34 rotateMatrixY;
        math::MTX34 rotateMatrixZ;
        math::MTX34RotXYZRad(&rotateMatrixX, this->mResource.GetViewRotate().x, 0.0f, 0.0f);
        math::MTX34RotXYZRad(&rotateMatrixY, 0.0f, this->mResource.GetViewRotate().y, 0.0f);
        math::MTX34RotXYZRad(&rotateMatrixZ, 0.0f, 0.0f, this->mResource.GetViewRotate().z);

        math::MTX34Mult(&transformMatrix, &rotateMatrixY, &rotateMatrixX);
        math::MTX34Mult(&transformMatrix, &transformMatrix, &rotateMatrixZ);

        math::VEC3Transform(&upwardVector, &transformMatrix, &upwardVector);
        math::VEC3Transform(&upwardVector, &rotateMatrix, &upwardVector);

        math::VEC3Transform(&targetPosition, &transformMatrix, &targetPosition);
        math::VEC3Transform(&targetPosition, &rotateMatrix, &targetPosition);
        math::VEC3Add(&targetPosition, &targetPosition, &cameraPosition);

        math::MTX34LookAt(viewMatrix,&cameraPosition,&upwardVector,&targetPosition);
    }
    else{
        math::MTX34CameraRotateRad(viewMatrix, &cameraPosition, &this->mResource.GetViewRotate());
    }
}

}
}