// Filename: snd_Voice.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_Voice.h>

#include <cstring>
#include <nw/snd/snd_Util.h>
#include <nw/snd/snd_Config.h>
#include <nw/snd/snd_VoiceManager.h>
#include <nw/snd/snd_HardwareManager.h>

namespace nw {
namespace snd {
namespace internal {
namespace driver {

Voice::Voice():
    m_Callback(NULL),
    m_IsActive(false),
    m_IsStart(false),
    m_IsStarted(false),
    m_IsPause(false),
    m_SyncFlag(0)
{
    for (int i = 0; i < CHANNEL_MAX; i++)
    {
        m_pHardwareChannel[i] = NULL;
    }
}

Voice::~Voice()
{
    for (int i = 0; i < CHANNEL_MAX; i++)
    {
        if (m_pHardwareChannel[i] != NULL)
        {
            nn::snd::CTR::FreeVoice(m_pHardwareChannel[i]);
        }
    }
}

bool Voice::Alloc(int channelCount, int priority, Voice::VoiceCallback callback, void* callbackData)
{
    
}

} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw