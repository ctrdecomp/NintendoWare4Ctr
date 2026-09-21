// Filename: ut_BinaryFileFormat.cpp
//
// Project: NintendoWare4Ctr

#include <nw/types.h>
#include <nw/assert.h>
#include <nw/ut/ut_BinaryFileFormat.h>

namespace nw{
namespace ut{

bool IsValidBinaryFile(const BinaryFileHeader* pHeader,u32 signature,u32 version, ushort minBlocks)
{
    NW_POINTER_ASSERT(pHeader);
    
    if (pHeader->signature != signature)
    {
        NW_WARNING(false, "Signature check failed ('%c%c%c%c' must be '%c%c%c%c').",
            static_cast<char>(BitExtract(pHeader->signature, 24, 8)),
            static_cast<char>(BitExtract(pHeader->signature, 16, 8)),
            static_cast<char>(BitExtract(pHeader->signature,  8, 8)),
            static_cast<char>(BitExtract(pHeader->signature,  0, 8)),
            static_cast<char>(BitExtract(signature, 24, 8)),
            static_cast<char>(BitExtract(signature, 16, 8)),
            static_cast<char>(BitExtract(signature,  8, 8)),
            static_cast<char>(BitExtract(signature,  0, 8))
        );
        return false;
    }
    
    if (pHeader->byteOrder != BYTE_ORDER_MARK)
    {
        NW_WARNING(false, "Unsupported byte order.");
        return false;
    }
    
    if (NW_UT_VERSION_MAJOR(version) != NW_UT_VERSION_MAJOR(pHeader->version) || NW_UT_VERSION_MINOR(version) < NW_UT_VERSION_MINOR(pHeader->version)  || NW_UT_VERSION_BINARYBUGFIX(version) > NW_UT_VERSION_BINARYBUGFIX(pHeader->version) )
    {
        NW_WARNING(false, "Version check faild (bin:'%d.%d.%d.%d', lib:'%d.%d.%d.%d').",
            NW_UT_VERSION_MAJOR(pHeader->version),
            NW_UT_VERSION_MINOR(pHeader->version),
            NW_UT_VERSION_MICRO(pHeader->version),
            NW_UT_VERSION_BINARYBUGFIX(pHeader->version),
            NW_UT_VERSION_MAJOR(version),
            NW_UT_VERSION_MINOR(version),
            NW_UT_VERSION_MICRO(version),
            NW_UT_VERSION_BINARYBUGFIX(version)
        );
        return false;
    }
    
    if (pHeader->fileSize < sizeof(BinaryFileHeader) + sizeof(BinaryBlockHeader) * minBlocks)
    {
        NW_WARNING(false, "Too small file size(=%d).", pHeader->fileSize);
        return false;
    }
    
    if (pHeader->dataBlocks < minBlocks)
    {
        NW_WARNING(false, "Too small number of data blocks(=%d).", pHeader->dataBlocks);
        return false;
    }

    return true;
}

bool IsReverseEndianBinaryFile(const BinaryFileHeader* pFileHeader)
{
    NW_POINTER_ASSERT(pFileHeader);
    return (pFileHeader->byteOrder != BYTE_ORDER_MARK);
}

BinaryBlockHeader*  GetNextBinaryBlockHeader(BinaryFileHeader* pFileHeader,BinaryBlockHeader* pBlockHeader)
{
    NW_POINTER_ASSERT( pFileHeader );
    
    void* ptr;
    if (!IsReverseEndianBinaryFile(pFileHeader))
    {
        if (pBlockHeader == NULL)
        {
            if (pFileHeader->dataBlocks == 0) return NULL;
            ptr = AddOffsetToPtr(pFileHeader, pFileHeader->headerSize);
        }
        else{
            ptr = AddOffsetToPtr(pBlockHeader, pBlockHeader->size);
        }
    
        if (ptr >= AddOffsetToPtr(pFileHeader, pFileHeader->fileSize))
        {
            return NULL;
        }
    }
    else{
        if (pBlockHeader == NULL)
        {
            if (pFileHeader->dataBlocks == 0 ) return NULL;
            ptr = AddOffsetToPtr( pFileHeader, ReverseEndian(pFileHeader->headerSize));
        }
        else{
            ptr = AddOffsetToPtr(pBlockHeader, ReverseEndian(pBlockHeader->size));
        }
        
        if (ptr >= AddOffsetToPtr(pFileHeader, ReverseEndian(pFileHeader->fileSize)))
        {
            return NULL;
        }
    }
    
    return reinterpret_cast<BinaryBlockHeader*>(ptr);
}

}
}