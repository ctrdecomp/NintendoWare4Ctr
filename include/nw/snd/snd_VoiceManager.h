#ifndef NW_SND_VOICE_MANAGER_H
#define NW_SND_VOICE_MANAGER_H

#include <nw/snd/snd_Voice.h>

namespace nw {
namespace snd {
namespace internal {
namespace driver {

class VoiceManager
{
    friend class Voice;
public:
    typedef ut::LinkList<Voice, offsetof(Voice, m_LinkNode)> VoiceList;

    static VoiceManager& GetInstance();

    size_t GetRequiredMemSize(int voiceCount);
    void Initialize(void* mem, size_t memSize);
    void Finalize();

    Voice* AllocVoice(int voiceChannelCount, int priority, Voice::VoiceCallback callback, void* callbackData);
    void FreeVoice(Voice* voice);

    void StopAllVoices();
    
    void UpdateAllVoices();
    void UpdateAllVoicesSync(u32 syncFlag);
    
    int GetVoiceCount() const;
    unsigned long GetActiveCount() const;
    unsigned long GetFreeCount() const;
    const VoiceList& GetVoiceList() const;
private:
    VoiceManager();
    ~VoiceManager();

    void ChangeVoicePriority(Voice* voice);
    int DropLowestPriorityVoice(int priority);
    void UpdateEachVoicePriority(const VoiceList::Iterator& beginItr,const VoiceList::Iterator& endItr);

    void AppendVoiceList(Voice* voice);
    void RemoveVoiceList(Voice* voice);

    bool m_Initialized;
    VoiceList m_PrioVoiceList;
    VoiceList m_FreeVoiceList;
};

} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_VOICE_MANAGER_H