// Filename: lyt_Drawer.cpp
//
// Project: NintendoWare4Ctr

#include <nw/lyt/lyt_Layout.h>
#include <nw/lyt/lyt_Pane.h>
#include <nw/lyt/lyt_Drawer.h>
#include <nw/lyt/lyt_DrawerCommand.h>
#include <nw/lyt/lyt_DrawInfo.h>
#include <nw/lyt/lyt_GraphicsResource.h>

namespace nw{
namespace lyt{

Drawer::Drawer():   
    m_PrevTexObj(0),
    m_ActiveTexureNum(0),
    m_TexCoordNum(0),
    m_IsBlendDefault(true),
    m_AlphaTestEnable(false),
    m_CurrentTexEnvType(TEX_ENV_TYPE_NUM)
{
}

Drawer::Drawer(GraphicsResource& graphicsResource)
{
    this->Initialize(graphicsResource);
}

void Drawer::Draw(const Layout* pLayout, DrawInfo& drawInfo)
{
    if (pLayout == NULL)
    {
        return;
    }

    if (pLayout->GetRootPane() == NULL || pLayout->GetRootPane()->IsVisible() == false) 
        return;

    pLayout->GetRootPane()->MakeUniformData(&drawInfo, this);
}

void Drawer::Draw(const Pane* pPane, DrawInfo& drawInfo)
{
    if (pPane == NULL)
    {
        return;
    }

    if (pPane->IsVisible())
    {
        pPane->MakeUniformData(&drawInfo, this);
    }
}

void Drawer::DrawBegin(const DrawInfo& drawInfo)
{
    u32 flag = 0;
    this->DrawBegin(flag);

    Base::SetProjectionMtx(drawInfo.GetProjectionMtx());
}

void Drawer::DrawBegin(u32 flag)
{
    m_ActiveTexureNum = 0;
    m_TexCoordNum = 0;
    m_PrevTexObj = 0;
    m_CurrentTexEnvType = TEX_ENV_TYPE_NUM;
    m_IsBlendDefault = true;
    
    SetUpTextures(NULL, false);

    Base::DrawBegin(flag);
}


void Drawer::DrawEnd(u32 flag)
{
    FlushBuffer();

    Base::DrawEnd(flag);
}

void Drawer::Initialize(GraphicsResource& graphicsResource, void* vertexBuffer)
{
    const u32 cmdBufSize = GetCommandBufferSize(graphicsResource.GetRectShaderBinary(),graphicsResource.GetRectShaderBinarySize());

    const u32 allocSize = cmdBufSize + (vertexBuffer == NULL ? GetVertexBufferSize(): 0);

    void* commandBuffer = Layout::AllocDeviceMemory(allocSize, 4);

    if (vertexBuffer == NULL)
    {
        Base::Initialize(commandBuffer,graphicsResource.GetRectShaderBinary(),graphicsResource.GetRectShaderBinarySize());
    }
    else
    {
        Base::Initialize(vertexBuffer,commandBuffer,graphicsResource.GetRectShaderBinary(),graphicsResource.GetRectShaderBinarySize());
    }
}

void Drawer::Finalize()
{
    void *const comamndBuffer = this->m_CommandBuffer;

    Base::Finalize();
    Layout::FreeDeviceMemory(comamndBuffer);
}

}
}