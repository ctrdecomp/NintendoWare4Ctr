#pragma once

#include <nw/gfx/gfx_TransformNode.h>
#include <nw/gfx/res/gfx_ResFog.h>

#include <nw/ut/ut_MovePtr.h>
#include <functional>

namespace nw{
namespace gfx{

class Camera;

class Fog : public TransformNode
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(Fog);

    #define NW_FOG_TABLE_COMMAND_NUM    (132)
    #define NW_FOG_TABLE_COMMAND_SIZE   (NW_FOG_TABLE_COMMAND_NUM * 4)

public:
    NW_UT_RUNTIME_TYPEINFO;

    struct Description : public TransformNode::Description
    {
        Description() {}
    };

    struct UpdateFunctor
    {
        UpdateFunctor(Camera* camera): 
            camera(camera) {}
        
        void operator() (Fog* fog)
        {
            if (fog != NULL)
            {
                fog->Update(camera);
            }
        }
        
        Camera* camera;
    };

    class DynamicBuilder
    {
    public:
        DynamicBuilder() {}
        ~DynamicBuilder() {}

        DynamicBuilder& IsFixedSizeMemory(bool isFixedSizeMemory)
        {
            m_Description.isFixedSizeMemory = isFixedSizeMemory;
            return *this;
        }

        DynamicBuilder& MaxChildren(int maxChildren)
        {
            m_Description.maxChildren = maxChildren;
            return *this;
        }

        DynamicBuilder& MaxCallbacks(int maxCallbacks)
        {
            m_Description.maxCallbacks = maxCallbacks;
            return *this;
        }

        Fog* Create(nw::os::IAllocator* allocator);
        size_t GetMemorySize(size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT) const;

    private:
        Fog::Description m_Description;
    };


    static Fog* Create(SceneNode* parent,ResSceneObject resource,const Fog::Description& description,nw::os::IAllocator* allocator);

    static size_t GetMemorySize(ResFog resource,Description description,size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT)
    {
        nw::os::MemorySizeCalculator size(alignment);

        GetMemorySizeInternal(&size, resource, description);

        return size.GetSizeWithPadding(alignment);
    }

    static void GetMemorySizeInternal( nw::os::MemorySizeCalculator* pSize,ResFog resource,Description description);

    void Update(const Camera* camera);

    virtual void Accept(ISceneVisitor* visitor);

    ResFog GetResFog()
    {
        return ResStaticCast<ResFog>(this->GetResSceneObject());
    }

    const ResFog GetResFog() const
    {
        return ResStaticCast<ResFog>(this->GetResSceneObject());
    }

    AnimGroup* GetAnimGroup() { return m_AnimGroup; }

    const AnimGroup* GetAnimGroup() const { return m_AnimGroup; }

    AnimObject* GetAnimObject() { return this->m_AnimBinding->GetAnimObject(0); }

    const AnimObject* GetAnimObject() const { return this->m_AnimBinding->GetAnimObject(0); }

    void SetAnimObject(AnimObject* animObject) { this->m_AnimBinding->SetAnimObject(0, animObject); }

protected:
    struct ResFogDataDestroyer : public std::unary_function<ResFogData*, void>
{
        ResFogDataDestroyer(nw::os::IAllocator* allocator = 0): 
            m_Allocator(allocator) {}
        result_type operator()(argument_type data)
        {
            DestroyResFog(this->m_Allocator, data);
        }

        nw::os::IAllocator* m_Allocator;
    };

    struct ResFogUpdaterDataDestroyer : public std::unary_function<ResFogUpdaterData*, void>
    {
        ResFogUpdaterDataDestroyer(os::IAllocator* allocator = 0): 
            m_Allocator(allocator) {}
        result_type operator()(argument_type data)
        {
            DestroyResFogUpdater(this->m_Allocator, data);
        }

        nw::os::IAllocator* m_Allocator;
    };

    typedef nw::ut::MovePtr<ResFogData, ResFogDataDestroyer> ResPtr;
    typedef nw::ut::MovePtr<ResFogUpdaterData, ResFogUpdaterDataDestroyer> ResUpdaterPtr;

    Fog(nw::os::IAllocator* allocator,ResFog resObj,const Fog::Description& description): 
        TransformNode(allocator,resObj,description),
        m_Near(0.0f),
        m_Far(0.0f),
        m_WScale(0.0f),
        m_AnimGroup(NULL),
        m_OriginalValue(NULL) {}

    Fog(nw::os::IAllocator* allocator,ResPtr resource,const Fog::Description& description): 
        TransformNode(allocator,ResFog(resource.Get()),description),
        m_Resource(resource),
        m_Near(0.0f),
        m_Far(0.0f),
        m_WScale(0.0f),
        m_AnimGroup(NULL),
        m_OriginalValue(NULL) {}

    virtual ~Fog()
    {
        nw::ut::SafeDestroy(this->m_AnimGroup);

        if (this->m_OriginalValue.IsValid())
        {
            GetAllocator().Free(this->m_OriginalValue.ptr());
            m_OriginalValue = ResFog(NULL);
        }
    }

private:
    virtual Result Initialize(nw::os::IAllocator* allocator);

    Result CreateAnimGroup(nw::os::IAllocator* allocator);

    void BindAnim(AnimGroup* animGroup);

    Result CreateOriginalValue(nw::os::IAllocator* allocator);

    static ResFogData*    CreateResFog(nw::os::IAllocator* allocator, const char* name = NULL);

    static void DestroyResFog(nw::os::IAllocator* allocator, ResFogData* resFog);

    static ResFogUpdaterData*    CreateResFogUpdater(nw::os::IAllocator* allocator);

    static void DestroyResFogUpdater(nw::os::IAllocator* allocator, ResFogUpdaterData* resFogUpdater);

    void SetupFogSampler(ResImageLookupTable fogSampler,ResFogUpdater fogUpdater,const nw::math::MTX44& inverseProjectionMatrix);

    ResPtr m_Resource;
    ResUpdaterPtr m_UpdaterCache;
    f32 m_Near;
    f32 m_Far;
    f32 m_WScale;
    AnimGroup* m_AnimGroup;
    ResFog m_OriginalValue;
    static const float FOG_DENSITY;
    static const ResFogUpdater::FogUpdaterType FOG_UPDATER_TYPE;
    static const float FOG_MAX_DEPTH;
    static const float FOG_MIN_DEPTH;
};

}
}