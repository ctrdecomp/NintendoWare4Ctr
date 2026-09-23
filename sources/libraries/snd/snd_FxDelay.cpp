// Filename: snd_FxDelay.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_FxDelay.h>

namespace nw {
namespace snd {

FxDelay::FxDelay(): 
    m_pBuffer(NULL), 
    m_BufferSize(0), 
    m_CurFrame(0), 
    m_FeedbackGain(0x0),
    m_LpfCoef1(0x10000), 
    m_LpfCoef2(0x0), 
    m_ProcessChannelCount( 4 ),
    m_IsActive( false )
    {
    FreeBuffer();
    
    for (int ch = 0; ch < m_ProcessChannelCount; ch++)
    {
        m_WorkBuffer.m_Lpf[ch] = 0;
    }
}

FxDelay::~FxDelay()
{
    if (m_IsActive)
    {
        Finalize();
    }
    if (m_pBuffer != NULL)
    {
        ReleaseWorkBuffer();
    }
}

bool FxDelay::SetParam(const FxDelay::Param& param)
{
{
        if (param.m_Damping < 0.0f || param.m_Damping > 1.0f)
        {
            return false;
        }

        if (param.m_FeedbackGain < 0.0f || param.m_FeedbackGain > 1.0f)
        {
            return false;
        }
    }

    {
        if (m_IsActive == true)
        {
            if (param.m_DelayTime > m_DelayTimeAtInitialize)
            {
                return false;
            }
            if (m_IsEnableSurroundAtInitialize == false && param.m_IsEnableSurround == true)
            {
                return false;
            }
        }

        m_DelayFrames = (param.m_DelayTime * 1000 ) / NN_SND_USECS_PER_FRAME;
        if (m_DelayFrames == 0)
        {
            m_DelayFrames = 1;
        }

        if (param.m_IsEnableSurround == false)
        {
            m_ProcessChannelCount = 2;
        }
        else
        {
            m_ProcessChannelCount = 4;
        }

    }

    {
        m_FeedbackGain = static_cast<s32>(static_cast<s32>(0x80L) * param.m_FeedbackGain);

        f32 lpf_coef = param.m_Damping;
        if (lpf_coef > 0.95f)
        {
            lpf_coef = 0.95f;
        }
        f32 lpf_coef_1 = 1.f - lpf_coef;
        f32 lpf_coef_2 = lpf_coef;

        m_LpfCoef1 = static_cast<s32>(static_cast<s32>(0x80L) * lpf_coef_1);
        m_LpfCoef2 = static_cast<s32>(static_cast<s32>(0x80L) * lpf_coef_2);
    }

    m_Param = param;
    return true;
}

size_t FxDelay::GetRequiredMemSize()
{
    size_t result = (sizeof(s32) * NN_SND_SAMPLES_PER_FRAME * m_DelayFrames) * m_ProcessChannelCount;
    result += 32;
    return result;
}

bool FxDelay::AssignWorkBuffer(uptr buffer, size_t size)
{
    if (buffer == NULL)
    {
        return false;
    }

    m_pBuffer     = buffer;
    m_BufferSize = size;

    return true;
}

void FxDelay::ReleaseWorkBuffer()
{
    m_pBuffer = NULL;
}

bool FxDelay::Initialize()
{
    if (m_IsActive)
    {
        return false;
    }

    m_DelayTimeAtInitialize = m_Param.m_DelayTime;
    m_IsEnableSurroundAtInitialize = m_Param.m_IsEnableSurround;

    this->AllocBuffer();
    this->InitializeParam();

    m_IsActive = true;

    return true;
}

void FxDelay::Finalize()
{
    if (!m_IsActive)
    {
        return;
    }

    m_IsActive = false;
    ::std::memset(reinterpret_cast<void*>(&this->m_Param), 0, sizeof(Param));

    this->FreeBuffer();
}

void FxDelay::UpdateBuffer(int numChannels, nn::snd::CTR::AuxBusData* data, s32 sampleLength, nw::snd::SampleFormat format, f32 sampleRate, nw::snd::OutputMode mode)
{
    if (!m_IsActive)
    {
        return;
    }

    NW_NULL_ASSERT(data);
    NW_ASSERT(m_DelayFrames != 0);

    s32* input[CHANNEL_INDEX_NUM];
    input[CHANNEL_INDEX_FRONT_LEFT]  = data->frontLeft;
    input[CHANNEL_INDEX_FRONT_RIGHT] = data->frontRight;
    input[CHANNEL_INDEX_REAR_LEFT]   = data->rearLeft;
    input[CHANNEL_INDEX_REAR_RIGHT]  = data->rearRight;

    const u32 start_pos = NN_SND_SAMPLES_PER_FRAME * m_CurFrame;

    for (u32 ch = 0; ch < m_ProcessChannelCount; ch++)
    {
        u32 cur_pos = start_pos;

        s32* pInput = &input[ch][0];
        s32* pDelay = &m_WorkBuffer.m_Delay[ch][0] + start_pos;
        s32 lpfData = m_WorkBuffer.m_Lpf[ch];

        for (u32 samp = 0; samp < sampleLength; samp++)
        {
            s32 delay_out = pDelay[cur_pos];
            
            s32 feedback = delay_out;
            if (feedback < 0)
            {
                s32 tmp = -feedback;
                tmp = (tmp * m_FeedbackGain) >> 7;
                feedback = -tmp;
            }
            else
            {
                feedback = (feedback * m_FeedbackGain) >> 7;
            }

            feedback = *pInput - feedback;

            s32 lpf_out = m_LpfCoef1 * feedback + m_LpfCoef2 * lpfData;

            lpf_out >>= 7;

            lpfData = lpf_out;

            pDelay[cur_pos] = lpf_out;
            pInput[cur_pos] = delay_out;

            ++cur_pos;
        }
    }

    if (++m_CurFrame >= m_DelayFrames)
    {
        m_CurFrame = 0;
    }
}

void FxDelay::AllocBuffer()
{
    const size_t ch_buffer_size = sizeof(s32) * NN_SND_SAMPLES_PER_FRAME * m_DelayFrames;

    uptr ptr = math::RoundUp(m_pBuffer, 32 );

    for (int ch = 0; ch < m_ProcessChannelCount; ch++)
    {
        m_WorkBuffer.m_Delay[ch] = reinterpret_cast<s32*>(ptr);
        ptr += ch_buffer_size;
    }
}

void FxDelay::FreeBuffer()
{
    for (int ch = 0; ch < m_ProcessChannelCount; ch++)
    {
        m_WorkBuffer.m_Delay[ch] = NULL;
    }
}

void FxDelay::InitializeParam()
{
    m_CurFrame = 0;

    ::std::memset(reinterpret_cast<void*>(m_pBuffer), 0, m_BufferSize);

    for (int ch = 0; ch < m_ProcessChannelCount; ch++)
    {
        m_WorkBuffer.m_Lpf[ch] = 0;
    }
}
} // namespace snd
} // namespace nw