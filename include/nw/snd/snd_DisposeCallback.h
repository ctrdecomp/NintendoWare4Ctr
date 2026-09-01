#ifndef NW_SND_DISPOSE_CALLBACK_H_
#define NW_SND_DISPOSE_CALLBACK_H_

#include <nw/ut/ut_LinkList.h>

namespace nw { 
namespace snd { 
namespace internal { 
namespace driver {

class DisposeCallback
{
public:
    ut::LinkListNode m_DisposeLink;

    virtual ~DisposeCallback() { }
    virtual void InvalidateData(const void* start, const void* end) = 0;
};

} // namespace nw
} // namespace snd
} // namespace internal
} // namespace driver

#endif // NW_SND_DISPOSE_CALLBACK_H_
