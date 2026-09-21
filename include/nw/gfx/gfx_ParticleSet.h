#pragma once

#include <nw/gfx/gfx_TransformNode.h>

#include <nw/gfx/gfx_ParticleModel.h>
#include <nw/gfx/res/gfx_ResParticleSet.h>
#include <nw/gfx/res/gfx_ResParticleAnimationOption.h>
#include <nw/gfx/gfx_ParticleRandom.h>
#include <nw/gfx/gfx_ParticleTime.h>

namespace nw{
namespace os{
    class IAllocator;
}
namespace gfx{

class ParticleCollection;
class ParticleContext;
class ParticleShape;

class ParticleSet : public SceneNode
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(ParticleSet);

public:
    NW_UT_RUNTIME_TYPEINFO;

    struct Description : public SceneNode::Description
    {
        s32 maxInitializers;
        s32 maxUpdaters;

        Description():
            maxInitializers(0),
            maxUpdaters(0) {}
    };

    struct Initializer
    {
        bool m_IsCopied;
        const ResParticleInitializerData* resource;
        u32 work;

        nw::ut::ResTypeInfo m_Type;
        void* m_TargetStreams[2];
    };

    struct Updater
    {
        bool m_IsCopied;
        const ResParticleUpdaterData* resource;
        u32 work;

        nw::ut::ResTypeInfo m_Type;
        void* m_TargetStreams[2];

        u32 m_Flags;

        enum Flag
        {
            FLAG_IS_HAS_CURVE_ANIM = 0x1 << 0
        };

        inline bool IsEnabledFlags(bit32 flags) const
        {
            return ut::CheckFlag(m_Flags, flags);
        }

        inline void EnableFlags(bit32 flags)
        {
            m_Flags = ut::EnableFlag(m_Flags, flags);
        }
    };

    static ParticleSet* Create(
        SceneNode* parent,
        ResSceneObject resource,
        const ParticleSet::Description& description,
        nw::os::IAllocator* mainAllocator,
        nw::os::IAllocator* deviceAllocator,
        ParticleShape* shape);

    static size_t GetMemorySize(
        ResParticleSet resource,
        const ParticleSet::Description& description,
        size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT)
        {
        nw::os::MemorySizeCalculator size(alignment);
        GetMemorySizeInternal(&size, resource, description);
        return size.GetSizeWithPadding(alignment);
    }

    static void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize, ResParticleSet resource, const ParticleSet::Description& description);

    static size_t GetDeviceMemorySize(ResParticleSet resource, size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT)
    {
        nw::os::MemorySizeCalculator size(alignment);
        GetDeviceMemorySizeInternal(&size, resource);
        return size.GetSizeWithPadding(alignment);
    }

    static void GetDeviceMemorySizeInternal(nw::os::MemorySizeCalculator* pSize, ResParticleSet resource);

    virtual void Accept(ISceneVisitor* visitor);

    ResParticleSet GetResParticleSet()
    {
        return ResDynamicCast<ResParticleSet>(GetResSceneObject());
    }

    const ResParticleSet GetResParticleSet() const
    {
        return ResDynamicCast<ResParticleSet>(GetResSceneObject());
    }

    bool AttachParticleCollection(ParticleCollection* node)
    {
        if (m_ParticleCollection != NULL)
        {
            return false;
        }

        m_ParticleCollection = node;
        return true;
    }

    ParticleCollection* GetParticleCollection()
    {
        return m_ParticleCollection;
    }

    const ParticleCollection* GetParticleCollection() const
    {
        return m_ParticleCollection;
    }

    nw::ut::MoveArray<Initializer>* GetInitializers()
    {
        return &m_Initializers;
    }

    const nw::ut::MoveArray<Initializer>* GetInitializers() const
    {
        return &m_Initializers;
    }

    nw::ut::MoveArray<Updater>* GetUpdaters()
    {
        return &m_Updaters;
    }

    const nw::ut::MoveArray<Updater>* GetUpdaters() const
    {
        return &m_Updaters;
    }

    void AddParticles(const nw::math::MTX34& emitterMatrix,const nw::math::VEC3* const positions,ParticleSet* parentParticleSet,
        const u16* const parentIndices,
        int count);


    void UpdateParticles(ParticleContext* particleContext, bool enableSwapBuffer = true);

    void ClearParticleCollection();

    void Srand(u32 seed)
    {
        m_ParticleRandom.Srand(seed);
    }

    const nw::math::VEC3& GetScaleOffset() const
    {
        return m_ScaleOffset;
    }

    void SetScaleOffset(const nw::math::VEC3& offset)
    {
        m_ScaleOffset = offset;
    }

    const nw::math::VEC3& GetRotateOffset() const
    {
        return m_RotateOffset;
    }

    void SetRotateOffset(const nw::math::VEC3& offset)
    {
        m_RotateOffset = offset;
    }

    const nw::math::MTX34& WorldMatrix()
    {
        if (GetResParticleSet().GetIsForceWorld())
        {
            return nw::math::MTX34::Identity();
        }
        else{
            ParticleModel* model = static_cast<ParticleModel*>(GetParent());
            return model->WorldMatrix();
        }
    }

    const nw::math::MTX34& InverseWorldMatrix()
    {
        if (GetResParticleSet().GetIsForceWorld())
        {
            return nw::math::MTX34::Identity();
        }
        else{
            ParticleModel* model = static_cast<ParticleModel*>(GetParent());
            return model->InverseWorldMatrix();
        }
    }

    bool IsAscendingOrder() const
    {
        const ResParticleShapeBuilder& resShapeBuilder = GetResParticleSet().GetParticleShapeBuilder();
        return (resShapeBuilder.IsValid()) ? resShapeBuilder.IsAscendingOrder() : true;
    }

    void ResetDebugHint()
    {
        m_ProcessStep = STEP_NONE;
    }

    void BeginDraw()
    {
        m_ProcessStep = STEP_DRAW;
    }

    void BeginUpdate(bool enableSwapBuffer)
    {
        if (enableSwapBuffer)
        {
            NW_WARNING(m_ProcessStep == STEP_NONE || m_ProcessStep == STEP_DRAW,
                "detected an illegal order(Update with SwapBuffer Prev:%d)", m_ProcessStep);
            m_ProcessStep = STEP_UPDATE1;
        }
        else{
            NW_WARNING(
                m_ProcessStep == STEP_UPDATE1 || m_ProcessStep == STEP_UPDATE2,
                "detected an illegal order(Update without SwapBuffer Prev:%d)", m_ProcessStep);
            m_ProcessStep = STEP_UPDATE2;
        }
    }

    void InitializeParticles(int startIndex, int count, int incrIndex, ParticleTime time);

protected:
    ParticleSet(nw::os::IAllocator* allocator, ResParticleSet resObj, const ParticleSet::Description& description);

    virtual ~ParticleSet();

private:
    ParticleCollection* m_ParticleCollection;

    ParticleRandom m_ParticleRandom;

    nw::ut::MoveArray<Initializer> m_Initializers;
    nw::ut::MoveArray<Updater> m_Updaters;

    nw::math::VEC3 m_ScaleOffset;
    nw::math::VEC3 m_RotateOffset;

    bool m_UsePrevTranslate;

    enum ProcessStep
    {
        STEP_NONE,
        STEP_UPDATE1,
        STEP_UPDATE2,
        STEP_DRAW
    } m_ProcessStep;
};

}
}