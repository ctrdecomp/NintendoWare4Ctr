// Filename: snd_Bank.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_Bank.h>
#include <nw/snd/snd_WaveFileReader.h>
#include <nw/snd/snd_SoundArchive.h>
#include <nw/snd/snd_SoundArchivePlayer.h>

namespace nw {
namespace snd {
namespace internal {
namespace driver {

Channel* Bank::NoteOn(const void* bankFile, const NoteOnInfo& noteOnInfo, const SoundArchive& archive,
        const SoundArchivePlayer& player, const PlayerHeapDataManager* dataMgr) const
{
    VelocityRegionInfo regionInfo;
{
    BankFileReader reader(bankFile);
    if(reader.ReadVelocityRegionInfo(&regionInfo, noteOnInfo.prgNo, noteOnInfo.key, noteOnInfo.velocity) == false)
    {
        return NULL;
    }
}
    const void* waveFile = Util::GetWaveFile(regionInfo.waveArchiveId, regionInfo.waveIndex, archive, dataMgr);
    if(waveFile == NULL)
    {
        if(dataMgr != NULL)
        {
            waveFile = Util::GetWaveFile(regionInfo.waveArchiveId, regionInfo.waveIndex, archive, player);
            if(waveFile == NULL)
            {
                return NULL;
            }
        }
        else
        {
            return NULL;
        }
    }
    WaveInfo info;
{
    WaveFileReader reader(waveFile);

    if(reader.ReadWaveInfo(&info) == false)
    {
        return NULL;
    }

}

    Channel* pChannel = Channel::AllocChannel(ut::Min(static_cast<int>(info.channelCount), 2), noteOnInfo.priority, noteOnInfo.channelCallback, noteOnInfo.channelCallbackData);
    if (pChannel == NULL)
    {
        return NULL;
    }

    pChannel->SetKey(noteOnInfo.key, regionInfo.originalKey);
    
    register int velocity = noteOnInfo.velocity;
    velocity *= velocity;
    velocity *= regionInfo.volume;
    float initVolume = static_cast<float>(velocity);
    initVolume /= (127.0f * 127.0f * 127.0f);
    pChannel->SetInitVolume(initVolume);
    pChannel->SetTune(regionInfo.pitch);
    
    pChannel->SetAttack(regionInfo.adshrCurve.attack);
    pChannel->SetHold(regionInfo.adshrCurve.hold);
    pChannel->SetDecay(regionInfo.adshrCurve.decay);
    pChannel->SetSustain(regionInfo.adshrCurve.sustain);
    pChannel->SetRelease(regionInfo.adshrCurve.release);
    
    float initPan = static_cast<f32>(noteOnInfo.initPan + regionInfo.pan - 64) / 63.0f;
    pChannel->SetInitPan(initPan);

    pChannel->SetKeyGroupId(regionInfo.keyGroup);
    pChannel->SetIsIgnoreNoteOff(regionInfo.isIgnoreNoteOff);
    pChannel->SetInterpolationType(regionInfo.interpolationType);
    pChannel->Start(info, noteOnInfo.length, 0);

    return pChannel;
}

} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw