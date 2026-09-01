#ifndef NW_SND_STREAM_TRACK_H_
#define NW_SND_STREAM_TRACK_H_

#include <nn/snd.h>
#include <nw/snd/snd_StreamSoundFileReader.h>

#include <nw/snd/snd_Voice.h>
#include <nw/snd/snd_Global.h>

namespace nw {
namespace snd {
namespace internal {
namespace driver {

struct StreamChannel
{
    static const int WAVE_BUFFER_MULTIPLYING_FACTOR = 4;
    static const int WAVE_BUFFER_MAX = 5 * WAVE_BUFFER_MULTIPLYING_FACTOR;

    enum AdpcmContextType
    {
        ADPCM_CONTEXT_HEAD,
        ADPCM_CONTEXT_LOOP,
        ADPCM_CONTEXT_NUM
    };

    void AppendWaveBuffer(nn::snd::CTR::WaveBuffer* pBuffer, bool lastFlag);

    void* m_pBufferAddress;
    DspAdpcmParam m_AdpcmParam;
    DspAdpcmLoopParam m_AdpcmLoopParam;

    Voice* m_pVoice;
    nn::snd::CTR::WaveBuffer m_WaveBuffer[WAVE_BUFFER_MAX];
    nn::snd::CTR::AdpcmContext m_AdpcmContext[ADPCM_CONTEXT_NUM];
};


struct StreamTrack
{
public:
    bool m_ActiveFlag;

    StreamChannel* m_pChannels[Voice::CHANNEL_MAX];

    StreamSoundFileReader::TrackInfo m_TrackInfo;
    f32 m_Volume;
    f32 m_Pan;
    f32 m_SurroundPan;
};

} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw


#endif // NW_SND_STREAM_TRACK_H_