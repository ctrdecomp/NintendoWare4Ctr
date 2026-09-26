// Filename: snd_BankFile.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_BankFile.h>

namespace nw {
namespace snd {
namespace internal {

// FileHeader

const BankFile::InfoBlock* BankFile::FileHeader::GetInfoBlock() const
{
    return (InfoBlock*) Util::SoundFileHeader::GetBlock(0x5800);
}

}
}
}