#ifndef NW_SND_UTIL_H_
#define NW_SND_UTIL_H_

#include <nw/types.h>
#include <nw/ut/ut_BinaryFileFormat.h>
#include <nw/snd/snd_ItemType.h>

namespace nw { 
namespace snd { 
namespace internal {

class Util
{
public:
    static const void* GetWaveFile(u32 waveArchiveId, u32 waveIndex, const SoundArchive& arc, const SoundArchivePlayer& player);
    static const void* GetWaveFile(u32 waveArchiveId, u32 waveIndex, const SoundArchive& arc, const PlayerHeapDataManager* mgr);

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

    struct Reference
    {
        nw::ut::ResU16  typeId;
        u16             padding;
        nw::ut::ResS32  offset;
    };
    
    struct ReferenceTable : public Table<Reference>
    {

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
    };


    static NW_INLINE ItemType GetItemType(u32 id){ return static_cast<ItemType>(id >> 24); }
    static NW_INLINE u32 GetItemIndex(u32 id){ return id & 0x00ffffff; }
    static inline u32 GetMaskedItemId(u32 id, internal::ItemType type){ return id | (static_cast<u32>(type) << 24); }
    
    class SoundFileHeader
    {
    public:
        nw::ut::BinaryFileHeader header;
        BlockReferenceTable blockReferenceTable;
    };

    struct BitFlag
    {
        nw::ut::ResU32  bitFlag;
    };

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
        PerfHistogram(){ Initialize(); }

        // Created so ct works
        void Initialize()
        {
            m_MaxLoad = m_SumLoad = 0.0;
            m_Count = m_IrregularCount = 0;
            std::memset(m_LoadCount, 0, sizeof(m_LoadCount));
        }

        f32 GetAverage() const { return m_SumLoad / m_Count; }
        f32 GetMaxLoad() const { return m_MaxLoad; }
        int GetCount() const { return m_Count; }
        int GetIrregularCount() const { return m_IrregularCount; }

    private:
        static int const LOAD_COUNT_NUM = 101;
        f32 m_MaxLoad;
        f32 m_SumLoad;
        int m_LoadCount[LOAD_COUNT_NUM];
        int m_Count, m_IrregularCount;
    };
};

} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_UTIL_H_
