// Filename: snd_WaveSoundFile.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_WaveSoundFile.h>
#include <nw/ut/ut_Inlines.h>

namespace nw {
namespace snd {
namespace internal {

namespace {

const u8 DEFAULT_PAN = 64;
const s8 DEFAULT_SURROUND_PAN = 0;
const f32 DEFAULT_PITCH = 1.0f;
const u8 DEFAULT_MAIN_SEND = 127;
const u8 DEFAULT_FX_SEND = 0;
const AdshrCurve DEFAULT_ADSHR_CURVE(
    127,
    127,
    127,
    127,
    127
);
const u8 DEFAULT_KEY  = 64;
const u8 DEFAULT_VOLUME         = 96;

enum WaveSoundInfoBitFlag
{
    WAVE_SOUND_INFO_PAN = 0x00,
    WAVE_SOUND_INFO_PITCH,
    WAVE_SOUND_INFO_SEND = 0x08,
    WAVE_SOUND_INFO_ENVELOPE,
    WAVE_SOUND_INFO_RANDOMIZER
};

enum NoteInfoBitFlag
{
    NOTE_INFO_KEY = 0x00,
    NOTE_INFO_VOLUME,
    NOTE_INFO_PAN,
    NOTE_INFO_PITCH,
    NOTE_INFO_SEND = 0x08,
    NOTE_INFO_ENVELOPE,
    NOTE_INFO_RANDOMIZER,
    NOTE_INFO_LFO
};

struct SendValue
{
    u8 mainSend;
    Util::Table<u8,u8> fxSend;
};

} // namespace ""

// WaveSoundFile::FileHeader
const WaveSoundFile::InfoBlock* WaveSoundFile::FileHeader::GetInfoBlock() const
{
    return reinterpret_cast<const InfoBlock*>(GetBlock(0x7000));
}

// WaveSoundFile::InfoBlockBody

const WaveSoundFile::WaveSoundData& WaveSoundFile::InfoBlockBody::GetWaveSoundData(u32 index) const
{
    NW_ASSERT(index < GetWaveSoundCount());
    const void* pWaveSoundData = GetWaveSoundDataReferenceTable().GetReferedItem(index, 0x4900);
    NW_NULL_ASSERT(pWaveSoundData);
    return *reinterpret_cast<const WaveSoundData*>(pWaveSoundData);
}

const Util::ReferenceTable& WaveSoundFile::InfoBlockBody::GetWaveSoundDataReferenceTable() const
{
    return *reinterpret_cast<const Util::ReferenceTable*>(ut::AddOffsetToPtr(this, toWaveSoundDataReferenceTable.offset));
}

const Util::WaveIdTable& WaveSoundFile::InfoBlockBody::GetWaveIdTable() const
{
    return *reinterpret_cast<const Util::WaveIdTable*>(ut::AddOffsetToPtr(this, toWaveIdTable.offset));
}

// WaveSoundFile::WaveSoundData

const WaveSoundFile::WaveSoundInfo& WaveSoundFile::WaveSoundData::GetWaveSoundInfo() const
{
    NW_ASSERT(toWaveSoundInfo.IsValidTypeId(ElementType_WaveSoundFile_WaveSoundInfo));

    return *reinterpret_cast<const WaveSoundInfo*>(ut::AddOffsetToPtr(this, toWaveSoundInfo.offset));
}

const Util::ReferenceTable& WaveSoundFile::WaveSoundData::GetTrackInfoReferenceTable() const
{
    return *reinterpret_cast<const Util::ReferenceTable*>(ut::AddOffsetToPtr(this, toTrackInfoReferenceTable.offset));
}

const Util::ReferenceTable& WaveSoundFile::WaveSoundData::GetNoteInfoReferenceTable() const
{
    return *reinterpret_cast<const Util::ReferenceTable*>(ut::AddOffsetToPtr(this, toNoteInfoReferenceTable.offset));
}

const WaveSoundFile::TrackInfo& WaveSoundFile::WaveSoundData::GetTrackInfo(u32 index) const
{
    NW_ASSERT(index < GetTrackCount());

    const void* pTrackInfo = GetTrackInfoReferenceTable().GetReferedItem(index, 18691);
    NW_NULL_ASSERT(pTrackInfo);

    return *reinterpret_cast<const TrackInfo*>(pTrackInfo);
}

const WaveSoundFile::NoteInfo& WaveSoundFile::WaveSoundData::GetNoteInfo(u32 index) const
{
    NW_ASSERT(index < GetNoteCount());

    const void* pNoteInfo = GetNoteInfoReferenceTable().GetReferedItem(index, 0x4902);
    NW_NULL_ASSERT(pNoteInfo);

    return *reinterpret_cast<const NoteInfo*>(pNoteInfo);
}

// WaveSoundFile::WaveSoundInfo

u8 WaveSoundFile::WaveSoundInfo::GetPan() const
{
    u32 value;
    bool result = optionParameter.GetValue(&value, WAVE_SOUND_INFO_PAN);
    if (result == false)
    {
        return DEFAULT_PAN;
    }
    return Util::DevideBy8bit(value, 0);
}
s8 WaveSoundFile::WaveSoundInfo::GetSurroundPan() const
{
    u32 value;
    bool result = optionParameter.GetValue( &value, WAVE_SOUND_INFO_PAN );
    if (result == false)
    {
        return DEFAULT_SURROUND_PAN;
    }
    return static_cast<s8>(Util::DevideBy8bit(value, 1));
}
f32 WaveSoundFile::WaveSoundInfo::GetPitch() const
{
    f32 value;
    bool result = optionParameter.GetValueF32(&value, WAVE_SOUND_INFO_PITCH);
    if (result == false)
    {
        return DEFAULT_PITCH;
    }
    return value;
}

void WaveSoundFile::WaveSoundInfo::GetSendValue(u8* mainSend, u8* fxSend, u8 fxSendCount) const
{
    u32 value;
    bool result = optionParameter.GetValue( &value, WAVE_SOUND_INFO_SEND );
    if (result == false)
    {
        *mainSend = DEFAULT_MAIN_SEND;
        for (int i = 0; i < fxSendCount; i++)
        {
            fxSend[i] = DEFAULT_FX_SEND;
        }
        return;
    }

    const SendValue& sendValue = *reinterpret_cast<const SendValue*>(ut::AddOffsetToPtr(this, value));

    NW_ASSERT(fxSendCount <= sendValue.fxSend.count);
    *mainSend = sendValue.mainSend;
    int countSize = sendValue.fxSend.count > AUX_BUS_NUM ? AUX_BUS_NUM : sendValue.fxSend.count;
    for (int i = 0; i < countSize; i++)
    {
        fxSend[i] = sendValue.fxSend.item[i];
    }
}

const AdshrCurve& WaveSoundFile::WaveSoundInfo::GetAdshrCurve() const
{
    u32 offsetToReference;
    bool result = optionParameter.GetValue(&offsetToReference, WAVE_SOUND_INFO_ENVELOPE);
    if (result == false)
    {
        return DEFAULT_ADSHR_CURVE;
    }

    const Util::Reference& ref = *reinterpret_cast<const Util::Reference*>(ut::AddOffsetToPtr(this, offsetToReference));
    return *reinterpret_cast<const AdshrCurve*>(ut::AddOffsetToPtr(&ref, ref.offset));
}

// WaveSoundFile::TrackInfo

const Util::ReferenceTable& WaveSoundFile::TrackInfo::GetNoteEventReferenceTable() const
{
    return *reinterpret_cast<const Util::ReferenceTable*>(ut::AddOffsetToPtr(this, toNoteEventReferenceTable.offset));
}

const WaveSoundFile::NoteEvent& WaveSoundFile::TrackInfo::GetNoteEvent(u32 index) const
{
    NW_ASSERT(index < GetNoteEventCount());

    const void* pNoteEvent = GetNoteEventReferenceTable().GetReferedItem(index, 18692);
    NW_NULL_ASSERT(pNoteEvent);

    return *reinterpret_cast<const NoteEvent*>(pNoteEvent);
}

// WaveSoundFile::NoteInfo

u8 WaveSoundFile::NoteInfo::GetOriginalKey() const
{
    u32 value;
    bool result = optionParameter.GetValue(&value, NOTE_INFO_KEY);
    if (result == false)
    {
        return DEFAULT_KEY;
    }
    return Util::DevideBy8bit(value, 0);
}

u8 WaveSoundFile::NoteInfo::GetVolume() const
{
    u32 value;
    bool result = optionParameter.GetValue(&value, NOTE_INFO_VOLUME);
    if (result == false)
    {
        return DEFAULT_VOLUME;
    }
    return Util::DevideBy8bit(value, 0);
}

u8 WaveSoundFile::NoteInfo::GetPan() const
{
    u32 value;
    bool result = optionParameter.GetValue(&value, NOTE_INFO_PAN);
    if (result == false)
    {
        return DEFAULT_PAN;
    }
    return Util::DevideBy8bit(value, 0);
}
u8 WaveSoundFile::NoteInfo::GetSurroundPan() const
{
    u32 value;
    bool result = optionParameter.GetValue(&value, NOTE_INFO_PAN);
    if (result == false)
    {
        return DEFAULT_SURROUND_PAN;
    }
    return static_cast<s8>(Util::DevideBy8bit(value, 1));
}

f32 WaveSoundFile::NoteInfo::GetPitch() const
{
    f32 value;
    bool result = optionParameter.GetValueF32(&value, NOTE_INFO_PITCH);
    if (result == false)
    {
        return DEFAULT_PITCH;
    }
    return value;
}

void WaveSoundFile::NoteInfo::GetSendValue(u8* mainSend, u8* fxSend[], u8 fxSendCount) const
{
    u32 value;
    bool result = optionParameter.GetValue(&value, NOTE_INFO_SEND);
    if (result == false)
    {
        *mainSend = DEFAULT_MAIN_SEND;
        for (int i = 0; i < fxSendCount; i++)
        {
            *fxSend[i] = DEFAULT_FX_SEND;
        }
        return;
    }

    const SendValue& sendValue = *reinterpret_cast<const SendValue*>(ut::AddOffsetToPtr(this, value));

    NW_ASSERT(fxSendCount <= sendValue.fxSend.count);
    *mainSend = sendValue.mainSend;
    int countSize = sendValue.fxSend.count > AUX_BUS_NUM ? AUX_BUS_NUM : sendValue.fxSend.count;
    for (int i = 0; i < countSize; i++)
    {
        *fxSend[i] = sendValue.fxSend.item[i];
    }
}

const AdshrCurve& WaveSoundFile::NoteInfo::GetAdshrCurve() const
{
    u32 offsetToReference;
    bool result = optionParameter.GetValue(&offsetToReference, NOTE_INFO_ENVELOPE);
    if (result == false)
    {
        return DEFAULT_ADSHR_CURVE;
    }

    const Util::Reference& ref = *reinterpret_cast<const Util::Reference*>(ut::AddOffsetToPtr(this, offsetToReference));
    return *reinterpret_cast<const AdshrCurve*>(ut::AddOffsetToPtr(&ref, ref.offset));
}

} // namespace internal
} // namespace snd
} // namespace nw