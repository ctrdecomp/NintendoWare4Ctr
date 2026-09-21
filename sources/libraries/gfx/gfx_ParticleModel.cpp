#include <nw/gfx/gfx_ParticleModel.h>
#include <nw/gfx/gfx_ParticleContext.h>
#include <nw/gfx/gfx_SkeletalModel.h>
#include <nw/gfx/gfx_ParticleSet.h>
#include <nw/gfx/gfx_ParticleEmitter.h>
#include <nw/gfx/gfx_ISceneVisitor.h>

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/ut/ut_Foreach.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION( ParticleModel, Model );

void ParticleSetsAreEmpty::operator()(const ParticleSet* particleSet)
{
    if (particleSet->GetParticleCollection()->GetCount() > 0)
    {
        *this->m_Result = false;
    }
}

void ParticleSetsResetDebugHint::operator()(ParticleSet* particleSet)
{
    particleSet->ResetDebugHint();
}

void ParticleSetsClear::operator()(ParticleSet* particleSet)
{
    particleSet->ClearParticleCollection();
}

void ParticleModel::GetMemorySizeInternal(os::MemorySizeCalculator* pSize,ResParticleModel resNode,const ParticleModel::Description& description)
{
    os::MemorySizeCalculator& size = *pSize;

    size += sizeof(ParticleModel);

    ParticleModel::GetMemorySizeForInitialize(&size,resNode,description);

    int setCount = resNode.GetParticleSetsCount();
    NW_ASSERT(setCount == resNode.GetShapesCount());
    for (int i = 0; i < setCount; ++i)
    {
        gfx::ResParticleSet resParticleSet = resNode.GetParticleSets(i);

        gfx::ResParticleShape resParticleShape = ResDynamicCast<ResParticleShape>(resNode.GetShapes(i));
        NW_ASSERT(resParticleShape.IsValid());

        ParticleShape::GetMemorySizeInternal(&size, resParticleSet.GetParticleCollection().GetCapacity());

        ParticleSet::Description particleSetDescription;
        particleSetDescription.maxCallbacks = 0;
        particleSetDescription.maxChildren = 0;
        particleSetDescription.isFixedSizeMemory = true;

        ParticleSet::GetMemorySizeInternal(&size,resParticleSet,particleSetDescription);
    }
}

void ParticleModel::GetDeviceMemorySizeInternal(os::MemorySizeCalculator* pSize,ResParticleModel resNode,const ParticleModel::Description&)
{
    os::MemorySizeCalculator& size = *pSize;

    int setCount = resNode.GetParticleSetsCount();
    NW_ASSERT(setCount == resNode.GetShapesCount());
    for (int i = 0; i < setCount; ++i)
    {
        gfx::ResParticleSet resParticleSet = resNode.GetParticleSets(i);

        gfx::ResParticleShape resParticleShape = ResDynamicCast<ResParticleShape>(resNode.GetShapes(i));
        NW_ASSERT(resParticleShape.IsValid());

        ParticleShape::GetDeviceMemorySizeInternal(
            pSize, resParticleSet.GetParticleCollection().GetCapacity());

        ParticleSet::GetDeviceMemorySizeInternal(pSize, resParticleSet);
    }
}

ParticleModel* ParticleModel::Create(SceneNode* parent,ResSceneObject resource,const ParticleModel::Description& modelDescription,os::IAllocator* mainAllocator,os::IAllocator* deviceAllocator)
{
    NW_NULL_ASSERT(mainAllocator);
    NW_NULL_ASSERT(deviceAllocator);

    ResParticleModel resNode = ResDynamicCast<ResParticleModel>(resource);
    NW_ASSERT(resNode.IsValid());

    void* memory = mainAllocator->Alloc(sizeof(ParticleModel));
    if (memory == NULL)
    {
        return NULL;
    }

    ParticleModel* node = new(memory) ParticleModel(mainAllocator,resNode,modelDescription);

    {
        Result result = node->Initialize(mainAllocator);
        if (!result.IsSuccess())
        {
            SafeDestroy(node);
            return NULL;
        }
    }

    bool isSuccess = true;

    int setCount = resNode.GetParticleSetsCount();
    NW_ASSERT(setCount == resNode.GetShapesCount());
    for (int i = 0; i < setCount; ++i)
    {
        gfx::ResParticleSet resParticleSet = resNode.GetParticleSets(i);

        gfx::ResParticleShape resParticleShape = ResDynamicCast<ResParticleShape>(resNode.GetShapes(i));
        NW_ASSERT(resParticleShape.IsValid());

        ParticleShape* shapeNode = ParticleShape::Create(resParticleShape,resParticleSet.GetParticleCollection().GetCapacity(),mainAllocator,deviceAllocator);

        if (shapeNode == NULL)
        {
            isSuccess = false;
            break;
        }

        node->AttachParticleShape(shapeNode);

        ParticleSet::Description description;
        description.maxCallbacks = 0;
        description.maxChildren = 0;
        description.isFixedSizeMemory = true;

        ParticleSet* setNode = ParticleSet::Create(node,resParticleSet,description,mainAllocator,deviceAllocator,shapeNode);

        if (setNode == NULL)
        {
            isSuccess = false;
            break;
        }

        node->AttachParticleSet(setNode);
    }

    if (isSuccess == false)
    {
        for (int i = 0; i < node->m_MaximumParticleSet; ++i)
        {
            if (node->m_ParticleSets[i] != NULL)
            {
                SafeDestroy(node->m_ParticleSets[i]);
                node->m_ParticleSets[i] = NULL;
            }
        }

        SafeDestroy(node);
        return NULL;
    }

    if (parent)
    {
        bool result = parent->AttachChild(node);
        NW_ASSERT(result);
    }

    return node;
}

void ParticleModel::Accept(ISceneVisitor* visitor)
{
    visitor->VisitParticleModel(this);
    AcceptChildren(visitor);
}

void ParticleModel::GetMemorySizeForInitialize(os::MemorySizeCalculator* pSize,ResParticleModel resource,const ParticleModel::Description& description)
{
    os::MemorySizeCalculator& size = *pSize;

    Model::GetMemorySizeForInitialize(pSize, resource, description);

    size += sizeof(ParticleSet*) * description.particleSetCount;
    size += sizeof(ParticleShape*) * description.particleSetCount;
}

Result  ParticleModel::Initialize(os::IAllocator* allocator)
{
    Result result = INITIALIZE_RESULT_OK;

    result |= Model::Initialize(allocator);
    NW_ENSURE_AND_RETURN(result);

    if (m_MaximumParticleSet != 0)
    {
        void* memory = allocator->Alloc(sizeof(ParticleSet*) * m_MaximumParticleSet);
        if (!memory)
        {
            result |= Result::MASK_FAIL_BIT;
        }
        NW_ENSURE_AND_RETURN(result);

        m_ParticleSets = ut::MoveArray<ParticleSet*>(memory, m_MaximumParticleSet, allocator);
        this->m_ParticleSets.Resize(this->m_MaximumParticleSet);
        for (int i = 0; i < m_ParticleSets.size(); ++i)
        {
            m_ParticleSets[i] = NULL;
        }
    }

    if (m_MaximumParticleSet != 0)
    {
        void* memory = allocator->Alloc(sizeof(ParticleShape*) * m_MaximumParticleSet);
        if (!memory)
        {
            result |= Result::MASK_FAIL_BIT;
        }
        NW_ENSURE_AND_RETURN(result);

        m_ParticleShapes = ut::MoveArray<ParticleShape*>(memory, m_MaximumParticleSet, allocator);
        m_ParticleShapes.Resize(m_MaximumParticleSet);
        for (int i = 0; i < m_ParticleShapes.size(); ++i)
        {
            m_ParticleShapes[i] = NULL;
        }
    }

    return result;

}

}
}