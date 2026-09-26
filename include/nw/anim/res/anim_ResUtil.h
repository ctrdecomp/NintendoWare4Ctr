#ifndef NW_ANIM_RESUTIL_H_
#define NW_ANIM_RESUTIL_H_

#include <nw/anim/res/anim_ResAnim.h>

namespace nw {
namespace anim {
namespace res {

ResAnim CloneTextureAnim(os::IAllocator* allocator, const ResAnim& src);

void DestroyClonedTextureAnim(os::IAllocator* allocator, ResAnim anim);

} // namespace res
} // namespace anim
} // namespace nw

#endif // NW_ANIM_RESUTIL_H_ 