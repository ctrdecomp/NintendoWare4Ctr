// Filename: gfx_ParticleCollection.cpp
//
// Project: NintendoWare4Ctr

#include <nw/gfx/gfx_ParticleCollection.h>
#include <nw/gfx/gfx_ISceneVisitor.h>

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>

namespace nw{
namespace gfx{

template <typename T>
static int ParticleStreamCreateSize(int capacity,int prevSize)
{
    NW_ASSERT(capacity > 0);

    prevSize = ut::RoundUp(prevSize, 8);

    const int size = (capacity + internal::PARTICLE_SIMD_WIDTH_MAX) * sizeof(T) + 8;

    return prevSize + size;
}

template <typename T>
static int ParticleParameterAttributeCreateSize(int prevSize)
{
    prevSize = ut::RoundUp(prevSize, 4);

    const int size = sizeof(T);

    return prevSize + size;
}


template <typename T>
static bool ParticleParameterAttributeCreate(ParticleCollection::ParticleAttribute* storage, ParticleUsage usage, 
    const ResParticleParameterAttribute& resource, u8** buffer)
{
    NW_NULL_ASSERT(buffer);

    const int size = sizeof(T);

    *buffer = reinterpret_cast<u8*>(ut::RoundUp(*buffer, 4));
    void* memory = *buffer;
    *buffer += size;

    nw::os::MemCpy(memory, resource.GetData(), size);

    storage->m_Usage = static_cast<s32>(usage);
    storage->m_IsStream = false;
    storage->m_Stream = reinterpret_cast<f32*>(memory);

    return memory != NULL;
}

template <typename T>
static bool ParticleStreamCreate(ParticleCollection::ParticleAttribute* storage,ParticleUsage usage,int capacity,u8** buffer)
{
    NW_ASSERT(capacity > 0);
    NW_NULL_ASSERT(buffer);

    const int size = (capacity + internal::PARTICLE_SIMD_WIDTH_MAX) * sizeof(T) + 8;

    *buffer = reinterpret_cast<u8*>(ut::RoundUp(*buffer, 8));
    void* memory = *buffer;
    *buffer += size;

    std::memset(memory, 0, size);

    storage->m_Usage = static_cast<s32>(usage);
    storage->m_IsStream = true;
    storage->m_Stream = reinterpret_cast<f32*>(memory);

    return memory != NULL;
}

NW_UT_RUNTIME_TYPEINFO_DEFINITION(ParticleCollection, SceneObject);

ParticleCollection::ParticleCollection(os::IAllocator* allocator,os::IAllocator* deviceAllocator,void* deviceMemory,ResParticleCollection resObj): 
    GfxObject(allocator),
    m_BufferSide(false),
    m_MinActiveIndex(0),
    m_MaxActiveIndex(0),
    m_ResParticleCollection(resObj),
    m_ParticleShape(NULL),
    m_ParticleSet(NULL),
    m_DeviceAllocator(deviceAllocator),
    m_DeviceMemory(deviceMemory),
    m_LastBuffer(0)
    {
    NW_UNUSED_VARIABLE(resObj);

    for (int i = 0; i < PARTICLEUSAGE_COUNT; ++i)
    {
        this->m_ParticleAttribute[i].m_Stream = NULL;
        this->m_ParticleAttribute[i].m_Usage = PARTICLEUSAGE_INVALID;
        this->m_StreamStride[i] = 0;

        this->m_IsStream[i] = false;
        for (int j = 0; j < 2; ++j)
        {
            this->m_StreamPtr[i][j] = NULL;
        }
    }
}

ParticleCollection::~ParticleCollection()
{
    if (m_DeviceMemory != NULL)
    {
        this->m_DeviceAllocator->Free(this->m_DeviceMemory);
    }

    for (int i = 0; i < PARTICLEUSAGE_COUNT; ++i)
    {
        if (this->m_ParticleAttribute[i].m_Stream != NULL)
        {
            this->m_ParticleAttribute[i].m_Stream = NULL;
        }

        for (int j = 0; j < 2; ++j)
        {
            this->m_StreamPtr[i][j] = NULL;
        }
    }
}

void ParticleCollection::GetDeviceMemorySizeInternal(os::MemorySizeCalculator* pSize,ResParticleCollection resNode)
{
    os::MemorySizeCalculator& size = *pSize;

    const int capacity = resNode.GetCapacity();

    int deviceMemorySize = 0;

    {
        ResParticleAttributeArray attributes = resNode.GetAttributes();
        ResParticleAttributeArray::iterator attributeEnd = attributes.end();
        for (ResParticleAttributeArray::iterator it = attributes.begin(); it != attributeEnd; ++it)
        {
            ParticleUsage usage = (ParticleUsage)(*it).GetUsage();

            if ((*it).GetTypeInfo() != ResParticleParameterAttribute::TYPE_INFO)
            {
                switch (usage)
                {
                case PARTICLEUSAGE_BIRTH:
                case PARTICLEUSAGE_LIFE:
                case PARTICLEUSAGE_VELOCITY:
                    break;
                case PARTICLEUSAGE_ALPHA:
                case PARTICLEUSAGE_TEXTUREROTATE0:
                    deviceMemorySize = ParticleShape::AddVertexStreamSize(GL_FLOAT, 1, capacity, deviceMemorySize);
                    break;
                case PARTICLEUSAGE_TRANSLATE:
                case PARTICLEUSAGE_SCALE:
                case PARTICLEUSAGE_ROTATE:
                case PARTICLEUSAGE_COLOR:
                case PARTICLEUSAGE_SCALE_EXT:
                    deviceMemorySize = ParticleShape::AddVertexStreamSize(GL_FLOAT, 3, capacity, deviceMemorySize);
                    break;
                case PARTICLEUSAGE_TEXTURETRANSLATE0:
                case PARTICLEUSAGE_TEXTURESCALE0:
                    deviceMemorySize = ParticleShape::AddVertexStreamSize(GL_FLOAT, 2, capacity, deviceMemorySize);
                    break;
                }
            }
        }
    }

    size.Add(deviceMemorySize, 32);
}

ParticleCollection* ParticleCollection::Create(ParticleSet* parent,ResParticleCollection resNode,
    os::IAllocator* mainAllocator,os::IAllocator* deviceAllocator,ParticleShape* shape)
{
    NW_NULL_ASSERT(mainAllocator);
    NW_NULL_ASSERT(deviceAllocator);
    NW_NULL_ASSERT(shape);

    NW_ASSERT(resNode.IsValid());

    const int capacity = resNode.GetCapacity();

    int collectionMemorySize = sizeof(ParticleCollection);
    collectionMemorySize = ut::RoundUp(collectionMemorySize, 4);

    int deviceMemorySize = 0;

    bool hasAttribute[PARTICLEUSAGE_COUNT];
    for (int i = 0; i < PARTICLEUSAGE_COUNT; ++i)
    {
        hasAttribute[i] = false;
    }

    {
    {
            ParticleUsage usage = PARTICLEUSAGE_ACTIVEINDEX;
            hasAttribute[usage] = true;
        }

        {
            ParticleUsage usage = PARTICLEUSAGE_FREEINDEX;

            collectionMemorySize = ParticleStreamCreateSize<u16>(capacity, collectionMemorySize);
            collectionMemorySize = ut::RoundUp(collectionMemorySize, 4);

            hasAttribute[usage] = true;
        }

        {
            ParticleUsage usage = PARTICLEUSAGE_NEG_TIMELIMIT;

            collectionMemorySize = ParticleStreamCreateSize<f32>(capacity, collectionMemorySize);

            hasAttribute[usage] = true;
        }

        ResParticleAttributeArray attributes = resNode.GetAttributes();
        ResParticleAttributeArray::iterator attributeEnd = attributes.end();
        for (ResParticleAttributeArray::iterator it = attributes.begin(); it != attributeEnd; ++it)
        {
            ParticleUsage usage = (ParticleUsage)(*it).GetUsage();

            if ((*it).GetTypeInfo() == ResParticleParameterAttribute::TYPE_INFO)
            {
                ResParticleParameterAttribute resParam((*it).ptr());

                switch (usage)
                {
                case PARTICLEUSAGE_BIRTH:
                case PARTICLEUSAGE_LIFE:
                    collectionMemorySize =
                        ParticleParameterAttributeCreateSize<f32>(collectionMemorySize);
                    break;
                case PARTICLEUSAGE_VELOCITY:
                    NW_FATAL_ERROR("PARTICLEUSAGE_VELOCITY must not be ParameterAttribute");
                    break;
                case PARTICLEUSAGE_ALPHA:
                case PARTICLEUSAGE_TEXTUREROTATE0:
                    collectionMemorySize = shape->AddVertexParamSize(GL_FLOAT, 1, collectionMemorySize);
                    break;
                case PARTICLEUSAGE_TRANSLATE:
                case PARTICLEUSAGE_SCALE:
                case PARTICLEUSAGE_ROTATE:
                case PARTICLEUSAGE_COLOR:
                case PARTICLEUSAGE_SCALE_EXT:
                    collectionMemorySize = shape->AddVertexParamSize(GL_FLOAT, 3, collectionMemorySize);
                    break;
                case PARTICLEUSAGE_TEXTURETRANSLATE0:
                case PARTICLEUSAGE_TEXTURESCALE0:
                    collectionMemorySize = shape->AddVertexParamSize(GL_FLOAT, 2, collectionMemorySize);
                    break;
                }

                hasAttribute[usage] = true;
            }
            else{
                switch (usage)
                {
                case PARTICLEUSAGE_BIRTH:
                case PARTICLEUSAGE_LIFE:
                    collectionMemorySize = ParticleStreamCreateSize<f32>(capacity, collectionMemorySize);
                    break;
                case PARTICLEUSAGE_VELOCITY:
                    collectionMemorySize = ParticleStreamCreateSize<nw::math::VEC3>(capacity, collectionMemorySize);
                    break;
                case PARTICLEUSAGE_ALPHA:
                case PARTICLEUSAGE_TEXTUREROTATE0:
                    deviceMemorySize = shape->AddVertexStreamSize(GL_FLOAT, 1, capacity, deviceMemorySize);
                    break;
                case PARTICLEUSAGE_TRANSLATE:
                case PARTICLEUSAGE_SCALE:
                case PARTICLEUSAGE_ROTATE:
                case PARTICLEUSAGE_COLOR:
                case PARTICLEUSAGE_SCALE_EXT:
                    deviceMemorySize = shape->AddVertexStreamSize(GL_FLOAT, 3, capacity, deviceMemorySize);
                    break;
                case PARTICLEUSAGE_TEXTURETRANSLATE0:
                case PARTICLEUSAGE_TEXTURESCALE0:
                    deviceMemorySize = shape->AddVertexStreamSize(GL_FLOAT, 2, capacity, deviceMemorySize);
                    break;
                }

                hasAttribute[usage] = true;
            }
        }

        for (int usage = 0; usage < PARTICLEUSAGE_COUNT; ++usage)
        {
            if (!hasAttribute[usage])
            {
                switch (usage)
                {
                case PARTICLEUSAGE_BIRTH:
                case PARTICLEUSAGE_LIFE:
                case PARTICLEUSAGE_VELOCITY:
                    NW_FATAL_ERROR("PARTICLEUSAGE_BIRTH/LIFE/VELOCITY must not be ParameterAttribute");
                    break;
                case PARTICLEUSAGE_ALPHA:
                    collectionMemorySize = shape->AddVertexParamSize(GL_FLOAT, 1, collectionMemorySize);
                    break;
                case PARTICLEUSAGE_TEXTUREROTATE0:
                    collectionMemorySize = shape->AddVertexParamSize(GL_FLOAT, 1, collectionMemorySize);
                    break;
                case PARTICLEUSAGE_TRANSLATE:
                case PARTICLEUSAGE_ROTATE:
                    collectionMemorySize = shape->AddVertexParamSize(GL_FLOAT, 3, collectionMemorySize);
                    break;
                case PARTICLEUSAGE_COLOR:
                case PARTICLEUSAGE_SCALE:
                case PARTICLEUSAGE_SCALE_EXT:
                    collectionMemorySize = shape->AddVertexParamSize(GL_FLOAT, 3, collectionMemorySize);
                    break;
                case PARTICLEUSAGE_TEXTURETRANSLATE0:
                    collectionMemorySize = shape->AddVertexParamSize(GL_FLOAT, 2, collectionMemorySize);
                    break;
                case PARTICLEUSAGE_TEXTURESCALE0:
                    collectionMemorySize = shape->AddVertexParamSize(GL_FLOAT, 2, collectionMemorySize);
                    break;
                }

                hasAttribute[usage] = false;
            }
        }
    }

    u8* deviceMemory = NULL;
    if (deviceMemorySize > 0)
    {
        deviceMemory = reinterpret_cast<u8*>(deviceAllocator->Alloc(deviceMemorySize, 32));
        if (deviceMemory == NULL)
        {
            return NULL;
        }
    }

    u8* nodeMemory = reinterpret_cast<u8*>(mainAllocator->Alloc(collectionMemorySize, 32));
    if (nodeMemory == NULL)
    {
        if (deviceMemory != NULL)
        {
            deviceAllocator->Free(deviceMemory);
        }

        return NULL;
    }

    ParticleCollection* node = new(nodeMemory) ParticleCollection(mainAllocator,deviceAllocator,deviceMemory,resNode);

    nodeMemory += sizeof(ParticleCollection);
    nodeMemory = reinterpret_cast<u8*>(ut::RoundUp(nodeMemory, 4));

    node->m_Capacity = capacity;

    node->m_ParticleShape = shape;

    {
    {
            ParticleUsage usage = PARTICLEUSAGE_ACTIVEINDEX;

            node->m_IsStream[usage] = true;
            for (int i = 0; i < 2; ++i)
            {
                node->m_StreamPtr[usage][i] = shape->GetPrimitiveStreamPtr((ParticleBuffer)i);
                node->m_StreamStride[usage] = 2;
            }

            hasAttribute[usage] = true;
        }

        {
            ParticleUsage usage = PARTICLEUSAGE_FREEINDEX;

            ParticleStreamCreate<u16>(&node->m_ParticleAttribute[usage], static_cast<ParticleUsage>(usage), capacity, &nodeMemory);

            node->m_IsStream[usage] = true;
            for (int i = 0; i < 2; ++i)
            {
                node->m_StreamPtr[usage][i] = node->m_ParticleAttribute[usage].m_Stream;
                node->m_StreamStride[usage] = 2;
            }

            hasAttribute[usage] = true;
        }

        {
            ParticleUsage usage = PARTICLEUSAGE_NEG_TIMELIMIT;

            ParticleStreamCreate<f32>(&node->m_ParticleAttribute[usage], static_cast<ParticleUsage>(usage), capacity, &nodeMemory);

            node->m_IsStream[usage] = true;
            for (int i = 0; i < 2; ++i)
            {
                node->m_StreamPtr[usage][i] = node->m_ParticleAttribute[usage].m_Stream;
                node->m_StreamStride[usage] = 2;
            }

            hasAttribute[usage] = true;
        }

        ResParticleAttributeArray attributes = resNode.GetAttributes();
        ResParticleAttributeArray::iterator attributeEnd = attributes.end();
        for (ResParticleAttributeArray::iterator it = attributes.begin(); it != attributeEnd; ++it)
        {
            ParticleUsage usage = (ParticleUsage)(*it).GetUsage();

            void* ptr0 = NULL;
            void* ptr1 = NULL;

            if ((*it).GetTypeInfo() == ResParticleParameterAttribute::TYPE_INFO)
            {
                ResParticleParameterAttribute resParam((*it).ptr());

                void* ptr = NULL;

                switch (usage)
                {
                case PARTICLEUSAGE_BIRTH:
                case PARTICLEUSAGE_LIFE:
                    ParticleParameterAttributeCreate<ParticleTime>(&node->m_ParticleAttribute[usage], usage, resParam, &nodeMemory);
                    ptr = node->m_ParticleAttribute[usage].m_Stream;{
                        *(ParticleTime*)ptr =  *resParam.GetData();
                    }
                    break;
                case PARTICLEUSAGE_VELOCITY:
                    NW_FATAL_ERROR("PARTICLEUSAGE_VELOCITY must not be ParameterAttribute");
                    break;
                case PARTICLEUSAGE_ALPHA:
                case PARTICLEUSAGE_TEXTUREROTATE0:{
                        ParticleShape::VertexAttribute* data = shape->AddVertexParam(usage, GL_FLOAT, 1, resParam.GetData(), &nodeMemory);
                        ptr = data->m_Stream[0];
                    }
                    break;
                case PARTICLEUSAGE_TRANSLATE:
                case PARTICLEUSAGE_SCALE:
                case PARTICLEUSAGE_ROTATE:
                case PARTICLEUSAGE_COLOR:
                case PARTICLEUSAGE_SCALE_EXT:{
                        ParticleShape::VertexAttribute* data = shape->AddVertexParam(usage, GL_FLOAT, 3, resParam.GetData(), &nodeMemory);
                        ptr = data->m_Stream[0];
                    }
                    break;
                case PARTICLEUSAGE_TEXTURETRANSLATE0:
                case PARTICLEUSAGE_TEXTURESCALE0:{
                        ParticleShape::VertexAttribute* data = shape->AddVertexParam(usage, GL_FLOAT, 2, resParam.GetData(), &nodeMemory);
                        ptr = data->m_Stream[0];
                    }
                    break;
                }

                hasAttribute[usage] = true;
                node->m_IsStream[usage] = false;
                node->m_StreamPtr[usage][0] = ptr;
                node->m_StreamPtr[usage][1] = ptr;
                node->m_StreamStride[usage] = 0;
            }
            else{
                switch (usage)
                {
                case PARTICLEUSAGE_BIRTH:
                case PARTICLEUSAGE_LIFE:
                    ParticleStreamCreate<ParticleTime>(&node->m_ParticleAttribute[usage], static_cast<ParticleUsage>(usage), capacity, &nodeMemory);
                    node->m_StreamPtr[usage][0] = node->m_ParticleAttribute[usage].m_Stream;
                    node->m_StreamPtr[usage][1] = node->m_ParticleAttribute[usage].m_Stream;
                    node->m_StreamStride[usage] = 4;
                    break;
                case PARTICLEUSAGE_VELOCITY:
                    ParticleStreamCreate<nw::math::VEC3>(&node->m_ParticleAttribute[usage], static_cast<ParticleUsage>(usage), capacity, &nodeMemory);
                    node->m_StreamPtr[usage][0] = node->m_ParticleAttribute[usage].m_Stream;
                    node->m_StreamPtr[usage][1] = node->m_ParticleAttribute[usage].m_Stream;
                    node->m_StreamStride[usage] = 12;
                    break;
                case PARTICLEUSAGE_ALPHA:
                case PARTICLEUSAGE_TEXTUREROTATE0:{
                        ParticleShape::VertexAttribute* datas = shape->AddVertexStream(usage, GL_FLOAT, 1, capacity, &deviceMemory);
                        node->m_StreamPtr[usage][0] = datas->m_Stream[0];
                        node->m_StreamPtr[usage][1] = datas->m_Stream[1];
                        node->m_StreamStride[usage] = 4;
                    }
                    break;
                case PARTICLEUSAGE_TRANSLATE:
                case PARTICLEUSAGE_SCALE:
                case PARTICLEUSAGE_ROTATE:
                case PARTICLEUSAGE_COLOR:
                case PARTICLEUSAGE_SCALE_EXT:{
                        ParticleShape::VertexAttribute* datas = shape->AddVertexStream(usage, GL_FLOAT, 3, capacity, &deviceMemory);
                        node->m_StreamPtr[usage][0] = datas->m_Stream[0];
                        node->m_StreamPtr[usage][1] = datas->m_Stream[1];
                        node->m_StreamStride[usage] = 12;
                    }
                    break;
                case PARTICLEUSAGE_TEXTURETRANSLATE0:
                case PARTICLEUSAGE_TEXTURESCALE0:{
                        ParticleShape::VertexAttribute* datas = shape->AddVertexStream(usage, GL_FLOAT, 2, capacity, &deviceMemory);
                        node->m_StreamPtr[usage][0] = datas->m_Stream[0];
                        node->m_StreamPtr[usage][1] = datas->m_Stream[1];
                        node->m_StreamStride[usage] = 8;
                    }
                    break;
                }

                node->m_IsStream[usage] = true;
                hasAttribute[usage] = true;
            }
        }

        for (int usage = 0; usage < PARTICLEUSAGE_COUNT; ++usage)
        {
            if (!hasAttribute[usage])
            {
                float paramZero[] = {0.0f, 0.0f, 0.0f, 0.0f};
                float paramOne[] = {1.0f, 1.0f, 1.0f, 1.0f};

                void* ptr = NULL;

                switch (usage)
                {
                case PARTICLEUSAGE_BIRTH:
                case PARTICLEUSAGE_LIFE:
                case PARTICLEUSAGE_VELOCITY:
                    NW_FATAL_ERROR("PARTICLEUSAGE_BIRTH/LIFE/VELOCITY must not be ParameterAttribute");
                    break;
                case PARTICLEUSAGE_ALPHA:{
                        ParticleShape::VertexAttribute* data = shape->AddVertexParam(usage, GL_FLOAT, 1, paramOne, &nodeMemory);
                        ptr = data->m_Stream[0];
                    }
                    break;
                case PARTICLEUSAGE_TEXTUREROTATE0:{
                        ParticleShape::VertexAttribute* data = shape->AddVertexParam(usage, GL_FLOAT, 1, paramZero, &nodeMemory);
                        ptr = data->m_Stream[0];
                    }
                    break;
                case PARTICLEUSAGE_TRANSLATE:
                case PARTICLEUSAGE_ROTATE:{
                        ParticleShape::VertexAttribute* data = shape->AddVertexParam(usage, GL_FLOAT, 3, paramZero, &nodeMemory);
                        ptr = data->m_Stream[0];
                    }
                    break;
                case PARTICLEUSAGE_COLOR:
                case PARTICLEUSAGE_SCALE:
                case PARTICLEUSAGE_SCALE_EXT:{
                        ParticleShape::VertexAttribute* data = shape->AddVertexParam(usage, GL_FLOAT, 3, paramOne, &nodeMemory);
                        ptr = data->m_Stream[0];
                    }
                    break;
                case PARTICLEUSAGE_TEXTURETRANSLATE0:{
                        ParticleShape::VertexAttribute* data = shape->AddVertexParam(usage, GL_FLOAT, 2, paramZero, &nodeMemory);
                        ptr = data->m_Stream[0];
                    }
                    break;
                case PARTICLEUSAGE_TEXTURESCALE0:{
                        ParticleShape::VertexAttribute* data = shape->AddVertexParam(usage, GL_FLOAT, 2, paramOne, &nodeMemory);
                        ptr = data->m_Stream[0];
                    }
                    break;
                }

                hasAttribute[usage] = true;
                node->m_IsStream[usage] = false;
                node->m_StreamPtr[usage][0] = ptr;
                node->m_StreamPtr[usage][1] = ptr;
                node->m_StreamStride[usage] = 0;
            }
        }
    }

    node->Clear();

    if (parent)
    {
        bool result = parent->AttachParticleCollection(node);
        NW_ASSERT(result);

        node->m_ParticleSet = parent;
    }

    return node;
}

void ParticleCollection::Clear()
{
    const int capacity = this->GetCapacity();

    this->SetCount(0);
    this->SetMinActiveIndex(0xffff);
    this->SetMaxActiveIndex(0);

    for (int j = 0; j < 2; ++j)
    {
        u16* ptr = (u16*)this->GetStreamPtr(PARTICLEUSAGE_ACTIVEINDEX, (ParticleBuffer)j);
        std::memset(ptr, 0xff, sizeof(u16) * capacity);
    }

    {
        u16* ptr = (u16*)this->GetStreamPtr(PARTICLEUSAGE_FREEINDEX, PARTICLE_BUFFER_FRONT);
        for (int i = capacity - 1; i >= 0; --i)
        {
            *ptr++ = i;
        }
    }

#if 0
{
        f32* ptr = (f32*)this->GetStreamPtr(PARTICLEUSAGE_NEG_TIMELIMIT, PARTICLE_BUFFER_FRONT);
        std::memset(ptr, 0xff, sizeof(f32) * capacity);
    }
#endif
}

}
}