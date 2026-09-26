// Filename: snd_DisposeCallbackManager.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_DisposeCallbackManager.h>

namespace nw {
namespace snd {
namespace internal {
namespace driver {

DisposeCallbackManager::~DisposeCallbackManager()
{
}

DisposeCallbackManager& DisposeCallbackManager::GetInstance()
{
    static DisposeCallbackManager instance;
    return instance;
}

void DisposeCallbackManager::RegisterDisposeCallback(DisposeCallback* callback)
{
    m_CallbackList.PushBack(callback);
}

void DisposeCallbackManager::UnregisterDisposeCallback(DisposeCallback* callback)
{
    m_CallbackList.Erase(callback);
}

void DisposeCallbackManager::Dispose(void* mem, unsigned long size)
{
    const void* start = mem;
    const void* end = static_cast<u8*>(mem) + size;

    NW_UT_LINKLIST_FOREACH_SAFE(itr, m_CallbackList,
        {itr->InvalidateData(start, end); })
}

} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw