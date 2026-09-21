#ifndef NW_SND_BANK_FILE_H_
#define NW_SND_BANK_FILE_H_

#include <nw/types.h>
#include <nw/snd/snd_Util.h>
#include <nw/snd/snd_Global.h>

namespace nw { 
namespace snd { 
namespace internal {

struct BankFile
{
    struct InfoBlockBody;
    struct InfoBlock;
    struct FileHeader : public Util::SoundFileHeader
    {
        const InfoBlock* GetInfoBlock() const;
    };
    struct Instrument;

    struct InfoBlockBody
    {
        Util::Reference toWaveIdTable;
        Util::Reference toInstrumentReferenceTable;

        const Util::WaveIdTable& GetWaveIdTable() const;
        const Util::ReferenceTable& GetInstrumentReferenceTable() const;

        s32 GetInstrumentCount() const { return GetInstrumentReferenceTable().count; }
        const Util::WaveId& GetWaveId(u32 index) const { return GetWaveIdTable().GetWaveId(index); }

        const Instrument* GetInstrument(int programNo) const;
    };

    struct InfoBlock
    {
        ut::BinaryBlockHeader header;
        InfoBlockBody         body;
    };

    struct KeyRegion;
    struct Instrument
    {
        Util::Reference toKeyRegionChunk;
        const KeyRegion* GetKeyRegion(u32 key) const;
    };

    struct VelocityRegion;
    struct KeyRegion
    {
        Util::Reference toVelocityRegionChunk;
        const VelocityRegion* GetVelocityRegion(u32 velocity) const;
    };

    struct RegionParameter;
    struct VelocityRegion
    {
        nw::ut::ResU32  waveIdTableIndex;
        Util::BitFlag   optionParameter;

        u8 GetOriginalKey() const;
        u8 GetVolume() const;
        u8 GetPan() const;
        f32 GetPitch() const;
        bool IsIgnoreNoteOff() const;
        u8 GetKeyGroup() const;
        u8 GetInterpolationType() const;
        const AdshrCurve& GetAdshrCurve() const;

        const RegionParameter* GetRegionParameter() const;
    };

    struct RegionParameter
    {
        u8 originalKey;
        u8 padding1[3];

        u8 volume;
        u8 padding2[3];

        u8 pan;
        s8 surroundPan;
        u8 padding3[2];

        f32 pitch;

        bool isIgnoreNoteOff;
        u8 keyGroup;
        u8 interpolationType;
        u8 padding4[1];

        u32 offset;
        Util::Reference refToAdshrCurve;
        AdshrCurve adshrCurve;
    };
};

} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_BANK_FILE_H_