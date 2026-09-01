// Filename: gfx_ResModel.cpp
//
// Project: NintendoWare4Ctr

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/gfx_SceneObject.h>
#include <nw/gfx/res/gfx_ResShape.h>
#include <nw/gfx/res/gfx_ResModel.h>
#include <nw/gfx/res/gfx_ResMaterial.h>
#include <nw/gfx/res/gfx_ResGraphicsFile.h>
#include <nw/gfx/gfx_Common.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2extern.h>

namespace nw {
namespace gfx {
namespace res {

Result ResModel::Setup(os::IAllocator* allocator, ResGraphicsFile graphicsFile){
    Result result = RESOURCE_RESULT_OK;
    NW_ASSERT(this->IsValid());
    NW_ASSERT(internal::ResCheckRevision(*this));
    NW_ASSERT(internal::ResCheckRevision(graphicsFile));
    
    // Setup Shape
    s32 shapeNum = this->GetShapesCount();
    for (int i = 0; i < shapeNum; ++i){
        if (!ut::CheckFlag(this->GetShapes(i).GetFlags(), ResShape::FLAG_HAS_BEEN_SETUP)){
            result |= this->GetShapes(i).Setup(allocator);
        }
    }

    // Setup Material
    s32 materialNum = this->GetMaterialsCount();
    for (int i = 0; i < materialNum; ++i){
        if (!ut::CheckFlag(this->GetMaterials(i).GetFlags(), ResMaterialData::FLAG_HAS_BEEN_SETUP)){
            result |= this->GetMaterials(i).Setup(allocator, graphicsFile);
        }
    }
    
    // Setup Mesh
    if ((result.GetDescription() & RESOURCE_RESULT_NOT_FOUND_SHADER) == 0){
        s32 meshNum = this->GetMeshesCount();
        for (int i = 0; i < meshNum; ++ i){
            ResMesh mesh = this->GetMeshes( i );
            if (!ut::CheckFlag(mesh.GetFlags(), ResMesh::FLAG_HAS_BEEN_SETUP)){
                mesh.Setup( *this, allocator, graphicsFile );
            }
        }
    }

    return result;
}

void ResModel::Cleanup(){
    // Cleanup Shape
    s32 shapeNum = this->GetShapesCount();
    for(int i = 0; i < shapeNum; ++i){
        if (ut::CheckFlag(this->GetShapes(i).GetFlags(), ResShape::FLAG_HAS_BEEN_SETUP)){
            this->GetShapes(i).Cleanup();
        }
    }

    // Cleanup Material
    s32 materialNum = this->GetMaterialsCount();
    for(int i = 0; i < materialNum; ++i){
        this->GetMaterials(i).Cleanup();
    }

    // Cleanup Mesh
    s32 meshNum = this->GetMeshesCount();
    for(int i = 0; i < meshNum; ++ i){
        this->GetMeshes(i).Cleanup();
    }
}

}
}
}