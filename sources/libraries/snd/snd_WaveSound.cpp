// Filename: snd_WaveSound.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_WaveSound.h>
#include <nn/os.h>
#include <nw/snd/snd_SoundInstanceManager.h>
#include <nw/snd/snd_SoundDataManager.h>
#include <nw/snd/snd_DriverCommandManager.h>
#include <nw/snd/snd_PlayerHeap.h>
#include <nw/snd/snd_TaskManager.h>

namespace nw {
namespace snd {
namespace internal {

WaveSound::WaveSound(WaveSoundInstanceManager& manager):
    BasicSound(),
    m_pManager(manager),
    m_InitializeFlag(false)
{
}

void WaveSound::Initialize()
{
    BasicSound::Initialize();

    m_pTempSpecialHandle = NULL;
    m_LoadingFlag = false;
    m_PreparedFlag = false;

    m_InitializeFlag = true;
}

void WaveSound::Finalize()
{
    if (!m_InitializeFlag)
    {
        return;
    }

    m_InitializeFlag = false;
    if (m_LoadingFlag)
    {
        TaskManager::GetInstance().CancelTask(&m_DataLoadTask);
        m_DataLoadTask.Wait();
    }

    BasicSound::Finalize();
    m_DataManager.Finalize();
    m_pManager.Free(this);
}

void WaveSound::Prepare(const void* wsdFile, const StartInfo& startInfo)
{
    NW_NULL_ASSERT(wsdFile);
    NW_NULL_ASSERT(startInfo.callback);

    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    DriverCommandWaveSoundSetup* command = cmdmgr.AllocCommand<DriverCommandWaveSoundSetup>();
    command->id = DRIVER_COMMAND_WAVESND_SETUP;
    command->player = &m_WaveSoundPlayerInstance;
    command->wsdFile = wsdFile;
    command->waveSoundOffset = startInfo.index;
    command->startOffsetType = startInfo.startOffsetType;
    command->offset = startInfo.startOffset;
    command->callback = startInfo.callback;
    command->callbackData = startInfo.callbackData;
    cmdmgr.PushCommand(command);

    m_PreparedFlag = true;
}

bool WaveSound::RegisterDataLoadTask(const LoadInfo& loadInfo, const StartInfo& startInfo)
{
    PlayerHeap* heap = GetPlayerHeap();
    if (heap == NULL)
    {
        return false;
    }
    m_DataManager.Initialize(loadInfo.arc);
    m_DataLoadTask.m_pDataManager = &m_DataManager;
    m_DataLoadTask.m_pAllocator = heap;
    m_DataLoadTask.m_Callback = NotifyAsyncLoadFinishedFunc;
    m_DataLoadTask.m_CallbackData = this;

    m_DataLoadTask.m_pSoundDataManager = loadInfo.mgr;
    m_DataLoadTask.m_pSoundArchive = loadInfo.arc;
    m_DataLoadTask.m_LoadInfoWsd = *(loadInfo.wsd);
    m_DataLoadTask.m_Index = startInfo.index;

    m_StartInfo = startInfo;

    TaskManager::GetInstance().AppendTask( &m_DataLoadTask, TaskManager::PRIORITY_MIDDLE );
    return true;
}

void WaveSound::SetChannelPriority(int priority)
{
    NW_MINMAX_ASSERT(priority, 0, 127);

    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    DriverCommandWaveSoundChannelPrio* command = cmdmgr.AllocCommand<DriverCommandWaveSoundChannelPrio>();
    command->id = DRIVER_COMMAND_WAVESND_CHANNELPRIO;
    command->player = &m_WaveSoundPlayerInstance;
    command->priority = priority;
    cmdmgr.PushCommand(command);
}

void WaveSound::SetReleasePriorityFix(bool fix)
{
    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    DriverCommandWaveSoundPrioFix* command = cmdmgr.AllocCommand<DriverCommandWaveSoundPrioFix>();
    command->id = DRIVER_COMMAND_WAVESND_PRIOFIX;
    command->player = &m_WaveSoundPlayerInstance;
    command->priorityFix = fix;
    cmdmgr.PushCommand(command);
}

long WaveSound::GetPlaySamplePosition() const
{
    if (!IsPlayerAvailable())
    {
        return 0;
    }
    return m_WaveSoundPlayerInstance.GetPlaySamplePosition();
}

void WaveSound::NotifyAsyncLoadFinishedFunc(bool result, const LoadItemInfo* wsd, void* userData)
{
    NW_NULL_ASSERT(userData);
    WaveSound* sound = static_cast<WaveSound*>(userData);

    sound->m_LoadingFlag = false;

    if (!result)
    {
        sound->Stop(0);
        return;
    }

    sound->Prepare(wsd->address, sound->m_StartInfo);

    sound->m_WaveSoundPlayerInstance.SetPlayerHeapDataManager(&sound->m_DataManager);
}

/* WaveSound::DataLoadTask */

WaveSound::DataLoadTask::DataLoadTask():
    m_pDataManager(NULL),
    m_pAllocator(NULL),
    m_Callback(NULL),
    m_CallbackData(NULL)
{
}

void WaveSound::DataLoadTask::Execute()
{
    m_pAllocator->Clear();

    if ((m_LoadInfoWsd.address == NULL) && (m_LoadInfoWsd.itemId != SoundArchive::INVALID_ID))
    {
        SoundArchive::ItemId soundId = m_LoadInfoWsd.itemId;

        if (!m_pDataManager->LoadData(soundId, m_pAllocator, SoundArchiveLoader::LOAD_WSD))
        {
            NW_WARNING(false, "failed to load WSD(%08x) to PlayerHeap", soundId);
            if (m_Callback)
            {
                m_Callback(false, NULL, m_CallbackData);
                return;
            }
        }
        m_LoadInfoWsd.address = m_pDataManager->detail_GetFileAddressByItemId(soundId);
    }

    bool result = Util::IsLoadedWaveArchive(m_LoadInfoWsd.address, m_Index, *m_pSoundArchive, *m_pSoundDataManager);
    if (!result)
    {
        if (!m_pDataManager->detail_LoadWaveArchiveByWaveSoundFile(m_LoadInfoWsd.address, m_Index, m_pAllocator))
        {
            NW_WARNING(false, "failed to load WSD(%08x)'s WARC to PlayerHeap", m_LoadInfoWsd.itemId);
            if (m_Callback)
            {
                m_Callback(false, NULL, m_CallbackData);
                return;
            }
        }
    }

    if (m_Callback)
    {
        m_Callback(true, &m_LoadInfoWsd, m_CallbackData);
    }
}

} // namespace internal
} // namespace snd
} // namespace nw