#pragma once

#include <nw/gfx/gfx_TransformNode.h>

#include <nw/gfx/gfx_UserRenderCommand.h>

namespace nw{
namespace gfx{

class UserRenderNode : public TransformNode
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(UserRenderNode);

public:
    NW_UT_RUNTIME_TYPEINFO;

    struct Description : public TransformNode::Description
    {
        gfx::ResMaterial::TranslucencyKind translucencyKind;
        u8 priority;
        u8 layerId;
        UserRenderCommand* userRenderCommand;

        Description() : translucencyKind(gfx::ResMaterial::TRANSLUCENCY_KIND_OPAQUE), priority(0), layerId(0), userRenderCommand(NULL) {}
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

        UserRenderNode* Create(nw::os::IAllocator* allocator);

        size_t GetMemorySize(size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT) const
        {
            nw::os::MemorySizeCalculator size(alignment);

            size += sizeof(UserRenderNode);

            GetMemorySizeForInitialize(&size, m_Description);

            return size.GetSizeWithPadding(alignment);
        }

    private:
        UserRenderNode::Description m_Description;
    };

    static size_t GetMemorySize(Description description, size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT)
    {
        nw::os::MemorySizeCalculator size(alignment);

        GetMemorySizeInternal(&size, description);

        return size.GetSizeWithPadding(alignment);
    }

    static void GetMemorySizeInternal( nw::os::MemorySizeCalculator* pSize, Description description)
    {
        nw::os::MemorySizeCalculator& size = *pSize;

        size += sizeof(UserRenderNode);
        GetMemorySizeForInitialize(pSize, description);
    }

    void SetTranslucencyKind(const gfx::ResMaterial::TranslucencyKind translucencyKind)
    {
        m_TranslucencyKind = translucencyKind;
    }

    gfx::ResMaterial::TranslucencyKind GetTranslucencyKind() const
    {
        return m_TranslucencyKind;
    }

    void SetPriority(const u8 priority)
    {
        m_Priority = priority;
    }

    u8 GetPriority() const   
    {
        return m_Priority;
    }

    void SetLayerId(const u8 layerId)
    {
        m_LayerId = layerId;
    }

    u8 GetLayerId() const
    {
        return m_LayerId;
    }

    void SetUserRenderCommand(UserRenderCommand* userRenderCommand)
    {

        UserRenderCommand* lastUserRenderCommand = this->GetUserRenderCommand();
        if (lastUserRenderCommand != NULL)
        {
            lastUserRenderCommand->SetUserRenderNode(NULL);
        }

        m_UserRenderCommand = userRenderCommand;
        userRenderCommand->SetUserRenderNode(this);
    }
    
    UserRenderCommand* GetUserRenderCommand() const
    {
        return m_UserRenderCommand;
    }

    virtual void Accept(ISceneVisitor* visitor);
    
protected:

    UserRenderNode(nw::os::IAllocator* allocator,const UserRenderNode::Description& description);
    virtual ~UserRenderNode() {}

    static void GetMemorySizeForInitialize( nw::os::MemorySizeCalculator* pSize,        Description description)
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

}
}