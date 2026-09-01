#ifndef NW_SND_FX_BASE_H_
#define NW_SND_FX_BASE_H_

#include <nn/snd.h>
#include <nw/ut/ut_LinkList.h>
#include <nw/ut/ut_Preprocessor.h>
#include <nw/snd/snd_Global.h>

namespace nw{
namespace snd{

class FxBase
{
public:
    virtual ~FxBase() {}
    virtual bool Initialize() = 0;
    virtual void Finalize() = 0;
    virtual void UpdateBuffer(int numChannels,nn::snd::CTR::AuxBusData* data,s32 sampleLength,SampleFormat format,f32 sampleRate,OutputMode mode) = 0;
    virtual void OnChangeOutputMode() {}

    ut::LinkListNode m_Link;
};

} // namespace nw
} // namespace snd

#endif // NW_SND_FX_BASE_H_