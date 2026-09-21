#ifndef NW_SND_PLAYER_HEAP_DATA_MANAGER_H_
#define NW_SND_PLAYER_HEAP_DATA_MANAGER_H_

#include <nw/snd/snd_DisposeCallback.h>
#include <nw/snd/snd_SoundArchiveLoader.h>
#include <nw/snd/snd_SoundArchive.h>

namespace nw { 
namespace snd { 
namespace internal {

class PlayerHeapDataManager : public internal::driver::DisposeCallback,
                              public internal::SoundArchiveLoader
                              {
public:
    PlayerHeapDataManager();
    virtual ~PlayerHeapDataManager();

    void Initialize(const SoundArchive* arc);
    void Finalize();

    const void* SetFileAddress(SoundArchive::FileId fileId, const void* address);
    const void* GetFileAddress(SoundArchive::FileId) const;

    static const int FILE_ADDRESS_COUNT = 1 + 4 + 4; // bfseq + 4 * bfbnk + 4 * bfwar

protected:
    virtual void InvalidateData(const void* start, const void* end);
    virtual const void* SetFileAddressToTable(SoundArchive::FileId, const void* address);
    virtual const void* GetFileAddressFromTable(SoundArchive::FileId fileId) const;
    virtual const void* GetFileAddressImpl(SoundArchive::FileId fileId) const;

private:
    struct FileAddress
    {
        SoundArchive::FileId fileId;
        const void* address;
    };

    FileAddress m_FileAddress[FILE_ADDRESS_COUNT];
    bool m_IsInitialized;
    bool m_IsFinalized;
};

} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_PLAYER_HEAP_DATA_MANAGER_H_
