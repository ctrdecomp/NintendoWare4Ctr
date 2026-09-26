#ifndef NW_SND_UTIL_H_
#define NW_SND_UTIL_H_

#include <nw/types.h>
#include <nw/ut/ut_BinaryFileFormat.h>
#include <nw/snd/snd_ItemType.h>

namespace nw { 
namespace snd { 

class SoundArchive;
class SoundArchivePlayer;

namespace internal {

class SoundArchiveLoader;
class PlayerHeapDataManager;
struct LoadItemInfo;

class Util
{
public:
    static const int SEMITONE_MAX = 12;
    static const int MICROTONE_MAX = 256;

    static const int VOLUME_DB_MIN = -904;
    static const int VOLUME_DB_MAX = 60;

    static const int PITCH_DIVISION_BIT = 8;
    static const int VOLUME_MIN = static_cast<int>(10 * VOLUME_DB_MIN);
    static const int VOLUME_MAX = static_cast<int>(10 * VOLUME_DB_MAX); 

    static const void* GetWaveFile(u32 waveArchiveId, u32 waveIndex, const SoundArchive& arc, const SoundArchivePlayer& player);
    static const void* GetWaveFile(u32 waveArchiveId, u32 waveIndex, const SoundArchive& arc, const PlayerHeapDataManager* mgr);
    static bool IsLoadedWaveArchive(const void* wsdFile, u32 index, const SoundArchive& arc, const SoundArchiveLoader& mgr);
    static bool IsLoadedWaveArchive(const void* bankFile, const SoundArchive& arc, const SoundArchiveLoader& mgr);

    static bool IsDeviceMemory(uptr memory, size_t size)
    {
        const uptr head = nn::os::GetDeviceMemoryAddress();
        const uptr tail = head + nn::os::GetDeviceMemorySize();
        if (head <= memory && (memory + size) <= tail)
        {
            return true;
        }
        return false;
    }

    enum PanCurve
    {
        PAN_CURVE_SQRT,
        PAN_CURVE_SINCOS,
        PAN_CURVE_LINEAR
    };

    struct PanInfo
    {
        PanCurve curve;
        bool centerZeroFlag;
        bool zeroClampFlag;
        PanInfo(): 
            curve(PAN_CURVE_SQRT), 
            centerZeroFlag(false), 
            zeroClampFlag(false) 
        {
        }
    };

    static f32 CalcPitchRatio(int pitch);
    static f32 CalcVolumeRatio(f32 dB);
    static f32 CalcLpfFreq(f32 scale);
    static f32 CalcPanRatio(f32 pan, const PanInfo& info);
    static f32 CalcSurroundPanRatio(f32 surroundPan, const PanInfo& info);
    static u16 CalcRandom();

    static unsigned long GetSampleByByte(unsigned long byte, SampleFormat format);
    static unsigned long GetByteBySample(unsigned long sample, SampleFormat format);

    template< typename ITEM_TYPE, typename COUNT_TYPE=nw::ut::ResU32 >
    struct Table
    {
        COUNT_TYPE count;
        ITEM_TYPE item[1];
    };

    template <class Child>
    class Singleton
    {
    public:
        static Child& GetInstance()
        {
            static Child& instance;
            return instance;
        }
    };

    class AutoStopWatch
    {
    public:
        AutoStopWatch(nn::os::Tick& tick): 
            m_Tick(tick)
        {
            m_TmpTick = nn::os::Tick::GetSystemCurrent();
        }
        ~AutoStopWatch()
        {
            m_Tick = nn::os::Tick::GetSystemCurrent() - m_TmpTick;
        }
    private:
        nn::os::Tick& m_Tick;
        nn::os::Tick  m_TmpTick;
    };

    struct Reference
    {
        nw::ut::ResU16 typeId;
        u16 padding;
        nw::ut::ResS32 offset;

        static const s32 INVALID_OFFSET = -1;

        NW_INLINE bool IsValidTypeId(u16 validId) const
        {
            if (validId == typeId)
            {
                return true;
            }
            return false;
        }
    };
    
    struct ReferenceTable : public Table<Reference>
    {
        const void* GetReferedItem(u32 index) const
        {
            if (index >= count)
            {
                return NULL;
            }
            return ut::AddOffsetToPtr(this, item[index].offset);
        }

        const void* GetReferedItem(u32 index, u16 typeId) const
        {
            if (index >= count)
            {
                return NULL;
            }
            if (item[index].typeId != typeId)
            {
                return NULL;
            }
            return ut::AddOffsetToPtr(this, item[index].offset);
        }
    };

    struct ReferenceWithSize : public Reference
    {
        nw::ut::ResU32  size;
    };

    struct ReferenceWithSizeTable : public Table<ReferenceWithSize>
    {

    };

    struct BlockReferenceTable
    {
        ReferenceWithSize item[1];

        NW_INLINE const ReferenceWithSize* GetReference(u16 typeId, u16 count ) const
        {
            for (int i = 0; i < count; i++)
            {
                if (item[i].IsValidTypeId(typeId))
                {
                    return &item[i];
                }
            }
            return NULL;
        }

        NW_INLINE const void* GetReferedItem(const void* origin, u16 typeId, u16 count) const
        {
            const ReferenceWithSize* ref = GetReference(typeId, count);
            if (ref == NULL)
            {
                return NULL;
            }

            if (ref->offset == 0)
            {
                return NULL;
            }
            return ut::AddOffsetToPtr(origin, ref->offset);
        }
    };


    static NW_INLINE ItemType GetItemType(u32 id) { return static_cast<ItemType>(id >> 24); }
    static NW_INLINE u32 GetItemIndex(u32 id) { return id & 0x00ffffff; }
    static NW_INLINE u32 GetMaskedItemId(u32 id, internal::ItemType type) { return id | (static_cast<u32>(type) << 24); }
    
    class SoundFileHeader
    {
    public:
        nw::ut::BinaryFileHeader header;
        BlockReferenceTable blockReferenceTable;

        NW_INLINE const void* GetBlock(u16 typeId) const
        {
            return blockReferenceTable.GetReferedItem(this,typeId, header.dataBlocks);
        }
    };

    struct BitFlag
    {
        nw::ut::ResU32 bitFlag;

        bool GetValue(u32* value, u32 bitNumber) const
        {
            u32 count = GetTrueCount(bitNumber);

            if (count == 0)
            {
                return false;
            }

            *value = *reinterpret_cast<const nw::ut::ResU32*>(ut::AddOffsetToPtr(this, (count * sizeof(nw::ut::ResU32))));
            return true;
        }

        bool GetValueF32(f32* value, u32 bitNumber) const
        {
            u32 count = GetTrueCount(bitNumber);
            if (count == 0)
            {
                return false;
            }
            *value = *reinterpret_cast<const nw::ut::ResF32*>(ut::AddOffsetToPtr(this, (count * sizeof(nw::ut::ResF32))));
            return true;
        }

        static const int BIT_NUMBER_MAX = 31;
        NW_INLINE u32 GetTrueCount(u32 bitNumber) const
        {
            NW_ASSERT(bitNumber <= BIT_NUMBER_MAX);

            bool ret = false;
            int count = 0;
            for (u32 i = 0; i <= bitNumber; i++)
            {
                if (bitFlag & (0x1 << i))
                {
                    count++;
                    if (i == bitNumber)
                    {
                        ret = true;
                    }
                }
            }

            if (ret)
            {
                return count;
            }
            else
            {
                return 0;
            }
        }
    };

    static NW_INLINE u8 DevideBy8bit(u32 value, int index)
    {
        return static_cast<u8>((value >> (8*index)) & 0xff);
    }

    struct WaveId
    {
        ut::ResU32 waveArchiveId;
        ut::ResU32 waveIndex;
    };

    struct WaveIdTable
    {
        Table<WaveId> table;

        const WaveId* GetWaveId(u32 index) const
        {
            if (index >= table.count)
            {
                return NULL;
            }
            return &table.item[index];
        }
        
        u32 GetCount() const { return table.count; }
    };

    class PerfHistogram
    {
    public:
        PerfHistogram() 
        { 
            Reset(); 
        }

        void Reset()
        {
            m_MaxLoad = m_SumLoad = 0.0;
            m_Count = m_IrregularCount = 0;
            std::memset(m_LoadCount, 0, sizeof(m_LoadCount));
        }

        f32 GetAverage() const { return m_SumLoad / m_Count; }
        f32 GetMaxLoad() const { return m_MaxLoad; }
        int GetCount() const { return m_Count; }
        int GetIrregularCount() const { return m_IrregularCount; }

        void SetLoad(f32 load)
        {
            if (load < 0.0f || load > 100.0f)
            {
                m_IrregularCount += 1;
            }

            if (load > m_MaxLoad)
            {
                m_MaxLoad = load;
            }
            
            int loadIndex = static_cast<int>(load);
            if (loadIndex < LOAD_COUNT_NUM)
            {
                m_LoadCount[loadIndex] += 1;
            }
            m_Count += 1;
            m_SumLoad += load;
        }

    private:
        static int const LOAD_COUNT_NUM = 101;
        f32 m_MaxLoad;
        f32 m_SumLoad;
        int m_LoadCount[LOAD_COUNT_NUM];
        int m_Count, m_IrregularCount;
    };
private:
    static const int CALC_LPF_FREQ_TABLE_SIZE = 24;
    static const f32 CALC_LPF_FREQ_INTERCEPT;
    static const u16 CalcLpfFreqTable[CALC_LPF_FREQ_TABLE_SIZE];
};

} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_UTIL_H_
