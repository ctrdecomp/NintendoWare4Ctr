// Filename: anim_ResAnimCurve.cpp
//
// Project: NintendoWare4Ctr

#include <nw/anim/res/anim_ResAnimCurve.h>
#include <nw/anim/res/anim_ResAnim.h>
#include <nw/anim/res/anim_ResAnimGroup.h>

#include <cmath>
#include <nn/math/math_Arithmetic.h>

namespace nw { 
namespace anim {
namespace res {

namespace {

    template <typename T>
    class ResAnimTraits;

    template <>
    class ResAnimTraits<ResFloatKeyFV64Data>
    {
    public:
        typedef ResFloatKeyFV64Data KeyType;
        typedef f32                 FrameType;
        
        static const KeyType*   GetKey(const ResFloatSegmentFVData* pSegment, uint keyIdx)
        { 
            return &(pSegment->fv64.m_KeyValue[keyIdx]); 
        }
        
        static FrameType QuantizedFrame(const ResFloatSegmentFVData*, f32 frame) { return frame; }
        static FrameType GetFrame(const KeyType* pKey) { return pKey->GetFrame(); }
        static f32       GetFrameF32(const ResFloatSegmentFVData*, const KeyType* pKey) { return pKey->GetFrameF32(); }
        static f32       GetValue(const ResFloatSegmentFVData*, const KeyType* pKey) { return pKey->GetValue(); }
    };

    template <>
    class ResAnimTraits<ResFloatKeyFV32Data>
    {
    public:
        typedef ResFloatKeyFV32Data KeyType;
        typedef u32                 FrameType;
        
        static const KeyType*   GetKey(const ResFloatSegmentFVData* pSegment, uint keyIdx)
        { 
            return &(pSegment->fv32.m_KeyValue[keyIdx]); 
        }
        
        static FrameType QuantizedFrame(const ResFloatSegmentFVData* pSegment, f32 frame)
        {
            f32 decoded = frame / pSegment->fv32.m_FrameScale;
            return FrameType(internal::Round(decoded));
        }

        static FrameType GetFrame(const KeyType* pKey) { return pKey->GetFrame(); }
        static f32       GetFrameF32(const ResFloatSegmentFVData* pSegment, const KeyType* pKey)
        {
            f32 frame = pKey->GetFrameF32();
            return frame * pSegment->fv32.m_FrameScale;
        }

        static f32       GetValue(const ResFloatSegmentFVData* pSegment, const KeyType* pKey) 
        {
            f32 value = pKey->GetValue();
            return value * pSegment->fv32.m_Scale + pSegment->fv32.m_Offset; 
        }
    };

    template <>
    class ResAnimTraits<ResFloatKeyFVSS128Data>
    {
    public:
        typedef ResFloatKeyFVSS128Data KeyType;
        typedef f32                    FrameType;
        
        static const KeyType*   GetKey(const ResFloatSegmentFVData* pSegment, uint keyIdx)
        { 
            return &(pSegment->fvss128.m_KeyValue[keyIdx]); 
        }
        
        static FrameType QuantizedFrame(const ResFloatSegmentFVData*, f32 frame) { return frame; }
        static FrameType GetFrame(const KeyType* pKey) { return pKey->m_Frame; }
        static f32       GetFrameF32(const ResFloatSegmentFVData*, const KeyType* pKey) { return pKey->m_Frame; }
        static f32       GetValue(const ResFloatSegmentFVData*, const KeyType* pKey) { return pKey->m_Value; }
        static f32       GetInSlope(const KeyType* pKey) { return pKey->m_InSlope; }
        static f32       GetOutSlope(const KeyType* pKey) { return pKey->m_OutSlope; }
    };

    template <>
    class ResAnimTraits<ResFloatKeyFVSS64Data>
    {
    public:
        typedef ResFloatKeyFVSS64Data KeyType;
        typedef u32                   FrameType;
        
        static const KeyType*   GetKey(const ResFloatSegmentFVData* pSegment, uint keyIdx)
        { 
            return &(pSegment->fvss64.m_KeyValue[keyIdx]); 
        }
        
        static FrameType QuantizedFrame(const ResFloatSegmentFVData* pSegment, f32 frame)
        {
            f32 decoded = frame / pSegment->fvss64.m_FrameScale;
            return FrameType(internal::Round(decoded));
        }

        static FrameType GetFrame(const KeyType* pKey) { return pKey->GetFrame(); }
        static f32       GetFrameF32(const ResFloatSegmentFVData* pSegment, const KeyType* pKey)
        {
            f32 frame = pKey->GetFrameF32();
            return frame * pSegment->fvss64.m_FrameScale;
        }

        static f32       GetValue(const ResFloatSegmentFVData* pSegment, const KeyType* pKey) 
        {
            f32 value = pKey->GetValue();
            return value * pSegment->fvss64.m_Scale + pSegment->fvss64.m_Offset;
        }

        static f32       GetInSlope(const KeyType* pKey) { return pKey->GetInSlope(); }
        static f32       GetOutSlope(const KeyType* pKey) { return pKey->GetOutSlope(); }
    };

    template <>
    class ResAnimTraits<ResFloatKeyFVSS48Data>
    {
    public:
        typedef ResFloatKeyFVSS48Data KeyType;
        typedef u32                  FrameType;
        
        static const KeyType*   GetKey(const ResFloatSegmentFVData* pSegment, uint keyIdx)
        { 
            return &(pSegment->fvss48.m_KeyValue[keyIdx]); 
        }
        
        static FrameType QuantizedFrame(const ResFloatSegmentFVData* pSegment, f32 frame)
        {
            f32 decoded = frame / pSegment->fvss48.m_FrameScale;
            return FrameType(internal::Round(decoded));
        }

        static FrameType GetFrame(const KeyType* pKey) { return pKey->GetFrame(); }
        static f32       GetFrameF32(const ResFloatSegmentFVData* pSegment, const KeyType* pKey)
        {
            f32 frame = pKey->GetFrameF32();
            return frame * pSegment->fvss48.m_FrameScale;
        }

        static f32       GetValue(const ResFloatSegmentFVData* pSegment, const KeyType* pKey) 
        { 
            f32 value = pKey->GetValue();
            return value * pSegment->fvss48.m_Scale + pSegment->fvss48.m_Offset;
        }

        static f32       GetInSlope(const KeyType* pKey) { return pKey->GetInSlope(); }
        static f32       GetOutSlope(const KeyType* pKey) { return pKey->GetOutSlope(); }
    };


    template <>
    class ResAnimTraits<ResFloatKeyFVS96Data>
    {
    public:
        typedef ResFloatKeyFVS96Data KeyType;
        typedef f32                  FrameType;
        
        static const KeyType*   GetKey(const ResFloatSegmentFVData* pSegment, uint keyIdx)
        { 
            return &(pSegment->fvs96.m_KeyValue[keyIdx]); 
        }
        
        static FrameType QuantizedFrame(const ResFloatSegmentFVData*, f32 frame) { return frame; }
        static FrameType GetFrame(const KeyType* pKey) { return pKey->GetFrame(); }
        static f32       GetFrameF32(const ResFloatSegmentFVData*, const KeyType* pKey) { return pKey->GetFrameF32(); }
        static f32       GetValue(const ResFloatSegmentFVData*, const KeyType* pKey) { return pKey->GetValue(); }
        static f32       GetSlope(const KeyType* pKey) { return pKey->GetSlope(); }
    };

    template <>
    class ResAnimTraits<ResFloatKeyFVS48Data>
    {
    public:
        typedef ResFloatKeyFVS48Data  KeyType;
        typedef u32                   FrameType;
        
        static const KeyType*   GetKey(const ResFloatSegmentFVData* pSegment, uint keyIdx)
        { 
            return &(pSegment->fvs48.m_KeyValue[keyIdx]); 
        }
        
        static FrameType QuantizedFrame(const ResFloatSegmentFVData* pSegment, f32 frame)
        {
            f32 decoded = frame / pSegment->fvs48.m_FrameScale;
            return internal::CastF32ToS10_5(decoded);
        }

        static FrameType GetFrame(const KeyType* pKey) { return pKey->GetFrame(); }
        static f32       GetFrameF32(const ResFloatSegmentFVData* pSegment, const KeyType* pKey)
        {
            f32 frame = pKey->GetFrameF32();
            return frame * pSegment->fvs48.m_FrameScale;
        }

        static f32       GetValue(const ResFloatSegmentFVData* pSegment, const KeyType* pKey) 
        {
            f32 value = pKey->GetValue();
            return value * pSegment->fvs48.m_Scale + pSegment->fvs48.m_Offset;
        }

        static f32       GetSlope(const KeyType* pKey) { return pKey->GetSlope(); }
    };

    template <>
    class ResAnimTraits<ResFloatKeyFVS32Data>
    {
    public:
        typedef ResFloatKeyFVS32Data KeyType;
        typedef u32                  FrameType;
        
        static const KeyType*   GetKey(const ResFloatSegmentFVData* pSegment, uint keyIdx)
        { 
            return &(pSegment->fvs32.m_KeyValue[keyIdx]); 
        }
        
        static FrameType QuantizedFrame(const ResFloatSegmentFVData* pSegment, f32 frame)
        {
            f32 decoded = frame / pSegment->fvs32.m_FrameScale;
            return FrameType(internal::Round(decoded));
        }

        static FrameType GetFrame(const KeyType* pKey) { return pKey->GetFrame(); }
        static f32       GetFrameF32(const ResFloatSegmentFVData* pSegment, const KeyType* pKey)
        {
            f32 frame = pKey->GetFrameF32();
            return frame * pSegment->fvs32.m_FrameScale;
        }

        static f32       GetValue(const ResFloatSegmentFVData* pSegment, const KeyType* pKey) 
        { 
            f32 value = f32(pKey->m_ValueSlope[0] + ((pKey->m_ValueSlope[1] & 0x0F) << 8));
            return value * pSegment->fvs32.m_Scale + pSegment->fvs32.m_Offset;
        }

        static f32       GetSlope(const KeyType* pKey) { return pKey->GetSlope(); }
    };


    template <>
    class ResAnimTraits<ResIntKeyFV64Data>
    {
    public:
        typedef ResIntKeyFV64Data KeyType;
        typedef f32               FrameType;
        
        static const KeyType*   GetKey(const ResIntCurveFVData* pCurve, uint keyIdx)
        { 
            return &(pCurve->fv64.m_KeyValue[keyIdx]); 
        }
        
        static FrameType QuantizedFrame(const ResIntCurveFVData*, f32 frame) { return frame; }
        static FrameType GetFrame(const KeyType* pKey) { return pKey->GetFrame(); }
        static f32       GetFrameF32(const ResIntCurveFVData*, const KeyType* pKey) { return pKey->GetFrameF32(); }
        static s32       GetValue(const ResIntCurveFVData*, const KeyType* pKey) { return pKey->GetValue(); }
    };

    template <>
    class ResAnimTraits<ResIntKeyFV32Data>
    {
    public:
        typedef ResIntKeyFV32Data KeyType;
        typedef u32               FrameType;
        
        static const KeyType*   GetKey(const ResIntCurveFVData* pCurve, uint keyIdx)
        { 
            return &(pCurve->fv32.m_KeyValue[keyIdx]); 
        }
        
        static FrameType QuantizedFrame(const ResIntCurveFVData*, f32 frame) { return u32(frame); }
        static FrameType GetFrame(const KeyType* pKey) { return pKey->GetFrame(); }
        static f32       GetFrameF32(const ResIntCurveFVData*, const KeyType* pKey) { return pKey->GetFrameF32(); }
        static s32       GetValue(const ResIntCurveFVData* /*pCurve*/, const KeyType* pKey)
        { 
            return pKey->GetValue();
        }
    };


    template <>
    class ResAnimTraits<ResIntKeyFV16Data>
    {
    public:
        typedef ResIntKeyFV16Data KeyType;
        typedef u32               FrameType;
        
        static const KeyType* GetKey(const ResIntCurveFVData* pCurve, uint keyIdx)
        { 
            return &(pCurve->fv16.m_KeyValue[keyIdx]); 
        }
        
        static FrameType QuantizedFrame(const ResIntCurveFVData*, f32 frame) { return u32(frame); }
        static FrameType GetFrame(const KeyType* pKey) { return u32(pKey->m_Frame); }
        static f32       GetFrameF32(const ResIntCurveFVData*, const KeyType* pKey) { return f32(GetFrame(pKey)); }
        static s32       GetValue(const ResIntCurveFVData* /*pCurve*/, const KeyType* pKey) 
        {
            return s32(pKey->m_Value);
        }
    };

    typedef f32 (*NormalizeFrameFunc)( f32 frame, f32 startFrame, f32 endFrame );
    
    f32 NormalizeFrameNonePre_(f32 frame, f32 startFrame, f32 /* endFrame */)
    {
        return (frame < startFrame) ? startFrame : frame;
    }
    
    f32 NormalizeFrameNonePost_(f32 frame, f32 /*startFrame*/, f32 endFrame)
    {
        return (frame > endFrame) ? endFrame : frame;
    }

    f32 NormalizeFrameRepeatPre_(f32 frame, f32 startFrame, f32 endFrame)
    {
        f32 duration = endFrame - startFrame;
        
        s32 cnt = static_cast<s32>(std::floor((frame - startFrame) / duration));
        frame -= cnt * duration;
        
        return frame;
    }

    f32 NormalizeFrameRepeatPost_(f32 frame, f32 startFrame, f32 endFrame)
    {
        f32 duration = endFrame - startFrame;
        
        s32 cnt = static_cast<s32>(std::floor((frame - startFrame) / duration));
        frame -= cnt * duration;
        
        return frame;
    }

    f32 NormalizeFrameMirrorPre_(f32 frame, f32 startFrame, f32 endFrame)
    {
        bool needsReverse = false;
        f32 duration = endFrame - startFrame;
        
        s32 cnt = static_cast<s32>(std::floor((frame - startFrame) / duration));
        frame -= cnt * duration;
        needsReverse = (cnt & 1)? true : false;
        
        return needsReverse ? startFrame + endFrame - frame : frame;
    }

    f32 NormalizeFrameMirrorPost_(f32 frame, f32 startFrame, f32 endFrame)
    {
        bool needsReverse = false;
        f32 duration = endFrame - startFrame;

        s32 cnt = static_cast<s32>(std::floor((frame - startFrame) / duration));
        frame -= cnt * duration;
        needsReverse = (cnt & 1)? true : false;
        
        return needsReverse ? startFrame + endFrame - frame : frame;
    }

    f32 NormalizeFrame_(f32 frame, const ResAnimCurveData* pCurve)
    {
        static const NormalizeFrameFunc preRepeatMethod[] = 
        {
            NormalizeFrameNonePre_,
            NormalizeFrameRepeatPre_,
            NormalizeFrameMirrorPre_,
        };
        
        static const NormalizeFrameFunc postRepeatMethod[] =
        {
            NormalizeFrameNonePost_,
            NormalizeFrameRepeatPost_,
            NormalizeFrameMirrorPost_,
        };
        
        NW_ASSERT(pCurve->m_InRepeatMethod < ResAnimCurveData::METHOD_NUM);
        NW_ASSERT(pCurve->m_OutRepeatMethod < ResAnimCurveData::METHOD_NUM);
        
        if (frame < pCurve->m_StartFrame)
        {
            return preRepeatMethod[pCurve->m_InRepeatMethod](frame, pCurve->m_StartFrame, pCurve->m_EndFrame);
        }

        if (frame >= pCurve->m_EndFrame)
        {
            return postRepeatMethod[pCurve->m_OutRepeatMethod](frame, pCurve->m_StartFrame, pCurve->m_EndFrame);
        }

        return frame;
    }

    template <typename Traits, typename Segment>
    const typename Traits::KeyType* GetKeyFV_(const Segment* pSegment, f32 frame)
    {
        typename Traits::FrameType quantizedFrame = Traits::QuantizedFrame(pSegment, frame);

        const typename Traits::KeyType* pFirstKey = Traits::GetKey(pSegment, 0);
        if (quantizedFrame < Traits::GetFrame(pFirstKey))
        {
            return pFirstKey;
        }

        const typename Traits::KeyType* pLastKey = Traits::GetKey(pSegment, pSegment->m_NumFrameValues - 1);
        if (Traits::GetFrame(pLastKey) <= quantizedFrame)
        {
            return pLastKey;
        }

        uint keyIdx = static_cast<uint>(frame * pSegment->m_InvDuration * (pSegment->m_NumFrameValues - 1));
        NW_ASSERT(keyIdx <= pSegment->m_NumFrameValues - 1U);
        
        const typename Traits::KeyType* pKey = Traits::GetKey(pSegment, keyIdx);

        if (quantizedFrame < Traits::GetFrame(pKey))
        {
            do
            {
                NW_ASSERT(Traits::GetKey(pSegment, 0) < pKey);
                --pKey;
            } while (quantizedFrame < Traits::GetFrame(pKey));
        }
        else
        {
            do
            {
                NW_ASSERT(pKey < Traits::GetKey(pSegment, pSegment->m_NumFrameValues - 1));
                ++pKey;
            } while (Traits::GetFrame(pKey) <= quantizedFrame);

            NW_ASSERT(Traits::GetKey(pSegment, 0) < pKey);
            --pKey;
        }
        
        return pKey;
    }

    NW_INLINE const ResFloatSegmentData*
    GetFloatSegment_(const ResSegmentFloatCurveData* pCurve, f32 frame)
    {
        NW_NULL_ASSERT(pCurve);
        NW_ASSERT(pCurve->m_StartFrame <= frame && frame <= pCurve->m_EndFrame);

        if (pCurve->m_Flags & ResSegmentFloatCurveData::FLAG_MONO_SEGMENT)
        {
            return reinterpret_cast<const ResFloatSegmentData*>(pCurve->segmentsTable.toSegments[0].to_ptr());
        }
        
        s32 segmentCount = pCurve->segmentsTable.m_NumSegments;
        
        NW_ASSERT(segmentCount > 0);
        
        const ut::Offset* pOffsetTable = &(pCurve->segmentsTable.toSegments[0]);
        
        for (const ut::Offset* pOffset = pOffsetTable; pOffset < pOffsetTable + segmentCount; ++pOffset)
        {
            const ResFloatSegmentData* pSegment = reinterpret_cast<const ResFloatSegmentData*>( pOffset->to_ptr() );
            
            if (pSegment->m_EndFrame > frame)
            {
                return pSegment;
            }
        }
        
        return reinterpret_cast<const ResFloatSegmentData*>( pOffsetTable[ segmentCount - 1 ].to_ptr() );
    }

    template <typename Traits>
    const typename Traits::KeyType* GetFloatKeyFV_(const ResFloatSegmentFVData* pSegment, f32 frame)
    {
        return GetKeyFV_<Traits, ResFloatSegmentFVData>(pSegment, frame);
    }


    template <typename Traits>
    f32 CalcStepFloatSegmentFV_(const ResFloatSegmentFVData* pSegment, f32 frame)
    {
        const typename Traits::KeyType* pKey = GetFloatKeyFV_<Traits>(pSegment, frame);
        
        return Traits::GetValue(pSegment, pKey);
    }


    template <typename Traits>
    f32 CalcLinearFloatSegmentFV_(const ResFloatSegmentFVData* pSegment, f32 frame)
    {
        const typename Traits::KeyType* pKey = GetFloatKeyFV_<Traits>(pSegment, frame);
        const typename Traits::KeyType* pLastKey = Traits::GetKey(pSegment, pSegment->m_NumFrameValues - 1);
        
        f32 keyFrame = Traits::GetFrameF32( pSegment, pKey );

        if (keyFrame == frame || pKey == pLastKey) 
        {
            return Traits::GetValue(pSegment, pKey);
        }
        
        const typename Traits::KeyType* pNextKey = pKey + 1;
        
        f32 nextKeyFrame = Traits::GetFrameF32(pSegment, pNextKey);
        f32 rate = (frame - keyFrame) / (nextKeyFrame - keyFrame);
        
        return Traits::GetValue(pSegment, pKey) * (1.0f - rate)
            + Traits::GetValue(pSegment, pNextKey) * rate;
    }

    template <typename Traits>
    f32 CalcHermiteFloatSegmentFVSS_(const ResFloatSegmentFVData* pSegment, f32 frame)
    {
        const typename Traits::KeyType* pKey = GetFloatKeyFV_<Traits>( pSegment, frame);
        const typename Traits::KeyType* pLastKey = Traits::GetKey(pSegment, pSegment->m_NumFrameValues - 1);
        
        f32 keyFrame = Traits::GetFrameF32(pSegment, pKey);
        
        if (keyFrame == frame || pKey == pLastKey) 
        {
            return Traits::GetValue(pSegment, pKey);
        }
        
        const typename Traits::KeyType* pNextKey = pKey + 1;
        
        f32 p  = frame - keyFrame;
        f32 d  = Traits::GetFrameF32(pSegment, pNextKey) - keyFrame;
        f32 v0 = Traits::GetValue(pSegment, pKey);
        f32 v1 = Traits::GetValue(pSegment, pNextKey);
        f32 t0 = Traits::GetOutSlope(pKey);
        f32 t1 = Traits::GetInSlope(pNextKey);
        
        return nw::math::Hermite( v0, t0, v1, t1, p, d );
    }

    template <typename Traits>
    f32 CalcHermiteFloatSegmentFVS_(const ResFloatSegmentFVData* pSegment, f32 frame)
    {
        const typename Traits::KeyType* pKey = GetFloatKeyFV_<Traits>( pSegment, frame);
        const typename Traits::KeyType* pLastKey = Traits::GetKey(pSegment, pSegment->m_NumFrameValues - 1);
        
        f32 keyFrame = Traits::GetFrameF32(pSegment, pKey);
        
        if (keyFrame == frame || pKey == pLastKey) 
        {
            return Traits::GetValue(pSegment, pKey);
        }
        
        const typename Traits::KeyType* pNextKey = pKey + 1;
        
        f32 p  = frame - keyFrame;
        f32 d  = Traits::GetFrameF32(pSegment, pNextKey) - keyFrame;
        f32 v0 = Traits::GetValue(pSegment, pKey);
        f32 v1 = Traits::GetValue(pSegment, pNextKey);
        f32 t0 = Traits::GetSlope(pKey);
        f32 t1 = Traits::GetSlope(pNextKey);
        
        return nw::math::Hermite(v0, t0, v1, t1, p, d);
    }

    typedef f32 (*CalcFloatSegmentFVFunc)(const ResFloatSegmentFVData* pSegment, f32 frame);

    static CalcFloatSegmentFVFunc s_CalcFloatSegmentFVTable[][8] = 
    {
        {
            CalcStepFloatSegmentFV_<ResAnimTraits<ResFloatKeyFVSS128Data>>,
            CalcStepFloatSegmentFV_<ResAnimTraits<ResFloatKeyFVSS64Data>>,
            CalcStepFloatSegmentFV_<ResAnimTraits<ResFloatKeyFVSS48Data>>,
            CalcStepFloatSegmentFV_<ResAnimTraits<ResFloatKeyFVS96Data>>,
            CalcStepFloatSegmentFV_<ResAnimTraits<ResFloatKeyFVS48Data>>,
            CalcStepFloatSegmentFV_<ResAnimTraits<ResFloatKeyFVS32Data>>,
            CalcStepFloatSegmentFV_<ResAnimTraits<ResFloatKeyFV64Data>>,
            CalcStepFloatSegmentFV_<ResAnimTraits<ResFloatKeyFV32Data>>,
        },
        {
            CalcLinearFloatSegmentFV_< ResAnimTraits<ResFloatKeyFVSS128Data> >,
            CalcLinearFloatSegmentFV_< ResAnimTraits<ResFloatKeyFVSS64Data> >,
            CalcLinearFloatSegmentFV_< ResAnimTraits<ResFloatKeyFVSS48Data> >,
            CalcLinearFloatSegmentFV_< ResAnimTraits<ResFloatKeyFVS96Data> >,
            CalcLinearFloatSegmentFV_< ResAnimTraits<ResFloatKeyFVS48Data> >,
            CalcLinearFloatSegmentFV_< ResAnimTraits<ResFloatKeyFVS32Data> >,
            CalcLinearFloatSegmentFV_< ResAnimTraits<ResFloatKeyFV64Data> >,
            CalcLinearFloatSegmentFV_< ResAnimTraits<ResFloatKeyFV32Data> >,
        },
        {
            CalcHermiteFloatSegmentFVSS_<ResAnimTraits<ResFloatKeyFVSS128Data>>,
            CalcHermiteFloatSegmentFVSS_<ResAnimTraits<ResFloatKeyFVSS64Data>>,
            CalcHermiteFloatSegmentFVSS_<ResAnimTraits<ResFloatKeyFVSS48Data>>,
            CalcHermiteFloatSegmentFVS_<ResAnimTraits<ResFloatKeyFVS96Data>>,
            CalcHermiteFloatSegmentFVS_<ResAnimTraits<ResFloatKeyFVS48Data>>,
            CalcHermiteFloatSegmentFVS_<ResAnimTraits<ResFloatKeyFVS32Data>>,
            NULL,
            NULL,
        }
    };


    template <typename Traits>
    const typename Traits::KeyType* GetIntKeyFV_(const ResIntCurveFVData* pCurve, f32 frame)
    {
        return GetKeyFV_<Traits, ResIntCurveFVData>(pCurve, frame);
    }


    template <typename Traits>
    s32 CalcIntCurveFV_(const ResIntCurveFVData* pCurve, f32 frame)
    {
        const typename Traits::KeyType* pKey = GetIntKeyFV_<Traits>(pCurve, frame);
        
        return Traits::GetValue(pCurve, pKey);
    }

    typedef s32 (*CalcIntCurveFVFunc)(const ResIntCurveFVData* pCurve, f32 frame);

    static CalcIntCurveFVFunc s_CalcIntCurveFVTable[] = 
    {
        CalcIntCurveFV_<ResAnimTraits<ResIntKeyFV64Data>>,
        CalcIntCurveFV_<ResAnimTraits<ResIntKeyFV32Data>>,
        CalcIntCurveFV_<ResAnimTraits<ResIntKeyFV16Data>>,
    };


    NW_INLINE f32 CalcFloatSegment_(const ResFloatSegmentData* pSegment, f32 frame)
    {
        NW_NULL_ASSERT(pSegment);
        NW_ASSERT(pSegment->m_StartFrame <= frame && frame <= pSegment->m_EndFrame);
        NW_ASSERT(!(pSegment->m_Flags & ResFloatSegmentData::FLAG_BAKED));
        
        if (pSegment->m_Flags & ResFloatSegmentData::FLAG_CONSTANT)
        {
            return pSegment->constantValue;
        }
        
        u32 quantizeType = (pSegment->m_Flags & ResFloatSegmentData::FLAG_QUANTIZATION_TYPE_MASK) >>  
            ResFloatSegmentData::FLAG_QUANTIZATION_TYPE_SHIFT;

        u32 interporateMode = (pSegment->m_Flags & ResFloatSegmentData::FLAG_INTERPORATE_MODE_MASK) >>
            ResFloatSegmentData::FLAG_INTERPORATE_MODE_SHIFT;
            
        return s_CalcFloatSegmentFVTable[interporateMode][quantizeType](&(pSegment->fv), frame - pSegment->m_StartFrame);
    }


    NW_INLINE f32 CalcSegmentFloatCurve_(const ResSegmentFloatCurveData* pCurve, f32 frame)
    {
        NW_NULL_ASSERT(pCurve);
        NW_ASSERT(pCurve->m_StartFrame <= frame && frame <= pCurve->m_EndFrame);
        
        if ( pCurve->m_Flags & ResSegmentFloatCurveData::FLAG_CONSTANT )
        {
            return pCurve->m_ConstantValue;
        }
        
        const ResFloatSegmentData* pSegment = GetFloatSegment_( pCurve, frame );

        if ( frame < pSegment->m_StartFrame )
        {
            frame = pSegment->m_StartFrame;
        }
        
        return CalcFloatSegment_(pSegment, frame);
    }


    NW_INLINE f32 CalcCompositeFloatCurve_(const ResCompositeFloatCurveData* pCurve, f32 frame)
    {
        const ResFloatCurveData* pLeftCurve = 
            reinterpret_cast<const ResFloatCurveData*>(pCurve->toLeftCurve.to_ptr());
        const ResFloatCurveData* pRightCurve = 
            reinterpret_cast<const ResFloatCurveData*>(pCurve->toRightCurve.to_ptr());
        
        f32 leftValue  = CalcFloatCurve(pLeftCurve, frame);
        f32 rightValue = CalcFloatCurve(pRightCurve, frame);
        
        return leftValue + rightValue;
    }



    bool CalcBoolCurveCV_(const ResBoolCurveData* pCurve, f32 frame)
    {
        NW_ASSERT(pCurve->m_StartFrame <= frame && frame <= pCurve->m_EndFrame);

        float frameOffset = frame - pCurve->m_StartFrame;

        if (frame == pCurve->m_EndFrame)
        {
            frameOffset = math::FCeil(frameOffset);
        }

        u32 index = u32(frameOffset) / 8;
        u32 shift = u32(frameOffset) % 8;
        
        return ((pCurve->cv.m_KeyValue[index] >> shift) & 0x1) != 0;
    }


    void CalcVector3CurveCV_(math::VEC3* result, bit32* flags, const ResVector3CurveData* pCurve, f32 frame)
    {
        u32 index = u32(frame);
        f32 remainder = frame - index;
        const ResVector3CurveData::FrameValue& frameValue = pCurve->frames.m_KeyValue[index];

        if (remainder == 0)
        {
            *result = frameValue.cv;
            *flags |= frameValue.flag;
        }
        else if (frame == pCurve->m_EndFrame)
        {
            u32 lastIndex = index + 1; 

            const ResVector3CurveData::FrameValue& lastFrameValue = pCurve->frames.m_KeyValue[lastIndex];
            *result = lastFrameValue.cv;
            *flags |= lastFrameValue.flag;
        }
        else
        {
            f32 nextFrame = NormalizeFrame_(frame + 1.0f, pCurve);
            u32 nextIndex = u32(nextFrame);
            const ResVector3CurveData::FrameValue& nextFrameValue = pCurve->frames.m_KeyValue[nextIndex];

            VEC3Lerp(
                result,
                static_cast<const math::VEC3*>(&frameValue.cv),
                static_cast<const math::VEC3*>(&nextFrameValue.cv),
                remainder);

            *flags |= (frameValue.flag & nextFrameValue.flag);
        }
    }

    void CalcRotateCurveCV_(math::MTX34* result, bit32* flags, const ResVector4CurveData* pCurve, f32 frame)
    {
        u32 index = u32(frame);
        f32 remainder = frame - index;
        const ResVector4CurveData::FrameValue& frameValue = pCurve->frames.m_KeyValue[index];

        math::MTX34 mtx;

        if (remainder == 0)
        {
            math::QUAT quaternion(frameValue.cv);

            math::QUATToMTX34(&mtx, &quaternion);
            
            *flags |= frameValue.flag;
        }
        else if (frame == pCurve->m_EndFrame)
        {
            u32 lastIndex = index + 1; 

            const ResVector4CurveData::FrameValue& lastFrameValue = pCurve->frames.m_KeyValue[lastIndex];
            math::QUAT quaternion(lastFrameValue.cv);
            math::QUATToMTX34(&mtx, &quaternion);
            *flags |= lastFrameValue.flag;
        }
        else
        {
            f32 nextFrame = NormalizeFrame_(frame + 1.0f, pCurve);
            u32 nextIndex = u32(nextFrame);
            const ResVector4CurveData::FrameValue& nextFrameValue = pCurve->frames.m_KeyValue[nextIndex];

            math::QUAT lerpResult;
            math::QUAT q1(frameValue.cv);
            math::QUAT q2(nextFrameValue.cv);

            math::QUATLerp(&lerpResult, &q1, &q2, remainder);
            math::QUATToMTX34(&mtx, &lerpResult);

            *flags |= (frameValue.flag & nextFrameValue.flag);
        }

        result->f._00 = mtx.f._00;
        result->f._01 = mtx.f._01;
        result->f._02 = mtx.f._02;
        result->f._10 = mtx.f._10;
        result->f._11 = mtx.f._11;
        result->f._12 = mtx.f._12;
        result->f._20 = mtx.f._20;
        result->f._21 = mtx.f._21;
        result->f._22 = mtx.f._22;
    }

    
    void CalcTranslateCurveCV_(math::MTX34* result, bit32* flags, const ResVector3CurveData* pCurve, f32 frame)
    {
        u32 index = u32(frame);
        f32 remainder = frame - index;
        const ResVector3CurveData::FrameValue& frameValue = pCurve->frames.m_KeyValue[index];

        if (remainder == 0)
        {
            result->f._03 = frameValue.cv.x;
            result->f._13 = frameValue.cv.y;
            result->f._23 = frameValue.cv.z;

            *flags |= frameValue.flag;
        }
        else if (frame == pCurve->m_EndFrame)
        {
            u32 lastIndex = index + 1; 

            const ResVector3CurveData::FrameValue& lastFrameValue = pCurve->frames.m_KeyValue[lastIndex];
            result->f._03 = lastFrameValue.cv.x;
            result->f._13 = lastFrameValue.cv.y;
            result->f._23 = lastFrameValue.cv.z;
            *flags |= lastFrameValue.flag;
        }
        else
        {
            f32 nextFrame = NormalizeFrame_(frame + 1.0f, pCurve);
            u32 nextIndex = u32(nextFrame);
            const ResVector3CurveData::FrameValue& nextFrameValue = pCurve->frames.m_KeyValue[nextIndex];

            math::VEC3 v;
            VEC3Lerp(&v,
                static_cast<const math::VEC3*>(&frameValue.cv),
                static_cast<const math::VEC3*>(&nextFrameValue.cv),
                remainder);

            result->f._03 = v.x;
            result->f._13 = v.y;
            result->f._23 = v.z;

            *flags |= (frameValue.flag & nextFrameValue.flag);
        }
    }
} // namespace

f32 CalcFloatCurve(const ResFloatCurveData* pCurve, f32 frame)
{
    NW_NULL_ASSERT(pCurve);

    frame = NormalizeFrame_(frame, pCurve);
    
    if (pCurve->m_Flags & ResFloatCurveData::FLAG_COMPOSITE_CURVE)
    {
        return CalcCompositeFloatCurve_(reinterpret_cast<const ResCompositeFloatCurveData*>(pCurve), frame);
    }
    else
    {
        return CalcSegmentFloatCurve_(reinterpret_cast<const ResSegmentFloatCurveData*>(pCurve), frame);
    }
}


s32 CalcIntCurve(const ResIntCurveData* pCurve, f32 frame)
{
    NW_NULL_ASSERT(pCurve);
    NW_ASSERT(!(pCurve->m_Flags & ResIntCurveData::FLAG_BAKED));
    
    frame = NormalizeFrame_(frame, pCurve);
    
    if (pCurve->m_Flags & ResIntCurveData::FLAG_CONSTANT)
    {
        return pCurve->constantValue;
    }
    
    u32 quantizedType = (pCurve->m_Flags & ResIntCurveData::FLAG_QUANTIZATION_TYPE_MASK) >> 
            ResIntCurveData::FLAG_QUANTIZATION_TYPE_SHIFT;
    
    return s_CalcIntCurveFVTable[quantizedType](&(pCurve->fv), frame);
}


bool CalcBoolCurve(const ResBoolCurveData* pCurve, f32 frame)
{
    NW_NULL_ASSERT(pCurve);
    
    frame = NormalizeFrame_(frame, pCurve);
    
    if (pCurve->m_Flags & ResBoolCurveData::FLAG_CONSTANT)
    {
        return ((pCurve->m_Flags & ResBoolCurveData::FLAG_CONSTANT_VALUE) != 0);
    }
    
    if (pCurve->m_Flags & ResBoolCurveData::FLAG_BAKED)
    {
        return CalcBoolCurveCV_(pCurve, frame);
    }
    else
    {
        u32 quantizedType = (pCurve->m_Flags & ResBoolCurveData::FLAG_QUANTIZATION_TYPE_MASK) >> 
                ResBoolCurveData::FLAG_QUANTIZATION_TYPE_SHIFT;

        return  (s_CalcIntCurveFVTable[quantizedType](&(pCurve->fv), frame) != 0);
    }
}


void CalcVector3Curve(math::VEC3* result, bit32* flags, const ResVector3CurveData* pCurve, f32 frame)
{
    NW_NULL_ASSERT(pCurve);

    if (pCurve->m_Flags & ResVector3CurveData::FLAG_CONSTANT)
    {
        frame = pCurve->m_StartFrame;
    }
    else
    {
        frame = NormalizeFrame_(frame, pCurve);
    }

    CalcVector3CurveCV_(result, flags, pCurve, frame);
}

void CalcRotateCurve(math::MTX34* result, bit32* flags, const ResVector4CurveData* pCurve, f32 frame)
{
    NW_NULL_ASSERT(pCurve);

    frame = NormalizeFrame_(frame, pCurve);

    if (pCurve->m_Flags & ResVector4CurveData::FLAG_CONSTANT)
    {
        frame = pCurve->m_StartFrame;
    }

    CalcRotateCurveCV_(result, flags, pCurve, frame);
}

void CalcTranslateCurve(math::MTX34* result, bit32* flags, const ResVector3CurveData* pCurve, f32 frame)
{
    NW_NULL_ASSERT(pCurve);

    if (pCurve->m_Flags & ResVector3CurveData::FLAG_CONSTANT)
    {
        frame = pCurve->m_StartFrame;
    }
    else
    {
        frame = NormalizeFrame_(frame, pCurve);
    }

    CalcTranslateCurveCV_(result, flags, pCurve, frame);
}


void CalcTransformCurve(math::MTX34* result, const ResFullBakedCurveData* pCurve, f32 frame)
{
    NW_NULL_ASSERT(pCurve);

    frame = NormalizeFrame_(frame, pCurve);
    u32 index = u32(frame);

    if (frame == pCurve->m_EndFrame)
    {
        index += 1;
    }

    MTX34Copy(result, static_cast<const math::MTX34*>(&pCurve->frames.m_KeyValue[index]));
}

} /* namespace res */
} /* namespace anim */
} /* namespace nw */