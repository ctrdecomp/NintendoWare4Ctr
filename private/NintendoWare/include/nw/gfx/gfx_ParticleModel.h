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

#ifndef NW_GFX_PARTICLEMODEL_H_
#define NW_GFX_PARTICLEMODEL_H_

#include <nw/gfx/gfx_Model.h>
#include <nw/gfx/res/gfx_ResParticleModel.h>
#include <nw/gfx/gfx_ParticleShape.h>
#include <nw/anim/anim_AnimFrameController.h>
#include <nw/ut/ut_MoveArray.h>


namespace nw
{
namespace gfx
{

class ParticleSet;

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ParticleSetsAreEmpty
{
public:
    //
    //
    ParticleSetsAreEmpty(bool* result)
    {
        NW_NULL_ASSERT(result);
        m_Result = result;
        *m_Result = true;
    }

    //
    //
    void operator()(const ParticleSet* particleSet);

private:
    bool* m_Result;
};

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ParticleSetsResetDebugHint
{
public:
    //
    //
    void operator()(ParticleSet* particleSet);
};

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ParticleSetsClear
{
public:
    //
    ParticleSetsClear() {}

    //
    //
    void operator()(ParticleSet* particleSet);
};

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ParticleModel : public Model
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(ParticleModel);

public:
    NW_UT_RUNTIME_TYPEINFO;

    //
    struct Description : public Model::Description
    {
        //
        Description() :
            particleSetCount(0)
        {}

        uint particleSetCount;
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
    static ParticleModel* Create(
        SceneNode* parent,
        ResSceneObject resource,
        const ParticleModel::Description& modelDescription,
        os::IAllocator* mainAllocator,
        os::IAllocator* deviceAllocator);

    //
    //
    //
    //
    //
    static size_t GetMemorySize(
        ResParticleModel resource,
        const ParticleModel::Description& modelDescription,
        size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT)
    {
        os::MemorySizeCalculator size(alignment);

        GetMemorySizeInternal(&size, resource, modelDescription);

        return size.GetSizeWithPadding(alignment);
    }

    //
    static void GetMemorySizeInternal(
        os::MemorySizeCalculator* pSize,
        ResParticleModel resource,
        const ParticleModel::Description& modelDescription);

    //
    //
    //
    //
    static size_t GetDeviceMemorySize(
        ResParticleModel resource,
        const ParticleModel::Description& modelDescription,
        size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT)
    {
        os::MemorySizeCalculator size(alignment);

        GetDeviceMemorySizeInternal(&size, resource, modelDescription);

        return size.GetSizeWithPadding(alignment);
    }

    //
    static void GetDeviceMemorySizeInternal(
        os::MemorySizeCalculator* pSize,
        ResParticleModel resource,
        const ParticleModel::Description& modelDescription);

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
    void UpdateParticleFrame()
    {
        m_ParticleAnimFrameController.UpdateFrame();
    }

    //
    //
    //
    //
    //
    template<typename TFunction>
    void ForeachParticleSet(TFunction function)
    {
        for (int i = 0; i < (int)this->GetParticleSetsCount(); ++i)
        {
            ParticleSet* particleSet = this->GetParticleSets(i);

            function(particleSet);
        }
    }

    //
    //
    //
    //
    //
    template<typename TFunction>
    void ForeachConstParticleSet(TFunction function) const
    {
        for (int i = 0; i < (int)this->GetParticleSetsCount(); ++i)
        {
            const ParticleSet* particleSet = this->GetParticleSets(i);

            function(particleSet);
        }
    }

    //

    //----------------------------------------
    //
    //

    //
    //
    ResParticleModel GetResModel()
    {
        return ResDynamicCast<ResParticleModel>(this->GetResSceneObject());
    }

    //
    //
    const ResParticleModel GetResModel() const
    {
        return ResDynamicCast<ResParticleModel>(this->GetResSceneObject());
    }

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    bool AttachParticleSet(ParticleSet* node)
    {
        if (m_ParticleSetCount >= this->m_ParticleSets.size()) return false;

        this->m_ParticleSets[m_ParticleSetCount] = node;
        ++m_ParticleSetCount;
        return true;
    }

    //
    //
    u32 GetParticleSetsCount() const
    {
        return m_ParticleSetCount;
    }

    //
    //
    //
    //
    ParticleSet* GetParticleSets(int index)
    {
        NW_ASSERT(index >= 0 && index < m_ParticleSetCount);
        return this->m_ParticleSets[index];
    }

    //
    //
    //
    //
    const ParticleSet* GetParticleSets(int index) const
    {
        NW_ASSERT(index >= 0 && index < m_ParticleSetCount);
        return this->m_ParticleSets[index];
    }

    //
    //
    //
    bool AttachParticleShape(ParticleShape* node)
    {
        if (m_ParticleShapeCount >= this->m_ParticleShapes.size()) return false;

        this->m_ParticleShapes[m_ParticleSetCount] = node;
        ++m_ParticleShapeCount;
        return true;
    }

    //
    //
    u32 GetParticleShapesCount() const
    {
        return m_ParticleShapeCount;
    }

    //
    //
    //
    //
    ParticleShape* GetParticleShapes(int index)
    {
        NW_ASSERT(index >= 0 && index < m_ParticleShapeCount);
        return this->m_ParticleShapes[index];
    }

    //
    //
    //
    //
    const ParticleShape* GetParticleShapes(int index) const
    {
        NW_ASSERT(index >= 0 && index < m_ParticleShapeCount);
        return this->m_ParticleShapes[index];
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
    //
    bool HasParticle() const
    {
        bool result;
        ParticleSetsAreEmpty function(&result);
        this->ForeachConstParticleSet(function);
        return !result;
    }

    //
    //
    void ResetDebugHint()
    {
#ifdef NW_CHECK_PARTICLE_PROCESS
        ParticleSetsResetDebugHint function;
        this->ForeachParticleSet(function);
#endif
    }

    //

protected:
    virtual Result Initialize(os::IAllocator* allocator);

    //
    static void GetMemorySizeForInitialize(
        os::MemorySizeCalculator* pSize,
        ResParticleModel resource,
        const ParticleModel::Description& modelDescription);

    //----------------------------------------
    //
    //

    //
    ParticleModel(
        os::IAllocator* allocator,
        ResTransformNode resource,
        const ParticleModel::Description& description)
    : Model(
        allocator,
        resource,
        description),
        m_MaximumParticleSet(description.particleSetCount),
        m_ParticleSetCount(0),
        m_ParticleShapeCount(0),
        m_ParticleAnimFrameController(0, 16777215, anim::PlayPolicy_Loop)
    {}

    //
    virtual ~ParticleModel()
    {
        for (int i = 0; i < m_ParticleShapes.size(); ++i) // TBD
        {
            SafeDestroy(m_ParticleShapes[i]);

            // Since m_ParticleSet is a product of the AttachChild function, it is automatically deallocated by SceneNode.
            // 
        }
    }

    //

private:
    uint m_MaximumParticleSet;

    int m_ParticleSetCount;
    ut::MoveArray<ParticleSet*> m_ParticleSets;

    int m_ParticleShapeCount;
    ut::MoveArray<ParticleShape*> m_ParticleShapes;

    anim::AnimFrameController m_ParticleAnimFrameController;
};


} // namespace gfx
} // namespace nw

#endif // NW_GFX_PARTICLEMODEL_H_
