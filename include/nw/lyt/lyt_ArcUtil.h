#pragma once

#include <nw/lyt/lyt_Arc.h>
#include <nw/lyt/lyt_ResourceAccessor.h>
#include <nw/lyt/lyt_Resources.h>
#include <nw/lyt/lyt_Types.h>

namespace nw {
namespace lyt {

class ArcUtil{
public:
    static bool OpenTextureDir(ARCHandle* pArcHandle, const wchar_t* pRootName, ARCDir* pDir){
        return OpenResourceDir(pArcHandle, pRootName, res::RESOURCETYPE_TEXTURE, pDir);
    }

    static bool ReadTextureDir(ARCDir* pDir, ARCDirEntry* pDirEnt){
        return ReadResourceDir(pDir, L".bclim", 6, pDirEnt);
    }

    static bool OpenFontDir(ARCHandle* pArcHandle, const wchar_t* pRootName, ARCDir* pDir){
        return OpenResourceDir(pArcHandle, pRootName, res::RESOURCETYPE_FONT, pDir);
    }

    static bool ReadFontDir(ARCDir* pDir, ARCDirEntry* pDirEnt){
        return ReadResourceDir(pDir, L".bcfnt", 6, pDirEnt);
    }

    static bool OpenResourceDir(ARCHandle* pArcHandle, const wchar_t* pRootName, ResType resType, ARCDir* pDir);
    static bool ReadResourceDir(ARCDir* pDir, const wchar_t* pExtStr, u32 extLen, ARCDirEntry* pDirEnt);
};

}
}