#pragma once

#include <nw/gfx/gfx_TransformNode.h>

#include <nw/gfx/res/gfx_ResParticleEmitter.h>
#include <nw/gfx/gfx_ParticleCollection.h>
#include <nw/gfx/gfx_ParticleRandom.h>
#include <nw/anim/anim_AnimFrameController.h>

namespace nw{
namespace gfx{

class ParticleEmitter : public TransformNode
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(ParticleEmitter);

public:
    NW_UT_RUNTIME_TYPEINFO;

    struct Description : public TransformNode::Description
    {
        Description() {}
    };


    static ParticleEmitter* Create(SceneNode* parent,ResSceneObject resource,const ParticleEmitter::Description& description,nw::os::IAllocator* allocator);

    static size_t GetMemorySize(ResParticleEmitter resource,const ParticleEmitter::Description& description,size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT)
    {
        os::MemorySizeCalculator size(alignment);

        GetMemorySizeInternal(&size, resource, description);

        return size.GetSizeWithPadding(alignment);
    }

    static void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize,ResParticleEmitter resource,const ParticleEmitter::Description& description);

    virtual void Accept(ISceneVisitor* visitor);

    void UpdateParticleFrame()
    {
        this->m_ParticleAnimFrameController.UpdateFrame();
    }

    void Reset()
    {
        m_IsFirstEmission = true;
        m_EmissionCount = 0;
        m_NextEmissionTime = 0;

        this->m_ParticleAnimFrameController.SetFrame(0);
    }

    ResParticleEmitter GetResParticleEmitter()
    {
        return ResDynamicCast<ResParticleEmitter>( this->GetResSceneObject() );
    }

    const ResParticleEmitter GetResParticleEmitter() const
    {
        return ResDynamicCast<ResParticleEmitter>( this->GetResSceneObject() );
    }

    ResParticleEmitterParameter GetResParticleEmitterParameterCopy(bool copyOnly = false)
    {
        if (m_ResParameter.IsValid())
        {
            return m_ResParameter;
        }

        if (!copyOnly && this->GetResParticleEmitter().IsValid())
        {
            return ResParticleEmitterParameter(&this->GetResParticleEmitter().ptr()->m_IsResourceCopyEnabled);
        }

        return m_ResParameter;
    }

    const ResParticleEmitterParameter GetResParticleEmitterParameterCopy(bool copyOnly = false) const
    {
        if (this->m_ResParameter.IsValid())
        {
            return m_ResParameter;
        }

        if (!copyOnly && this->GetResParticleEmitter().IsValid())
        {
            return ResParticleEmitterParameter(&this->GetResParticleEmitter().ptr()->m_IsResourceCopyEnabled);
        }

        return m_ResParameter;
    }

    ResParticleForm GetResParticleFormCopy(bool copyOnly = false)
    {
        if (this->m_ResForm.IsValid())
        {
            return m_ResForm;
        }

        if (!copyOnly && this->GetResParticleEmitter().IsValid())
        {
            return this->GetResParticleEmitter().GetParticleForm();
        }

        return m_ResForm;
    }

    const ResParticleForm GetResParticleFormCopy(bool copyOnly = false) const
    {
        if (this->m_ResForm.IsValid())
        {
            return m_ResForm;
        }

        if (!copyOnly && this->GetResParticleEmitter().IsValid())
        {
            return this->GetResParticleEmitter().GetParticleForm();
        }

        return m_ResForm;
    }

    int GetEmissionCount(f32 prevTime, f32 time);

    void SetParticleSet(ParticleSet* particleSet)
    {
        this->m_ParticleSet = particleSet;
    }

    ParticleSet* GetParticleSet()
    {
        return this->m_ParticleSet;
    }

    const ParticleSet* GetParticleSet() const
    {
        return this->m_ParticleSet;
    }

    anim::AnimFrameController& ParticleAnimFrameController()
    {
        return m_ParticleAnimFrameController;
    }

    const anim::AnimFrameController& ParticleAnimFrameController() const
    {
        return m_ParticleAnimFrameController;
    }

    void Srand(u32 seed)
    {
        m_ParticleRandom.Srand(seed);
    }

    bool IsAlive() const
    {
        const ResParticleEmitterParameter resource = this->GetResParticleEmitterParameterCopy(false);
        if (!resource.IsValid())
        {
            return false;
        }

        if (resource.GetEmissionRatio() == 0)
        {
            return false;
        }

        if (resource.GetEmissionSpanInfinity())
        {
            return true;
        }

        f32 time = this->m_ParticleAnimFrameController.GetFrame();
        f32 cookedTime = time - resource.GetEmissionStart() - 1;
        if (cookedTime >= resource.GetEmissionSpan())
        {
            return false;
        }

        return true;
    }

    int Emission(ParticleContext* particleContext);

    static void CalcCubeForm(const ResParticleCubeForm& cubeForm,int emissionCount,ParticleRandom* random,nw::math::VEC3* positions);

    static void CalcCylinderForm(const ResParticleCylinderForm& cylinderForm,int emissionCount,ParticleRandom* random,nw::math::VEC3* positions);

    static void CalcDiscForm(const ResParticleDiscForm& discForm,int emissionCount,ParticleRandom* random,nw::math::VEC3* positions);

    static void CalcPointForm(const ResParticlePointForm& pointForm,int emissionCount,ParticleRandom* random,nw::math::VEC3* positions);

    static void CalcSphereForm(const ResParticleSphereForm& sphereForm,int emissionCount,ParticleRandom* random,nw::math::VEC3* positions);

    static void CalcRectangleForm(const ResParticleRectangleForm& rectangleForm,int emissionCount,ParticleRandom* random,nw::math::VEC3* positions);
protected:

    ParticleEmitter(nw::os::IAllocator* allocator,ResParticleEmitter resObj,const ParticleEmitter::Description& description,
        ResParticleEmitterParameter resParameterObj,
        ResParticleForm resFormObj);

    virtual ~ParticleEmitter();

private:
    bool m_IsFirstEmission;
    f32 m_EmissionCount;
    s32 m_NextEmissionTime;
    ParticleSet* m_ParticleSet;

    ParticleRandom m_ParticleRandom;
    anim::AnimFrameController m_ParticleAnimFrameController;

    ResParticleEmitterParameter m_ResParameter;
    ResParticleForm m_ResForm;
};

}
}