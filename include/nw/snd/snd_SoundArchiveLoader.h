#ifndef NW_SND_SOUND_ARCHIVE_LOADER_H_
#define NW_SND_SOUND_ARCHIVE_LOADER_H_

#include <nw/snd/snd_Util.h>
#include <nw/snd/snd_SoundArchive.h>

namespace nw { 
namespace snd {

class SoundMemoryAllocatable;

namespace internal {

struct LoadItemInfo
{
    nw::snd::SoundArchive::ItemId itemId;
    const void* address;

    LoadItemInfo(): 
        itemId(nw::snd::SoundArchive::INVALID_ID), 
        address(NULL)
        {
    }
};

class SoundArchiveLoader
{
public:
    static const u32 SIGNATURE_INDIVIDUAL_WAVE = NW_UT_MAKE_SIGWORD('C', 'I', 'W', 'V');

    static const u32 FILE_ALIGNMENT = 64;

    struct IndividualWaveInfo
    {
        u32 signature;
        u32 fileId;
        u32 waveIndex;
        u8 padding[SoundArchiveLoader::FILE_ALIGNMENT - sizeof(u32) * 3];

        IndividualWaveInfo(u32 id, u32 index): 
            signature(SoundArchiveLoader::SIGNATURE_INDIVIDUAL_WAVE), 
            fileId(id), 
            waveIndex(index)
            {
        }
    };

    enum LoadFlag
    {
        LOAD_SEQ    = 1 << 0,
        LOAD_WSD    = 1 << 1,
        LOAD_BANK   = 1 << 2,
        LOAD_WARC   = 1 << 3,

        LOAD_ALL    = 0xFFFFFFFF
    };

    SoundArchiveLoader();
    virtual ~SoundArchiveLoader();

    bool IsAvailable() const;

    bool LoadData(SoundArchive::ItemId id, SoundMemoryAllocatable* pAllocator, u32 loadFlag = LOAD_ALL, size_t loadBlockSize = 0);
    bool LoadData(const char* pItemName, SoundMemoryAllocatable* pAllocator, u32 loadFlag = LOAD_ALL, size_t loadBlockSize = 0);

    bool IsDataLoaded(SoundArchive::ItemId itemId, u32 loadFlag = LOAD_ALL) const;
    bool IsDataLoaded(const char* pItemName, u32 loadFlag = LOAD_ALL) const;

    bool CancelLoading();

    const void* detail_GetFileAddressByItemId(SoundArchive::ItemId itemId) const;

    bool detail_LoadWaveArchiveByBankFile(const void* bankFile, SoundMemoryAllocatable* pAllocator);

    bool detail_LoadWaveArchiveByWaveSoundFile(const void* wsdFile, s32 waveSoundIndex, SoundMemoryAllocatable* pAllocator);

protected:
    void SetSoundArchive(const SoundArchive* arc);
    const SoundArchive* GetSoundArchive() const { return m_pSoundArchive; }

    const void* GetFileAddressFromSoundArchive(SoundArchive::FileId fileId) const;

    virtual const void* SetFileAddressToTable(SoundArchive::FileId fileId, const void* address) = 0;
    virtual const void* GetFileAddressFromTable(SoundArchive::FileId fileId) const = 0;
    virtual const void* GetFileAddressImpl(SoundArchive::FileId fileId) const = 0;

private:
    bool LoadSequenceSound(SoundArchive::ItemId soundId, SoundMemoryAllocatable* pAllocator, u32 loadFlag, size_t loadBlockSize);
    bool LoadWaveSound(SoundArchive::ItemId soundId, SoundMemoryAllocatable* pAllocator, 
        u32 loadFlag, size_t loadBlockSize, SoundArchive::ItemId waveSoundSetId = SoundArchive::INVALID_ID);
    bool LoadBank(SoundArchive::ItemId bankId, SoundMemoryAllocatable* pAllocator,
        u32 loadFlag, size_t loadBlockSize);
    bool LoadWaveArchive(SoundArchive::ItemId warcId, SoundMemoryAllocatable* pAllocator,
        u32 loadFlag, size_t loadBlockSize);
    const void* LoadWaveArchiveTable(
        SoundArchive::ItemId warcId,
        SoundMemoryAllocatable* pAllocator,
        size_t loadBlockSize
    );
    bool LoadIndividualWave(
        SoundArchive::ItemId warcId,
        u32 waveIndex,
        SoundMemoryAllocatable* pAllocator,
        size_t loadBlockSize
    );
    bool LoadGroup(
        SoundArchive::ItemId groupId,
        SoundMemoryAllocatable* pAllocator,
        size_t loadBlockSize
    );
    bool LoadSoundGroup(
        SoundArchive::ItemId soundGroupId,
        SoundMemoryAllocatable* pAllocator,
        u32 loadFlag,
        size_t loadBlockSize
    );
    const void* LoadImpl(
        SoundArchive::FileId fileId,
        SoundMemoryAllocatable* pAllocator,
        size_t loadBlockSize,
        bool needDeviceMemory = false
    );
    bool LoadWaveArchiveImpl(
        SoundArchive::ItemId warcId,
        u32 waveIndex,
        SoundMemoryAllocatable* pAllocator,
        u32 loadFlag,
        size_t loadBlockSize = 0
    );

    bool PostProcessForLoadedGroupFile(
        const void* pGroupFile,
        SoundMemoryAllocatable* pAllocator = NULL,
        size_t loadBlockSize = 0
    );

    bool IsSequenceSoundDataLoaded(SoundArchive::ItemId itemId, u32 loadFlag) const;
    bool IsWaveSoundDataLoaded(SoundArchive::ItemId itemId, u32 loadFlag) const;
    bool IsBankDataLoaded(SoundArchive::ItemId itemId, u32 loadFlag) const;
    bool IsWaveArchiveDataLoaded(SoundArchive::ItemId itemId, u32 waveIndex) const;
    bool IsGroupDataLoaded(SoundArchive::ItemId itemId) const;
    bool IsSoundGroupDataLoaded(SoundArchive::ItemId itemId, u32 loadFlag) const;

    void* LoadFile(
        SoundArchive::FileId fileId,
        SoundMemoryAllocatable* allocator,
        size_t loadBlockSize,
        bool needDeviceMemory
    );
    s32 ReadFile(
        SoundArchive::FileId fileId,
        void* buffer,
        size_t size,
        s32 offset,
        size_t loadBlockSize
    );

    void SetWaveArchiveTableWithSeqInEmbeddedGroup(SoundArchive::ItemId seqId, SoundMemoryAllocatable* pAllocator);
    void SetWaveArchiveTableWithBankInEmbeddedGroup(SoundArchive::ItemId bankId, SoundMemoryAllocatable* pAllocator);
    void SetWaveArchiveTableWithWsdInEmbeddedGroup(SoundArchive::ItemId wsdId, SoundMemoryAllocatable* pAllocator);
    void SetWaveArchiveTableInEmbeddedGroupImpl(SoundArchive::ItemId warcId, SoundMemoryAllocatable* pAllocator);

    const SoundArchive* m_pSoundArchive;
    u32 m_StreamArea[128];

    u16 m_LoadDataCallCount;
    bool m_IsCancelLoading;
};

} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_SOUND_ARCHIVE_LOADER_H_
