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

#ifndef NW_GFX_USERRENDERCOMMAND_H_
#define NW_GFX_USERRENDERCOMMAND_H_

#include <nw/gfx/gfx_RenderElement.h>

#include <functional>

namespace nw
{
namespace gfx
{

class UserRenderNode;

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class UserRenderCommand : public RenderCommand
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(UserRenderCommand);

public:
    //
    virtual void Invoke(RenderContext* renderContext) = 0;

    //
    //
    //
    void SetUserRenderNode(UserRenderNode* userRenderNode)
    {
        m_Parent = userRenderNode;
    }

    //
    //
    //
    UserRenderNode* GetUserRenderNode() const
    {
        return m_Parent;
    }

protected:
    //
    UserRenderCommand() {}
    //
    virtual ~UserRenderCommand() {}

    UserRenderNode* m_Parent;
};

//

} // namespace gfx
} // namespace nw

#endif // NW_GFX_USERRENDERCOMMAND_H_
