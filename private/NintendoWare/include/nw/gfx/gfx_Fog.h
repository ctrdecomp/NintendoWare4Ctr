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

#ifndef NW_GFX_FOG_H_
#define NW_GFX_FOG_H_

#include <nw/gfx/gfx_TransformNode.h>
#include <nw/gfx/res/gfx_ResFog.h>

#include <nw/ut/ut_MovePtr.h>
#include <functional>

namespace nw
{
namespace gfx
{

class Camera;

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class Fog : public TransformNode
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(Fog);

    #define NW_FOG_TABLE_COMMAND_NUM    (132)   // idex(2) + table128(129) + terminate(1)
    #define NW_FOG_TABLE_COMMAND_SIZE   (NW_FOG_TABLE_COMMAND_NUM * 4)

public:
    NW_UT_RUNTIME_TYPEINFO;

    //
    struct Description : public TransformNode::Description
    {
        //
        Description(){}
    };

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    struct UpdateFunctor
    {
        UpdateFunctor(Camera* camera) : camera(camera) {}

        void operator() (Fog* fog)
        {
            if (fog != NULL)
            {
                fog->Update(camera);
            }
        }

        Camera* camera;
    };

    //----------------------------------------
    //
    //

    //
    //
    //
    class DynamicBuilder
    {
    public:
        //
        DynamicBuilder() {}
        //
        ~DynamicBuilder() {}

        //
        //
        //
        //
        //
        DynamicBuilder& IsFixedSizeMemory(bool isFixedSizeMemory)
        {
            m_Description.isFixedSizeMemory = isFixedSizeMemory;
            return *this;
        }

        //
        DynamicBuilder& MaxChildren(int maxChildren)
        {
            m_Description.maxChildren = maxChildren;
            return *this;
        }

        //
        DynamicBuilder& MaxCallbacks(int maxCallbacks)
        {
            m_Description.maxCallbacks = maxCallbacks;
            return *this;
        }

        //
        //
        //
        //
        //
        //
        Fog* Create(os::IAllocator* allocator);

        //
        //
        //
        //
        //
        //
        size_t GetMemorySize(size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT) const;

    private:
        Fog::Description m_Description;
    };

    //
    //
    //
    //
    //
    //
    //
    //
    //
    static Fog* Create(
        SceneNode* parent,
        ResSceneObject resource,
        const Fog::Description& description,
        os::IAllocator* allocator);

    //
    //
    //
    //
    //
    static size_t GetMemorySize(
        ResFog resource,
        Description description,
        size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT
    )
    {
        os::MemorySizeCalculator size(alignment);

        GetMemorySizeInternal(&size, resource, description);

        return size.GetSizeWithPadding(alignment);
    }

    //
    static void GetMemorySizeInternal(
        os::MemorySizeCalculator* pSize,
        ResFog resource,
        Description description);

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    void Update(const Camera* camera);

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
    ResFog GetResFog()
    {
        return ResStaticCast<ResFog>(this->GetResSceneObject());
    }

    //
    const ResFog GetResFog() const
    {
        return ResStaticCast<ResFog>(this->GetResSceneObject());
    }

    //

    //----------------------------------------
    //
    //

    //
    AnimGroup* GetAnimGroup() { return m_AnimGroup; }

    //
    const AnimGroup* GetAnimGroup() const { return m_AnimGroup; }

    //
    AnimObject* GetAnimObject() { return m_AnimBinding->GetAnimObject(0); }

    //
    const AnimObject* GetAnimObject() const { return m_AnimBinding->GetAnimObject(0); }

    //
    //
    //
    void SetAnimObject(AnimObject* animObject) { m_AnimBinding->SetAnimObject(0, animObject); }

    //
protected:
    struct ResFogDataDestroyer : public std::unary_function<ResFogData*, void>
    {
        ResFogDataDestroyer(os::IAllocator* allocator = 0) : m_Allocator(allocator)
        {}
        result_type operator()(argument_type data)
        {
            DestroyResFog(m_Allocator, data);
        }

        os::IAllocator* m_Allocator;
    };

    struct ResFogUpdaterDataDestroyer : public std::unary_function<ResFogUpdaterData*, void>
    {
        ResFogUpdaterDataDestroyer(os::IAllocator* allocator = 0) : m_Allocator(allocator)
        {}
        result_type operator()(argument_type data)
        {
            DestroyResFogUpdater(m_Allocator, data);
        }

        os::IAllocator* m_Allocator;
    };

    //
    typedef ut::MovePtr<ResFogData, ResFogDataDestroyer> ResPtr;
    typedef ut::MovePtr<ResFogUpdaterData, ResFogUpdaterDataDestroyer> ResUpdaterPtr;

    //----------------------------------------
    //
    //

    //
    Fog(
        os::IAllocator* allocator,
        ResFog resObj,
        const Fog::Description& description)
     : TransformNode(
        allocator,
        resObj,
        description),
        m_Near(0.0f),
        m_Far(0.0f),
        m_WScale(0.0f),
        m_AnimGroup(NULL),
        m_OriginalValue(NULL)
    {}

    //
    Fog(
        os::IAllocator* allocator,
        ResPtr resource,
        const Fog::Description& description)
    : TransformNode(
        allocator,
        ResFog(resource.Get()),
        description),
      m_Resource(resource),
      m_Near(0.0f),
      m_Far(0.0f),
      m_WScale(0.0f),
      m_AnimGroup(NULL),
      m_OriginalValue(NULL)
    {}

    //
    virtual ~Fog()
    {
        ut::SafeDestroy(m_AnimGroup);

        if (m_OriginalValue.IsValid())
        {
            GetAllocator().Free(m_OriginalValue.ptr());
            m_OriginalValue = ResFog(NULL);
        }
    }

    //

    virtual Result Initialize(os::IAllocator* allocator);

private:
    // Creates an animation group.
    Result CreateAnimGroup(os::IAllocator* allocator);

    // Cancels references to animation members.
    void BindAnim(AnimGroup* animGroup);

    Result CreateOriginalValue(os::IAllocator* allocator);

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    static ResFogData*    CreateResFog(os::IAllocator* allocator, const char* name = NULL);

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    static void DestroyResFog(os::IAllocator* allocator, ResFogData* resFog);

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    static ResFogUpdaterData*    CreateResFogUpdater(os::IAllocator* allocator);

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    static void DestroyResFogUpdater(os::IAllocator* allocator, ResFogUpdaterData* resFogUpdater);

    void SetupFogSampler(
        ResImageLookupTable fogSampler,
        ResFogUpdater fogUpdater,
        const math::MTX44& inverseProjectionMatrix);

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

} // namespace gfx
} // namespace nw

#endif // NW_GFX_FOG_H_
