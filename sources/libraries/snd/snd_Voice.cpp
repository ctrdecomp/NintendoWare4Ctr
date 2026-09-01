// Filename: snd_Voice.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_Voice.h>

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
    for(int i = 0; i < CHANNEL_MAX; i++)
        m_pHardwareChannel[i] = NULL;
}

Voice::~Voice()
{
    for(int i = 0; i < CHANNEL_MAX; i++)
    {
        if(m_pHardwareChannel[i] != NULL)
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