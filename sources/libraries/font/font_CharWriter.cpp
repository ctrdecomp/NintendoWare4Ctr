// Filename: font_CharWriter.cpp
//
// Project: NintendoWare4Ctr

#include <GLES2/gl2.h>
#include <GLES2/gl2extern.h>
#include <nn/types.h>
#include <nn/Assert.h>

#include <nw/font/font_CharWriter.h>
#include <nw/font/font_DispStringBuffer.h>

#include <nn/math/math_Utility.h>

#include <cstdarg>
#include <cstdio>

namespace nw {
namespace font {
namespace {

void MultiplyAlpha(ut::FloatColor* pDst,const ut::Color8 src,u8 alpha)
{
    const f32 floatAlphaMax = ut::Color8::ALPHA_MAX;

    pDst->r = src.r / floatAlphaMax;
    pDst->g = src.g / floatAlphaMax;
    pDst->b = src.b / floatAlphaMax;
    pDst->a = (src.a / floatAlphaMax) * (alpha / floatAlphaMax);
}

}

CharWriter::CharWriter() :   
    m_FixedWidth(0),
    m_pFont(NULL),
    m_pTextWriterResource(NULL),
    m_pDispStringBuffer(NULL),
    m_IsWidthFixed(false),
    m_Alpha(ut::Color8::ALPHA_MAX)
{
    this->ResetColorMapping();
    this->SetGradationMode(GRADMODE_NONE);
    this->SetTextColor(ut::Color8::WHITE);
    this->SetScale(1, 1);
    this->SetCursor(0, 0, 0);
}

CharWriter::~CharWriter() { }

void CharWriter::SetupGXCommon()
{
    const int *const locations = *this->m_pTextWriterResource->GetTexEnvUniformLocations();

    glUniform1i(locations[internal::LOC_FRAGMENTLIGHTING_ENABLED], GL_FALSE);
    glUniform1i(locations[internal::LOC_TEXTURE0_SAMPLERTYPE], GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(locations[internal::LOC_FOG_MODE], GL_FALSE);
    glUniform1i(locations[internal::LOC_FRAGOPERATION_ENABLEALPHATEST], GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    glDisable(GL_COLOR_LOGIC_OP);
}

void CharWriter::SetupGX()
{
    this->m_pTextWriterResource->ResetLoadingTexture();
    SetupGXCommon();

    bool bAlphaTex = false;
    if (this->m_pFont)
    {
        switch (m_pFont->GetTextureFormat())
        {
        case FONT_SHEET_FORMAT_A4:
        case FONT_SHEET_FORMAT_A8:
            bAlphaTex = true;
            break;
        }
    }

    if (this->m_Alpha != ut::Color8::ALPHA_MAX || this->m_ColorMapping.min != DEFAULT_COLOR_MAPPING_MIN || this->m_ColorMapping.max != DEFAULT_COLOR_MAPPING_MAX)
    {
        SetupGXWithColorMapping(bAlphaTex);
    }
    else
    {
        this->SetupGXDefault(bAlphaTex);
    }

    this->SetupVertexFormat();
}

void CharWriter::SetFontSize(f32 width,f32 height)
{
    NN_POINTER_ASSERT(this->m_pFont);
    SetScale(width / this->m_pFont->GetWidth(), height / this->m_pFont->GetHeight());
}

f32 CharWriter::GetFontWidth() const
{
    NN_POINTER_ASSERT(this->m_pFont);
    return this->m_pFont->GetWidth() * this->m_Scale.x;
}

f32 CharWriter::GetFontHeight() const
{
    NN_POINTER_ASSERT(m_pFont);
    return this->m_pFont->GetHeight() * this->m_Scale.y;
}

f32 CharWriter::GetFontAscent() const
{
    NN_POINTER_ASSERT(this->m_pFont);
    return this->m_pFont->GetAscent() * this->m_Scale.y;
}

f32 CharWriter::GetFontDescent() const
{
    NN_POINTER_ASSERT(this->m_pFont);
    return this->m_pFont->GetDescent() * this->m_Scale.y;
}

f32 CharWriter::Print(CharCode code)
{
    Glyph glyph;
    this->m_pFont->GetGlyph(&glyph, code);

    CharWidths &widths = glyph.widths;

    f32 width;
    f32 left;
    if (this->m_IsWidthFixed)
    {
        f32 margin = (this->m_FixedWidth - widths.charWidth * this->m_Scale.x) / 2;

        width = this->m_FixedWidth;
        left = margin + widths.left * this->m_Scale.x;
    }
    else
    {
        width = widths.charWidth * m_Scale.x;
        left = widths.left * m_Scale.x;
    }

    PrintGlyph(this->m_CursorPos.x + left, glyph);

    this->m_CursorPos.x += width;

    return width;
}

void CharWriter::DrawGlyph(const Glyph& glyph)
{
    NN_POINTER_ASSERT(&glyph);
    PrintGlyph(this->m_CursorPos.x, glyph);
    this->m_CursorPos.x += glyph.widths.glyphWidth * this->m_Scale.x;
}

void CharWriter::PrintGlyph(f32 x,const Glyph& glyph)
{
    NN_POINTER_ASSERT(&glyph);

    const f32 y = this->m_CursorPos.y;

    const f32 texLeft = 1.0f * glyph.cellX / glyph.texWidth;
    const f32 texRight = 1.0f * (glyph.cellX + glyph.widths.glyphWidth) / glyph.texWidth;

    const f32 texTop = 1.0f * (glyph.texHeight - glyph.cellY) / glyph.texHeight;
    const f32 texBottom = 1.0f * (glyph.texHeight - (glyph.cellY + glyph.height)) / glyph.texHeight;

    if (NULL != this->m_pDispStringBuffer)
    {
        const f32 width = glyph.widths.glyphWidth * this->m_Scale.x;
        const f32 height = - glyph.height * this->m_Scale.y;
        const u32 charIdx = this->m_pDispStringBuffer->charCount;

        if (charIdx >= this->m_pDispStringBuffer->charCountMax)
        {
            return;
        }

        this->m_pDispStringBuffer->charCount++;

        internal::CharAttribute* pCharAttrs = &this->m_pDispStringBuffer->GetCharAttrs()[charIdx];

        pCharAttrs->pos.Set(width,height,x,y);

        for (int i = 0; i < internal::TEXTCOLOR_MAX; ++i)
        {
            pCharAttrs->color[i] = this->m_TextColors[i];
        }

        pCharAttrs->tex.Set(texLeft,texTop,texRight,texBottom);
        pCharAttrs->pTexObj = glyph.pTextureObject;
    }
    else{
        const f32 posLeft = x;
        const f32 posRight = posLeft + glyph.widths.glyphWidth * this->m_Scale.x;

        const f32 posTop = y + glyph.height * this->m_Scale.y;
        const f32 posBottom = y;


        internal::VertexAttribute *const pVtxAttrs = this->m_pTextWriterResource->GetVertexAttributeArray();

        {
            GLfloat* attrs = pVtxAttrs[internal::TRIFAN_VTX_RT].pos;
            attrs[internal::POS_X] = posRight;
            attrs[internal::POS_Y] = posTop;

            attrs = pVtxAttrs[internal::TRIFAN_VTX_LT].pos;
            attrs[internal::POS_X] = posLeft;
            attrs[internal::POS_Y] = posTop;

            attrs = pVtxAttrs[internal::TRIFAN_VTX_LB].pos;
            attrs[internal::POS_X] = posLeft;
            attrs[internal::POS_Y] = posBottom;

            attrs = pVtxAttrs[internal::TRIFAN_VTX_RB].pos;
            attrs[internal::POS_X] = posRight;
            attrs[internal::POS_Y] = posBottom;
        }
        for (int i = 0; i < internal::TRIFAN_VTX_MAX; ++i)
        {
            pVtxAttrs[i].color = this->m_VertexColors[i];
        }

        {
            GLfloat* attrs = pVtxAttrs[internal::TRIFAN_VTX_RT].tex;
            attrs[internal::TEXCOORD_X] = texRight;
            attrs[internal::TEXCOORD_Y] = texTop;

            attrs = pVtxAttrs[internal::TRIFAN_VTX_LT].tex;
            attrs[internal::TEXCOORD_X] = texLeft;
            attrs[internal::TEXCOORD_Y] = texTop;

            attrs = pVtxAttrs[internal::TRIFAN_VTX_LB].tex;
            attrs[internal::TEXCOORD_X] = texLeft;
            attrs[internal::TEXCOORD_Y] = texBottom;

            attrs = pVtxAttrs[internal::TRIFAN_VTX_RB].tex;
            attrs[internal::TEXCOORD_X] = texRight;
            attrs[internal::TEXCOORD_Y] = texBottom;
        }

        this->LoadTexture(glyph);
        this->m_pTextWriterResource->UpdatePosZ(this->m_CursorPos.z);

        glDrawArrays(GL_TRIANGLE_FAN, 0, internal::TRIFAN_VTX_MAX);
    }
}

void CharWriter::LoadTexture(const Glyph& glyph)
{
    NN_POINTER_ASSERT(&glyph);

    bool doLoad = false;
    GLuint texName = 0;

    if (NULL == glyph.pTextureObject)
    {
        texName = this->m_pTextWriterResource->m_TextureId;
        doLoad =  m_pTextWriterResource->SetLoadingTexture(glyph.pTexture);
    }
    else
    {
        texName = glyph.pTextureObject->GetName();
        if (texName == 0)
        {
            glGenTextures(1, &texName);
            const_cast<internal::TextureObject*>(glyph.pTextureObject)->SetName(texName);
            doLoad = true;
        }
        else
        {
            doLoad = 0 != glyph.isSheetUpdated;
        }
    }

    glBindTexture(GL_TEXTURE_2D, texName);

    if (doLoad)
    {
        internal::LoadTexture(glyph.texWidth,glyph.texHeight,glyph.texFormat,glyph.pTexture, m_pFont->IsLinearFilterEnableAtSmall(), m_pFont->IsLinearFilterEnableAtLarge());
    }
}

void CharWriter::StartPrint()
{
    m_pDispStringBuffer->charCount = 0;
    this->m_pDispStringBuffer->ClearCommand();
}

u32 CharWriter::GetDispStringBufferSize(u32 charNum)
{
    const u32 drawFlagBytes = nn::math::RoundUp(charNum, 8) / 8;
    return sizeof(DispStringBuffer)+ sizeof(internal::CharAttribute) * charNum + nn::math::RoundUp(drawFlagBytes, sizeof(u32))
                + sizeof(u32) * DispStringBuffer::CalcCommandBufferCapacity(charNum);
}

DispStringBuffer* CharWriter::InitDispStringBuffer(void* drawBuffer,u32 charNum)
{
    return new (drawBuffer) DispStringBuffer(charNum);
}

void CharWriter::SetupGXDefault(bool bAlphaTex)
{
    const int (*locations)[internal::TCLOC_MAX] = this->m_pTextWriterResource->m_TexEnvUniformLocations;

    static const GLint src[] = { GL_PRIMARY_COLOR, GL_TEXTURE0, GL_CONSTANT };
    static const GLint operandRgb[] = { GL_SRC_COLOR, GL_SRC_COLOR, GL_SRC_COLOR };
    static const GLint operandAlp[] = { GL_SRC_ALPHA, GL_SRC_ALPHA, GL_SRC_ALPHA };
    const GLint combineMode = bAlphaTex ? GL_REPLACE: GL_MODULATE;

    glUniform3iv(locations[internal::TEXENV_5][internal::TCLOC_SRCRGB], 1, src);
    glUniform3iv(locations[internal::TEXENV_5][internal::TCLOC_SRCALPHA], 1, src);
    glUniform3iv(locations[internal::TEXENV_5][internal::TCLOC_OPERANDRGB], 1, operandRgb);
    glUniform3iv(locations[internal::TEXENV_5][internal::TCLOC_OPERANDALPHA], 1, operandAlp);
    glUniform1i (locations[internal::TEXENV_5][internal::TCLOC_COMBINERGB], combineMode);
    glUniform1i (locations[internal::TEXENV_5][internal::TCLOC_COMBINEALPHA], GL_MODULATE);
    glUniform1f (locations[internal::TEXENV_5][internal::TCLOC_SCALERGB], 1.0);
    glUniform1f (locations[internal::TEXENV_5][internal::TCLOC_SCALEALPHA], 1.0);
}

void CharWriter::SetupGXWithColorMapping(bool bAlphaTex)
{
    const int (*locations)[internal::TCLOC_MAX] = this->m_pTextWriterResource->m_TexEnvUniformLocations;

    static const GLint Src0[] = { GL_TEXTURE0, GL_CONSTANT, GL_CONSTANT };
    static const GLint OpRgb0[] = { GL_SRC_COLOR, GL_SRC_COLOR, GL_SRC_COLOR };
    static const GLint OpRgb1[] = { GL_ONE_MINUS_SRC_COLOR, GL_SRC_COLOR, GL_SRC_COLOR };
    static const GLint OpAlp0[] = { GL_SRC_ALPHA, GL_SRC_ALPHA, GL_SRC_ALPHA };
    glUniform3iv(locations[internal::TEXENV_3][internal::TCLOC_SRCRGB      ], 1, Src0);
    glUniform3iv(locations[internal::TEXENV_3][internal::TCLOC_SRCALPHA    ], 1, Src0);
    glUniform3iv(locations[internal::TEXENV_3][internal::TCLOC_OPERANDRGB  ], 1, bAlphaTex ? OpRgb1: OpRgb0);
    glUniform3iv(locations[internal::TEXENV_3][internal::TCLOC_OPERANDALPHA], 1, OpAlp0);
    glUniform1i (locations[internal::TEXENV_3][internal::TCLOC_COMBINERGB  ], GL_MODULATE);
    glUniform1i (locations[internal::TEXENV_3][internal::TCLOC_COMBINEALPHA], GL_MODULATE);
    glUniform1f (locations[internal::TEXENV_3][internal::TCLOC_SCALERGB    ], 1.0);
    glUniform1f (locations[internal::TEXENV_3][internal::TCLOC_SCALEALPHA  ], 1.0);
    ut::FloatColor maxCol;
    MultiplyAlpha(&maxCol, this->m_ColorMapping.max, this->m_Alpha);
    glUniform4fv(locations[internal::TEXENV_3][internal::TCLOC_CONSTRGBA   ], 1, maxCol.ToArray());

    static const GLint Src1[] = { GL_TEXTURE0, GL_CONSTANT, GL_PREVIOUS };
    static const GLint OpAlp1[] = { GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_SRC_ALPHA };
    glUniform3iv(locations[internal::TEXENV_4][internal::TCLOC_SRCRGB      ], 1, Src1);
    glUniform3iv(locations[internal::TEXENV_4][internal::TCLOC_SRCALPHA    ], 1, Src1);
    glUniform3iv(locations[internal::TEXENV_4][internal::TCLOC_OPERANDRGB  ], 1, bAlphaTex ? OpRgb0: OpRgb1);
    glUniform3iv(locations[internal::TEXENV_4][internal::TCLOC_OPERANDALPHA], 1, OpAlp1);
    glUniform1i (locations[internal::TEXENV_4][internal::TCLOC_COMBINERGB  ], GL_MULT_ADD_DMP);
    glUniform1i (locations[internal::TEXENV_4][internal::TCLOC_COMBINEALPHA], GL_MULT_ADD_DMP);
    glUniform1f (locations[internal::TEXENV_4][internal::TCLOC_SCALERGB    ], 1.0);
    glUniform1f (locations[internal::TEXENV_4][internal::TCLOC_SCALEALPHA  ], 1.0);
    ut::FloatColor minCol;
    MultiplyAlpha(&minCol, this->m_ColorMapping.min, this->m_Alpha);
    glUniform4fv(locations[internal::TEXENV_4][internal::TCLOC_CONSTRGBA   ], 1, minCol.ToArray());

    static const GLint Src2[] = { GL_PRIMARY_COLOR, GL_PREVIOUS, GL_PREVIOUS };
    glUniform3iv(locations[internal::TEXENV_5][internal::TCLOC_SRCRGB      ], 1, Src2);
    glUniform3iv(locations[internal::TEXENV_5][internal::TCLOC_SRCALPHA    ], 1, Src2);
    glUniform3iv(locations[internal::TEXENV_5][internal::TCLOC_OPERANDRGB  ], 1, OpRgb0);
    glUniform3iv(locations[internal::TEXENV_5][internal::TCLOC_OPERANDALPHA], 1, OpAlp0);
    glUniform1i (locations[internal::TEXENV_5][internal::TCLOC_COMBINERGB  ], GL_MODULATE);
    glUniform1i (locations[internal::TEXENV_5][internal::TCLOC_COMBINEALPHA], GL_MODULATE);
    glUniform1f (locations[internal::TEXENV_5][internal::TCLOC_SCALERGB    ], 1.0);
    glUniform1f (locations[internal::TEXENV_5][internal::TCLOC_SCALEALPHA  ], 1.0);
}

}
}