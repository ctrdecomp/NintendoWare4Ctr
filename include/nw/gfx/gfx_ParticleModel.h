#pragma once

#include <nw/gfx/gfx_Model.h>
#include <nw/gfx/res/gfx_ResParticleModel.h>
#include <nw/gfx/gfx_ParticleShape.h>
#include <nw/anim/anim_AnimFrameController.h>
#include <nw/ut/ut_MoveArray.h>

namespace nw{
namespace gfx{

class ParticleSet;

class ParticleSetsAreEmpty
{
public:
    ParticleSetsAreEmpty(bool* result)
    {
        NW_NULL_ASSERT(result);
        m_Result = result;
        *m_Result = true;
    }

    void operator()(const ParticleSet* particleSet);

private:
    bool* m_Result;
};

class ParticleSetsResetDebugHint
{
public:
    void operator()(ParticleSet* particleSet);
};

class ParticleSetsClear
{
public:
    ParticleSetsClear() {}

    void operator()(ParticleSet* particleSet);
};

class ParticleModel : public Model
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(ParticleModel);

public:
    NW_UT_RUNTIME_TYPEINFO;

    struct Description : public Model::Description
    {
        Description() :
            particleSetCount(0) {}

        uint particleSetCount;
    };

    static ParticleModel* Create(
        SceneNode* parent,
        ResSceneObject resource,
        const ParticleModel::Description& modelDescription,
        nw::os::IAllocator* mainAllocator,
        nw::os::IAllocator* deviceAllocator);

    static size_t GetMemorySize(ResParticleModel resource,const ParticleModel::Description& modelDescription,size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT)
        {
        nw::os::MemorySizeCalculator size(alignment);

        GetMemorySizeInternal(&size, resource, modelDescription);

        return size.GetSizeWithPadding(alignment);
    }

    static void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize,ResParticleModel resource,const ParticleModel::Description& modelDescription);

    static size_t GetDeviceMemorySize(ResParticleModel resource,const ParticleModel::Description& modelDescription,size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT)
    {
        nw::os::MemorySizeCalculator size(alignment);

        GetDeviceMemorySizeInternal(&size, resource, modelDescription);

        return size.GetSizeWithPadding(alignment);
    }

    static void GetDeviceMemorySizeInternal(nw::os::MemorySizeCalculator* pSize,ResParticleModel resource,const ParticleModel::Description& modelDescription);
    
    virtual void Accept(ISceneVisitor* visitor);

    void UpdateParticleFrame()
    {
        this->m_ParticleAnimFrameController.UpdateFrame();
    }

    template<typename TFunction>
    void ForeachParticleSet(TFunction function)
    {
        for (int i = 0; i < (int)this->GetParticleSetsCount(); ++i)
        {
            ParticleSet* particleSet = this->GetParticleSets(i);

            function(particleSet);
        }
    }

    template<typename TFunction>
    void ForeachConstParticleSet(TFunction function) const
    {
        for (int i = 0; i < (int)this->GetParticleSetsCount(); ++i)
        {
            const ParticleSet* particleSet = this->GetParticleSets(i);

            function(particleSet);
        }
    }

    ResParticleModel GetResModel()
    {
        return ResDynamicCast<ResParticleModel>(this->GetResSceneObject());
    }

    const ResParticleModel GetResModel() const
    {
        return ResDynamicCast<ResParticleModel>(this->GetResSceneObject());
    }

    bool AttachParticleSet(ParticleSet* node)
    {
        if (m_ParticleSetCount >= this->m_ParticleSets.size()) return false;

        this->m_ParticleSets[m_ParticleSetCount] = node;
        ++this->m_ParticleSetCount;
        return true;
    }

    u32 GetParticleSetsCount() const
    {
        return m_ParticleSetCount;
    }

    ParticleSet* GetParticleSets(int index)
    {
        NW_ASSERT(index >= 0 && index < m_ParticleSetCount);
        return this->m_ParticleSets[index];
    }

    const ParticleSet* GetParticleSets(int index) const
    {
        NW_ASSERT(index >= 0 && index < m_ParticleSetCount);
        return this->m_ParticleSets[index];
    }

    bool AttachParticleShape(ParticleShape* node)
    {
        if (m_ParticleShapeCount >= this->m_ParticleShapes.size()) return false;

        this->m_ParticleShapes[this->m_ParticleSetCount] = node;
        ++this->m_ParticleShapeCount;
        return true;
    }

    u32 GetParticleShapesCount() const
    {
        return m_ParticleShapeCount;
    }

    ParticleShape* GetParticleShapes(int index)
    {
        NW_ASSERT(index >= 0 && index < m_ParticleShapeCount);
        return this->m_ParticleShapes[index];
    }

    const ParticleShape* GetParticleShapes(int index) const
    {
        NW_ASSERT(index >= 0 && index < m_ParticleShapeCount);
        return this->m_ParticleShapes[index];
    }

    anim::AnimFrameController& ParticleAnimFrameController()
    {
        return m_ParticleAnimFrameController;
    }

    const anim::AnimFrameController& ParticleAnimFrameController() const
    {
        return m_ParticleAnimFrameController;
    }

    bool HasParticle() const
    {
        bool result;
        ParticleSetsAreEmpty function(&result);
        this->ForeachConstParticleSet(function);
        return !result;
    }

protected:
    virtual Result Initialize(nw::os::IAllocator* allocator);

    static void GetMemorySizeForInitialize(nw::os::MemorySizeCalculator* pSize,ResParticleModel resource,const ParticleModel::Description& modelDescription);

    ParticleModel(nw::os::IAllocator* allocator,ResTransformNode resource,const ParticleModel::Description& description): 
        Model(allocator,resource,description),
        m_MaximumParticleSet(description.particleSetCount),
        m_ParticleSetCount(0),
        m_ParticleShapeCount(0),
        m_ParticleAnimFrameController(0, 16777215, anim::PlayPolicy_Loop) {}

    virtual ~ParticleModel()
    {
        for (int i = 0; i < m_ParticleShapes.size(); ++i)
            SafeDestroy(this->m_ParticleShapes[i]);
        }
    private:
        uint m_MaximumParticleSet;
        int m_ParticleSetCount;
        nw::ut::MoveArray<ParticleSet*> m_ParticleSets;
        int m_ParticleShapeCount;
        nw::ut::MoveArray<ParticleShape*> m_ParticleShapes;
        anim::AnimFrameController m_ParticleAnimFrameController;
};

}
}