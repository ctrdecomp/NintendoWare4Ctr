#ifndef NW_SND_BANK_FILE_READER_H_
#define NW_SND_BANK_FILE_READER_H_

#include <nw/snd/snd_Global.h>
#include <nw/snd/snd_BankFile.h>
#include <nw/ut/ut_BinaryFileFormat.h>

namespace nw { 
namespace snd { 
namespace internal {

struct VelocityRegionInfo
{
    u32 waveArchiveId;
    u32 waveIndex;

    f32 pitch;
    AdshrCurve adshrCurve;

    u8 originalKey;
    u8 volume;
    u8 pan;
    bool isIgnoreNoteOff;
    u8 keyGroup;
    u8 interpolationType;
};

class BankFileReader
{
public:
    static const u32 SIGNATURE_FILE = NW_UT_MAKE_SIGWORD('C', 'B', 'N', 'K');

    BankFileReader(const void* bankFile);

    bool ReadVelocityRegionInfo(VelocityRegionInfo* info,int programNo,int key,int velocity) const;

    const Util::WaveIdTable& GetWaveIdTable() const;

    int GetInstrumentCount() const{ return m_pInfoBlockBody->GetInstrumentCount(); }
    
private:
    const BankFile::FileHeader*     m_pHeader;
    const BankFile::InfoBlockBody*  m_pInfoBlockBody;
};

} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_BANK_FILE_READER_H_
