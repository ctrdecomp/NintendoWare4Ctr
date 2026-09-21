// Filename: gfx_ParticleEmitter.cpp
//
// Project: NintendoWare4Ctr

#include <nw/gfx/gfx_ParticleContext.h>
#include <nw/gfx/gfx_ParticleEmitter.h>
#include <nw/gfx/gfx_ParticleRandom.h>
#include <nw/gfx/gfx_ISceneVisitor.h>

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>

namespace nw {
namespace gfx {

NW_UT_RUNTIME_TYPEINFO_DEFINITION(ParticleEmitter, TransformNode);

ParticleEmitter::ParticleEmitter(os::IAllocator* allocator,
    ResParticleEmitter resObj, const ParticleEmitter::Description& description,
    ResParticleEmitterParameter resParameterObj, ResParticleForm resFormObj):
    TransformNode(allocator, resObj, description),
    m_IsFirstEmission(true),
    m_EmissionCount(0),
    m_NextEmissionTime(0),
    m_ParticleSet(NULL),
    m_ParticleAnimFrameController(0, 16777215, anim::PlayPolicy_Loop),
    m_ResParameter(resParameterObj),
    m_ResForm(resFormObj)
{
}

ParticleEmitter::~ParticleEmitter()
{
    if (this->m_ResParameter.IsValid())
    {
        this->GetAllocator().Free(this->m_ResParameter.ptr());
    }

    if (this->m_ResForm.IsValid())
    {
        this->GetAllocator().Free(this->m_ResForm.ptr());
    }
}

void ParticleEmitter::GetMemorySizeInternal(os::MemorySizeCalculator* pSize, ResParticleEmitter resNode,
    const ParticleEmitter::Description& description)
{
  os::MemorySizeCalculator& size = *pSize;

  ResParticleEmitterParameterData* resParameterData = NULL;
  if (resNode.GetIsResourceCopyEnabled())
  {
    size += sizeof(ResParticleEmitterParameterData);
  }

  ResParticleFormData* resFormData = NULL;
  const ResParticleForm resForm = resNode.GetParticleForm();
  if (resForm.IsValid() && resForm.GetIsResourceCopyEnabled())
  {
    switch(resForm.GetTypeInfo())
    {
    case ResParticleCubeForm::TYPE_INFO:
      size += sizeof(ResParticleCubeFormData);
      break;
    case ResParticleCylinderForm::TYPE_INFO:
      size += sizeof(ResParticleCylinderFormData);
      break;
    case ResParticleDiscForm::TYPE_INFO:
      size += sizeof(ResParticleDiscFormData);
      break;
    case ResParticlePointForm::TYPE_INFO:
      size += sizeof(ResParticlePointFormData);
      break;
    case ResParticleRectangleForm::TYPE_INFO:
      size += sizeof(ResParticleRectangleFormData);
      break;
    case ResParticleSphereForm::TYPE_INFO:
      size += sizeof(ResParticleSphereFormData);
      break;
    default:
      NW_FATAL_ERROR("unknown form type");
    }
  }

  size += sizeof(ParticleEmitter);

  TransformNode::GetMemorySizeForInitialize(pSize, resNode, description);
}

ParticleEmitter* ParticleEmitter::Create(SceneNode* parent, ResSceneObject resource,
    const ParticleEmitter::Description& description, os::IAllocator* allocator)
{
    NW_NULL_ASSERT(allocator);

    ResParticleEmitter resNode = ResDynamicCast<ResParticleEmitter>(resource);
    NW_ASSERT(resNode.IsValid());

    ResParticleEmitterParameterData* resParameterData = NULL;
    if (resNode.GetIsResourceCopyEnabled())
    {
        void* resourceMemory = allocator->Alloc(sizeof(ResParticleEmitterParameterData));
        if (resourceMemory == NULL)
        {
            return NULL;
        }

        resParameterData = new(resourceMemory) ResParticleEmitterParameterData;
        nw::os::MemCpy(resParameterData, &resNode.ptr()->m_IsResourceCopyEnabled,
            sizeof(ResParticleEmitterParameterData));
    }

    ResParticleFormData* resFormData = NULL;
    const ResParticleForm resForm = resNode.GetParticleForm();
    if (resForm.IsValid() && resForm.GetIsResourceCopyEnabled())
    {
        int size = 0;

        switch(resForm.GetTypeInfo())
        {
        case ResParticleCubeForm::TYPE_INFO:
            {
                size = sizeof(ResParticleCubeFormData);
                void* resourceMemory = allocator->Alloc(size);
                if (resourceMemory != NULL)
                {
                    resFormData = new(resourceMemory) ResParticleCubeFormData;
                }
            }
            break;
        case ResParticleCylinderForm::TYPE_INFO:
            {
                size = sizeof(ResParticleCylinderFormData);
                void* resourceMemory = allocator->Alloc(size);
                if (resourceMemory != NULL)
                {
                    resFormData = new(resourceMemory) ResParticleCylinderFormData;
                }
            }
            break;
        case ResParticleDiscForm::TYPE_INFO:
            {
                size = sizeof(ResParticleDiscFormData);
                void* resourceMemory = allocator->Alloc(size);
                if (resourceMemory != NULL)
                {
                    resFormData = new(resourceMemory) ResParticleDiscFormData;
                }
            }
            break;
        case ResParticlePointForm::TYPE_INFO:
            {
                size = sizeof(ResParticlePointFormData);
                void* resourceMemory = allocator->Alloc(size);
                if (resourceMemory != NULL)
                {
                    resFormData = new(resourceMemory) ResParticlePointFormData;
                }
            }
            break;
        case ResParticleRectangleForm::TYPE_INFO:
            {
                size = sizeof(ResParticleRectangleFormData);
                void* resourceMemory = allocator->Alloc(size);
                if (resourceMemory != NULL)
                {
                    resFormData = new(resourceMemory) ResParticleRectangleFormData;
                }
            }
            break;
        case ResParticleSphereForm::TYPE_INFO:
            {
                size = sizeof(ResParticleSphereFormData);
                void* resourceMemory = allocator->Alloc(size);
                if (resourceMemory != NULL)
                {
                    resFormData = new(resourceMemory) ResParticleSphereFormData;
                }
            }
            break;
        default:
            NW_FATAL_ERROR("unknown form type");
        }

        if (resFormData == NULL)
        {
            if (resParameterData != NULL)
            {
                allocator->Free(resParameterData);
                return NULL;
            }
        }
        else
        {
            nw::os::MemCpy(resFormData, resForm.ptr(), size);
        }
    }

    void* memory = allocator->Alloc(sizeof(ParticleEmitter));
    if (memory == NULL)
    {
        if (resParameterData != NULL)
        {
            allocator->Free(resParameterData);
        }

        if (resFormData != NULL)
        {
            allocator->Free(resFormData);
        }

        return NULL;
    }

    ParticleEmitter* node = new(memory) ParticleEmitter(
        allocator, resNode,
        description, ResParticleEmitterParameter(resParameterData),
        ResParticleForm(resFormData));

    {
        Result result = node->Initialize(allocator);
        if (!result.IsSuccess())
        {
            SafeDestroy(node);
            return NULL;
        }
    }

    if (parent)
    {
        bool result = parent->AttachChild(node);
        NW_ASSERT(result);
    }

    return node;
}


void ParticleEmitter::Accept(ISceneVisitor* visitor)
{
    visitor->VisitParticleEmitter(this);
    AcceptChildren(visitor);
}

} // namespace gfx
} // namespace nw