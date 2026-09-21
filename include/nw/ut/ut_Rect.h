#pragma once

#include <nw/types.h>

namespace nw {
namespace ut {

struct Rect
{
public:
    f32 left;
    f32 top;
    f32 right;
    f32 bottom;

    Rect():
        left(0),
        top(0),
        right(0),
        bottom(0) {}

    Rect(f32 l, f32 t, f32 r, f32 b):
        left(l),
        top(t),
        right(r),
        bottom(b) {}

    Rect(const Rect& v):
        left(v.left),
        top(v.top),
        right(v.right),
        bottom(v.bottom) {}

    ~Rect() {}

    f32 GetWidth()  const { return right - left; }
    f32 GetHeight() const { return bottom - top; }
    f32 GetX()      const { return left; }
    f32 GetY()      const { return top; }

    void SetWidth(f32 width) { right  = left + width; }
    void SetHeight(f32 height) { bottom = top + height; }

    void MoveTo(f32 x, f32 y)
    {
        right = x + GetWidth();
        left = x;
        bottom = y + GetHeight();
        top = y;
    }

    void Move(f32 dx, f32 dy)
    {
        left += dx;
        right += dx;
        top += dy;
        bottom += dy;
    }

    void SetOriginAndSize(f32 x, f32 y, f32 width, f32 height)
    {
        left   = x;
        right  = x + width;
        top    = y;
        bottom = y + height;
    }

    void Normalize()
    {
        const f32 l = left;
        const f32 t = top;
        const f32 r = right;
        const f32 b = bottom;

        left = (r - l) >= 0 ? l : r;
        right = (r - l) >= 0 ? r : l;
        top = (b - t) >= 0 ? t : b;
        bottom = (b - t) >= 0 ? b : t;
    }
};

} // namespace ut
} // namespace nw