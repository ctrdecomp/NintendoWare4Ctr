// Filename: lyt_TexResource.cpp
//
// Project: NintendoWare4Ctr

#include <nw/lyt/lyt_TexResource.h>

namespace nw{
namespace lyt{

bool TexResource::Set(void* pTexRes, u32 size)
{
    const int RESOURCE_ALIGNMENT = 128;
    const int BLOCK_ALIGNMENT = 4;
    const u32 MIN_SIZE = sizeof(ut::BinaryFileHeader) + sizeof(res::Image) + sizeof(res::ImageSize);

    do
    {
        NW_NULL_ASSERT(pTexRes);
        if (pTexRes == NULL)
        {
            break;
        }

        NW_ASSERT((u32)(pTexRes) % RESOURCE_ALIGNMENT == 0);
        if (!((u32)(pTexRes) % RESOURCE_ALIGNMENT == 0))
        {
            break;
        }

        NW_ASSERT(size % BLOCK_ALIGNMENT == 0);
        if (!(size % BLOCK_ALIGNMENT == 0))
        {
            break;
        }

        NW_ASSERT(size > MIN_SIZE);
        if (!(size > MIN_SIZE))
        {
            break;
        }

        res::ImageSize* pImageSize = internal::ConvertOffsToPtr<res::ImageSize>(pTexRes, size - sizeof(res::ImageSize));

        u32 imageSize = pImageSize->imageSize;

        NW_ASSERT(imageSize < size); 
        if (!(imageSize < size))
        {
            break;
        }

        ut::BinaryFileHeader* pFileHead = internal::ConvertOffsToPtr<ut::BinaryFileHeader>(pTexRes, ut::RoundUp(imageSize, BLOCK_ALIGNMENT));

        if (!ut::IsValidBinaryFile(pFileHead, res::FILESIGNATURE_CLIM, res::BinaryFileFormatVersion))
        {
            break;
        }

        res::Image* pImage = 0;

        ut::BinaryBlockHeader* pBlockHead = NULL;
        for (int i = 0; i < pFileHead->dataBlocks; ++i)
        {
            pBlockHead = ut::GetNextBinaryBlockHeader(pFileHead, pBlockHead);
            NW_NULL_ASSERT(pBlockHead);

            ut::SigWord kind = pBlockHead->kind;
            switch (kind)
            {
            case res::DATABLOCKKIND_IMAGE:
                pImage = reinterpret_cast<res::Image *>(pBlockHead);
                break;
            }
        }

        NW_NULL_ASSERT(pImage);
        if (pImage == NULL)
        {
            break;
        }

        m_pTop = const_cast<void*>(pTexRes);
        m_pImage = const_cast<res::Image*>(pImage);
        m_pImageSize = const_cast<res::ImageSize*>(pImageSize);

        return true;

    } while (false);

    m_pTop = NULL;
    m_pImage = NULL;
    m_pImageSize = NULL;

    return false;
}

}
}