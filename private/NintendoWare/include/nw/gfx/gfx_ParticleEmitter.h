/*---------------------------------------------------------------------------*
  Project:  NintendoWare

  Copyright (C)Nintendo/HAL Laboratory, Inc.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary
  information of Nintendo and/or its licensed developers and are protected by
  national and international copyright laws. They may not be disclosed to third
  parties or copied or duplicated in any form, in whole or in part, without the
  prior written consent of Nintendo.

  The content herein is highly confidential and should be handled accordingly.
 *---------------------------------------------------------------------------*/

#ifndef NW_GFX_PARTICLEEMITTER_H_
#define NW_GFX_PARTICLEEMITTER_H_

#include <nw/gfx/gfx_TransformNode.h>

#include <nw/gfx/res/gfx_ResParticleEmitter.h>
#include <nw/gfx/gfx_ParticleCollection.h>
#include <nw/gfx/gfx_ParticleRandom.h>
#include <nw/anim/anim_AnimFrameController.h>

#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 1301 // padding inserted in struct.
#endif
namespace nw
{
namespace gfx
{
class ParticleEmitter;

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ParticleEmitter : public TransformNode
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(ParticleEmitter);

public:
    NW_UT_RUNTIME_TYPEINFO;

    //
    struct Description : public TransformNode::Description
    {
        //
        Description()
        {}
    };

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    //
    //
    //
    //
    //
    static ParticleEmitter* Create(
        SceneNode* parent,
        ResSceneObject resource,
        const ParticleEmitter::Description& description,
        os::IAllocator* allocator);

    //
    //
    //
    //
    //
    static size_t GetMemorySize(
        ResParticleEmitter resource,
        const ParticleEmitter::Description& description,
        size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT)
    {
        os::MemorySizeCalculator size(alignment);

        GetMemorySizeInternal(&size, resource, description);

        return size.GetSizeWithPadding(alignment);
    }

    //
    static void GetMemorySizeInternal(
        os::MemorySizeCalculator* pSize,
        ResParticleEmitter resource,
        const ParticleEmitter::Description& description);

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    virtual void Accept(ISceneVisitor* visitor);

    //
    //
    //
    void UpdateParticleFrame()
    {
        m_ParticleAnimFrameController.UpdateFrame();
    }

    //
    //
    void Reset()
    {
        m_IsFirstEmission = true;
        m_EmissionCount = 0;
        m_NextEmissionTime = 0;

        // For the frame controller, just set Frame to 0.
        m_ParticleAnimFrameController.SetFrame(0);
    }

    //

    //----------------------------------------
    //
    //

    //
    //
    ResParticleEmitter GetResParticleEmitter()
    {
        return ResDynamicCast<ResParticleEmitter>( this->GetResSceneObject() );
    }

    //
    //
    const ResParticleEmitter GetResParticleEmitter() const
    {
        return ResDynamicCast<ResParticleEmitter>( this->GetResSceneObject() );
    }

    //
    // Do not maintain the resources obtained here.
    //
    //
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

        return m_ResParameter; // invalid
    }

    //
    // Do not maintain the resources obtained here.
    //
    //
    const ResParticleEmitterParameter GetResParticleEmitterParameterCopy(bool copyOnly = false) const
    {
        if (m_ResParameter.IsValid())
        {
            return m_ResParameter;
        }

        if (!copyOnly && this->GetResParticleEmitter().IsValid())
        {
            return ResParticleEmitterParameter(&this->GetResParticleEmitter().ptr()->m_IsResourceCopyEnabled);
        }

        return m_ResParameter; // invalid
    }

    //
    // Do not maintain the resources obtained here.
    //
    //
    ResParticleForm GetResParticleFormCopy(bool copyOnly = false)
    {
        if (m_ResForm.IsValid())
        {
            return m_ResForm;
        }

        if (!copyOnly && this->GetResParticleEmitter().IsValid())
        {
            return this->GetResParticleEmitter().GetParticleForm();
        }

        return m_ResForm; // invalid
    }

    //
    // Do not maintain the resources obtained here.
    //
    //
    const ResParticleForm GetResParticleFormCopy(bool copyOnly = false) const
    {
        if (m_ResForm.IsValid())
        {
            return m_ResForm;
        }

        if (!copyOnly && this->GetResParticleEmitter().IsValid())
        {
            return this->GetResParticleEmitter().GetParticleForm();
        }

        return m_ResForm; // invalid
    }

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    //
    //
    int GetEmissionCount(f32 prevTime, f32 time);

    //
    //
    //
    //
    void SetParticleSet(ParticleSet* particleSet)
    {
        this->m_ParticleSet = particleSet;
    }

    //
    //
    ParticleSet* GetParticleSet()
    {
        return this->m_ParticleSet;
    }

    //
    //
    const ParticleSet* GetParticleSet() const
    {
        return this->m_ParticleSet;
    }

    //
    //
    anim::AnimFrameController& ParticleAnimFrameController()
    {
        return m_ParticleAnimFrameController;
    }

    //
    //
    const anim::AnimFrameController& ParticleAnimFrameController() const
    {
        return m_ParticleAnimFrameController;
    }

    //
    void Srand(u32 seed)
    {
        m_ParticleRandom.Srand(seed);
    }

    //
    //
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

        f32 time = m_ParticleAnimFrameController.GetFrame();
        f32 cookedTime = time - resource.GetEmissionStart() - 1;
        if (cookedTime >= resource.GetEmissionSpan())
        {
            return false;
        }

        return true;
    }

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    //
    int Emission(ParticleContext* particleContext);

    //

    //----------------------------------------
    //
    //

#ifdef NW_GFX_PARTICLE_COMPAT_1_1
    //
    static void CalcCubeForm(
        const ResParticleCubeForm& cubeForm,
        int emissionCount,
        ParticleRandom* random,
        nw::math::VEC3* positions);

    //
    static void CalcCylinderForm(
        const ResParticleCylinderForm& cylinderForm,
        int emissionCount,
        ParticleRandom* random,
        nw::math::VEC3* positions);

    //
    static void CalcDiscForm(
        const ResParticleDiscForm& discForm,
        int emissionCount,
        ParticleRandom* random,
        nw::math::VEC3* positions);

    //
    static void CalcPointForm(
        const ResParticlePointForm& pointForm,
        int emissionCount,
        ParticleRandom* random,
        nw::math::VEC3* positions);

    //
    static void CalcSphereForm(
        const ResParticleSphereForm& sphereForm,
        int emissionCount,
        ParticleRandom* random,
        nw::math::VEC3* positions);

    //
    static void CalcRectangleForm(
        const ResParticleRectangleForm& rectangleForm,
        int emissionCount,
        ParticleRandom* random,
        nw::math::VEC3* positions);
#else
    //
    static void CalcCubeForm(
        const ResParticleCubeForm& cubeForm,
        int emissionCount,
        ParticleRandom* random,
        u16* activeIndex,
        int incrIndex,
        nw::math::VEC3* targetTranslate);

    //
    static void CalcCylinderForm(
        const ResParticleCylinderForm& cylinderForm,
        int emissionCount,
        ParticleRandom* random,
        u16* activeIndex,
        int incrIndex,
        nw::math::VEC3* targetTranslate);

    //
    static void CalcDiscForm(
        const ResParticleDiscForm& discForm,
        int emissionCount,
        ParticleRandom* random,
        u16* activeIndex,
        int incrIndex,
        nw::math::VEC3* targetTranslate);

    //
    static void CalcPointForm(
        const ResParticlePointForm& pointForm,
        int emissionCount,
        ParticleRandom* random,
        u16* activeIndex,
        int incrIndex,
        nw::math::VEC3* targetTranslate);

    //
    static void CalcSphereForm(
        const ResParticleSphereForm& sphereForm,
        int emissionCount,
        ParticleRandom* random,
        u16* activeIndex,
        int incrIndex,
        nw::math::VEC3* targetTranslate);

    //
    static void CalcRectangleForm(
        const ResParticleRectangleForm& rectangleForm,
        int emissionCount,
        ParticleRandom* random,
        u16* activeIndex,
        int incrIndex,
        nw::math::VEC3* targetTranslate);
#endif

    //

protected:
    //----------------------------------------
    //
    //

    //
    //
    //
    //
    //
    //
    ParticleEmitter(
        os::IAllocator* allocator,
        ResParticleEmitter resObj,
        const ParticleEmitter::Description& description,
        ResParticleEmitterParameter resParameterObj,
        ResParticleForm resFormObj);

    //
    virtual ~ParticleEmitter();

    //

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

} // namespace gfx
} // namespace nw
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 1301 // padding inserted in struct.
#endif

#endif // NW_GFX_PARTICLEEMITTER_H_
