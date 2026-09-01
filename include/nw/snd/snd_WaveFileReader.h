#ifndef NW_SND_WAVE_FILE_READER_H_
#define NW_SND_WAVE_FILE_READER_H_

#include <nw/snd/snd_WaveFile.h>

namespace nw {
namespace snd {
namespace internal {

class WaveFileReader
{
public:
    static const u32 SIGNATURE_FILE = NW_UT_MAKE_SIGWORD('C', 'W', 'A', 'V');
    static SampleFormat GetSampleFormat(u8 encodeMethod);

    WaveFileReader(const void* waveFile);

    bool ReadWaveInfo(WaveInfo* info, const void* waveDataOffsetOrigin = NULL) const;
private:
    const void* GetWaveDataAddress(const WaveFile::ChannelInfo* info, const void* waveDataOffsetOrigin ) const;

    const WaveFile::InfoBlockBody*  m_pInfoBlockBody;
    const void*                     m_pDataBlockBody;
};

} // namespace internal
} // namespace snd
} // namespace nw


#endif // NW_SND_WAVE_FILE_READER_H_