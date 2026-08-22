#pragma once

#include <nw/ut/ut_ResTypes.h>

#if ( NW_PLATFORM_ENDIAN == NW_ENDIAN_LITTLE )

 // LE
 #define NW_UT_MAKE_SIGWORD(a,b,c,d)        \
    static_cast<nw::ut::SigWord >(          \
          (static_cast<u8>(a) <<  0)        \
        | (static_cast<u8>(b) <<  8)        \
        | (static_cast<u8>(c) << 16)        \
        | (static_cast<u8>(d) << 24)        \
    )

#else

 // BE
 #define NW_UT_MAKE_SIGWORD(a,b,c,d)        \
    static_cast<nw::ut::SigWord >(          \
          (static_cast<u8>(a) << 24)        \
        | (static_cast<u8>(b) << 16)        \
        | (static_cast<u8>(c) <<  8)        \
        | (static_cast<u8>(d) <<  0)        \
    )
#endif

#define NW_UT_MAKE_VERSION_2(major, minor)                              \
    static_cast<u32>(                                                   \
        (static_cast<u8>(major) << 24) | (static_cast<u8>(minor) << 16) \
    )

#define NW_UT_MAKE_VERSION(major, minor, micro, binaryBugFix)            \
    static_cast<u32>(                                                    \
        (static_cast<u8>(major) << 24) | (static_cast<u8>(minor) << 16)  \
      | (static_cast<u8>(micro) <<  8) | (static_cast<u8>(binaryBugFix)) \
    )

#define NW_UT_VERSION_MAJOR(version)        (((version) >> 24) & 0xff)
#define NW_UT_VERSION_MINOR(version)        (((version) >> 16) & 0xff)
#define NW_UT_VERSION_MICRO(version)        (((version) >>  8) & 0xff)
#define NW_UT_VERSION_BINARYBUGFIX(version) (((version) >>  0) & 0xff)

namespace nw {
namespace ut {
    typedef SigWord SigWord;
namespace{
    const u16 BYTE_ORDER_MARK = 0xFEFF;
}

struct BinaryFileHeader{
    SigWord signature;
    u16 byteOrder;
    ResU16 headerSize;
    ResU32 version;
    ResU32 fileSize;
    ResU16 dataBlocks;
    u16 reserved;
};

struct BinaryBlockHeader{
    SigWord kind;
    u32 size;
};

struct BinaryRevisionHeader{
    SigWord signature;
    ResU32  revision;
};

bool IsValidBinaryFile(const BinaryFileHeader* pHeader,u32 signature,u32 version,u16 minBlocks = 1);
bool IsReverseEndianBinaryFile(const BinaryFileHeader* pFileHeader);
BinaryBlockHeader* GetNextBinaryBlockHeader(BinaryFileHeader* pFileHeader,BinaryBlockHeader* pBlockHeader);

inline const BinaryBlockHeader* GetNextBinaryBlockHeader(const BinaryFileHeader* pFileHeader,const BinaryBlockHeader* pBlockHeader){
    return static_cast<const BinaryBlockHeader*>(GetNextBinaryBlockHeader(const_cast<BinaryFileHeader*>(pFileHeader),const_cast<BinaryBlockHeader*>(pBlockHeader)));
}

namespace internal {

inline bool CheckRevision(u32 resRevision, u32 libRevision){
    if (NW_UT_VERSION_MAJOR(libRevision) != NW_UT_VERSION_MAJOR(resRevision)){
        return false;
    }

    if (NW_UT_VERSION_MINOR(libRevision) < NW_UT_VERSION_MINOR(resRevision)){
        return false;
    }

    if (NW_UT_VERSION_BINARYBUGFIX(libRevision) > NW_UT_VERSION_BINARYBUGFIX(resRevision)){
        return false;
    }
    
    return true;
}

}

}
}