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

#ifndef NW_GFX_PARTICLESET_H_
#define NW_GFX_PARTICLESET_H_

#include <nw/gfx/gfx_TransformNode.h>

#include <nw/gfx/gfx_ParticleModel.h>
#include <nw/gfx/res/gfx_ResParticleSet.h>
#include <nw/gfx/res/gfx_ResParticleAnimationOption.h>
#include <nw/gfx/gfx_ParticleRandom.h>
#include <nw/gfx/gfx_ParticleTime.h>

#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 1301 // padding inserted in struct.
#pragma diag_suppress 2530 // padding added to end of struct.
#endif
namespace nw
{
namespace gfx
{

class ParticleCollection;
class ParticleContext;
class ParticleShape;

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ParticleSet : public SceneNode
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(ParticleSet);

public:
    NW_UT_RUNTIME_TYPEINFO;

    //
    struct Description : public SceneNode::Description
    {
        s32 maxInitializers;         //
        s32 maxUpdaters;             //

        //
        Description() :
            maxInitializers(0),
            maxUpdaters(0)
        {}
    };

    //
    struct Initializer
    {
        bool m_IsCopied; //
        const ResParticleInitializerData *resource; //
        u32 work; //

        // Cached for increased speed
        nw::ut::ResTypeInfo     m_Type;
        void* m_TargetStreams[2]; //
    };

    //
    struct Updater
    {
        bool m_IsCopied; //
        const ResParticleUpdaterData *resource; //
        u32 work; //

        // Cached for increased speed
        nw::ut::ResTypeInfo     m_Type;
        void* m_TargetStreams[2]; //

        u32                     m_Flags;        //

        //
        enum Flag
        {
            FLAG_IS_HAS_CURVE_ANIM         = 0x1 << 0       //
        };

        //
        //
        //
        inline bool IsEnabledFlags(bit32 flags) const
        {
            return ut::CheckFlag(m_Flags, flags);
        }

        //
        //
        inline void EnableFlags(bit32 flags)
        {
            m_Flags = ut::EnableFlag(m_Flags, flags);
        }
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
    //
    //
    static ParticleSet* Create(
        SceneNode* parent,
        ResSceneObject resource,
        const ParticleSet::Description& description,
        os::IAllocator* mainAllocator,
        os::IAllocator* deviceAllocator,
        ParticleShape* shape);

    //
    //
    //
    //
    //
    static size_t GetMemorySize(
        ResParticleSet resource,
        const ParticleSet::Description& description,
        size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT)
    {
        os::MemorySizeCalculator size(alignment);

        GetMemorySizeInternal(&size, resource, description);

        return size.GetSizeWithPadding(alignment);
    }

    //
    static void GetMemorySizeInternal(
        os::MemorySizeCalculator* pSize,
        ResParticleSet resource,
        const ParticleSet::Description& description);

    //
    //
    //
    //
    static size_t GetDeviceMemorySize(
        ResParticleSet resource,
        size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT)
    {
        os::MemorySizeCalculator size(alignment);

        GetDeviceMemorySizeInternal(&size, resource);

        return size.GetSizeWithPadding(alignment);
    }

    //
    static void GetDeviceMemorySizeInternal(
        os::MemorySizeCalculator* pSize,
        ResParticleSet resource);

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

    //----------------------------------------
    //
    //

    //
    //
    ResParticleSet GetResParticleSet()
    {
        return ResDynamicCast<ResParticleSet>( this->GetResSceneObject() );
    }

    //
    //
    const ResParticleSet GetResParticleSet() const
    {
        return ResDynamicCast<ResParticleSet>( this->GetResSceneObject() );
    }

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    bool AttachParticleCollection(ParticleCollection* node)
    {
        if (this->m_ParticleCollection != NULL)
        {
            return false;
        }

        this->m_ParticleCollection = node;
        return true;
    }

    //
    //
    ParticleCollection* GetParticleCollection()
    {
        return this->m_ParticleCollection;
    }

    //
    //
    const ParticleCollection* GetParticleCollection() const
    {
        return this->m_ParticleCollection;
    }

    //
    //
    //
    ut::MoveArray<Initializer>* GetInitializers()
    {
        return &this->m_Initializers;
    }

    //
    //
    //
    const ut::MoveArray<Initializer>* GetInitializers() const
    {
        return &this->m_Initializers;
    }

    //
    //
    //
    ut::MoveArray<Updater>* GetUpdaters()
    {
        return &this->m_Updaters;
    }

    //
    //
    //
    const ut::MoveArray<Updater>* GetUpdaters() const
    {
        return &this->m_Updaters;
    }

#ifdef NW_GFX_PARTICLE_COMPAT_1_1
    //
    //
    void AddParticles(
        const nw::math::MTX34& emitterMatrix,
        const nw::math::VEC3* const positions,
        ParticleSet* parentParticleSet,
        const u16* const parentIndices,
        int count);
#else
    //
    //
    //
    //
    //
    //
    //
    int AddParticles(int count);
#endif

    //
    //
    //
    void UpdateParticles(
        ParticleContext* particleContext,
        bool enableSwapBuffer = true);

    //
    //
    //
    //
    void ClearParticleCollection();

    //
    void Srand(u32 seed)
    {
        m_ParticleRandom.Srand(seed);
    }

    //
    //
    //
    const nw::math::VEC3& GetScaleOffset() const
    {
        return this->m_ScaleOffset;
    }

    //
    //
    //
    void SetScaleOffset(const nw::math::VEC3& offset)
    {
        this->m_ScaleOffset = offset;
    }

    //
    //
    //
    const nw::math::VEC3& GetRotateOffset() const
    {
        return this->m_RotateOffset;
    }

    //
    //
    //
    void SetRotateOffset(const nw::math::VEC3& offset)
    {
        this->m_RotateOffset = offset;
    }

    //
    const nw::math::MTX34& WorldMatrix()
    {
        if (this->GetResParticleSet().GetIsForceWorld())
        {
            return nw::math::MTX34::Identity();
        }
        else
        {
            ParticleModel* model = static_cast<ParticleModel*>(this->GetParent());
            return model->WorldMatrix();
        }
    }

    //
    const nw::math::MTX34& InverseWorldMatrix()
    {
        if (this->GetResParticleSet().GetIsForceWorld())
        {
            return nw::math::MTX34::Identity();
        }
        else
        {
            ParticleModel* model = static_cast<ParticleModel*>(this->GetParent());
            return model->InverseWorldMatrix();
        }
    }

    //
    //
    bool IsAscendingOrder() const
    {
        const ResParticleShapeBuilder& resShapeBuilder =
            this->GetResParticleSet().GetParticleShapeBuilder();
        return (resShapeBuilder.IsValid()) ? resShapeBuilder.IsAscendingOrder() : true;
    }

    //
    //
    void ResetDebugHint()
    {
#ifdef NW_CHECK_PARTICLE_PROCESS
        m_ProcessStep = STEP_NONE;
#endif
    }

#ifdef NW_CHECK_PARTICLE_PROCESS
    //
    //
    //
    void BeginDraw()
    {
        // Rendering is possible under any conditions
        m_ProcessStep = STEP_DRAW;
    }

    //
    //
    //
    void BeginUpdate(bool enableSwapBuffer)
    {
        if (enableSwapBuffer)
        {
            NW_WARNING(
                m_ProcessStep == STEP_NONE ||
                m_ProcessStep == STEP_DRAW,
                "detected an illegal order(Update with SwapBuffer Prev:%d)", m_ProcessStep);
            m_ProcessStep = STEP_UPDATE1;
        }
        else
        {
            NW_WARNING(
                m_ProcessStep == STEP_UPDATE1 ||
                m_ProcessStep == STEP_UPDATE2,
                "detected an illegal order(Update without SwapBuffer Prev:%d)", m_ProcessStep);
            m_ProcessStep = STEP_UPDATE2;
        }
    }

#endif

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
    //
    void InitializeParticles(
        int startIndex,
        int count,
        int incrIndex,
        ParticleTime time);

    //

protected:
    //----------------------------------------
    //
    //

    //
    ParticleSet(
        os::IAllocator* allocator,
        ResParticleSet resObj,
        const ParticleSet::Description& description);

    //
    virtual ~ParticleSet();

    //

private:
    ParticleCollection* m_ParticleCollection;

    ParticleRandom m_ParticleRandom;

    ut::MoveArray<Initializer> m_Initializers;
    ut::MoveArray<Updater> m_Updaters;

    math::VEC3 m_ScaleOffset;
    math::VEC3 m_RotateOffset;

    bool m_UsePrevTranslate;

#ifdef NW_CHECK_PARTICLE_PROCESS
    //
    enum ProcessStep
    {
        STEP_NONE,    //
        STEP_UPDATE1, //
        STEP_UPDATE2, //
        STEP_DRAW     //
    } m_ProcessStep;
#endif

};

} // namespace gfx
} // namespace nw
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 2530 // padding added to end of struct.
#pragma diag_default 1301 // padding inserted in struct.
#endif

#endif // NW_GFX_PARTICLESET_H_
