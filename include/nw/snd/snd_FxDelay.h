#ifndef NW_SND_FX_DELAY_H_
#define NW_SND_FX_DELAY_H_

#include <nw/snd/snd_FxBase.h>

namespace nw { 
namespace snd {

class FxDelay : public FxBase
{
public:
    struct DelayParam
    {
        f32 delay;
        f32 feedback;
        f32 outGain;
        f32 maxDelay;
        f32 lpf;

        DelayParam(): 
            delay(160.0f), 
            feedback(0.4f), 
            outGain(1.0f), 
            maxDelay(160.0f), 
            lpf(1.0f)
        {
        }
    };
};

} // namespace nn
} // namespace snd

#endif // NW_SND_FX_DELAY_H_
