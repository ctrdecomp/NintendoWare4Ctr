#include <nn/gx.h>

#include <nw/gfx/gfx_ParticleShape.h>
#include <nw/gfx/gfx_ParticleUtil.h>
#include <nw/gfx/gfx_particleModel.h>
#include <nw/gfx/gfx_ParticleSet.h>
#include <nw/gfx/gfx_ParticleContext.h>
#include <nw/gfx/gfx_ParticleEmitter.h>
#include <nw/gfx/gfx_ISceneVisitor.h>
#include <nw/gfx/res/gfx_ResVertex.h>

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/ut/ut_Foreach.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(ParticleShape, SceneObject);

static const int MAX_ATTRIBUTES_NUM = 12;

static void SetupParticleVertexAttributeCommand(ParticleShape* shape,ParticleSet* particleSet,ParticleBuffer bufferSide){
    ParticleCollection* collection = particleSet->GetParticleCollection();

    enum{
        REG_VTX_SHADER_ATTR_NUM   = 0x2b9,
        REG_VTX_SHADER_ATTR_NUM_2 = 0x242,
        REG_VTX_MAP_0             = 0x2bb,
        REG_VTX_MAP_1             = 0x2bc,
        REG_VTX_STREAM_BASE       = 0x200,
        REG_VTX_ARRAY_OFFSET      = 0x203,
        REG_VTX_PARAM_INDEX       = 0x232,
        REG_GEOM_MAP_0            = 0x28b,
        REG_GEOM_MAP_1            = 0x28c
    };

    u32 baseAddr = nngxGetPhysicalAddr(nn::gx::CTR::GetVramStartAddr(nn::gx::CTR::MEM_VRAMA));

    s32 vtxAttrNum = shape->GetVertexAttributesCount();

    u32* command = reinterpret_cast<u32*>(shape->mCommandCache[bufferSide]);

    const u32 HEADER_VTX_SHADER_ATTR_NUM   = internal::MakeCommandHeader(REG_VTX_SHADER_ATTR_NUM, 1, false, 0xb);
    const u32 HEADER_VTX_SHADER_ATTR_NUM_2 = internal::MakeCommandHeader(REG_VTX_SHADER_ATTR_NUM_2, 1, false, 0x1);
    const u32 HEADER_VTX_MAP_0             = internal::MakeCommandHeader(REG_VTX_MAP_0, 2, true, 0xF);


    command[0] = (vtxAttrNum - 1) | 0xa0000000;
    command[1] = HEADER_VTX_SHADER_ATTR_NUM;

    command[2] = (vtxAttrNum - 1);
    command[3] = HEADER_VTX_SHADER_ATTR_NUM_2;

    command[4] = 0;
    command[5] = HEADER_VTX_MAP_0;
    command[6] = 0;
    command[7] = 0;

    NW_ASSERT((baseAddr & 0x0f) == 0);
    command[8] = baseAddr >> 3;
    command[9] = internal::MakeCommandHeader(REG_VTX_STREAM_BASE, 3 + vtxAttrNum * 3, true, 0xF);
    command[10] = 0;
    command[11] = static_cast<u32>(vtxAttrNum - 1) << 28;

    u32* inputTable  = &command[4];
    u32* inputFormat = &command[10];
    u32* vertexParamMask = &command[11];

    int inputIndex = 0;
    u32 commandIndex = 12;

    u32 usedFlag = 0;

    for (s32 i = 0; i < shape->GetVertexAttributesCount(); ++i){
        if (shape->IsVertexStream(i)){
            s32 usage = shape->GetVertexAttributeUsage(i);
            NW_ASSERT(0 <= usage && usage < 12);
            inputTable[ (inputIndex / 8) * 2 ] |= (usage & 0xF) << (4 * (inputIndex % 8));
            usedFlag |= 0x1 << usage;

            u32 format    = shape->GetVertexAttributeFormatType(i);
            u32 dimension = shape->GetVertexAttributeDimension(i);

            inputFormat[ inputIndex / 8 ] |= internal::CommandCacheHelper::GetVertexFormat(dimension, format) << ((inputIndex % 8) * 4);

            u8* streamPtr = shape->GetVertexStreamPtr(i, bufferSide);
            NW_NULL_ASSERT(streamPtr);
            u32 bufferAddr = nngxGetPhysicalAddr(reinterpret_cast<uptr>(streamPtr));

            shape->SetVertexAttributeCommandPtr(i, bufferSide, &command[commandIndex]);

            NW_ASSERT((bufferAddr - baseAddr) < 0x10000000);
            command[commandIndex++] = bufferAddr - baseAddr;
            command[commandIndex++] = inputIndex;
            command[commandIndex++] = (internal::CommandCacheHelper::GetVertexSize(dimension, format) << 16) + (1 <<  28);

            ++inputIndex;
        }
    }

    for (s32 i = inputIndex; i < vtxAttrNum; ++i){
        command[commandIndex++] = 0;
        command[commandIndex++] = 0;
        command[commandIndex++] = 0;
    }

    if ((commandIndex & 1) == 1){
        command[commandIndex++] = 0;
    }

    for (s32 i = 0; i < shape->GetVertexAttributesCount(); ++i){
        if (!shape->IsVertexStream(i)){
            s32 usage = shape->GetVertexAttributeUsage(i);
            NW_ASSERT(0 <= usage && usage < 12);
            inputTable[ (inputIndex / 8) * 2 ] |= (usage & 0xF) << (4 * (inputIndex % 8));
            usedFlag |= 0x1 << usage;

            u32 data[4] = { 0, 0, 0, 0 };

            int count = shape->GetVertexAttributeDimension(i);
            f32* fdata = shape->GetVertexParameter(i);

            for (int j = 0; j < count; ++j){
                data[j] = ut::Float24::Float32ToBits24(fdata[j]);
            }

            const u32 HEADER_VTX_PARAM_INDEX = internal::MakeCommandHeader(REG_VTX_PARAM_INDEX, 4, true, 0xF);

            shape->SetVertexAttributeCommandPtr(i, bufferSide, &command[commandIndex + 2]);

            command[commandIndex]     = inputIndex;
            command[commandIndex + 1] = HEADER_VTX_PARAM_INDEX;
            command[commandIndex + 2] = (data[3] <<  8) | (data[2] >> 16);
            command[commandIndex + 3] = (data[2] << 16) | (data[1] >> 8);
            command[commandIndex + 4] = (data[1] << 24) | (data[0]);
            command[commandIndex + 5] = 0;

            vertexParamMask[0] |= 1 << (16 + inputIndex);

            ++inputIndex;
            commandIndex += 6;
        }
    }

    const u32 HEADER_GEOM_MAP_0 = internal::MakeCommandHeader(REG_GEOM_MAP_0, 2, true, 0xF);

    command[commandIndex + 0] = 0x76543210;
    command[commandIndex + 1] = HEADER_GEOM_MAP_0;
    command[commandIndex + 2] = 0xfedcba98;
    command[commandIndex + 3] = 0;
    commandIndex += 4;

    shape->mCommandCacheSize[bufferSide] = commandIndex * sizeof(u32);
}

static void SetupDeactivateParticleVertexAttributeCommand(ParticleShape* shape){
    enum{
        REG_VTX_ARRAY_OFFSET      = 0x203,
        REG_VTX_PARAM_INDEX       = 0x232
    };

    s32 vtxAttrNum = shape->GetVertexAttributesCount();

    u32* command = reinterpret_cast<u32*>(shape->mDeactivateVertexCommandCache);

    shape->mDeactivateVertexCommandCache = command;

    int inputIndex = 0;
    u32 commandIndex = 0;

    for (s32 i = 0; i < vtxAttrNum; ++i){
        if (shape->IsVertexStream(i)){
            command[commandIndex++] = 0;
            command[commandIndex++] = internal::MakeCommandHeader(REG_VTX_ARRAY_OFFSET + 2 + 3 * inputIndex, 1, false, 0xF);

            ++inputIndex;
        }
    }
    
    const u32 HEADER_VTX_PARAM_INDEX = internal::MakeCommandHeader(REG_VTX_PARAM_INDEX, 4, true, 0xF);

    for ( int i = 0; i < inputIndex; ++i ){
        command[commandIndex++] = i;
        command[commandIndex++] = HEADER_VTX_PARAM_INDEX;
        command[commandIndex++] = 0;
        command[commandIndex++] = 0;
        command[commandIndex++] = 0;
        command[commandIndex++] = 0;
    }
    
    shape->mDeactivateVertexCommandCacheSize = commandIndex * sizeof(u32);
    
    NW_ASSERT(shape->mDeactivateVertexCommandCacheSize <=  sizeof(u32) * (MAX_ATTRIBUTES_NUM * 8) );
}

static const int PrimitiveCommandSize = 26;

static void CreatePrimitiveCommandCache(ParticleShape* shape,ResParticleSet resParticleSet){
    enum{
        REG_VERTEX_UNIFORM_BOOL     = 0x2b0,

        REG_ELEMENTS_MODE           = 0x229,
        REG_ELEMENTS_MODE_2         = 0x253,
        REG_ELEMENTS_MODE_3         = 0x25e,
        REG_TRIANGLE_INDEX_RESET    = 0x25f,
        REG_DRAW_READY              = 0x245,
        REG_DRAW_KICK               = 0x22f,
        REG_VERTEX_CACHE_CLEAR      = 0x231,
        REG_COLOR_DEPTH_CACHE_CLEAR = 0x110,
        REG_COLOR_DEPTH_CACHE_FLUSH = 0x111
    };

    u32* command = reinterpret_cast<u32*>(shape->mPrimitiveCommandCache);

    u32 commandIndex = 0;

    s32 type;
    switch (resParticleSet.GetParticleShapeBuilder().GetTypeInfo()){
    case ResParticleBillboardShapeBuilder::TYPE_INFO:
        type = 0;
        break;
    case ResParticleWorldBillboardShapeBuilder::TYPE_INFO:
        type = 8;
        break;
    case ResParticleYBillboardShapeBuilder::TYPE_INFO:
        type = 2 | 1;
        break;
    case ResParticleXyPlaneShapeBuilder::TYPE_INFO:
        type = 4 | 1;
        break;
    default:
        NW_FATAL_ERROR("unknown shapebuilder type");
        break;
    }

    const u32 HEADER_VERTEX_UNIFORM_BOOL     = internal::MakeCommandHeader(REG_VERTEX_UNIFORM_BOOL, 5, true, 0xF);
    
    const u32 HEADER_ELEMENTS_MODE           = internal::MakeCommandHeader(REG_ELEMENTS_MODE, 1, false, 0x2);
    const u32 HEADER_ELEMENTS_MODE_2         = internal::MakeCommandHeader(REG_ELEMENTS_MODE_2, 1, false, 0x2);
    const u32 HEADER_ELEMENTS_MODE_3         = internal::MakeCommandHeader(REG_ELEMENTS_MODE_3, 1, false, 0x2);
    const u32 HEADER_TRIANGLE_INDEX_RESET    = internal::MakeCommandHeader(REG_TRIANGLE_INDEX_RESET, 1, false, 0xF);
    const u32 HEADER_DRAW_READY              = internal::MakeCommandHeader(REG_DRAW_READY, 1, false, 0xF);
    const u32 HEADER_DRAW_KICK               = internal::MakeCommandHeader(REG_DRAW_KICK, 1, false, 0xF);
    const u32 HEADER_VERTEX_CACHE_CLEAR      = internal::MakeCommandHeader(REG_VERTEX_CACHE_CLEAR, 1, false, 0xF);
    const u32 HEADER_COLOR_DEPTH_CACHE_FLUSH = internal::MakeCommandHeader(REG_COLOR_DEPTH_CACHE_FLUSH, 1, false, 0xF);
    const u32 HEADER_COLOR_DEPTH_CACHE_CLEAR = internal::MakeCommandHeader(REG_COLOR_DEPTH_CACHE_CLEAR, 1, false, 0xF);

    command[commandIndex++] = 0x7fff0000 | (type << 1);
    command[commandIndex++] = HEADER_VERTEX_UNIFORM_BOOL;
    command[commandIndex++] = 0;
    command[commandIndex++] = 0;
    command[commandIndex++] = 0;
    command[commandIndex++] = 0;

    command[commandIndex++] = 0;
    command[commandIndex++] = HEADER_ELEMENTS_MODE;
    command[commandIndex++] = 0;
    command[commandIndex++] = HEADER_ELEMENTS_MODE_2;
    command[commandIndex++] = 3 << 8;
    command[commandIndex++] = HEADER_ELEMENTS_MODE_3;
    command[commandIndex++] = 1;
    command[commandIndex++] = HEADER_TRIANGLE_INDEX_RESET;
    command[commandIndex++] = 0;
    command[commandIndex++] = HEADER_DRAW_READY;
    command[commandIndex++] = 1;
    command[commandIndex++] = HEADER_DRAW_KICK;
    command[commandIndex++] = 1;
    command[commandIndex++] = HEADER_DRAW_READY;
    command[commandIndex++] = 1;
    command[commandIndex++] = HEADER_VERTEX_CACHE_CLEAR;
    command[commandIndex++] = 1;
    command[commandIndex++] = HEADER_COLOR_DEPTH_CACHE_FLUSH;
    command[commandIndex++] = 1;
    command[commandIndex++] = HEADER_COLOR_DEPTH_CACHE_CLEAR;

    NW_ASSERT(commandIndex == PrimitiveCommandSize);

    shape->mPrimitiveCommandCacheSize = commandIndex * sizeof(u32);
}


/* ParticleShape */
void ParticleShape::CreateCommandCache(ParticleSet* particleSet){
    for (int side = 0; side < 2; ++side){
        SetupParticleVertexAttributeCommand(this,particleSet,(ParticleBuffer)side);
    }
    
    SetupDeactivateParticleVertexAttributeCommand(this);
    
    CreatePrimitiveCommandCache(this,particleSet->GetResParticleSet());
}

void ParticleShape::GetMemorySizeInternal(os::MemorySizeCalculator* pSize,int capacity){
    os::MemorySizeCalculator& size = *pSize;

    const int streamSize = (capacity + internal::PARTICLE_SIMD_WIDTH_MAX) * sizeof(u16);
    const int drawCommandCacheSize = sizeof(u32) * (12 + MAX_ATTRIBUTES_NUM * 6 + MAX_ATTRIBUTES_NUM * 4 + 4);
    const int primitiveCommandCacheSize = sizeof(u32) * PrimitiveCommandSize;
    const int deactivateVertexCommandCacheSize = sizeof(u32) * (MAX_ATTRIBUTES_NUM * 8);

    int deviceMemorySize = 0;
    if (size.GetAlignment() < 32){
        deviceMemorySize += 32 - size.GetAlignment();
    }

    deviceMemorySize = ut::RoundUp(deviceMemorySize, 32);
    deviceMemorySize += streamSize;
    deviceMemorySize = ut::RoundUp(deviceMemorySize, 32);
    deviceMemorySize += streamSize;

    int nodeMemorySize = sizeof(ParticleShape);
    nodeMemorySize = ut::RoundUp(nodeMemorySize, 32);
    nodeMemorySize += drawCommandCacheSize;
    nodeMemorySize = ut::RoundUp(nodeMemorySize, 32);
    nodeMemorySize += drawCommandCacheSize;
    nodeMemorySize = ut::RoundUp(nodeMemorySize, 32);
    nodeMemorySize += deactivateVertexCommandCacheSize;
    nodeMemorySize = ut::RoundUp(nodeMemorySize, 32);
    nodeMemorySize += primitiveCommandCacheSize;
    nodeMemorySize = ut::RoundUp(nodeMemorySize, 32);

    size.Add(nodeMemorySize, 32);
}

void ParticleShape::GetDeviceMemorySizeInternal(os::MemorySizeCalculator* pSize,int capacity){
    os::MemorySizeCalculator& size = *pSize;

    const int streamSize = (capacity + internal::PARTICLE_SIMD_WIDTH_MAX) * sizeof(u16);
    const int drawCommandCacheSize = sizeof(u32) * (12 + MAX_ATTRIBUTES_NUM * 6 + MAX_ATTRIBUTES_NUM * 4 + 4);
    const int primitiveCommandCacheSize = sizeof(u32) * PrimitiveCommandSize;
    const int deactivateVertexCommandCacheSize = sizeof(u32) * (MAX_ATTRIBUTES_NUM * 8);

    int deviceMemorySize = 0;
    if (size.GetAlignment() < 32){
        deviceMemorySize += 32 - size.GetAlignment();
    }

    deviceMemorySize = ut::RoundUp(deviceMemorySize, 32);
    deviceMemorySize += streamSize;
    deviceMemorySize = ut::RoundUp(deviceMemorySize, 32);
    deviceMemorySize += streamSize;

    size.Add(deviceMemorySize, 32);
}

ParticleShape* ParticleShape::Create(ResSceneObject resource,int capacity,os::IAllocator* mainAllocator,os::IAllocator* deviceAllocator){
    NW_NULL_ASSERT(mainAllocator);
    NW_NULL_ASSERT(deviceAllocator);

    ResParticleShape resNode = ResDynamicCast<ResParticleShape>(resource);
    NW_ASSERT(resNode.IsValid());

    const int streamSize = (capacity + internal::PARTICLE_SIMD_WIDTH_MAX) * sizeof(u16);
    const int drawCommandCacheSize = sizeof(u32) * (12 + MAX_ATTRIBUTES_NUM * 6 + MAX_ATTRIBUTES_NUM * 4 + 4);
    const int primitiveCommandCacheSize = sizeof(u32) * PrimitiveCommandSize;
    const int deactivateVertexCommandCacheSize = sizeof(u32) * (MAX_ATTRIBUTES_NUM * 8);

    int deviceMemorySize = 0;
    deviceMemorySize = ut::RoundUp(deviceMemorySize, 32);
    deviceMemorySize += streamSize;
    deviceMemorySize = ut::RoundUp(deviceMemorySize, 32);
    deviceMemorySize += streamSize;

    int nodeMemorySize = sizeof(ParticleShape);
    nodeMemorySize = ut::RoundUp(nodeMemorySize, 32);
    nodeMemorySize += drawCommandCacheSize;
    nodeMemorySize = ut::RoundUp(nodeMemorySize, 32);
    nodeMemorySize += drawCommandCacheSize;
    nodeMemorySize = ut::RoundUp(nodeMemorySize, 32);
    nodeMemorySize += deactivateVertexCommandCacheSize;
    nodeMemorySize = ut::RoundUp(nodeMemorySize, 32);
    nodeMemorySize += primitiveCommandCacheSize;
    nodeMemorySize = ut::RoundUp(nodeMemorySize, 32);

    u8* devicememory = reinterpret_cast<u8*>(deviceAllocator->Alloc(deviceMemorySize, 32));
    if (devicememory == NULL){
        return NULL;
    }

    u8* nodememory = reinterpret_cast<u8*>(mainAllocator->Alloc(nodeMemorySize, 32));
    if (nodememory == NULL){
        deviceAllocator->Free(devicememory);
        return NULL;
    }

    ParticleShape* node = new(nodememory) ParticleShape(
        capacity,
        mainAllocator,
        deviceAllocator,
        devicememory,
        resNode);
    nodememory += sizeof(ParticleShape);

    for (int i = 0; i < 2; ++i){
        devicememory = reinterpret_cast<u8*>(ut::RoundUp(devicememory, 32));
        node->mPrimitiveBuffer[i] = devicememory;
        devicememory += streamSize;


        u32 baseAddr = nngxGetPhysicalAddr(nn::gx::CTR::GetVramStartAddr(nn::gx::CTR::MEM_VRAMA));
        u32 bufferAddr = nngxGetPhysicalAddr(reinterpret_cast<uptr>(node->mPrimitiveBuffer[i]));

        NW_ASSERT((bufferAddr - baseAddr) < 0x10000000);
        node->mPrimitiveBufferOffset[i] = bufferAddr - baseAddr;
    }

    for (int side = 0; side < 2; ++side){
        nodememory = reinterpret_cast<u8*>(ut::RoundUp(nodememory, 32));
        node->mCommandCache[side] = reinterpret_cast<u32*>(nodememory);
        nodememory += drawCommandCacheSize;
    }
    
    nodememory = reinterpret_cast<u8*>(ut::RoundUp(nodememory, 32));
    node->mDeactivateVertexCommandCache = reinterpret_cast<u32*>(nodememory);
    nodememory += deactivateVertexCommandCacheSize;
    
    nodememory = reinterpret_cast<u8*>(ut::RoundUp(nodememory, 32));
    node->mPrimitiveCommandCache = reinterpret_cast<u32*>(nodememory);
    nodememory += primitiveCommandCacheSize;

    return node;
}

int ParticleShape::AddVertexStreamSize(u32 formatType,int dimension,int capacity,int prevSize){
    int formatSize = 1;
    switch (formatType){
    case GL_FLOAT:
        formatSize = 4;
        break;
    default:
        NW_FATAL_ERROR("unsupported formatType");
    }

    int streamSize = (capacity + internal::PARTICLE_SIMD_WIDTH_MAX) * dimension * formatSize;

    prevSize = ut::RoundUp(prevSize, 32);
    prevSize += streamSize;
    prevSize = ut::RoundUp(prevSize, 32);
    prevSize += streamSize;

    return prevSize + streamSize;
}

ParticleShape::VertexAttribute* ParticleShape::AddVertexStream(s32 usage,u32 formatType,int dimension,int capacity,u8** memory){
    int formatSize = 1;
    switch (formatType){
    case GL_FLOAT:
        formatSize = 4;
        break;
    default:
        NW_FATAL_ERROR("unsupported formatType");
    }

    *memory = reinterpret_cast<u8*>(ut::RoundUp(*memory, 32));

    int streamSize = (capacity + internal::PARTICLE_SIMD_WIDTH_MAX) * dimension * formatSize;
    void* stream0 = *memory;
    *memory += streamSize;

    void* stream1 = *memory;
    *memory += streamSize;

    std::memset(stream0, 0, streamSize);
    std::memset(stream1, 0, streamSize);

    return this->AddVertexAttribute(usage,formatType,dimension,true,reinterpret_cast<u8*>(stream0),reinterpret_cast<u8*>(stream1));
}

int ParticleShape::AddVertexParamSize(u32 formatType,int dimension,int prevSize){
    int formatSize = 1;
    switch (formatType){
    case GL_FLOAT:
        formatSize = 4;
        break;
    default:
        NW_FATAL_ERROR("unsupported formatType");
    }

    prevSize = ut::RoundUp(prevSize, 32);

    int streamSize = dimension * formatSize;

    return prevSize + streamSize;
}

ParticleShape::VertexAttribute* ParticleShape::AddVertexParam(s32 usage,u32 formatType,int dimension,f32* parameters,u8** memory){
    NW_UNUSED_VARIABLE(parameters);

    int formatSize = 1;
    switch (formatType){
    case GL_FLOAT:
        formatSize = 4;
        break;
    default:
        NW_FATAL_ERROR("unsupported formatType");
    }

    int streamSize = dimension * formatSize;
    *memory = reinterpret_cast<u8*>(ut::RoundUp(*memory, 32));
    void* stream = *memory;
    *memory += streamSize;

    nw::os::MemCpy(stream, parameters, streamSize);

    return this->AddVertexAttribute(usage,formatType,dimension,false,reinterpret_cast<u8*>(stream),NULL);
}

ParticleShape::ParticleShape(int capacity,os::IAllocator* allocator,os::IAllocator* deviceAllocator,void* deviceMemory,ResParticleShape resObj): 
    SceneObject(allocator, resObj),
    mCapacity(capacity),
    mBufferSide(false),
    mResVertexAttributeDataCount(0),
    mDeviceAllocator(deviceAllocator),
    mDeviceMemory(deviceMemory){
    mPrimitiveBuffer[0] = NULL;
    mPrimitiveBuffer[1] = NULL;
    mPrimitiveBufferOffset[0] = 0;
    mPrimitiveBufferOffset[1] = 0;

    for (int usage = 0; usage < PARTICLEUSAGE_COUNT; ++usage){
        for (int side = 0; side < 2; ++side){
            mVertexAttribute[usage].mStream[side] = NULL;
            mVertexAttribute[usage].mCommandPtr[side] = NULL;
        }
    }

    for (int i = 0; i < 2; ++i){
        mCommandCache[i] = NULL;
        mCommandCacheSize[i] = 0;
    }

    mDeactivateVertexCommandCache = NULL;
    mDeactivateVertexCommandCacheSize = 0;
    mPrimitiveCommandCache = NULL;
    mPrimitiveCommandCacheSize = 0;
}

ParticleShape::~ParticleShape(){
    this->mDeviceAllocator->Free(this->mDeviceMemory);

    for (int i = 0; i < 2; ++ i){
        if (this->mCommandCache[i] != NULL){
            this->mCommandCache[i] = NULL;
            this->mCommandCacheSize[i] = 0;
        }
    }

    if (this->mDeactivateVertexCommandCache != NULL){
        this->mDeactivateVertexCommandCache = NULL;
        this->mDeactivateVertexCommandCacheSize = 0;
    }
    
    if (this->mPrimitiveCommandCache != NULL){
        this->mPrimitiveCommandCache = NULL;
        this->mPrimitiveCommandCacheSize = 0;
    }
}

void ParticleShape::FlushBuffer(){
    for (s32 i = 0; i < this->GetVertexAttributesCount(); ++i){
        if (this->IsVertexStream(i)){
            nngxUpdateBuffer(this->GetVertexStreamPtr(i, PARTICLE_BUFFER_FRONT), this->GetVertexCapacity() * this->GetVertexAttributeDimension(i) * sizeof(f32));
        }
    }

    nngxUpdateBuffer(this->GetPrimitiveStreamPtr(PARTICLE_BUFFER_FRONT), this->GetVertexCapacity() * sizeof(u16));
}

}
}