// Filename: font_ResFontBase.cpp
//
// Project: NintendoWare4Ctr

#include <nw/font/font_ResFontBase.h>
#include <nw/ut/ut_Inlines.h>

#include <nn/gx/CTR/gx_CommandAccess.h>
#include <GLES2/gl2.h>

namespace nw {
namespace font {
namespace {

enum
{
    CMD_TEX_NEAREST = 0,
    CMD_TEX_LINEAR  = 1
};

#define NW_FONT_COMMAND_TEX_WRAP_FILTER( magFilter, minFilter )             \
      ( (magFilter) << 1 | (minFilter) << 2 | ( 0/*isETC1*/ ? 2 : 0 ) << 4  \
    | 0/*wrapT*/ << 8 | 0/*wrapS*/ << 12                                    \
    | 0/*minFilter2*/ << 24 )

}

ResFontBase::ResFontBase():   
    m_pResource(NULL),
    m_pFontInfo(NULL),
    m_pTexObjs(NULL),
    m_LastCharCode(0),
    m_LastGlyphIndex(GLYPH_INDEX_NOT_FOUND)
    {
    this->EnableLinearFilter(true, true);
}

ResFontBase::~ResFontBase() {}

void ResFontBase::SetResourceBuffer(void* pUserBuffer,FontInformation* pFontInfo)
{
    this->m_pResource = pUserBuffer;
    this->m_pFontInfo = pFontInfo;
}

void* ResFontBase::RemoveResourceBuffer()
{
    if (NULL != m_pTexObjs)
    {
        DeleteTextureNames();
        this->m_pTexObjs = NULL;
    }

    void* pUserData = m_pResource;

    m_pResource = NULL;
    m_pFontInfo = NULL;

    return pUserData;
}

int ResFontBase::GetWidth() const
{
    return m_pFontInfo->width;
}

int ResFontBase::GetHeight() const
{
    return m_pFontInfo->height;
}

int ResFontBase::GetAscent() const
{
    return m_pFontInfo->ascent;
}

int ResFontBase::GetDescent() const
{
    return m_pFontInfo->height - m_pFontInfo->ascent;
}

int ResFontBase::GetBaselinePos() const
{
    return m_pFontInfo->pGlyph->baselinePos;
}

int ResFontBase::GetCellHeight() const
{
    return m_pFontInfo->pGlyph->cellHeight;
}

int ResFontBase::GetCellWidth() const
{
    return m_pFontInfo->pGlyph->cellWidth;
}

int ResFontBase::GetMaxCharWidth() const
{
    return m_pFontInfo->pGlyph->maxCharWidth;
}

Font::Type ResFontBase::GetType() const
{
    return TYPE_RESOURCE;
}

TexFmt ResFontBase::GetTextureFormat() const
{
    return static_cast<TexFmt>(m_pFontInfo->pGlyph->sheetFormat);
}

int ResFontBase::GetLineFeed() const
{
    return m_pFontInfo->linefeed;
}

const CharWidths ResFontBase::GetDefaultCharWidths() const
{
    return m_pFontInfo->defaultWidth;
}

void ResFontBase::SetDefaultCharWidths(const CharWidths& widths)
{
    m_pFontInfo->defaultWidth = widths;
}

bool ResFontBase::SetAlternateChar( CharCode c )
{
    GlyphIndex index = this->FindGlyphIndex(c);

    if (index != GLYPH_INDEX_NOT_FOUND)
    {
        m_pFontInfo->alterCharIndex = index;
        return true;
    }

    return false;
}

void ResFontBase::SetLineFeed(int linefeed)
{
    m_pFontInfo->linefeed = static_cast<s8>(linefeed);
}

int ResFontBase::GetCharWidth(CharCode c) const
{
    return GetCharWidths(c).charWidth;
}

const CharWidths ResFontBase::GetCharWidths(CharCode c) const
{
    GlyphIndex index = this->GetGlyphIndex(c);
    return this->GetCharWidthsFromIndex(index);
}

void ResFontBase::GetGlyph( Glyph* glyph, CharCode c ) const
{
    GlyphIndex index = GetGlyphIndex(c);
    GetGlyphFromIndex(glyph, index);
}

bool ResFontBase::HasGlyph( CharCode c ) const
{
    return (GLYPH_INDEX_NOT_FOUND != FindGlyphIndex(c));
}

CharacterCode ResFontBase::GetCharacterCode() const
{
    return static_cast<CharacterCode>(m_pFontInfo->characterCode);
}

ResFontBase::GlyphIndex ResFontBase::GetGlyphIndex(CharCode c) const
{
    GlyphIndex index = FindGlyphIndex(c);
    return (index != GLYPH_INDEX_NOT_FOUND) ? index: m_pFontInfo->alterCharIndex;
}

ResFontBase::GlyphIndex ResFontBase::FindGlyphIndex(CharCode c) const
{
    if (c == m_LastCharCode)
    {
        return m_LastGlyphIndex;
    }

    m_LastCharCode = c;

    const FontCodeMap* pMap = m_pFontInfo->pMap;
    while (pMap != NULL)
    {
        if (pMap->ccodeBegin <= c && c <= pMap->ccodeEnd)
        {
            m_LastGlyphIndex = this->FindGlyphIndex(pMap, c);
            return m_LastGlyphIndex;
        }

        pMap = pMap->pNext;
    }

    m_LastGlyphIndex = GLYPH_INDEX_NOT_FOUND;
    return this->m_LastGlyphIndex;
}

ResFontBase::GlyphIndex ResFontBase::FindGlyphIndex(const FontCodeMap* pMap,CharCode c) const
{
    u16 index = GLYPH_INDEX_NOT_FOUND;

    switch (pMap->mappingMethod)
    {
    case FONT_MAPMETHOD_DIRECT:{
            u16 offset = pMap->GetMapInfo()[0];
            index = static_cast<u16>(c - pMap->ccodeBegin + offset);
        }
        break;
    case FONT_MAPMETHOD_TABLE:{
            const int table_index = c - pMap->ccodeBegin;

            index = pMap->GetMapInfo()[table_index];
        }
        break;
    case FONT_MAPMETHOD_SCAN:{
            const CMapInfoScan* const scanInfo= reinterpret_cast<const CMapInfoScan*>(pMap->GetMapInfo());
            const CMapScanEntry* first = &(scanInfo->GetEntries()[0]);
            const CMapScanEntry* last = &(scanInfo->GetEntries()[scanInfo->num - 1]);

            while ( first <= last )
            {
                const CMapScanEntry* mid = first + (last - first) / 2;

                if (mid->ccode < c)
                {
                    first = mid + 1;
                }
                else if (c < mid->ccode)
                {
                    last = mid - 1;
                }
                else{
                    index = mid->index;
                    break;
                }
            }
        }
        break;

    default:
    }

    return index;
}

const CharWidths& ResFontBase::GetCharWidthsFromIndex(GlyphIndex index) const
{
    const FontWidth* pWidth;

    pWidth = m_pFontInfo->pWidth;

    while (pWidth != NULL)
    {
        if (pWidth->indexBegin <= index && index <= pWidth->indexEnd)
        {
            return GetCharWidthsFromIndex(pWidth, index);
        }

        pWidth = pWidth->pNext;
    }

    return m_pFontInfo->defaultWidth;
}

const CharWidths& ResFontBase::GetCharWidthsFromIndex(const FontWidth* pWidth,GlyphIndex index) const
{
    return pWidth->GetWidthTable()[index - pWidth->indexBegin];
}

void ResFontBase::GetGlyphFromIndex(Glyph* glyph,GlyphIndex index) const
{
    const FontTextureGlyph& tg = *m_pFontInfo->pGlyph;

    const u32 cellsInASheet = internal::GetCellsInASheet(tg);
    const u32 sheetNo = index / cellsInASheet;
    const u32 offsetBytes = sheetNo * tg.sheetSize;
    const void* pSheet = tg.sheetImage + offsetBytes;

    glyph->pTexture  = pSheet;
    glyph->widths = GetCharWidthsFromIndex(index);
    glyph->pTextureObject = NULL != m_pTexObjs ? this->GetTextureObject(sheetNo) : 0;
    this->SetGlyphMember(glyph, index, tg);
}

void ResFontBase::SetGlyphMember(Glyph* glyph,GlyphIndex index,const FontTextureGlyph& tg)
{
    const u32 cellNo        = index       % internal::GetCellsInASheet(tg);
    const u32 cellUnitX     = cellNo      % tg.sheetRow;
    const u32 cellUnitY     = cellNo      / tg.sheetRow;
    const u32 cellPixelX    = cellUnitX   * (tg.cellWidth  + 1);
    const u32 cellPixelY    = cellUnitY   * (tg.cellHeight + 1);

    glyph->height       = tg.cellHeight;
    glyph->texFormat    = static_cast<TexFmt>(tg.sheetFormat);
    glyph->texWidth     = tg.sheetWidth;
    glyph->texHeight    = tg.sheetHeight;
    glyph->cellX        = static_cast<u16>(cellPixelX + 1);
    glyph->cellY        = static_cast<u16>(cellPixelY + 1);
}

void ResFontBase::EnableLinearFilter(bool atSmall,bool atLarge)
{
    const int magFilter = atLarge ? CMD_TEX_LINEAR: CMD_TEX_NEAREST;
    const int minFilter = atSmall ? CMD_TEX_LINEAR: CMD_TEX_NEAREST;
    m_WrapFilter = 
        NW_FONT_COMMAND_TEX_WRAP_FILTER(
            magFilter,
            minFilter);

    if (!this->IsManaging(NULL))
    {
        this->DeleteTextureNames();
        this->GenTextureNames();
    }
}

bool ResFontBase::IsLinearFilterEnableAtSmall() const
{
    return 0 != (m_WrapFilter & (1 << 2));
}

bool ResFontBase::IsLinearFilterEnableAtLarge() const 
{
    return 0 != (m_WrapFilter & (1 << 1));
}

u32 ResFontBase::GetTextureWrapFilterValue() const 
{
    return m_WrapFilter;
}

int ResFontBase::GetActiveSheetNum() const
{
    return m_pFontInfo->pGlyph->sheetNum;
}

void ResFontBase::GenTextureNames()
{
    internal::TextureObject* texObjs = GetTextureObjectsBufferPtr();


    const FontTextureGlyph& tg = *m_pFontInfo->pGlyph;
    const int sheetNum = GetActiveSheetNum();
    u32 offsetBytes = 0;
    for (int i = 0; i < sheetNum; ++i)
    {
        const GLuint texName = 0;
        const void *const pImage = tg.sheetImage + offsetBytes;
        const TexFmt format = static_cast<TexFmt>(tg.sheetFormat);
        texObjs[i].Set(texName, this, pImage, format, tg.sheetWidth, tg.sheetHeight);

        offsetBytes += tg.sheetSize;
    }
    nngxUpdateBuffer(tg.sheetImage, tg.sheetSize * sheetNum);  
}

void ResFontBase::DeleteTextureNames()
{
    internal::TextureObject* texObjs = GetTextureObjectsBufferPtr();
    NN_POINTER_ASSERT(texObjs);

    const int sheetNum = GetActiveSheetNum();
    for (int i = 0; i < sheetNum; ++i)
    {
        const GLuint texName = texObjs[i].GetName();
        if (texName != 0)
        {
            glDeleteTextures(1, &texName);
            texObjs[i].SetName(0);
        }

#if defined(NW_DEBUG)
        texObjs[i].Reset();
#endif
    }
}

}
}