#ifndef NW_SND_WAVE_SOUND_FILE_READER_H_
#define NW_SND_WAVE_SOUND_FILE_READER_H_

#include <nw/snd/snd_WaveSoundFile.h>
#include <nw/snd/snd_Global.h>
#include <nw/ut/ut_BinaryFileFormat.h>

namespace nw {
namespace snd {
namespace internal {

struct WaveSoundInfo;
struct WaveSoundNoteInfo;

class WaveSoundFileReader
{
public:
    static const u32 SIGNATURE_FILE = NW_UT_MAKE_SIGWORD('C', 'W', 'S', 'D'); // CWSD

    WaveSoundFileReader(const void* waveSoundFile);
    bool IsAvailable() const { return m_pHeader != NULL; }

    u32 GetWaveSoundCount() const;
    u32 GetNoteInfoCount(u32 index) const;
    u32 GetTrackInfoCount(u32 index) const;

    bool ReadWaveSoundInfo(WaveSoundInfo* dst, u32 index) const;
    bool ReadNoteInfo(WaveSoundNoteInfo* dst, u32 index, u32 noteIndex) const;

private:
    const WaveSoundFile::FileHeader*    m_pHeader;
    const WaveSoundFile::InfoBlockBody* m_pInfoBlockBody;
};

struct WaveSoundInfo
{
    f32 pitch;
    AdshrCurve adshr;
    u8 pan;
    u8 surroundPan;
    u8 mainSend;
    u8 fxSend[AUX_BUS_NUM];
};

struct WaveSoundNoteInfo
{
    u32 waveArchiveId;
    s32 waveIndex;
    AdshrCurve adshr;
    u8 originalKey;
    u8 pan;
    u8 surroundPan;
    u8 volume;
    f32 pitch;
};

} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_WAVE_SOUND_FILE_READER_H_