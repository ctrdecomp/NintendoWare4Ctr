#ifndef NW_SND_WAVE_SOUND_FILE_H_
#define NW_SND_WAVE_SOUND_FILE_H_

#include <nw/ut/ut_BinaryFileFormat.h>
#include <nw/snd/snd_Util.h>
#include <nw/snd/snd_Global.h>
#include <nw/snd/snd_CurveAdshr.h>

namespace nw {
namespace snd {
namespace internal {

struct WaveSoundFile
{
    struct InfoBlock;
    struct FileHeader : public Util::SoundFileHeader
    {
        const InfoBlock* GetInfoBlock() const;
    };

    
    struct WaveSoundData;

    struct InfoBlockBody
    {
        Util::Reference toWaveIdTable;
        Util::Reference toWaveSoundDataReferenceTable;

        const Util::WaveIdTable& GetWaveIdTable() const;
        const Util::ReferenceTable& GetWaveSoundDataReferenceTable() const;

        u32 GetWaveIdCount() const { return GetWaveIdTable().GetCount(); }
        u32 GetWaveSoundCount() const { return GetWaveSoundDataReferenceTable().count; }

        const Util::WaveId* GetWaveId(u32 index) const { return GetWaveIdTable().GetWaveId(index); }
        const WaveSoundData& GetWaveSoundData( u32 index ) const;
    };

    struct InfoBlock
    {
        ut::BinaryBlockHeader   header;
        InfoBlockBody           body;
    };


    struct WaveSoundInfo;
    struct TrackInfo;
    struct NoteInfo;

    struct WaveSoundData
    {
        Util::Reference toWaveSoundInfo;
        Util::Reference toTrackInfoReferenceTable;
        Util::Reference toNoteInfoReferenceTable;

        const WaveSoundInfo& GetWaveSoundInfo() const;

        const Util::ReferenceTable& GetTrackInfoReferenceTable() const;
        const Util::ReferenceTable& GetNoteInfoReferenceTable() const;

        u32 GetTrackCount() const { return GetTrackInfoReferenceTable().count; }
        u32 GetNoteCount() const { return GetNoteInfoReferenceTable().count; }

        const TrackInfo& GetTrackInfo( u32 index ) const;
        const NoteInfo& GetNoteInfo( u32 index ) const;
    };
    
    struct WaveSoundInfo
    {
        Util::BitFlag optionParameter;

        u8 GetPan() const;
        s8 GetSurroundPan() const;
        f32 GetPitch() const;
        void GetSendValue( u8* mainSend, u8* fxSend, u8 fxSendCount ) const;
        const AdshrCurve& GetAdshrCurve() const;
    };

    struct NoteEvent;

    struct TrackInfo
    {
        Util::Reference toNoteEventReferenceTable;

        const Util::ReferenceTable& GetNoteEventReferenceTable() const;

        u32 GetNoteEventCount() const { return GetNoteEventReferenceTable().count; }

        const NoteEvent& GetNoteEvent( u32 index ) const;
    };

    struct NoteEvent
    {
        ut::ResF32  position;
        ut::ResF32  length;
        ut::ResU32  noteIndex;
        ut::ResU32  reserved;
    };

    struct NoteInfo
    {
        ut::ResU32  waveIdTableIndex;
        Util::BitFlag optionParameter;

        u8 GetOriginalKey() const;
        u8 GetVolume() const;
        u8 GetPan() const;
        u8 GetSurroundPan() const;
        f32 GetPitch() const;
        void GetSendValue(u8* mainSend, u8* fxSend[], u8 fxSendCount) const;
        const AdshrCurve& GetAdshrCurve() const;
    };
};

} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_WAVE_SOUND_FILE_H_