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

#ifndef NW_GFX_USERRENDERNODE_H_
#define NW_GFX_USERRENDERNODE_H_

#include <nw/gfx/gfx_TransformNode.h>

#include <nw/gfx/gfx_UserRenderCommand.h>

#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 1301 // padding inserted in struct.
#endif
namespace nw
{
namespace gfx
{

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class UserRenderNode : public TransformNode
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(UserRenderNode);

public:
    NW_UT_RUNTIME_TYPEINFO;

    //
    struct Description : public TransformNode::Description
    {
        gfx::ResMaterial::TranslucencyKind translucencyKind; //
        u8 priority; //
        u8 layerId;  //
        UserRenderCommand* userRenderCommand; //

        //
        Description() : translucencyKind(nw::gfx::ResMaterial::TRANSLUCENCY_KIND_OPAQUE), priority(0), layerId(0), userRenderCommand(NULL) {}
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
        UserRenderNode* Create(os::IAllocator* allocator);

        //
        //
        //
        //
        //
        //
        size_t GetMemorySize(size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT) const
        {
            os::MemorySizeCalculator size(alignment);

            size += sizeof(UserRenderNode);

            GetMemorySizeForInitialize(&size, m_Description);

            return size.GetSizeWithPadding(alignment);
        }

    private:
        UserRenderNode::Description m_Description;
    };

    //
    //
    //
    //
    static size_t GetMemorySize(
        Description description,
        size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT)
    {
        os::MemorySizeCalculator size(alignment);

        GetMemorySizeInternal(&size, description);

        return size.GetSizeWithPadding(alignment);
    }

    //
    static void GetMemorySizeInternal(
        os::MemorySizeCalculator* pSize,
        Description description)
    {
        os::MemorySizeCalculator& size = *pSize;

        size += sizeof(UserRenderNode);
        GetMemorySizeForInitialize(pSize, description);
    }

    //

    //----------------------------------------
    //
    //

    //
    void SetTranslucencyKind(const gfx::ResMaterial::TranslucencyKind translucencyKind)
    {
        m_TranslucencyKind = translucencyKind;
    }

    //
    //
    //
    gfx::ResMaterial::TranslucencyKind GetTranslucencyKind() const
    {
        return m_TranslucencyKind;
    }

    //
    void SetPriority(const u8 priority)
    {
        m_Priority = priority;
    }

    //
    //
    //
    u8 GetPriority() const
    {
        return m_Priority;
    }

    //
    void SetLayerId(const u8 layerId)
    {
        m_LayerId = layerId;
    }

    //
    //
    //
    u8 GetLayerId() const
    {
        return m_LayerId;
    }

    //
    //
    //
    void SetUserRenderCommand(UserRenderCommand* userRenderCommand)
    {
        // Cancel the UserRenderNode for the already set UserRenderCommand.
        UserRenderCommand* lastUserRenderCommand = this->GetUserRenderCommand();
        if ( lastUserRenderCommand != NULL )
        {
            lastUserRenderCommand->SetUserRenderNode(NULL);
        }

        m_UserRenderCommand = userRenderCommand;
        userRenderCommand->SetUserRenderNode(this);
    }

    //
    //
    //
    UserRenderCommand* GetUserRenderCommand() const
    {
        return m_UserRenderCommand;
    }

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    virtual void Accept(ISceneVisitor* visitor);

    //

protected:
    //----------------------------------------
    //
    //

    //
    UserRenderNode(
        os::IAllocator* allocator,
        const UserRenderNode::Description& description);

    //
    virtual ~UserRenderNode()
    {
    }

    //

    //
    //
    //
    static void GetMemorySizeForInitialize(
        os::MemorySizeCalculator* pSize,
        Description description)
    {
        NW_ASSERT(description.isFixedSizeMemory);

        return TransformNode::GetMemorySizeForInitialize(pSize, ResTransformNode(), description);
    }

protected:
    gfx::ResMaterial::TranslucencyKind m_TranslucencyKind;
    u8                                 m_Priority;
    u8                                 m_LayerId;
    UserRenderCommand*                 m_UserRenderCommand;

    Description m_Description;
};

} // namespace gfx
} // namespace nw
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 1301 // padding inserted in struct.
#endif

#endif // NW_GFX_USERRENDERNODE_H_
