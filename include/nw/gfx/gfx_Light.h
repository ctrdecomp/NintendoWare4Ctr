#pragma once

#include <nw/gfx/gfx_TransformNode.h>
#include <nw/gfx/gfx_AnimObject.h>
#include <nw/gfx/res/gfx_ResLight.h>

#include <nw/math/math_Transform.h>

namespace nw{
namespace gfx{

class Light : public TransformNode
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(Light);

public:
    NW_UT_RUNTIME_TYPEINFO;

    struct Description : public TransformNode::Description
    {
        Description() {}
    };

    virtual void Accept(ISceneVisitor* visitor);

    AnimGroup* GetAnimGroup() { return m_AnimGroup; }

    const AnimGroup* GetAnimGroup() const { return m_AnimGroup; }

    AnimObject* GetAnimObject()
    {
        NW_NULL_ASSERT(m_AnimBinding);
        return m_AnimBinding->GetAnimObject(0);
    }

    const AnimObject* GetAnimObject() const
    {
        NW_NULL_ASSERT(m_AnimBinding);
        return m_AnimBinding->GetAnimObject(0);
    }
protected:
    Light(nw::os::IAllocator* allocator,ResTransformNode resObj,const Light::Description& description): 
        TransformNode(allocator,resObj,description),
        m_OriginalValue(NULL),
        m_AnimGroup(NULL) {}

    virtual ~Light()
    {
        ut::SafeDestroy(this->m_AnimGroup);
    }

    Result CreateAnimGroup(nw::os::IAllocator* allocator);

    void* GetAnimTargetObject(const anim::res::ResAnimGroupMember& anim);    

    void DestroyOriginalValue();

    virtual u32 GetLightType() const = 0;
    virtual u32 GetLightKind() const = 0;

    ResLight m_OriginalValue;
    nw::math::Transform3 m_OriginalTransform;

private:
    bool ValidateLightAnimType(AnimObject* animObject);
    AnimGroup* m_AnimGroup;
};

}
}