#pragma once

#include <nw/math.h>

namespace nw {
namespace ut {

struct FloatColor;

struct Color8 {
public:
    typedef Color8 SelfType;

    static const int ELEMENT_MIN       = 0;
    static const int ELEMENT_MAX       = 255;
    static const int ALPHA_MIN         = ELEMENT_MIN;
    static const int ALPHA_MAX         = ELEMENT_MAX;
    static const int ALPHA_OPACITY     = ALPHA_MAX;
    static const int ALPHA_TRANSPARENT = ALPHA_MIN;

    static const u32 BLACK   = 0x000000FF;
    static const u32 GRAY    = 0x808080FF;
    static const u32 WHITE   = 0xFFFFFFFF;
    static const u32 RED     = 0xFF0000FF;
    static const u32 GREEN   = 0x00FF00FF;
    static const u32 BLUE    = 0x0000FFFF;
    static const u32 YELLOW  = 0xFFFF00FF;
    static const u32 MAGENTA = 0xFF00FFFF;
    static const u32 CYAN    = 0x00FFFFFF;

    Color8()                                          { SetU32(WHITE); }
    Color8(u32 color)                                 { SetU32(color); }
    Color8(s32 r, s32 g, s32 b, s32 a) : r(r), g(g), b(b), a(a) {}
    ~Color8() {}

    SelfType& operator=(u32 color)              { SetU32(color); return *this; }
    SelfType& operator=(FloatColor& color);
    operator u32() const                        { return ToU32(); }

    const SelfType operator+(const SelfType& rhs) const {
        return SelfType(
            nn::math::Min(r + rhs.r, ELEMENT_MAX), nn::math::Min(g + rhs.g, ELEMENT_MAX),
            nn::math::Min(b + rhs.b, ELEMENT_MAX), nn::math::Min(a + rhs.a, ELEMENT_MAX)
        );
    }
    const SelfType operator-(const SelfType& rhs) const {
        return SelfType(
            nn::math::Max(r - rhs.r, ELEMENT_MIN), nn::math::Max(g - rhs.g, ELEMENT_MIN),
            nn::math::Max(b - rhs.b, ELEMENT_MIN), nn::math::Max(a - rhs.a, ELEMENT_MIN)
        );
    }
    const SelfType operator*(const SelfType& rhs) const {
        return SelfType(
            r * rhs.r / ELEMENT_MAX, g * rhs.g / ELEMENT_MAX,
            b * rhs.b / ELEMENT_MAX, a * rhs.a / ELEMENT_MAX);
    }
    const SelfType operator/(const SelfType& rhs) const {
        return SelfType(
            (r < rhs.r) ? r * ELEMENT_MAX / rhs.r : ELEMENT_MAX,
            (g < rhs.g) ? g * ELEMENT_MAX / rhs.g : ELEMENT_MAX,
            (b < rhs.b) ? b * ELEMENT_MAX / rhs.b : ELEMENT_MAX,
            (a < rhs.a) ? a * ELEMENT_MAX / rhs.a : ELEMENT_MAX);
    }

    const SelfType operator+(u32 rhs) const { return *this + SelfType(rhs); }
    const SelfType operator-(u32 rhs) const { return *this - SelfType(rhs); }
    const SelfType operator*(u32 rhs) const { return *this * SelfType(rhs); }
    const SelfType operator/(u32 rhs) const { return *this / SelfType(rhs); }
    const SelfType operator|(u32 rhs)  const { return *this | SelfType(rhs); }
    const SelfType operator&(u32 rhs)  const { return *this & SelfType(rhs); }

    const SelfType operator|(const SelfType& rhs) const { return U32RawToColor(ToU32Raw() | rhs.ToU32Raw()); }
    const SelfType operator&(const SelfType& rhs) const { return U32RawToColor(ToU32Raw() & rhs.ToU32Raw()); }

    SelfType& operator+=(const SelfType& rhs) { *this = *this + rhs; return *this; }
    SelfType& operator-=(const SelfType& rhs) { *this = *this - rhs; return *this; }
    SelfType& operator*=(const SelfType& rhs) { *this = *this * rhs; return *this; }
    SelfType& operator/=(const SelfType& rhs) { *this = *this / rhs; return *this; }

    const SelfType operator++() {
        if (r < ELEMENT_MAX) r++; if (g < ELEMENT_MAX) g++;
        if (b < ELEMENT_MAX) b++; if (a < ELEMENT_MAX) a++;
        return *this;
    }
    const SelfType operator++(int) {
        SelfType old = *this;
        if (r < ELEMENT_MAX) r++; if (g < ELEMENT_MAX) g++;
        if (b < ELEMENT_MAX) b++; if (a < ELEMENT_MAX) a++;
        return old;
    }
    const SelfType operator--() {
        if (r > ELEMENT_MIN) r--; if (g > ELEMENT_MIN) g--;
        if (b > ELEMENT_MIN) b--; if (a > ELEMENT_MIN) a--;
        return *this;
    }
    const SelfType operator--(int) {
        SelfType old = *this;
        if (r > ELEMENT_MIN) r--; if (g > ELEMENT_MIN) g--;
        if (b > ELEMENT_MIN) b--; if (a > ELEMENT_MIN) a--;
        return old;
    }

    void Set(s32 red, s32 green, s32 blue, s32 alpha = ALPHA_OPACITY) {
        r = u8(red); g = u8(green); b = u8(blue); a = u8(alpha);
    }
    void Set(SelfType color) { *this = color; }

#if NN_ENDIAN == NN_ENDIAN_VALUE_LITTLE
    u32 ToU32() const {
        u32 val = ToU32Raw();
        return ((val >> 24) & 0xFF) | ((val >> 8) & 0xFF00) | ((val << 8) & 0xFF0000) | ((val << 24) & 0xFF000000);
    }
    void SetU32(u32 value) {
        ToU32RawRef() = ((value >> 24) & 0xFF) | ((value >> 8) & 0xFF00) | ((value << 8) & 0xFF0000) | ((value << 24) & 0xFF000000);
    }
#else
    u32  ToU32() const      { return ToU32RawRef(); }
    void SetU32(u32 value)  { ToU32RawRef() = value; }
#endif

    u8 r, g, b, a;

protected:
    static SelfType U32RawToColor(u32 value) { return *reinterpret_cast<SelfType*>(&value); }
    u32&       ToU32RawRef()       { return *reinterpret_cast<u32*>(this); }
    const u32& ToU32RawRef() const { return *reinterpret_cast<const u32*>(this); }
    u32        ToU32Raw()    const { return *reinterpret_cast<const u32*>(this); }
};

typedef Color8 Color;

struct FloatColor {
public:
    typedef FloatColor SelfType;

    static const int ELEMENT_MIN       = 0;
    static const int ELEMENT_MAX       = 1;
    static const int ALPHA_MIN         = ELEMENT_MIN;
    static const int ALPHA_MAX         = ELEMENT_MAX;
    static const int ALPHA_OPACITY     = ALPHA_MAX;
    static const int ALPHA_TRANSPARENT = ALPHA_MIN;

    FloatColor() : r(0.f), g(0.f), b(0.f), a(1.f) {}
    FloatColor(const nn::math::VEC4& vec) : r(vec.x), g(vec.y), b(vec.z), a(vec.w) {}
    FloatColor(Color8 color)              { *this = color; }
    FloatColor(f32 r, f32 g, f32 b, f32 a) : r(r), g(g), b(b), a(a) {}
    ~FloatColor() {}

    SelfType& operator=(Color8 color) {
        Set(s32(color.r), s32(color.g), s32(color.b), s32(color.a));
        return *this;
    }
    SelfType& operator=(const nn::math::VEC4& vec) { Set(vec.x, vec.y, vec.z, vec.w); return *this; }

    operator f32*()                  { return &r; }
    operator const f32*() const      { return &r; }
    operator Color8() const {
        return Color8(
            u8(0.5f + nn::math::Clamp(r, 0.f, 1.f) * 255.f),
            u8(0.5f + nn::math::Clamp(g, 0.f, 1.f) * 255.f),
            u8(0.5f + nn::math::Clamp(b, 0.f, 1.f) * 255.f),
            u8(0.5f + nn::math::Clamp(a, 0.f, 1.f) * 255.f));
    }
    operator nn::math::VEC4&()             { return ToVEC4(); }
    operator const nn::math::VEC4&() const { return ToVEC4(); }

    const SelfType operator+(const SelfType& rhs) const { return SelfType(r+rhs.r, g+rhs.g, b+rhs.b, a+rhs.a); }
    const SelfType operator-(const SelfType& rhs) const { return SelfType(r-rhs.r, g-rhs.g, b-rhs.b, a-rhs.a); }
    const SelfType operator*(const SelfType& rhs) const { return SelfType(r*rhs.r, g*rhs.g, b*rhs.b, a*rhs.a); }
    const SelfType operator/(const SelfType& rhs) const {
        return SelfType(rhs.r ? r/rhs.r : ELEMENT_MAX, rhs.g ? g/rhs.g : ELEMENT_MAX, rhs.b ? b/rhs.b : ELEMENT_MAX, rhs.a ? a/rhs.a : ELEMENT_MAX);
    }

    SelfType& operator+=(const SelfType& rhs) { *this = *this + rhs; return *this; }
    SelfType& operator-=(const SelfType& rhs) { *this = *this - rhs; return *this; }
    SelfType& operator*=(const SelfType& rhs) { *this = *this * rhs; return *this; }
    SelfType& operator/=(const SelfType& rhs) { *this = *this / rhs; return *this; }

    SelfType& operator+=(f32 v) { r+=v; g+=v; b+=v; a+=v; return *this; }
    SelfType& operator-=(f32 v) { r-=v; g-=v; b-=v; a-=v; return *this; }
    SelfType& operator*=(f32 v) { r*=v; g*=v; b*=v; a*=v; return *this; }
    SelfType& operator/=(f32 v) {
        if(v != 0.f) { r/=v; g/=v; b/=v; a/=v; }
        else { r = g = b = a = ELEMENT_MAX; }
        return *this;
    }

    bool operator==(const SelfType& rhs) const { return r==rhs.r && g==rhs.g && b==rhs.b && a==rhs.a; }
    bool operator!=(const SelfType& rhs) const { return !(*this == rhs); }

    void Set(f32 red, f32 green, f32 blue, f32 alpha = ALPHA_OPACITY) { r=red; g=green; b=blue; a=alpha; }
    void Set(s32 red, s32 green, s32 blue, s32 alpha = Color8::ALPHA_OPACITY) {
        r = f32(red)/255.f; g = f32(green)/255.f; b = f32(blue)/255.f; a = f32(alpha)/255.f;
    }
    void Set(const SelfType& color) { *this = color; }

    void Clamp()              { Clamp(0.f, 1.f); }
    void Clamp(f32 lo, f32 hi) {
        r = nn::math::Clamp(r, lo, hi); g = nn::math::Clamp(g, lo, hi);
        b = nn::math::Clamp(b, lo, hi); a = nn::math::Clamp(a, lo, hi);
    }

    f32*       ToArray()       { return reinterpret_cast<f32*>(this); }
    const f32* ToArray() const { return reinterpret_cast<const f32*>(this); }

    u32 ToPicaU32() const {
        u8 red   = u8(0.5f + nn::math::Clamp(r, 0.f, 1.f) * 255.f);
        u8 green = u8(0.5f + nn::math::Clamp(g, 0.f, 1.f) * 255.f);
        u8 blue  = u8(0.5f + nn::math::Clamp(b, 0.f, 1.f) * 255.f);
        u8 alpha = u8(0.5f + nn::math::Clamp(a, 0.f, 1.f) * 255.f);
        return (alpha << 24) | (blue << 16) | (green << 8) | red;
    }

    void SetColor(f32 red,f32 green,f32 blue){
        this->Set(red, green, blue, a);
    }

    void SetColor(s32 red,s32 green,s32 blue){

        f32 fr = static_cast<f32>( red ) / 255.f;
        f32 fg = static_cast<f32>( green ) / 255.f;
        f32 fb = static_cast<f32>( blue ) / 255.f;

        this->Set(fr, fg, fb, a);
    }

    f32 r, g, b, a;

protected:
    nn::math::VEC4&       ToVEC4()       { return *reinterpret_cast<nn::math::VEC4*>(this); }
    const nn::math::VEC4& ToVEC4() const { return *reinterpret_cast<const nn::math::VEC4*>(this); }
};

#if !defined(NN_SWAP_ENDIAN)
    typedef FloatColor ResFloatColor;
#else
    typedef struct {
        nw::ut::ResF32 r, g, b, a;
        operator FloatColor() const { FloatColor c; c.r=r; c.g=g; c.b=b; c.a=a; return c; }
        void operator=(const FloatColor& v) { r=v.r; g=v.g; b=v.b; a=v.a; }
    } ResFloatColor;
#endif

}
}