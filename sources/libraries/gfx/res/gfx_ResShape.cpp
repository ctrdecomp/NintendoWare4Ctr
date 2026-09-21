// Filename: gfx_ResShape.cpp
//
// Project: NintendoWare4Ctr

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/gfx_SceneObject.h>
#include <nw/gfx/gfx_ActivateCommand.h>
#include <nw/gfx/res/gfx_ResShape.h>
#include <nw/gfx/res/gfx_ResParticleShape.h>
#include <nw/gfx/res/gfx_ResTexture.h>
#include <nw/gfx/gfx_Common.h>
#include <nw/Assert.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2extern.h>
#include <nw/gfx/gfx_CommandUtil.h>

#include <nn/gx.h>

namespace nw {
namespace gfx {
namespace res {

typedef void (*SetupFunc)(nw::os::IAllocator* allocator, ResShape resShape);
typedef void (*CleanupFunc)(ResShape resShape);

static void ResShape_Setup(nw::os::IAllocator* allocator, ResShape resShape);
static void ResSeparateDataShape_Setup(nw::os::IAllocator* allocator, ResShape resShape);
static void ResParticleShape_Setup(nw::os::IAllocator* allocator, ResShape resShape);

static void ResShape_Cleanup(ResShape resShape);
static void ResSeparateDataShape_Cleanup(ResShape resShape);
static void ResParticleShape_Cleanup(ResShape resShape);

static SetupFunc s_Shape_SetupTable[] = 
{
    ResSeparateDataShape_Setup,
    ResParticleShape_Setup
};

static CleanupFunc s_Shape_CleanupTable[] = 
{
    ResSeparateDataShape_Cleanup,
    ResParticleShape_Cleanup
};

static void BufferData(u32 bufferID, ResIndexStream resStream, u32 loadFlag)
{
    u32 size = resStream.GetStreamCount();
    GLenum transtype = loadFlag & 0xFFFF0000;
    void* address = NULL;
    const u32 NN_GX_MEM_MASK = 0x00030000;

    switch (transtype)
    {
    case (NN_GX_MEM_FCRAM | GL_NO_COPY_FCRAM_DMP):
        nngxUpdateBuffer( resStream.GetStream(), size );
        break;
        
    case (NN_GX_MEM_VRAMA | GL_NO_COPY_FCRAM_DMP):
    case (NN_GX_MEM_VRAMB | GL_NO_COPY_FCRAM_DMP):
    {
            GLuint area = (transtype & NN_GX_MEM_MASK);
            address = __dmpgl_allocator(area, NN_GX_MEM_VERTEXBUFFER, bufferID, size);
            nngxAddVramDmaCommand(resStream.GetStream(), address, size);
            
            resStream.SetLocationAddress(address);
            resStream.ref().m_MemoryArea = area;
        }
        break;
        
    case (NN_GX_MEM_FCRAM | GL_COPY_FCRAM_DMP):
    {
            address = __dmpgl_allocator(NN_GX_MEM_FCRAM, NN_GX_MEM_VERTEXBUFFER, bufferID, size);
            nw::os::MemCpy(address, resStream.GetStream(), size);
            nngxUpdateBuffer(address, size);
            
            resStream.SetLocationAddress(address);
            resStream.ref().m_MemoryArea = NN_GX_MEM_FCRAM;
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

static void DeleteBuffer(u32 bufferID, ResIndexStream resStream)
{
    GLuint area = resStream.ref().m_MemoryArea;
    
    if (area != ResIndexStreamData::AREA_NO_MALLOC)
    {
        void* address = reinterpret_cast<void*>(resStream.GetLocationAddress());
        __dmpgl_deallocator( area, NN_GX_MEM_VERTEXBUFFER, bufferID, address);
        resStream.SetLocationAddress(static_cast<u32>(NULL));
    }
}

Result ResShape::Setup(nw::os::IAllocator* allocator)
{
    Result result = RESOURCE_RESULT_OK;
    switch (this->ref().typeInfo)
    {
    case ResSeparateDataShape::TYPE_INFO:{
            s_Shape_SetupTable[0]( allocator, *this );
            this->SetFlags(nw::ut::EnableFlag(this->GetFlags(), ResShape::FLAG_HAS_BEEN_SETUP));
        }
        break;
    case ResParticleShape::TYPE_INFO:{
            s_Shape_SetupTable[1]( allocator, *this );
            this->SetFlags(nw::ut::EnableFlag(this->GetFlags(), ResShape::FLAG_HAS_BEEN_SETUP));
        }
        break;
    default: {}
    }
    return result;
}

static void ResShape_Setup(nw::os::IAllocator* allocator, ResShape resShape)
{
    s32 primSetNum = resShape.GetPrimitiveSetsCount();
    
    for (int i = 0; i < primSetNum; ++i )
    {
        resShape.GetPrimitiveSets( i ).Setup(allocator);
    }
}

static void ResSeparateDataShape_Setup(nw::os::IAllocator* allocator, ResShape resShape)
{
    ResShape_Setup( allocator, resShape );
    
    ResSeparateDataShape resSeparateShape = ResStaticCast<ResSeparateDataShape>( resShape );
    
    s32 vtxAttrNum = resSeparateShape.GetVertexAttributesCount();
    for (int i = 0; i < vtxAttrNum; ++ i)
    {
        resSeparateShape.GetVertexAttributes(i).Setup();
    }
}

static void ResParticleShape_Setup(nw::os::IAllocator* allocator, ResShape resShape) { }

/* ResPrimitiveSet */

void ResPrimitiveSet::Setup(nw::os::IAllocator* allocator)
{
    NW_ASSERT(this->IsValid());
    
    ResPrimitiveArray primitiveArray = this->GetPrimitives();
    
    for (ResPrimitiveArray::iterator it = primitiveArray.begin(); it != primitiveArray.end(); ++it)
    {
        (*it).Setup(allocator);
    }
}

/* ResPrimitiveSetup */

void ResPrimitive::Setup(nw::os::IAllocator* allocator)
{
    NW_ASSERT(this->IsValid());
    
    if (allocator == NULL) { allocator = CommandCacheManager::GetAllocator(); }
    
    this->ref().m_CommandAllocator = allocator;
    u32 streamNum = this->GetIndexStreamsCount();
    
    GLuint* bufferObjects = reinterpret_cast<GLuint*>(this->GetBufferObjects());
    
    for (uint i = 0; i < streamNum; ++i)
    {
        ResIndexStream resStream = this->GetIndexStreams(u32(i));
        bufferObjects[ i ] = reinterpret_cast<u32>(resStream.ptr());
        
        s32 commandSize = internal::CalcSetupDrawIndexStreamCommand(resStream);
        resStream.ref().m_CommandCache = allocator->Alloc(commandSize, 4);
        resStream.ref().m_CommandCacheSize = commandSize;
        
        if (resStream.GetLocationAddress() != NULL)
        {
            resStream.ref().m_MemoryArea = ResIndexStreamData::AREA_NO_MALLOC;
            continue;
        }
        
        u32   size = resStream.GetStreamCount();
        void* buf  = resStream.GetStream();

        int loadFlag = resStream.GetLocationFlag();

        if (loadFlag == 0)
        {
            loadFlag = NN_GX_MEM_FCRAM | GL_NO_COPY_FCRAM_DMP;
        }
        
        BufferData(bufferObjects[i], resStream, loadFlag);
    }
}

void ResPrimitive::SetupDrawCommand(bool hasGeometryShader)
{
    ResIndexStreamArray indexStreams = this->GetIndexStreams();
    
    ResIndexStreamArray::iterator end = indexStreams.end();
    for (ResIndexStreamArray::iterator stream = indexStreams.begin(); stream != end; ++stream)
    {
        ResIndexStream indexStream = *stream;
        NW_NULL_ASSERT(indexStream.ref().m_CommandCache);

        internal::CommandBufferInfo bufferInfo(
            indexStream.ref().m_CommandCache,
            indexStream.ref().m_CommandCacheSize );
        
        s32 resultSize = internal::SetupDrawIndexStreamCommand(bufferInfo, indexStream, hasGeometryShader);
        NW_ASSERT(resultSize == indexStream.ref().m_CommandCacheSize);
    }
    
    ref().m_Flags |= ResPrimitive::FLAG_COMMAND_HAS_BEEN_SETUP;
}

/* ResShape */

void ResShape::Cleanup()
{
    switch (this->ref().typeInfo)
    {
    case ResSeparateDataShape::TYPE_INFO:{
            s_Shape_CleanupTable[0](*this);
            this->SetFlags(nw::ut::DisableFlag(this->GetFlags(), ResShape::FLAG_HAS_BEEN_SETUP));
        }
        break;
    case ResParticleShape::TYPE_INFO:{
            s_Shape_CleanupTable[1](*this);
            this->SetFlags(nw::ut::DisableFlag(this->GetFlags(), ResShape::FLAG_HAS_BEEN_SETUP));
        }
        break;
    default: { }
    }
}

static void ResShape_Cleanup(ResShape resShape)
{
    s32 primSetNum = resShape.GetPrimitiveSetsCount();
    
    for (s32 i = 0; i < primSetNum; ++i)
    {
        resShape.GetPrimitiveSets(i).Cleanup();
    }
}

static void ResSeparateDataShape_Cleanup(ResShape resShape)
{
    ResSeparateDataShape resSeparateShape = ResStaticCast<ResSeparateDataShape>(resShape);
    
    s32 vtxAttrNum = resSeparateShape.GetVertexAttributesCount();
    
    for (int i = 0; i < vtxAttrNum; ++ i)
    {
        resSeparateShape.GetVertexAttributes( i ).Cleanup();
    }
    
    ResShape_Cleanup(resShape);
}


static void ResParticleShape_Cleanup(ResShape resShape)
{
    ResParticleShape resParticleShape = ResStaticCast<ResParticleShape>( resShape );
}


/* ResPrimitiveSet */

void ResPrimitiveSet::Cleanup()
{
    NW_ASSERT(this->IsValid());
    
    ResPrimitiveArray primitiveArray = this->GetPrimitives();
    
    for (ResPrimitiveArray::iterator it = primitiveArray.begin(); it != primitiveArray.end(); ++it)
    {
        (*it).Cleanup();
    }
}

/* ResPrimitive */

void ResPrimitive::Cleanup()
{
    NW_ASSERT(this->IsValid());
    
    u32 streamNum = this->GetIndexStreamsCount();
    
    GLuint* bufferObjects = reinterpret_cast<GLuint*>(this->GetBufferObjects());
    
    for (uint i = 0; i < streamNum; ++i)
    {
        ResIndexStream resStream = this->GetIndexStreams( u32(i) );
        if (resStream.IsValid() && resStream.ref().m_CommandCache)
        {
            this->ref().m_CommandAllocator->Free(resStream.ref().m_CommandCache);
            resStream.ref().m_CommandCache = NULL;
            resStream.ref().m_CommandCacheSize = 0;
        }
        
        DeleteBuffer(bufferObjects[i], resStream);
    }
    
    std::memset(bufferObjects, 0, sizeof(GLuint) * streamNum);
    
    ref().m_CommandAllocator = NULL;
    ref().m_Flags &= ~FLAG_COMMAND_HAS_BEEN_SETUP;
}

/* ResSeperateDataShape */

u32 ResSeparateDataShape::GetVertexCount()
{
    int attributesCount = this->GetVertexAttributesCount();
    for (int i = 0 ; i < attributesCount ; ++i)
    {
        u32 vertexCount = this->GetVertexAttributes(i).GetVertexCount();

        if (vertexCount)
        {
            return vertexCount;
        }
    }
    
    NW_FATAL_ERROR("Vertex stream not found\n");

    return 0;
}

}
}
}