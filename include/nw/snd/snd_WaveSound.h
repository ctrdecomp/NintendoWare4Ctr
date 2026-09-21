#ifndef NW_SND_WAVE_SOUND_H_
#define NW_SND_WAVE_SOUND_H_

#include <nw/snd/snd_SoundInstanceManager.h>
#include <nw/snd/snd_BasicSound.h>
#include <nw/snd/snd_WaveSoundPlayer.h>
#include <nw/snd/snd_Task.h>
#include <nw/snd/snd_PlayerHeapDataManager.h>

namespace nw { 
namespace snd {

class WaveSoundHandle;

namespace internal {

class WaveSound;
class SoundDataManager;

class WaveSound : public BasicSound
{
public:
    NW_UT_RUNTIME_TYPEINFO;

    struct StartInfo
    {
        s32 index;
        driver::WaveSoundPlayer::StartOffsetType startOffsetType;
        s32 startOffset;
        const driver::WaveSoundPlayer::WaveSoundCallback* callback;
        u32 callbackData;
    };
protected:
    typedef void (*NotifyAsyncLoadFinished)(bool result, const LoadItemInfo* wsd, void* userData);

    class DataLoadTask : public Task
    {
    public:
        DataLoadTask();
        virtual void Execute();

        PlayerHeapDataManager* m_pDataManager;
        PlayerHeap* m_pAllocator;
        NotifyAsyncLoadFinished m_Callback;
        void* m_CallbackData;

        const SoundDataManager* m_pSoundDataManager;
        const SoundArchive* m_pSoundArchive;
        LoadItemInfo m_LoadInfoWsd;

        s32 m_Index;
    };
public:

    driver::WaveSoundPlayer m_WaveSoundPlayerInstance;
    WaveSoundHandle* m_pTempSpecialHandle;
    WaveSoundInstanceManager& m_pManager;

    DataLoadTask m_DataLoadTask;
    PlayerHeapDataManager m_DataManager;
    StartInfo m_StartInfo;

    volatile bool m_LoadingFlag;
    bool m_PreparedFlag;
    bool m_InitializeFlag;
};

typedef SoundInstanceManager<WaveSound> WaveSoundInstanceManager;

} // namespace internal
} // namespace snd
} // namespace mw

#endif // NW_SND_WAVE_SOUND_H_
