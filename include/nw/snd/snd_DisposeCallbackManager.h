#ifndef NW_SND_DISPOSE_CALLBACK_MANAGER_H_
#define NW_SND_DISPOSE_CALLBACK_MANAGER_H_

#include <nw/ut/ut_LinkList.h>
#include <nw/snd/snd_DisposeCallback.h>

namespace nw {
namespace snd {
namespace internal {
namespace driver {

class DisposeCallbackManager
{
public:
    typedef ut::LinkList< DisposeCallback, offsetof(DisposeCallback,m_DisposeLink) > CallbackList;

public:
    static DisposeCallbackManager& GetInstance();

    void Dispose(void* mem, unsigned long size);

    void RegisterDisposeCallback(DisposeCallback* callback);
    void UnregisterDisposeCallback(DisposeCallback* callback);

    unsigned long GetCallbackCount() const;

private:
    DisposeCallbackManager();

    CallbackList m_CallbackList;
};

} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_DISPOSE_CALLBACK_MANAGER_H_