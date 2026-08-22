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

#ifndef NW_GFX_SCENEOBJECT_H_
#define NW_GFX_SCENEOBJECT_H_

#include <nw/gfx/gfx_Common.h>
#include <nw/gfx/gfx_GfxObject.h>
#include <nw/gfx/res/gfx_ResSceneObject.h>
#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/ut/ut_RuntimeTypeInfo.h>
#include <nw/ut/ut_MovePtr.h>
#include <nw/ut/ut_Children.h>
#include <nw/os/os_Memory.h>

namespace nw
{
namespace gfx
{

class SceneNode;
class TransformNode;

//
typedef ut::Children<SceneNode, SceneNode, ut::ChildDetacher<SceneNode> > SceneNodeChildren;

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class SceneObject : public GfxObject
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(SceneObject);

public:
    NW_UT_RUNTIME_TYPEINFO;

    static const int DEFAULT_MAX_CHILDREN = 8; //
    static const int DEFAULT_MAX_CALLBACKS = 4; //
    static const int MAX_NAME_LENGTH = 256; //

    //----------------------------------------
    //
    //

    //
    ResSceneObject       GetResSceneObject() { return m_ResObject; }

    //
    const ResSceneObject GetResSceneObject() const { return m_ResObject; }

    //
    const char* GetName() const
    {
        NW_ASSERT(this->m_ResObject.IsValid());

        return this->m_ResObject.GetName();
    }

    //

protected:
    //----------------------------------------
    //
    //

    //
    SceneObject(os::IAllocator* allocator, ResSceneObject resObj)
    : GfxObject(allocator),
      m_ResObject(resObj)
    {}

    //
    virtual ~SceneObject() {}

    //

private:
    ResSceneObject  m_ResObject;
};

} // namespace gfx
} // namespace nw

#endif // NW_GFX_SCENEOBJECT_H_
