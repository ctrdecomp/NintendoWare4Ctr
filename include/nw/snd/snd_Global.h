#ifndef NW_SND_GLOBAL_H_
#define NW_SND_GLOBAL_H_

#include <nw/types.h>
#include <nw/ut/ut_ResTypes.h>

#include <nn/snd.h>

namespace nw { 
namespace snd {

enum OutputMode
{
    OUTPUT_MODE_MONO,
    OUTPUT_MODE_STEREO,
    OUTPUT_MODE_SURROUND,
    OUTPUT_MODE_NUM
};

enum AuxBus
{
    AUX_BUS_NULL = nn::snd::CTR::AUX_BUS_NULL,
    AUX_BUS_A = nn::snd::CTR::AUX_BUS_A,
    AUX_BUS_B = nn::snd::CTR::AUX_BUS_B,
    AUX_BUS_NUM = nn::snd::CTR::AUX_BUS_NUM
};

enum SampleFormat
{
    SAMPLE_FORMAT_PCM_S8,
    SAMPLE_FORMAT_PCM_S16,
    SAMPLE_FORMAT_PCM_S32,
    SAMPLE_FORMAT_DSP_ADPCM,
    SAMPLE_FORMAT_IMA_ADPCM
};

enum SeqMute
{
    SEQ_MUTE_OFF,
    SEQ_MUTE_NO_STOP,
    SEQ_MUTE_RELEASE,
    SEQ_MUTE_STOP
};

static const u32 WAVE_CHANNEL_MAX = 2;
static const u32 SEQ_BANK_MAX = 4;

enum PanMode
{
    PAN_MODE_DUAL,
    PAN_MODE_BALANCE,
    PAN_MODE_INVALID
};

enum PanCurve
{
    PAN_CURVE_SQRT,
    PAN_CURVE_SQRT_0DB,
    PAN_CURVE_SQRT_0DB_CLAMP,
    PAN_CURVE_SINCOS,
    PAN_CURVE_SINCOS_0DB,
    PAN_CURVE_SINCOS_0DB_CLAMP,
    PAN_CURVE_LINEAR,
    PAN_CURVE_LINEAR_0DB,
    PAN_CURVE_LINEAR_0DB_CLAMP,
    PAN_CURVE_INVALID
};

enum DecayCurve
{
    DECAY_CURVE_LOG = 1,
    DECAY_CURVE_LINEAR = 2
};

enum WaveType
{
    WAVE_TYPE_NWWAV,
    WAVE_TYPE_DSPADPCM,
    WAVE_TYPE_INVALID = -1
};

struct AdshrCurve
{
    u8 attack;
    u8 decay;
    u8 sustain;
    u8 hold;
    u8 release;

    AdshrCurve(u8 a = 0, u8 d = 0, u8 s = 0, u8 h = 0, u8 r = 0): 
        attack(a), 
        decay(d), 
        sustain(s), 
        hold(h), 
        release(r)
    {
    }
};

struct DspAdpcmParam
{
    ut::ResU16 coef[16];
    ut::ResU16 predScale;
    ut::ResU16 yn1;
    ut::ResU16 yn2;
};

enum BiquadFilterType
{
    BIQUAD_FILTER_TYPE_INHERIT = 0,
    BIQUAD_FILTER_TYPE_LPF     = 1,
    BIQUAD_FILTER_TYPE_HPF     = 2,
    BIQUAD_FILTER_TYPE_BPF512  = 3,
    BIQUAD_FILTER_TYPE_BPF1024 = 4,
    BIQUAD_FILTER_TYPE_BPF2048 = 5,

    BIQUAD_FILTER_TYPE_USER_MIN = 64,

    BIQUAD_FILTER_TYPE_USER_0 = BIQUAD_FILTER_TYPE_USER_MIN,
    BIQUAD_FILTER_TYPE_USER_1,
    BIQUAD_FILTER_TYPE_USER_2,
    BIQUAD_FILTER_TYPE_USER_3,
    BIQUAD_FILTER_TYPE_USER_4,
    BIQUAD_FILTER_TYPE_USER_5,
    BIQUAD_FILTER_TYPE_USER_6,
    BIQUAD_FILTER_TYPE_USER_7,
    BIQUAD_FILTER_TYPE_USER_8,
    BIQUAD_FILTER_TYPE_USER_9,
    BIQUAD_FILTER_TYPE_USER_10,
    BIQUAD_FILTER_TYPE_USER_11,
    BIQUAD_FILTER_TYPE_USER_12,
    BIQUAD_FILTER_TYPE_USER_13,
    BIQUAD_FILTER_TYPE_USER_14,
    BIQUAD_FILTER_TYPE_USER_15,
    BIQUAD_FILTER_TYPE_USER_16,
    BIQUAD_FILTER_TYPE_USER_17,
    BIQUAD_FILTER_TYPE_USER_18,
    BIQUAD_FILTER_TYPE_USER_19,
    BIQUAD_FILTER_TYPE_USER_20,
    BIQUAD_FILTER_TYPE_USER_21,
    BIQUAD_FILTER_TYPE_USER_22,
    BIQUAD_FILTER_TYPE_USER_23,
    BIQUAD_FILTER_TYPE_USER_24,
    BIQUAD_FILTER_TYPE_USER_25,
    BIQUAD_FILTER_TYPE_USER_26,
    BIQUAD_FILTER_TYPE_USER_27,
    BIQUAD_FILTER_TYPE_USER_28,
    BIQUAD_FILTER_TYPE_USER_29,
    BIQUAD_FILTER_TYPE_USER_30,
    BIQUAD_FILTER_TYPE_USER_31,
    BIQUAD_FILTER_TYPE_USER_32,
    BIQUAD_FILTER_TYPE_USER_33,
    BIQUAD_FILTER_TYPE_USER_34,
    BIQUAD_FILTER_TYPE_USER_35,
    BIQUAD_FILTER_TYPE_USER_36,
    BIQUAD_FILTER_TYPE_USER_37,
    BIQUAD_FILTER_TYPE_USER_38,
    BIQUAD_FILTER_TYPE_USER_39,
    BIQUAD_FILTER_TYPE_USER_40,
    BIQUAD_FILTER_TYPE_USER_41,
    BIQUAD_FILTER_TYPE_USER_42,
    BIQUAD_FILTER_TYPE_USER_43,
    BIQUAD_FILTER_TYPE_USER_44,
    BIQUAD_FILTER_TYPE_USER_45,
    BIQUAD_FILTER_TYPE_USER_46,
    BIQUAD_FILTER_TYPE_USER_47,
    BIQUAD_FILTER_TYPE_USER_48,
    BIQUAD_FILTER_TYPE_USER_49,
    BIQUAD_FILTER_TYPE_USER_50,
    BIQUAD_FILTER_TYPE_USER_51,
    BIQUAD_FILTER_TYPE_USER_52,
    BIQUAD_FILTER_TYPE_USER_53,
    BIQUAD_FILTER_TYPE_USER_54,
    BIQUAD_FILTER_TYPE_USER_55,
    BIQUAD_FILTER_TYPE_USER_56,
    BIQUAD_FILTER_TYPE_USER_57,
    BIQUAD_FILTER_TYPE_USER_58,
    BIQUAD_FILTER_TYPE_USER_59,
    BIQUAD_FILTER_TYPE_USER_60,
    BIQUAD_FILTER_TYPE_USER_61,
    BIQUAD_FILTER_TYPE_USER_62,
    BIQUAD_FILTER_TYPE_USER_63,

    BIQUAD_FILTER_TYPE_USER_MAX = BIQUAD_FILTER_TYPE_USER_63,

    BIQUAD_FILTER_TYPE_MIN = BIQUAD_FILTER_TYPE_INHERIT,
    BIQUAD_FILTER_TYPE_MAX = BIQUAD_FILTER_TYPE_USER_MAX
};

// for thread
typedef void(*SoundFrameUserCallback)(uptr arg);

enum SrcType
{
    SRC_TYPE_NONE,
    SRC_TYPE_LINEAR,
    SRC_TYPE_4TAP
};

struct AdshrCurve
{
    u8 attack;
    u8 decay;
    u8 sustain;
    u8 hold;
    u8 release;

    AdshrCurve(u8 a = 0, u8 d = 0, u8 s = 0, u8 h = 0, u8 r = 0): 
        attack(a),
        decay(d),
        sustain(s),
        hold(h),
        release(r) 
    {
    }
};

namespace internal{

static const int WAVE_CHANNEL_MAX = 2;

struct DspAdpcmLoopParam
{
    ut::ResU16 loopPredScale;
    ut::ResU16 loopYn1;
    ut::ResU16 loopYn2;
};

struct WaveInfo
{
    SampleFormat sampleFormat;
    bool loopFlag;
    int channelCount;
    int sampleRate;
    u32 loopStartFrame;
    u32 loopEndFrame;

    struct ChannelParam
    {
        const void* dataAddress;
        DspAdpcmParam adpcmParam;
        DspAdpcmLoopParam adpcmLoopParam;
    } channelParam[WAVE_CHANNEL_MAX];
};

typedef nn::snd::CTR::MixParam MixParam;

} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_GLOBAL_H_
