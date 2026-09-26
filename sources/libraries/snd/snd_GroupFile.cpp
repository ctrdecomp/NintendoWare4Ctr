// Filename: snd_GroupFile.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_GroupFile.h>

namespace nw {
namespace snd {
namespace internal {

// FileHeader

const GroupFile::InfoBlock* GroupFile::FileHeader::GetInfoBlock() const
{
    return (InfoBlock*)Util::SoundFileHeader::GetBlock(0x7800);
}

const GroupFile::FileBlock* GroupFile::FileHeader::GetFileBlock() const
{
    return (FileBlock*)Util::SoundFileHeader::GetBlock(0x7801);
}

const GroupFile::InfoExBlock* GroupFile::FileHeader::GetInfoExBlock() const
{
    return (InfoExBlock*)Util::SoundFileHeader::GetBlock(0x7802);
}

} // namespace internal
} // namespace snd
} // namespace nw