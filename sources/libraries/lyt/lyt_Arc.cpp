// Filename: lyt_Arc.cpp
//
// Project: NintendoWare4Ctr

#include <nw/lyt/lyt_Arc.h>
#include <ctype.h>

namespace nw{
namespace lyt{

typedef struct FSTEntry FSTEntry;

struct FSTArcEntry
{
    u32    isDirAndStringOff;
    u32    parentOrPosition;
    u32    nextEntryOrLength;
};

static bool isSame(const wchar_t* path, const wchar_t* string)
{
    while (*string != '\0')
    {
        if (tolower(*path++) != tolower(*string++))
        {
            return false;
        }
    }

    if ((*path == '/') || (*path == '\0'))
    {
        return true;
    }

    return false;
}


#define entryIsDir(fstStart, i)     \
    (((fstStart[i].isDirAndStringOff & 0xff000000) == 0)? false:true)
#define stringOff(fstStart, i)      \
        (fstStart[i].isDirAndStringOff & 0x00ffffff )
#define parentDir(fstStart, i)       \
        (fstStart[i].parentOrPosition )
#define nextDir(fstStart, i)        \
        (fstStart[i].nextEntryOrLength )
#define filePosition(fstStart, i)       \
        (fstStart[i].parentOrPosition )
#define fileLength(fstStart, i)         \
        (fstStart[i].nextEntryOrLength )

inline wchar_t* GetStringPtr(wchar_t* str,size_t offset)
        {
    return reinterpret_cast<wchar_t*>(reinterpret_cast<u8*>(str) + offset);
}

bool ARCInitHandle(void* arcStart, ARCHandle* handle)
{
    FSTArcEntry*           FSTEntries;
    ARCHeader*          arcHeader;

    arcHeader = (ARCHeader*)arcStart;
    
    handle->archiveStartAddr = arcStart;
    handle->FSTStart = FSTEntries = reinterpret_cast<FSTArcEntry*>((u32)arcStart + arcHeader->fstStart);
    handle->fileStart = (void*)((u32)arcStart + arcHeader->fileStart);

    handle->entryNum = nextDir(FSTEntries, 0);
    handle->FSTStringStart = reinterpret_cast<wchar_t*>(&(FSTEntries[handle->entryNum]));
    handle->FSTLength = (u32)arcHeader->fstSize;
    handle->currDir = 0;

    return true;
}

bool ARCOpenDir(ARCHandle* handle, s32 entrynum, ARCDir* dir)
{
    FSTArcEntry* FSTEntries = (FSTArcEntry*)handle->FSTStart;

    if (handle == NULL)
    {
        return false;
    }

    if (entrynum < 0 || static_cast<s32>(handle->entryNum) <= entrynum)
    {
        return false;
    }

    if (!entryIsDir(FSTEntries, entrynum))
    {
        return false;
    }

    dir->handle = handle;
    dir->entryNum = (u32)entrynum;
    dir->location = (u32)entrynum + 1;
    dir->next = nextDir(FSTEntries, entrynum);

    return true;
}

bool ARCFastOpen(ARCHandle* handle, s32 entrynum, ARCFileInfo* af)
{
    FSTArcEntry*           FSTEntries;

    FSTEntries = (FSTArcEntry*)handle->FSTStart;


    if ((entrynum < 0) || (entrynum >= static_cast<s32>(handle->entryNum)) || entryIsDir(FSTEntries, entrynum))
    {
        return false;
    }

    af->handle = handle;
    af->startOffset = filePosition(FSTEntries, entrynum);
    af->length = fileLength(FSTEntries, entrynum);

    return true;
}

bool ARCReadDir(ARCDir* dir, ARCDirEntry* dirent)
{
    u32         loc;
    FSTArcEntry*   FSTEntries;
    ARCHandle*  handle;

    handle = dir->handle;
    
    FSTEntries = (FSTArcEntry*)handle->FSTStart;

    loc = dir->location;
retry:
    if ((loc <= dir->entryNum) || (dir->next <= loc))
        return false;

    dirent->handle = handle;
    dirent->entryNum = loc;
    dirent->isDir = entryIsDir(FSTEntries, loc);
    dirent->name = GetStringPtr(handle->FSTStringStart, stringOff(FSTEntries, loc));

    if (dirent->name[0] == '.' && dirent->name[1] == '\0')
    {
        loc++;
        goto retry;
    }

    dir->location = entryIsDir(FSTEntries, loc)? nextDir(FSTEntries, loc) : (loc+1);

    return true;
}

bool ARCChangeDir(ARCHandle* handle, s32 entrynum)
{
    FSTArcEntry* FSTEntries = (FSTArcEntry*)handle->FSTStart;

    if (handle == NULL)
    {
        return false;
    }

    if (entrynum < 0 || static_cast<s32>(handle->entryNum) <= entrynum)
    {
        return false;
    }

    if (!entryIsDir(FSTEntries, entrynum))
    {
        return false;
    }

    handle->currDir = (u32)entrynum;

    return true;
}

bool ARCCloseDir(ARCDir* dir)
{
    return true;
}

s32 ARCConvertPathToEntrynum(ARCHandle* handle, const wchar_t* pathPtr)
{
    const wchar_t* ptr;
    wchar_t*     stringPtr;
    bool         isDir;
    s32          length;
    u32          dirLookAt;
    u32          i;
    const wchar_t*  origPathPtr = pathPtr;
    FSTArcEntry*    FSTEntries;
    
    dirLookAt = handle->currDir;
    FSTEntries = (FSTArcEntry*)handle->FSTStart;

    for (;;)
    {

        if (*pathPtr == '\0')
        {
            return (s32)dirLookAt;
        } 
        else if (*pathPtr == '/')
        {
            dirLookAt = 0;
            pathPtr++;
            continue;
        }
        else if (*pathPtr == '.')
        {
            if (*(pathPtr + 1) == '.')
            {
                if (*(pathPtr + 2) == '/')
                {
                    dirLookAt = parentDir(FSTEntries, dirLookAt);
                    pathPtr += 3;
                    continue;
                }
                else if (*(pathPtr + 2) == '\0')
                {
                    return (s32)parentDir(FSTEntries, dirLookAt);
                }
            }
            else if (*(pathPtr + 1) == '/')
            {
                pathPtr += 2;
                continue;
            }
            else if (*(pathPtr + 1) == '\0')
            {
                return (s32)dirLookAt;
            }
        }

        for (ptr = pathPtr; (*ptr != '\0') && (*ptr != '/'); ptr++);

        isDir = (*ptr == '\0')? false : true;
        length = (s32)(ptr - pathPtr);

        ptr = pathPtr;

        for (i = dirLookAt + 1; i < nextDir(FSTEntries, dirLookAt); i = entryIsDir(FSTEntries, i)? nextDir(FSTEntries, i): (i+1) )
        {
dot:
            if ((entryIsDir(FSTEntries, i) == false) &&
                 (isDir == true))
                 {
                continue;
            }

            stringPtr = GetStringPtr(handle->FSTStringStart, stringOff(FSTEntries, i));

            if (*stringPtr == '.' && *(stringPtr + 1) == '\0')
            {
                i++;
                goto dot;
            }

            if (isSame(ptr, stringPtr) == true)
            {
                goto next_hier;
            }

        }

        return -1;

next_hier:
        if (!isDir)
        {
            return (s32)i;
        }

        dirLookAt = i;
        pathPtr += length + 1;

    }
}

u32 ARCGetStartOffset(ARCFileInfo* af)
{
    return af->startOffset;
}

u32 ARCGetLength(ARCFileInfo* af)
{
    return af->length;
}

}
}