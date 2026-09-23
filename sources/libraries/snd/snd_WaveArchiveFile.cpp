// Filename: snd_WaveArchiveFile.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_WaveArchiveFile.h>

namespace nw {
namespace snd {
namespace internal {

const WaveArchiveFile::InfoBlock*
WaveArchiveFile::FileHeader::GetInfoBlock() const
{
    return reinterpret_cast<const InfoBlock*>(ut::AddOffsetToPtr(this, GetInfoBlockOffset()));
}
const WaveArchiveFile::FileBlock*
WaveArchiveFile::FileHeader::GetFileBlock() const
{
    return reinterpret_cast<const FileBlock*>(ut::AddOffsetToPtr(this, GetFileBlockOffset()));
}

u32 WaveArchiveFile::FileHeader::GetInfoBlockSize() const
{
    return GetReferenceBy(26624)->size;
}
u32 WaveArchiveFile::FileHeader::GetFileBlockSize() const
{
    return GetReferenceBy(26625)->size;
}
u32 WaveArchiveFile::FileHeader::GetInfoBlockOffset() const
{
    return GetReferenceBy(26624)->offset;
}
u32 WaveArchiveFile::FileHeader::GetFileBlockOffset() const
{
    return GetReferenceBy(26625)->offset;
}

const Util::ReferenceWithSize* WaveArchiveFile::FileHeader::GetReferenceBy(u16 typeId) const
{
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        if (toBlocks[i].typeId == typeId)
        {
            return &toBlocks[i];
        }
    }
    return NULL;
}

} // namespace internal
} // namespace snd
} // namespace nw
