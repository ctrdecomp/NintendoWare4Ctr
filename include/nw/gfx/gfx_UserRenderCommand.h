#pragma once

#include <nw/gfx/gfx_RenderElement.h>

#include <functional>

namespace nw{
namespace gfx{

class UserRenderNode;

class UserRenderCommand : public RenderCommand{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(UserRenderCommand);
    
public:
    virtual void Invoke(RenderContext* renderContext) = 0;

    void SetUserRenderNode(UserRenderNode* userRenderNode){
        mParent = userRenderNode;
    }

    UserRenderNode* GetUserRenderNode() const{
        return mParent;
    }

protected:
    UserRenderCommand() {}
    virtual ~UserRenderCommand() {}

    UserRenderNode* mParent;
};

}
}