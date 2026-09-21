// Filename: lyt_TextBox.cpp
//
// Project: NintendoWare4Ctr

#include <nw/lyt/lyt_DrawInfo.h>
#include <nw/lyt/lyt_GraphicsResource.h>
#include <nw/lyt/lyt_Layout.h>
#include <nw/lyt/lyt_TextBox.h>
#include <nw/lyt/lyt_Material.h>
#include <nw/lyt/lyt_Animation.h>
#include <nw/lyt/lyt_Common.h>
#include <nw/lyt/lyt_ResourceAccessor.h>
#include <nw/font/font_DispStringBuffer.h>

#include <nn/math/math_Arithmetic.h>

namespace nw{
namespace lyt{
namespace internal{
namespace{

inline u8 ClampColor(s16 colVal)
{
    return u8(colVal < 0 ? 0: (colVal > 255 ? 255: colVal));
}

inline f32 AdjustCenterValue(f32 value,bool isCeil)
{
    f32 ret = value / 2;
    return isCeil ? math::FCeil(ret): ret;
}

}
}

using namespace math;

NW_UT_RUNTIME_TYPEINFO_DEFINITION(TextBox, TextBox::Base);

TextBox::TextBox(u16 allocStrLen)
{
    this->Init(allocStrLen);
    this->InitMaterial();
}

TextBox::TextBox(u16 allocStrLen,const wchar_t* str,const font::Font* pFont)
{
    this->Init(allocStrLen);
    this->SetFont(pFont);
    this->SetString(str);
    this->InitMaterial();
}

TextBox::TextBox(u16 allocStrLen,const wchar_t* str,u16 strLen,const font::Font* pFont)
{
    this->Init(allocStrLen);
    this->SetFont(pFont);
    this->SetString(str, 0, strLen);
    this->InitMaterial();
}

TextBox::~TextBox()
{
    if (m_pMaterial && !this->m_pMaterial->IsUserAllocated())
    {
        Layout::DeleteObj(this->m_pMaterial);
        m_pMaterial = 0;
    }

    FreeStringBuffer();
}

void TextBox::Init(u16 allocStrLen)
{
    m_pTextBuf = 0;
    m_TextBufBytes = 0;
    m_TextLen = 0;
    m_pFont = 0;
    m_FontSize = Size(0, 0);
    SetTextPositionH(HORIZONTALPOSITION_CENTER);
    SetTextPositionV(VERTICALPOSITION_CENTER);
    m_LineSpace = 0;
    m_CharSpace = 0;
    m_pTagProcessor = 0;
    m_pDispStringBuf = 0;
    std::memset(&this->m_Bits, 0, sizeof(this->m_Bits));

    if (allocStrLen > 0)
    {
        AllocStringBuffer(allocStrLen);
    }
}

void TextBox::InitMaterial()
{
    m_pMaterial = Layout::NewObj<Material>();
    if (m_pMaterial)
    {
        this->m_pMaterial->ReserveMem(0, 0, 0);
    }
}

u8 TextBox::GetMaterialNum() const
{
    return m_pMaterial? 1 : 0;
}

Material* TextBox::GetMaterial(u32 idx) const
{
    return idx == 0 ? m_pMaterial : 0;
}

void TextBox::SetMaterial(Material* pMaterial)
{
    if (m_pMaterial && !this->m_pMaterial->IsUserAllocated())
    {
        Layout::DeleteObj(this->m_pMaterial);
    }
    m_pMaterial = pMaterial;
}

const ut::Color8 TextBox::GetVtxColor(u32 idx) const
{
    return this->GetTextColor(idx / 2);
}

void TextBox::SetVtxColor(u32 idx,ut::Color8 value)
{
    this->SetTextColor(idx / 2, value);
}

u8 TextBox::GetVtxColorElement(u32 idx) const
{
    return reinterpret_cast<const u8*>(&this->m_TextColors[idx / (2 * sizeof(ut::Color8))])[idx % sizeof(ut::Color8)];
}

void TextBox::SetVtxColorElement(u32 idx, u8 value)
{
    u8& elm =
        reinterpret_cast<u8*>(&this->m_TextColors[idx / (2 * sizeof(ut::Color8))])[idx % sizeof(ut::Color8)];
    elm = value;
}

const ut::Rect TextBox::GetTextDrawRect() const
{
    if (m_pFont == NULL)
    {
        return ut::Rect();
    }

    font::WideTextWriter writer;
    writer.SetCursor(0, 0);
    SetFontInfo(&writer);

    ut::Rect textRect;
    writer.CalcStringRect(&textRect, this->m_pTextBuf, this->m_TextLen);

    const Size textSize(textRect.GetWidth(), textRect.GetHeight());

    VEC2 ltPos = GetVtxPos();

    const VEC2 curPos  = AdjustTextPos(GetSize(), false);
    const VEC2 textPos = AdjustTextPos(textSize, true);

    ltPos.x += curPos.x - textPos.x;
    ltPos.y -= curPos.y - textPos.y;

    textRect.left   = ltPos.x;
    textRect.top    = ltPos.y;
    textRect.right  = ltPos.x + textSize.width;
    textRect.bottom = ltPos.y - textSize.height;

    return textRect;
}

void TextBox::DrawSelf(const DrawInfo& drawInfo)
{
    if (m_TextLen <= 0 || !m_pFont || !m_pMaterial)
    {
        return;
    }

    internal::FinalizeGraphics();

    GraphicsResource& graphicsResource = *drawInfo.GetGraphicsResource();
    graphicsResource.ResetGlState();

    font::TextWriterResource& writerResource = graphicsResource.GetTextWriterResource();
    font::WideTextWriter writer;
    
    writer.SetTextWriterResource(&writerResource);
    SetupTextWriter(&writer);

    writerResource.ActiveGlProgram();

    this->LoadMtx(drawInfo);

    ut::Color8 minCol = this->m_pMaterial->GetColor(INTERPOLATECOLOR_BLACK);
    ut::Color8 maxCol = this->m_pMaterial->GetColor(INTERPOLATECOLOR_WHITE);

    writer.SetColorMapping(minCol, maxCol);
    writer.SetAlpha(GetGlobalAlpha());

    writer.SetupGX();

    (void)writer.Print(this->m_pTextBuf, this->m_TextLen);

    writer.FinalizeGX();
}

void TextBox::AllocStringBuffer(u16 minLen)
{
    if (minLen == 0)
    {
        return;
    }

    u32 allocLen = minLen;
    ++allocLen;

    const u32 textBufBytes = allocLen * sizeof(wchar_t);
    if (textBufBytes >= 0x10000)
    {

    }

    if (textBufBytes <= m_TextBufBytes)
    {
        return;
    }

    this->FreeStringBuffer();

    const u32 drawBufSize = font::CharWriter::GetDispStringBufferSize(minLen);

    wchar_t* textBuf     = Layout::NewArray<wchar_t>(allocLen);
    void* pDispStringBuf = Layout::AllocMemory(drawBufSize);
    if (NULL == textBuf || NULL == pDispStringBuf)
    {
        if (NULL != textBuf)
        {
            Layout::DeletePrimArray(textBuf);
        }
        if (NULL != pDispStringBuf)
        {
            Layout::FreeMemory(pDispStringBuf);
        }
        return;
    }

    m_pTextBuf = textBuf;
    m_TextBufBytes = static_cast<u16>(textBufBytes);

    this->m_pDispStringBuf = font::CharWriter::InitDispStringBuffer(pDispStringBuf, minLen);
}

void TextBox::FreeStringBuffer()
{
    if (m_pTextBuf)
    {
        Layout::FreeMemory(this->m_pDispStringBuf);
        Layout::DeletePrimArray(this->m_pTextBuf);
        m_pDispStringBuf = 0;
        m_pTextBuf = 0;
        m_TextBufBytes = 0;
        m_TextLen = 0;
    }
}

u16 TextBox::SetString(const wchar_t* str,u16 dstIdx)
{
    return SetStringImpl(str, dstIdx, std::wcslen(str));
}

u16 TextBox::SetString(const wchar_t* str,u16 dstIdx,u16 strLen)
{
    return SetStringImpl(str, dstIdx, strLen);
}

u16 TextBox::SetStringImpl(const wchar_t* str,u16 dstIdx,u32 strLen)
{
    if (m_pFont == 0)
    {
        return 0;
    }

    if (m_pTextBuf == 0)
    {
        return 0;
    }

    const u16 bufLen = this->GetStringBufferLength();

    if (dstIdx >= bufLen)
    {
        return 0;
    }

    u32 cpLen = bufLen;
    cpLen -= dstIdx;

    cpLen = ut::Min(strLen, cpLen);

    std::memcpy(this->m_pTextBuf + dstIdx, str, cpLen * sizeof(wchar_t));

    m_TextLen = static_cast<u16>(dstIdx + cpLen);
    m_pTextBuf[this->m_TextLen] = 0;

    this->UpdatePTDirty(true);
    
    return static_cast<u16>(cpLen);
}

void TextBox::LoadMtx(const DrawInfo& drawInfo)
{
    MTX34 mtx;

    GetTextGlobalMtx(&mtx);

    drawInfo.GetGraphicsResource()->GetTextWriterResource().SetViewMtx(mtx);
}

void TextBox::SetFontInfo(font::WideTextWriter* pWriter) const
{
    pWriter->SetFont(this->m_pFont);
    if (m_pFont != NULL)
    {
        pWriter->SetFontSize(this->m_FontSize.width, m_FontSize.height);
        pWriter->SetLineSpace(this->m_LineSpace);
        pWriter->SetCharSpace(this->m_CharSpace);
        pWriter->SetWidthLimit(GetSize().width);
    }

    if (m_pTagProcessor)
    {
        pWriter->SetTagProcessor(this->m_pTagProcessor);
    }
}

void
TextBox::SetTextPos(font::WideTextWriter* pWriter) const
{
    u32 value = 0;

    switch (this->GetTextAlignment())
    {
    case TEXTALIGNMENT_SYNCHRONOUS:
    default:
        switch (this->GetTextPositionH())
        {
            case HORIZONTALPOSITION_LEFT:
            default:                        value = font::WideTextWriter::HORIZONTAL_ALIGN_LEFT;   break;
            case HORIZONTALPOSITION_CENTER: value = font::WideTextWriter::HORIZONTAL_ALIGN_CENTER; break;
            case HORIZONTALPOSITION_RIGHT:  value = font::WideTextWriter::HORIZONTAL_ALIGN_RIGHT;  break;
        }
        break;
    case TEXTALIGNMENT_LEFT:        value = font::WideTextWriter::HORIZONTAL_ALIGN_LEFT;    break;
    case TEXTALIGNMENT_CENTER:      value = font::WideTextWriter::HORIZONTAL_ALIGN_CENTER;  break;
    case TEXTALIGNMENT_RIGHT:       value = font::WideTextWriter::HORIZONTAL_ALIGN_RIGHT;   break;
    }

    switch (GetTextPositionH())
    {
    case HORIZONTALPOSITION_LEFT:
    default:
        value |= font::WideTextWriter::HORIZONTAL_ORIGIN_LEFT;
        break;
    case HORIZONTALPOSITION_CENTER:
        value |= font::WideTextWriter::HORIZONTAL_ORIGIN_CENTER;
        break;
    case HORIZONTALPOSITION_RIGHT:
        value |= font::WideTextWriter::HORIZONTAL_ORIGIN_RIGHT;
        break;
    }

    switch (GetTextPositionV())
    {
    case VERTICALPOSITION_TOP:
    default:
        value |= font::WideTextWriter::VERTICAL_ORIGIN_TOP;
        break;
    case VERTICALPOSITION_CENTER:
        value |= font::WideTextWriter::VERTICAL_ORIGIN_MIDDLE;
        break;
    case VERTICALPOSITION_BOTTOM:
        value |= font::WideTextWriter::VERTICAL_ORIGIN_BOTTOM;
        break;
    }

    pWriter->SetDrawFlag(value);
}

VEC2 TextBox::AdjustTextPos(const Size& size,bool isCeil) const
{
    VEC2 pos;

    switch (this->GetTextPositionH())
    {
    case HORIZONTALPOSITION_LEFT:
    default:
        pos.x = 0.f;
        break;
    case HORIZONTALPOSITION_CENTER:
        pos.x = internal::AdjustCenterValue(size.width, isCeil);
        break;
    case HORIZONTALPOSITION_RIGHT:
        pos.x = size.width;
        break;
    }

    switch (GetTextPositionV())
    {
    case VERTICALPOSITION_TOP:
    default:
        pos.y = 0.f;
        break;
    case VERTICALPOSITION_CENTER:
        pos.y = internal::AdjustCenterValue(size.height, isCeil);
        break;
    case VERTICALPOSITION_BOTTOM:
        pos.y = size.height;
        break;
    }

    return pos;
}

void TextBox::GetTextGlobalMtx(nw::math::MTX34* pMtx) const
{
    MTX34Copy(pMtx, &GetGlobalMtx());

    VEC2 pos = GetVtxPos();
    const VEC2 txtPos = AdjustTextPos(GetSize(), false);

    pos.x += txtPos.x;
    pos.y -= txtPos.y;

    pMtx->matrix[0][3] += pMtx->matrix[0][0] * pos.x + pMtx->matrix[0][1] * pos.y;
    pMtx->matrix[1][3] += pMtx->matrix[1][0] * pos.x + pMtx->matrix[1][1] * pos.y;
    pMtx->matrix[2][3] += pMtx->matrix[2][0] * pos.x + pMtx->matrix[2][1] * pos.y;

    pMtx->matrix[0][1] = - pMtx->matrix[0][1];
    pMtx->matrix[1][1] = - pMtx->matrix[1][1];
    pMtx->matrix[2][1] = - pMtx->matrix[2][1];
}

void TextBox::SetupDrawCharData(Drawer* pDrawer)
{
    font::WideTextWriter writer;

    writer.SetDispStringBuffer(this->m_pDispStringBuf);
    SetupTextWriter(&writer);

    if (m_Bits.isPTDirty)
    {
        writer.StartPrint();
        (void)writer.Print(this->m_pTextBuf, this->m_TextLen);
        writer.EndPrint();

        m_Bits.isPTDirty = false;
    }

    if (!this->m_pDispStringBuf->IsGeneratedCommand() && pDrawer)
    {
        pDrawer->BuildTextCommand(&writer);
    }
}

void TextBox::SetupTextWriter(font::WideTextWriter* pWriter)
{
    this->SetFontInfo(pWriter);
    this->SetTextPos(pWriter);

    ut::Color8 topCol = m_TextColors[TEXTCOLOR_TOP];
    ut::Color8 btmCol = m_TextColors[TEXTCOLOR_BOTTOM];
    pWriter->SetGradationMode(topCol != btmCol ? font::CharWriter::GRADMODE_V: font::CharWriter::GRADMODE_NONE);
    pWriter->SetTextColor(topCol, btmCol);
}

}
}