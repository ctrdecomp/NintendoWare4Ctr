#pragma once

#include <nw/gfx/gfx_Common.h>
#include <nw/gfx/gfx_GfxObject.h>
#include <nw/gfx/res/gfx_ResSceneObject.h>
#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/ut/ut_RuntimeTypeInfo.h>
#include <nw/ut/ut_MovePtr.h>
#include <nw/ut/ut_Children.h>
#include <nw/os/os_Memory.h>

namespace nw{
namespace gfx{

class SceneNode;
class TransformNode;

typedef nw::ut::Children<SceneNode, SceneNode, nw::ut::ChildDetacher<SceneNode> > SceneNodeChildren;

class SceneObject : public GfxObject
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(SceneObject);

public:
    NW_UT_RUNTIME_TYPEINFO;

    static const int DEFAULT_MAX_CHILDREN = 8;
    static const int DEFAULT_MAX_CALLBACKS = 4;
    static const int MAX_NAME_LENGTH = 256;

    ResSceneObject GetResSceneObject() { return m_ResObject; }
    const ResSceneObject GetResSceneObject() const { return m_ResObject; }

    const char* GetName() const
    {
        NW_ASSERT(this->m_ResObject.IsValid());
        return this->m_ResObject.GetName();
    }
    
protected:
    SceneObject(nw::os::IAllocator* allocator, ResSceneObject resObj): 
        GfxObject(allocator), 
        m_ResObject(resObj) {}

    virtual ~SceneObject() {}

private:
    ResSceneObject  m_ResObject;
};

}
}