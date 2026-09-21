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
        PerfHistogram() { Initialize(); }

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
