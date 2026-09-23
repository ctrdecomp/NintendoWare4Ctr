#ifndef NW_SND_SOUND_STARTABLE_H_
#define NW_SND_SOUND_STARTABLE_H_

#include <nw/snd/snd_SoundArchive.h>

namespace nw { 
namespace snd {

class SoundHandle;

class SoundStartable
{
public:
    class StartResult
    {
    public:
        enum ResultCode
        {
            START_SUCCESS = 0,

            START_ERR_LOW_PRIORITY,
            START_ERR_INVALID_LABEL_STRING,
            START_ERR_INVALID_SOUNDID,
            START_ERR_NOT_DATA_LOADED,
            START_ERR_NOT_SEQ_LOADED,
            START_ERR_NOT_BANK_LOADED,
            START_ERR_NOT_WSD_LOADED,
            START_ERR_NOT_WARC_LOADED,
            START_ERR_NOT_ENOUGH_PLAYER_HEAP,
            START_ERR_CANNOT_OPEN_FILE,
            START_ERR_NOT_AVAILABLE,
            START_ERR_CANNOT_ALLOCATE_TRACK,
            START_ERR_NOT_ENOUGH_INSTANCE,
            START_ERR_INVALID_PARAMETER,
            START_ERR_INVALID_SEQ_START_LOCATION_LABEL,
            START_ERR_ACTOR_NOT_INITIALIZED,
            START_ERR_USER = 128,
            START_ERR_UNKNOWN = 255
        };

    public:
        StartResult(): 
            m_Code(START_ERR_UNKNOWN)
        {
        }

        StartResult(ResultCode code): 
            m_Code(code)
        {
        }

        bool IsSuccess() const
        {
            return m_Code == START_SUCCESS;
        }

        ResultCode GetCode() const
        {
            return m_Code;
        }

        

    private:
        ResultCode m_Code;
    };

    struct StartInfo
    {
        enum EnableFlagBit
        {
            ENABLE_START_OFFSET        = 1 << 0,
            ENABLE_PLAYER_ID           = 1 << 1,
            ENABLE_PLAYER_PRIORITY     = 1 << 2,
            ENABLE_ACTOR_PLAYER_ID     = 1 << 3,
            ENABLE_SEQ_SOUND_INFO      = 1 << 4,
            ENABLE_STRM_SOUND_INFO     = 1 << 5,
            ENABLE_WAVE_SOUND_INFO     = 1 << 6,
            ENABLE_VOICE_RENDERER_TYPE = 1 << 7
        };

        enum StartOffsetType
        {
            START_OFFSET_TYPE_MILLISEC,
            START_OFFSET_TYPE_TICK,
            START_OFFSET_TYPE_SAMPLE
        };

        struct SeqSoundInfo
        {
            const void* seqDataAddress;
            const char* startLocationLabel;
            SoundArchive::ItemId bankIds[SoundArchive::SEQ_BANK_MAX];

            SeqSoundInfo(): 
                seqDataAddress(NULL), 
                startLocationLabel(NULL)
                {
                for (u32 i = 0; i < SoundArchive::SEQ_BANK_MAX; i++)
                    bankIds[i] = SoundArchive::INVALID_ID;
            }
        };

        u32 enableFlag;
        StartOffsetType startOffsetType;
        int startOffset;
        SoundArchive::ItemId playerId;
        int playerPriority;
        int actorPlayerId;
        SeqSoundInfo seqSoundInfo;

        StartInfo(): 
            enableFlag(0)
        {
        }
    };

public:
    virtual ~SoundStartable() { }

    StartResult StartSound(SoundHandle* pHandle, SoundArchive::ItemId soundId, const StartInfo* pStartInfo = NULL);
    StartResult StartSound(SoundHandle* pHandle, const char* pSoundName, const StartInfo* pStartInfo = NULL);

    StartResult HoldSound(SoundHandle* pHandle, SoundArchive::ItemId soundId, const StartInfo* pHoldInfo = NULL);
    StartResult HoldSound(SoundHandle* pHandle, const char* pSoundName, const StartInfo* pHoldInfo = NULL);

    StartResult PrepareSound(SoundHandle* pHandle, SoundArchive::ItemId soundId, const StartInfo* pStartInfo = NULL);
    StartResult PrepareSound(SoundHandle* pHandle, const char* pSoundName, const StartInfo* pStartInfo = NULL);

protected:
    virtual StartResult detail_SetupSound(SoundHandle* handle, u32 soundId, bool holdFlag, const StartInfo* startInfo) = 0;
    virtual SoundArchive::ItemId detail_GetItemId(const char* pString) = 0;
};


} // namespace snd
} // mamespace nw

#endif // NW_SND_SOUND_STARTABLE_H_
