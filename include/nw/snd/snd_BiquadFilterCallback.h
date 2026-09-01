#ifndef NW_SND_BIQUAD_FILTER_CALLBACK_H_
#define NW_SND_BIQUAD_FILTER_CALLBACK_H_

#include <nn/types.h>
#include <nn/snd.h>

namespace nw {
namespace snd {

class BiquadFilterCallback
{
public:
    typedef nn::snd::CTR::BiquadFilterCoefficients Coefficients;

    virtual ~BiquadFilterCallback() {}
    virtual void GetCoefficients( int type, f32 value, Coefficients* coef ) const = 0;
};

} // namespace nw
} // namespace snd

#endif // NW_SND_BIQUAD_FILTER_CALLBACK_H_