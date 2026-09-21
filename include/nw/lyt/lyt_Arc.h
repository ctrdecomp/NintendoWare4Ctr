#pragma once

#include "nn/types.h"

namespace nw {
namespace lyt {

typedef struct{
    u32 signature;
    u16 byteOrder;
    u16 headerSize;
    u32 version;
    u32 fileSize;
    s32 fstStart;
    s32 fstSize;
    s32 fileStart;
} ARCHeader;

const u32 DARCH_SIGNATURE       = 0x63726164;
const u16 DARCH_BYTE_ORDER_MARK = 0xFEFF;
const u32 DARCH_VERSION         = 0x01000000;

typedef struct
{
    void*    archiveStartAddr;
    void*    FSTStart;
    void*    fileStart;
    u32      entryNum;
    wchar_t* FSTStringStart;
    u32      FSTLength;
    u32      currDir;
} ARCHandle;

typedef struct
{
    ARCHandle* handle;
    u32        startOffset;
    u32        length;
} ARCFileInfo;

typedef struct
{
    ARCHandle* handle;
    u32        entryNum;
    u32        location;
    u32        next;
} ARCDir;

typedef struct{
    ARCHandle* handle;
    u32        entryNum;
    bool       isDir;
    wchar_t*   name;
} ARCDirEntry;

bool ARCInitHandle(void* arcStart, ARCHandle* handle);
bool ARCOpen(ARCHandle* handle, const wchar_t* fileName, ARCFileInfo* af);
bool ARCFastOpen(ARCHandle* handle, s32 entrynum, ARCFileInfo* af);

inline bool ARCOpen(ARCHandle* handle, ARCDirEntry* dirent, ARCFileInfo* af)
{
    return ARCFastOpen(handle, static_cast<s32>(dirent->entryNum), af);
}

s32 ARCConvertPathToEntrynum(ARCHandle* handle, const wchar_t* pathPtr);
bool ARCEntrynumIsDir(const ARCHandle* handle, s32 entrynum);
void* ARCGetStartAddrInMem(ARCFileInfo* af);
u32 ARCGetStartOffset(ARCFileInfo* af);
u32 ARCGetLength(ARCFileInfo* af);
bool ARCClose(ARCFileInfo* af);
bool ARCChangeDir(ARCHandle* handle, const wchar_t* dirName);
bool ARCChangeDir(ARCHandle* handle, s32 entrynum);

inline bool ARCChangeDir(ARCHandle* handle, ARCDir* dir)
{
    return ARCChangeDir(handle, static_cast<s32>(dir->entryNum));
}

inline bool ARCChangeDir(ARCHandle* handle, ARCDirEntry* dirent)
{
    return ARCChangeDir(handle, static_cast<s32>(dirent->entryNum));
}

bool ARCGetCurrentDir(ARCHandle* handle, wchar_t* path, u32 maxlen);
bool ARCOpenDir(ARCHandle* handle, const wchar_t* dirName, ARCDir* dir);
bool ARCOpenDir(ARCHandle* handle, s32 entrynum, ARCDir* dir);

inline bool ARCOpenDir(ARCHandle* handle, ARCDirEntry* dirent, ARCDir* dir)
{
    return ARCOpenDir(handle, static_cast<s32>(dirent->entryNum), dir);
}

bool ARCReadDir(ARCDir* dir, ARCDirEntry* dirent);
bool ARCCloseDir(ARCDir* dir);

inline u32 ARCTellDir(ARCDir* dir) { return dir->location; }
inline void ARCSeekDir(ARCDir* dir, u32 loc) { dir->location = loc; }
inline void ARCRewindDir(ARCDir* dir) { dir->location = dir->entryNum + 1; }
inline const wchar_t* ARCGetDirEntryName(ARCDirEntry* dirent) { return dirent->name; }
inline s32 ARCGetDirEntryEntrynum(ARCDirEntry* dirent) { return static_cast<s32>(dirent->entryNum); }
inline bool ARCDirEntryIsDir(ARCDirEntry* dirent) { return dirent->isDir; }

}
}