// Filename: snd_FxReverb.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_FxReverb.h>

namespace nw {
namespace snd {
namespace {

const f32 MSEC_PER_FRAME = NN_SND_USECS_PER_FRAME / 1000.f;
const int FX_SAMPLE_RATE = NN_SND_HW_I2S_CLOCK_32KHZ;   // 32728 Hz

inline f32 RoundUpToMsecPerFrame(u32 msec)
{
    return (msec > MSEC_PER_FRAME) ? msec : MSEC_PER_FRAME;
}

inline u32 ConvertMsecToSamples(f32 msec)
{
    return static_cast<u32>(msec / MSEC_PER_FRAME) * NN_SND_SAMPLES_PER_FRAME;
}
}

FxReverb::FilterSize FxReverb::s_DefaultFilterSize;

FxReverb::FxReverb(): 
    m_pBuffer(NULL),
    m_FilterSize(s_DefaultFilterSize),
    m_EarlyGain(0),
    m_FusedGain(0),
    m_LpfCoef1(0),
    m_LpfCoef2(0),
    m_IsActive(false)
    {

    m_EarlyLength = NN_SND_SAMPLES_PER_FRAME;
    m_EarlyPos = 0;

    m_PreDelayLength = NN_SND_SAMPLES_PER_FRAME;
    m_PreDelayPos = 0;

    for (int i = 0; i < 2; i++)
    {
        m_CombFilterLength[i] = NN_SND_SAMPLES_PER_FRAME;
        m_CombFilterPos[i] = 0;
        m_CombFilterCoef[i] = 0;
    }

    m_AllPassFilterLength = NN_SND_SAMPLES_PER_FRAME;
    m_AllPassFilterPos = 0;
    m_AllPassFilterCoef = 0;

    for (int ch = 0; ch < 4; ch++)
    {
        m_WorkBuffer.m_EarlyReflection[ch] = NULL;
        m_WorkBuffer.m_PreDelay[ch] = NULL;

        for (int i = 0; i < 2; i++)
        {
            m_WorkBuffer.m_CombFilter[ch][i] = NULL;
        }
        m_WorkBuffer.m_AllPassFilter[ch] = NULL;
        m_WorkBuffer.m_Lpf[ch] = 0;
        m_LastLpfOut[ch] = 0;
    }
}

FxReverb::~FxReverb()
{
    if (m_IsActive)
    {
        this->Finalize();
    }
    if (m_pBuffer != NULL)
    {
        this->ReleaseWorkBuffer();
    }
}

bool FxReverb::Initialize()
{
    m_EarlyReflectionTimeAtInitialize = m_Param.m_EarlyReflectionTime;
    m_PreDelayTimeAtInitialize = m_Param.m_PreDelayTime;
    m_FilterSizeAtInitialize = m_FilterSize;

    this->AllocBuffer();
    this->InitializeParam();
    m_IsActive = true;

    return true;
}

void FxReverb::Finalize()
{
    if (!m_IsActive)
    {
        return;
    }
    m_IsActive = false;
    FreeBuffer();
}

bool FxReverb::SetParam(const FxReverb::Param& param)
{
{
        if (param.m_Coloration < 0.0f || param.m_Coloration > 1.f)
        {
            return false;
        }

        if (param.m_Damping < 0.0f || param.m_Damping > 1.0)
        {
            return false;
        }

        if (param.m_EarlyGain < 0.0f || param.m_EarlyGain > 1.f)
        {
            return false;
        }

        if (param.m_FusedGain < 0.0f || param.m_FusedGain > 1.0f)
        {
            return false;
        }

        if (param.m_pFilterSize != NULL)
        {
            if (param.m_pFilterSize->m_Comb0 == 0 || param.m_pFilterSize->m_Comb1 == 0 || param.m_pFilterSize->m_AllPass == 0)
            {
                return false;
            }
        }
    }

    {
        if (m_IsActive == true)
        {
            if (param.m_EarlyReflectionTime > m_EarlyReflectionTimeAtInitialize)
            {
                return false;
            }

            if (param.m_PreDelayTime > m_PreDelayTimeAtInitialize)
            {
                return false;
            }

            if (param.m_pFilterSize != NULL)
            {
                if (param.m_pFilterSize->m_Comb0 > m_FilterSizeAtInitialize.m_Comb0)
                {
                    return false;
                }
                if (param.m_pFilterSize->m_Comb1 > m_FilterSizeAtInitialize.m_Comb1)
                {
                    return false;
                }
                if (param.m_pFilterSize->m_AllPass > m_FilterSizeAtInitialize.m_AllPass)
                {
                    return false;
                }
            }
        }

    }

    m_Param = param; 

    if (m_Param.m_pFilterSize != NULL)
    {
        m_FilterSize = *m_Param.m_pFilterSize;
        m_Param.m_pFilterSize = &m_FilterSize;
    }

    if (m_IsActive == true)
    {
        InitializeParam();
    }
    return true;
}

bool FxReverb::AssignWorkBuffer(uptr buffer, size_t size)
{
    if (buffer == NULL)
    {
        return false;
    }

    m_pBuffer = buffer;
    m_BufferSize = size;
    return true;
}

void FxReverb::ReleaseWorkBuffer()
{
    m_pBuffer = NULL;
}

size_t FxReverb::GetRequiredMemSize()
{
    const size_t bufSizeForEarlyReflection = sizeof(s32) * ConvertMsecToSamples(RoundUpToMsecPerFrame(m_Param.m_EarlyReflectionTime));
    const size_t bufSizeForPreDelay = sizeof(s32) * ConvertMsecToSamples(RoundUpToMsecPerFrame(m_Param.m_PreDelayTime));
    const size_t bufSizeForFilterComp0 = sizeof(s32) * m_FilterSize.m_Comb0;
    const size_t bufSizeForFilterComp1 = sizeof(s32) * m_FilterSize.m_Comb1;
    const size_t bufSizeForFilterAllPass = sizeof(s32) * m_FilterSize.m_AllPass;

    size_t result = (bufSizeForEarlyReflection + bufSizeForPreDelay + bufSizeForFilterComp0 + bufSizeForFilterComp1 + bufSizeForFilterAllPass ) * 2;

    result += 32;
    return result;
}

void FxReverb::AllocBuffer()
{
    const size_t bufSizeForEarlyReflection = sizeof(s32) * ConvertMsecToSamples(RoundUpToMsecPerFrame(m_Param.m_EarlyReflectionTime));
    const size_t bufSizeForPreDelay = sizeof(s32) * ConvertMsecToSamples(RoundUpToMsecPerFrame(m_Param.m_PreDelayTime));
    const size_t bufSizeForFilterComp0 = sizeof(s32) * m_FilterSize.m_Comb0;
    const size_t bufSizeForFilterComp1 = sizeof(s32) * m_FilterSize.m_Comb1;
    const size_t bufSizeForFilterAllPass = sizeof(s32) * m_FilterSize.m_AllPass;

    uptr ptr = math::RoundUp(m_pBuffer, 32);

    for (int ch = 0; ch < 2; ch++)
    {
        m_WorkBuffer.m_EarlyReflection[ch] = reinterpret_cast<s32*>(ptr);
        ptr += bufSizeForEarlyReflection;

        m_WorkBuffer.m_PreDelay[ch] = reinterpret_cast<s32*>(ptr);
        ptr += bufSizeForPreDelay;

        m_WorkBuffer.m_CombFilter[ch][0] = reinterpret_cast<s32*>(ptr);
        ptr += bufSizeForFilterComp0;

        m_WorkBuffer.m_CombFilter[ch][1] = reinterpret_cast<s32*>(ptr);
        ptr += bufSizeForFilterComp1;

        m_WorkBuffer.m_AllPassFilter[ch] = reinterpret_cast<s32*>(ptr);
        ptr += bufSizeForFilterAllPass;
    }
}

void FxReverb::FreeBuffer()
{
    for (int ch = 0; ch < 2; ch++)
    {
        m_WorkBuffer.m_EarlyReflection[ch] = NULL;
        m_WorkBuffer.m_PreDelay[ch]        = NULL;
        m_WorkBuffer.m_CombFilter[ch][0]   = NULL;
        m_WorkBuffer.m_CombFilter[ch][1]   = NULL;
        m_WorkBuffer.m_AllPassFilter[ch]   = NULL;
    }
}

void FxReverb::InitializeParam()
{
    f32 early_time = RoundUpToMsecPerFrame(m_Param.m_EarlyReflectionTime);
    m_EarlyLength = ConvertMsecToSamples(early_time);
    m_EarlyPos = 0;

    f32 pre_delay_time = RoundUpToMsecPerFrame(m_Param.m_PreDelayTime);
    m_PreDelayLength = ConvertMsecToSamples(pre_delay_time);
    m_PreDelayPos = 0;

    f32 fused_time_sec = static_cast<f32>(m_Param.m_FusedTime) / 1000.f;

    m_CombFilterLength[0] = static_cast<s32>(m_FilterSize.m_Comb0);
    m_CombFilterLength[1] = static_cast<s32>(m_FilterSize.m_Comb1);

    for (s32 i = 0; i < 2; i++)
    {
        m_CombFilterPos[i] = 0;

        f32 comb_coef = ::std::powf(10.f, (-3.f * static_cast<f32>(m_CombFilterLength[i]) / (fused_time_sec * FX_SAMPLE_RATE)));
        m_CombFilterCoef[i] = static_cast<s32>( static_cast<f32>(0x80L) * comb_coef );
    }

    m_AllPassFilterLength = static_cast<s32>(m_FilterSize.m_AllPass);
    m_AllPassFilterPos = 0;

    f32 all_pass_coef = m_Param.m_Coloration;
    m_AllPassFilterCoef = static_cast<s32>(static_cast<f32>(0x80L) * all_pass_coef);

    m_EarlyGain = static_cast<s32>(static_cast<f32>(0x80L) * m_Param.m_EarlyGain);
    m_FusedGain = static_cast<s32>(static_cast<f32>(0x80L) * m_Param.m_FusedGain);

    f32 lpf_coef = m_Param.m_Damping;
    if (lpf_coef > 0.95f) lpf_coef = 0.95f;

    if (m_Param.m_UseHpfDamping == true)
    {
        m_LpfCoef1 = static_cast<s32>(static_cast<s32>(0x80L) * (lpf_coef - 1.f ));
        m_LpfCoef2 = static_cast<s32>(static_cast<s32>(0x80L) * (-1.f) * lpf_coef);
    }
    else
    {
        m_LpfCoef1 = static_cast<s32>(static_cast<s32>(0x80L) * (1.f - lpf_coef));
        m_LpfCoef2 = static_cast<s32>(static_cast<s32>(0x80L) * lpf_coef);
    }

    ::std::memset(reinterpret_cast<void*>(m_pBuffer), 0, m_BufferSize);
}

void FxReverb::UpdateBuffer(int numChannels, nn::snd::CTR::AuxBusData* data, s32 sampleLength, SampleFormat format, f32 sampleRate, OutputMode mode)
{
    if (m_IsActive == false)
    {
        return;
    }
    NW_NULL_ASSERT(data);

    s32* input[nn::snd::CTR::CHANNEL_INDEX_NUM];
    input[nn::snd::CTR::CHANNEL_INDEX_FRONT_LEFT]  = data->frontLeft;
    input[nn::snd::CTR::CHANNEL_INDEX_FRONT_RIGHT] = data->frontRight;
    input[nn::snd::CTR::CHANNEL_INDEX_REAR_LEFT]   = data->rearLeft;
    input[nn::snd::CTR::CHANNEL_INDEX_REAR_RIGHT]  = data->rearRight;
    

    u32 early_pos           = 0;
    u32 pre_delay_pos       = 0;
    u32 comb_filter_pos0    = 0;
    u32 comb_filter_pos1    = 0;
    u32 allpass_filter_pos  = 0;

    for (int ch = 0; ch < m_ProcessChannelCount; ch++)
    {
        early_pos           = m_EarlyPos;
        pre_delay_pos       = m_PreDelayPos;
        comb_filter_pos0    = m_CombFilterPos[0];
        comb_filter_pos1    = m_CombFilterPos[1];
        allpass_filter_pos  = m_AllPassFilterPos;
        
        s32* early_reflection = m_WorkBuffer.m_EarlyReflection[ch];
        s32* pre_delay      = m_WorkBuffer.m_PreDelay[ch];
        
        for (s32 samp = 0; samp < sampleLength; samp++)
        {
            s32 indata = input[ch][samp];

            s32 early_out = early_reflection[early_pos] * m_EarlyGain;

            early_reflection[early_pos] = indata;

            s32 pre_delay_out = pre_delay[pre_delay_pos];
            pre_delay[pre_delay_pos] = indata;
            
            s32 filter_out = 0;

            s32* comb_line = m_WorkBuffer.m_CombFilter[ch][0];
            s32 out_tmp = comb_line[comb_filter_pos0];
            
            s32 comb_fb_0 = out_tmp;
            if (comb_fb_0 < 0)
            {
                s32 tmp = -comb_fb_0;
                tmp = ( tmp * m_CombFilterCoef[0] ) >> 7;
                comb_fb_0 = -tmp;
            }
            else
            {
                comb_fb_0 = ( comb_fb_0 * m_CombFilterCoef[0] ) >> 7;
            }
            
            comb_line[comb_filter_pos0] = pre_delay_out + comb_fb_0;
            filter_out += out_tmp;

            comb_line = m_WorkBuffer.m_CombFilter[ch][1];
            out_tmp = comb_line[comb_filter_pos1];
            
            s32 comb_fb_1 = out_tmp;
            if (comb_fb_1 < 0)
            {
                s32 tmp = -comb_fb_1;
                tmp = ( tmp * m_CombFilterCoef[1] ) >> 7;
                comb_fb_1 = -tmp;
            }
            else
            {
                comb_fb_1 = ( comb_fb_1 * m_CombFilterCoef[1] ) >> 7;
            }
            
            comb_line[comb_filter_pos1] = pre_delay_out + comb_fb_1;
            filter_out -= out_tmp;

            s32* allpass_line = m_WorkBuffer.m_AllPassFilter[ch];
            out_tmp = allpass_line[allpass_filter_pos];
            s32 allpass_coef = m_AllPassFilterCoef;
            
            s32 allpass_in = out_tmp;
            if (allpass_in < 0)
            {
                s32 tmp = -allpass_in;
                tmp = ( tmp * allpass_coef ) >> 7;
                allpass_in = -tmp;
            }
            else
            {
                allpass_in = ( allpass_in * allpass_coef ) >> 7;
            }
            allpass_in += filter_out;
            
            allpass_line[allpass_filter_pos] = allpass_in;
            
            s32 fo_2 = allpass_in;
            if (fo_2 < 0)
            {
                s32 tmp = -fo_2;
                tmp = ( tmp * allpass_coef ) >> 7;
                fo_2 = -tmp;
            }
            else
            {
                fo_2 = ( fo_2 * allpass_coef ) >> 7;
            }
            filter_out = out_tmp - fo_2;
            
            s32 tmp = m_LpfCoef2 * ( filter_out + m_LastLpfOut[ch] );
            tmp >>= 7;
            s32 fused_out = filter_out - tmp;
            m_LastLpfOut[ch] = fused_out;

            fused_out *= m_FusedGain;
            fused_out += early_out;
            fused_out >>= 7;
            input[ch][samp] = fused_out;
            
            ++early_pos;
            ++pre_delay_pos;
            ++comb_filter_pos0;
            ++comb_filter_pos1;
            ++allpass_filter_pos;
        }
    }

    if (early_pos >= m_EarlyLength)
    {
        m_EarlyPos = 0;
    }
    else
    {
        m_EarlyPos = early_pos;
    }
    
    if (pre_delay_pos >= m_PreDelayLength)
    {
        m_PreDelayPos = 0;
    }
    else
    {
        m_PreDelayPos = pre_delay_pos;
    }
    
    if (comb_filter_pos0 >= m_CombFilterLength[0])
    {
        m_CombFilterPos[0] = 0;
    }
    else
    {
        m_CombFilterPos[0] = comb_filter_pos0;
    }
    
    if (comb_filter_pos1 >= m_CombFilterLength[1])
    {
        m_CombFilterPos[1] = 0;
    }
    else
    {
        m_CombFilterPos[1] = comb_filter_pos1;
    }
    
    if (allpass_filter_pos >= m_AllPassFilterLength)
    {
        m_AllPassFilterPos = 0;
    }
    else
    {
        m_AllPassFilterPos = allpass_filter_pos;
    }
}

}
}