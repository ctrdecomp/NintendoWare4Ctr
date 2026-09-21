// Filename: lyt_Common.cpp
//
// Project: NintendoWare4Ctr

#include <nw/lyt/lyt_Common.h>
#include <nw/lyt/lyt_DrawInfo.h>
#include <nw/lyt/lyt_GraphicsResource.h>
#include <nw/lyt/lyt_Material.h>
#include <nw/lyt/lyt_Pane.h>
#include <nw/lyt/lyt_Layout.h>

#define ARRAY_LENGTH(a)   (sizeof(a) / sizeof((a)[0]))

namespace nw {
namespace lyt {
namespace {
    static const GLushort s_VertexIndex[] ={
        VERTEX_RT, VERTEX_LT, VERTEX_RB,
        VERTEX_LT, VERTEX_LB, VERTEX_RB,
    };

    static const int s_NumVertex = ARRAY_LENGTH(s_VertexIndex);
}
namespace internal{

using namespace nw::math;

bool EqualsResName(const char* name1, const char* name2)
{
    return 0 == std::strncmp(name1, name2, ResourceNameStrMax);
}

bool EqualsMaterialName(const char* name1, const char* name2)
{
    return 0 == std::strncmp(name1, name2, MaterialNameStrMax);
}

TexCoordAry::TexCoordAry():   
    m_Cap(0),
    m_Num(0),
    m_pData(NULL)
{
}

void TexCoordAry::Free()
{
    if (m_pData)
    {
        const u32 coordNum = m_Cap;
        Layout::DeleteArray<math::VEC2>(&m_pData[0][0], VERTEX_MAX * coordNum);
        m_pData = 0;

        m_Cap = 0;
        m_Num = 0;
    }
}

void TexCoordAry::Reserve(u8 num)
{
    NW_ASSERT(num <= TexMapMax);

    if (m_Cap < num)
    {
        Free();

        const u32 coordNum = num;
        math::VEC2 *const pVecAry = Layout::NewArray<math::VEC2>(VERTEX_MAX * coordNum);
        m_pData = reinterpret_cast<TexCoordQuad *>(pVecAry);
        if (m_pData)
        {
            m_Cap = num;
        }
    }
}

void TexCoordAry::SetSize(u8 num)
{
    if (m_pData && num <= m_Cap)
    {
        static const VEC2 texCoords[] ={
            VEC2(0.f, 0.f),
            VEC2(1.f, 0.f),
            VEC2(0.f, 1.f),
            VEC2(1.f, 1.f)
        };

        for (int j = m_Num; j < num; ++j)
        {
            for (int i = 0; i < VERTEX_MAX; ++i)
            {
                m_pData[j][i] = texCoords[i];
            }
        }
        m_Num = num;
    }
}

void TexCoordAry::GetCoord(u32 idx,TexCoordQuad coord) const
{
    NW_ASSERT(idx < m_Num);

    for (int i = 0; i < VERTEX_MAX; ++i)
    {
        coord[i] = m_pData[idx][i];
    }
}

void TexCoordAry::SetCoord(u32 idx,const TexCoordQuad coord)
{
    NW_ASSERT(idx < m_Num);

    for (int i = 0; i < VERTEX_MAX; ++i)
    {
        m_pData[idx][i] = coord[i];
    }
}

void TexCoordAry::Copy(const void* pResTexCoord,u8 texCoordNum)
{
    NW_ASSERT(texCoordNum <= m_Cap);

    m_Num = ut::Max(m_Num, texCoordNum);
    const math::VEC2 (*src)[VERTEX_MAX] = static_cast<const math::VEC2 (*)[VERTEX_MAX]>(pResTexCoord);
    for (int j = 0; j < texCoordNum; ++j)
    {
        for (int i = 0; i < VERTEX_MAX; ++i)
        {
            m_pData[j][i] = src[j][i];
        }
    }
}

void DrawQuad(const DrawInfo& drawInfo, const VEC2& basePt,const Size& size,u8 texCoordNum,const VEC2 (*texCoords)[VERTEX_MAX],const ut::Color8* vtxColors)
{
    GraphicsResource& gres = *drawInfo.GetGraphicsResource();

    gres.SetupProgram();

    if (vtxColors != NULL)
    {
        GLint loc = gres.GetUniformLocation(gres.UNIFORM_uVertexColor);
        glUniform4f(loc + 0, vtxColors[0].r, vtxColors[0].g, vtxColors[0].b, vtxColors[0].a);
        glUniform4f(loc + 1, vtxColors[1].r, vtxColors[1].g, vtxColors[1].b, vtxColors[1].a);
        glUniform4f(loc + 2, vtxColors[2].r, vtxColors[2].g, vtxColors[2].b, vtxColors[2].a);
        glUniform4f(loc + 3, vtxColors[3].r, vtxColors[3].g, vtxColors[3].b, vtxColors[3].a);
    }

    if (texCoordNum > 0 && texCoords != NULL)
    {
        for (int i = 0; i < TexMapMax; ++i)
        {
            int src = gres.GetTexCoordSrc(i);
            if (0 <= src && texCoords != NULL && src < texCoordNum)
            {
                GLint loc = gres.GetUniformLocation(gres.UNIFORM_uVertexTexCoord0 + i);
                glUniform4f(loc + 0, texCoords[src][0].x, texCoords[src][0].y, 0.0f, 1.0f);
                glUniform4f(loc + 1, texCoords[src][1].x, texCoords[src][1].y, 0.0f, 1.0f);
                glUniform4f(loc + 2, texCoords[src][2].x, texCoords[src][2].y, 0.0f, 1.0f);
                glUniform4f(loc + 3, texCoords[src][3].x, texCoords[src][3].y, 0.0f, 1.0f);
            }
        }
    }

    {
        GLint loc = gres.GetUniformLocation(gres.UNIFORM_uTransform);
        glUniform4f(loc, size.width, size.height, basePt.x, basePt.y);
    }

    gres.LoadMtxModelView();

    glDrawElements(GL_TRIANGLES, s_NumVertex, GL_UNSIGNED_SHORT, 0);
}

void FinalizeGraphics()
{
    glDisableVertexAttribArray(VERTEXATTR_POS);
    glDisableVertexAttribArray(VERTEXATTR_COLOR);
    glDisableVertexAttribArray(VERTEXATTR_TEXCOORD0);
    glDisableVertexAttribArray(VERTEXATTR_TEXCOORD1);
    glDisableVertexAttribArray(VERTEXATTR_TEXCOORD2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

}
}
}