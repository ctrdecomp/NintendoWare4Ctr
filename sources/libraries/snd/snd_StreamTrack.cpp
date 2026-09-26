// Filename: snd_SoundTrack.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_StreamTrack.h>

namespace nw {
namespace snd {
namespace internal {
namespace driver {

void StreamChannel::AppendWaveBuffer(nn::snd::CTR::WaveBuffer* pBuffer, bool lastFlag)
{
    NW_NULL_ASSERT(pBuffer);

    if (m_pVoice != NULL)
    {
        m_pVoice->AppendWaveBuffer(0, pBuffer, lastFlag);
    }
}

} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw