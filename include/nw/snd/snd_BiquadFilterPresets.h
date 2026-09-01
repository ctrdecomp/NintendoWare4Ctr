#ifndef NW_SND_BIQUAD_FILTER_PRESETS_H_
#define NW_SND_BIQUAD_FILTER_PRESETS_H_

#include <nw/snd/snd_BiquadFilterCallback.h>

namespace nw {
namespace snd {
namespace internal {

class BiquadFilterLpf : public BiquadFilterCallback
{
public:
    BiquadFilterLpf() {}
    virtual void GetCoefficients(int type, f32 value, Coefficients* coef) const;

private:
    static const int COEFFICIENTS_TABLE_SIZE = 112;
    static const Coefficients COEFFICIENTS_TABLE[COEFFICIENTS_TABLE_SIZE];
};

class BiquadFilterHpf : public BiquadFilterCallback
{
public:
    BiquadFilterHpf() {}
    virtual void GetCoefficients(int type, f32 value, Coefficients* coef) const;

private:
    static const int COEFFICIENTS_TABLE_SIZE = 97;
    static const Coefficients COEFFICIENTS_TABLE[COEFFICIENTS_TABLE_SIZE];
};

class BiquadFilterBpf512 : public BiquadFilterCallback
{
public:
    BiquadFilterBpf512() {}
    virtual void GetCoefficients(int type, f32 value, Coefficients* coef) const;

private:
    static const int COEFFICIENTS_TABLE_SIZE = 122;
    static const Coefficients COEFFICIENTS_TABLE[COEFFICIENTS_TABLE_SIZE];
};

class BiquadFilterBpf1024 : public BiquadFilterCallback
{
public:
    BiquadFilterBpf1024() {}
    virtual void GetCoefficients(int type, f32 value, Coefficients* coef) const;

private:
    static const int COEFFICIENTS_TABLE_SIZE = 93;
    static const Coefficients COEFFICIENTS_TABLE[COEFFICIENTS_TABLE_SIZE];
};

class BiquadFilterBpf2048 : public BiquadFilterCallback
{
public:
    BiquadFilterBpf2048() {}
    virtual void GetCoefficients(int type, f32 value, Coefficients* coef) const;

private:
    static const int COEFFICIENTS_TABLE_SIZE = 93;
    static const Coefficients COEFFICIENTS_TABLE[COEFFICIENTS_TABLE_SIZE];
};

} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_BIQUAD_FILTER_PRESETS_H_