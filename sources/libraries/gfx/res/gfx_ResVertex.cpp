#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/gfx_SceneObject.h>
#include <nw/gfx/res/gfx_ResVertex.h>
#include <nw/gfx/gfx_Common.h>
#include <nw/gfx/gfx_GlImplement.h>
#include <GLES2/gl2.h>

#include <nn/gx.h>
#include <nn/gx/CTR/gx_CommandAccess.h>

namespace nw {
namespace gfx {
namespace res {

static void BufferData(u32 bufferID, ResVertexStreamBase resStream, u32 loadFlag){
    u32 size = resStream.GetStreamCount();
    GLenum transtype = loadFlag & 0xFFFF0000;
    void* address = NULL;
    const u32 NN_GX_MEM_MASK = 0x00030000;
    
    switch (transtype){
    case (NN_GX_MEM_FCRAM | GL_NO_COPY_FCRAM_DMP):
        nngxUpdateBuffer( resStream.GetStream(), size );
        break;
        
    case (NN_GX_MEM_VRAMA | GL_NO_COPY_FCRAM_DMP):
    case (NN_GX_MEM_VRAMB | GL_NO_COPY_FCRAM_DMP):{
            GLuint area = (transtype & NN_GX_MEM_MASK);
            address = __dmpgl_allocator(area, NN_GX_MEM_VERTEXBUFFER, bufferID, size);
            nngxUpdateBuffer( resStream.GetStream(), size );
            internal::nwgfxAddVramDmaCommand( resStream.GetStream(), address, size );
            
            resStream.SetLocationAddress( address );
            resStream.ref().mMemoryArea = area;
        }
        break;
        
    case (NN_GX_MEM_FCRAM | GL_COPY_FCRAM_DMP):{
            address = __dmpgl_allocator(NN_GX_MEM_FCRAM, NN_GX_MEM_VERTEXBUFFER, bufferID, size);
            nw::os::MemCpy( address, resStream.GetStream(), size );
            nngxUpdateBuffer( address, size );
            
            resStream.SetLocationAddress( address );
            resStream.ref().mMemoryArea = NN_GX_MEM_FCRAM;
        }
        break;
        
    case (NN_GX_MEM_VRAMA | GL_COPY_FCRAM_DMP):
    case (NN_GX_MEM_VRAMB | GL_COPY_FCRAM_DMP):
        NW_FATAL_ERROR("GL_COPY_DMA is not supported!");
        break;
        
    default:
        break;
    }
}

static void DeleteBuffer( u32 bufferID, ResVertexStreamBase resStream ){
    GLuint area = resStream.ref().mMemoryArea;
    
    if (area != ResVertexStreamBaseData::AREA_NO_MALLOC){
        void* address = reinterpret_cast<void*>( resStream.GetLocationAddress() );
        __dmpgl_deallocator( area, NN_GX_MEM_VERTEXBUFFER, bufferID, address );
        resStream.SetLocationAddress( static_cast<u32>(NULL) );
    }
}

/* ResVertexAttribute */

void ResVertexAttribute::Setup(){
    NW_ASSERT( this->IsValid() );
    
    if (this->GetFlags() & ResVertexAttributeData::FLAG_VERTEX_PARAM){
        ResStaticCast<ResVertexParamAttribute>( *this ).Setup();
    }

    else if (this->GetFlags() & ResVertexAttributeData::FLAG_INTERLEAVE){
        ResStaticCast<ResInterleavedVertexStream>( *this ).Setup();
    }

    else{
        ResStaticCast<ResVertexStream>(*this).Setup();
    }
}

void ResVertexAttribute::Cleanup(){
    NW_ASSERT(this->IsValid());
    
    if (this->GetFlags() & ResVertexAttributeData::FLAG_VERTEX_PARAM){
        ResStaticCast<ResVertexParamAttribute>( *this ).Cleanup();
    }

    else if (this->GetFlags() & ResVertexAttributeData::FLAG_INTERLEAVE){
        ResStaticCast<ResInterleavedVertexStream>(*this).Cleanup();
    }

    else{
        ResStaticCast<ResVertexStream>(*this).Cleanup();
    }
}

/* ResVertexParamAttribute */

void ResVertexParamAttribute::Setup() {}

void ResVertexParamAttribute::Cleanup() {}

/* ResVertexStream */

void ResVertexStream::Setup(){
    if (ref().mBufferObject){
        return;
    }
    
    ResVertexStreamData* dataPtr = ptr();
    u32 bufferID = reinterpret_cast<u32>( dataPtr );
    this->ref().mBufferObject = bufferID;

    if (this->GetLocationAddress() != NULL){
        ref().mMemoryArea = ResVertexStreamBaseData::AREA_NO_MALLOC;
        return;
    }
    
    u32 size = this->GetStreamCount();

    int loadFlag = this->GetLocationFlag();

    if (loadFlag == 0){
        loadFlag = NN_GX_MEM_FCRAM | GL_NO_COPY_FCRAM_DMP;
    }
    
    BufferData(bufferID, *this, loadFlag);
}

void ResVertexStream::Cleanup(){
    GLuint bufferID = this->GetBufferObject();
    
    if (bufferID == 0){
        return;
    }
    
    DeleteBuffer(bufferID, *this);
    this->SetBufferObject( 0 );
}

/* ResInterleavedVertexStream */
void ResInterleavedVertexStream::Setup(){
    if (ref().mBufferObject){
        return;
    }
    
    u32 bufferID = reinterpret_cast<u32>( this->ptr() );
    this->ref().mBufferObject = bufferID;

    if (this->GetLocationAddress() != NULL){
        ref().mMemoryArea = ResVertexStreamBaseData::AREA_NO_MALLOC;
        return;
    }
    
    u32 size = this->GetStreamCount();

    int loadFlag = this->GetLocationFlag();

    if (loadFlag == 0){
        loadFlag = NN_GX_MEM_FCRAM | GL_NO_COPY_FCRAM_DMP;
    }
    
    BufferData(bufferID, *this, loadFlag);
}

void ResInterleavedVertexStream::Cleanup(){
    GLuint bufferID = this->GetBufferObject();
    
    if (bufferID == 0){
        return;
    }
    
    DeleteBuffer(bufferID, *this);
    this->SetBufferObject(0);
}

}
}
}