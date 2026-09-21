// Filename: lyt_ArcResourceAccessor.cpp
//
// Project: NintendoWare4Ctr

#include <nw/lyt/lyt_ArcResourceAccessor.h>
#include <nw/lyt/lyt_Resources.h>
#include <cctype>
#include <cwchar>

namespace nw{
namespace lyt{
namespace{

inline int wcsicmp(const wchar_t *string1,const wchar_t *string2)
{
    return std::wcscasecmp(string1, string2);
}

size_t strncpy(wchar_t* dest,std::size_t destCount,const char* src)
{
    --destCount;
    size_t length = 0;
    while (length < destCount && *src != '\0')
    {
        *dest = *src;
        ++dest;
        ++src;
        ++length;
    }

    *dest = L'\0';

    return length;
}

s32 FindNameResource(ARCHandle* pArcHandle,const wchar_t* resName)
{
    s32 entryNum = -1;

    ARCDir dir;
    bool bSuccess = ARCOpenDir(pArcHandle, L".", &dir);

    ARCDirEntry     dirEntry;

    while (ARCReadDir(&dir, &dirEntry))
    {
        if (dirEntry.isDir)
        {
            bSuccess = ARCChangeDir(pArcHandle, dirEntry.name);

            entryNum = FindNameResource(pArcHandle, resName);
            bSuccess = ARCChangeDir(pArcHandle, L"..");

            if (entryNum != -1)
            {
                break;
            }
        }
        else
        {
            if (wcsicmp(resName, dirEntry.name) == 0)
        {
                entryNum = s32(dirEntry.entryNum);
                break;
            }
        }
    }

    bSuccess = ARCCloseDir(&dir);
    return entryNum;
}

void* GetResourceSub(ARCHandle* pArcHandle,const wchar_t* resRootDir,nw::lyt::ResType resType,const wchar_t* name,u32* pSize)
{
    s32 entryNum = -1;

    if (-1 != ARCConvertPathToEntrynum(pArcHandle, resRootDir))
    {
        if (ARCChangeDir(pArcHandle, resRootDir))
    {
            if (resType == 0)
            {
                entryNum = FindNameResource(pArcHandle, name);
            }
            else
            {
                wchar_t resTypeStr[5];
                resTypeStr[0] = u8(resType >> 24);
                resTypeStr[1] = u8(resType >> 16);
                resTypeStr[2] = u8(resType >>  8);
                resTypeStr[3] = u8(resType >>  0);
                resTypeStr[4] = 0;

                if (-1 != ARCConvertPathToEntrynum(pArcHandle, resTypeStr))
                {
                    if (ARCChangeDir(pArcHandle, resTypeStr))
                {
                        entryNum = ARCConvertPathToEntrynum(pArcHandle, name);

                        bool bSuccess = ARCChangeDir(pArcHandle, L"..");
                    }
                }
            }

            bool bSuccess = ARCChangeDir(pArcHandle, L"..");
        }
    }

    if (entryNum != -1)
    {
        ARCFileInfo arcFileInfo;
        bool bSuccess = ARCFastOpen(pArcHandle, entryNum, &arcFileInfo);

        void* resPtr = ARCGetStartAddrInMem(&arcFileInfo);
        if (pSize)
        {
            *pSize = ARCGetLength(&arcFileInfo);
        }
        ARCClose(&arcFileInfo);

        return resPtr;
    }

    return NULL;
}

} // namespace ""

/* ArcResourceAccessor */

ArcResourceAccessor::ArcResourceAccessor():   
    m_ArcBuf(0)
{
}

bool ArcResourceAccessor::Attach(void* archiveStart,const char* resourceRootDirectory)
{
    bool bSuccess = ARCInitHandle(archiveStart, &this->m_ArcHandle);
    if (!bSuccess)
    {
        return false;
    }

    m_ArcBuf = archiveStart;

    const int dstBufCount = sizeof(m_ResRootDir) / sizeof(m_ResRootDir[0]);
    strncpy(this->m_ResRootDir, dstBufCount, resourceRootDirectory);

    return true;
}

void* ArcResourceAccessor::Detach()
{
    void* ret = m_ArcBuf;
    m_ArcBuf = 0;
    return ret;
}

void* ArcResourceAccessor::GetResource(ResType resType,const char* name,u32* pSize)
{
    const int dstBufCount = sizeof(this->m_ResNameWork) / sizeof(this->m_ResNameWork[0]);
    strncpy(this->m_ResNameWork, dstBufCount, name);
    return GetResourceSub(&this->m_ArcHandle, this->m_ResRootDir, resType, this->m_ResNameWork, pSize);
}

font::Font* ArcResourceAccessor::GetFont(const char *name)
{
    font::Font* pFont = this->m_FontList.FindFontByName(name);

    if (pFont == NULL)
    {
        pFont = this->LoadFont(name);

        if (pFont != NULL)
        {
            (void)m_FontList.RegistFont(name, pFont, true);
        }
    }

    return pFont;
}

const TextureInfo ArcResourceAccessor::GetTexture(const char *name)
{
    TextureInfo texInfo = this->m_TextureList.FindTextureByName(name);
    if (texInfo.IsValid())
    {
        return texInfo;
    }
    else
    {
        texInfo = this->LoadTexture(name);

        if (texInfo.IsValid())
        {
            (void) m_TextureList.RegistTexture(name, texInfo);
        }

        return texInfo;
    }
}

} // namespace lyt
} // namespace nw