#pragma once

#include <nw/ut/ut_Color.h>
#include <nw/math/math_Types.h>
#include <nw/lyt/lyt_Drawer.h>
#include <nw/lyt/lyt_TexMap.h>
#include <nw/lyt/lyt_Types.h>

namespace nw {
namespace lyt {
namespace res {
struct Material;
} // namespace res

namespace internal {
struct MatMemNums
{
    u32 texMap: 2;
    u32 texSRT: 2;
    u32 texCoordGen: 2;
    u32 tevStage: 3;
    u32 alpComp: 1;
    u32 blendMode: 1;
};
}

class AnimTransform;
class AnimResource;
class DrawInfo;
class Layout;
class TexMap;
struct ResBlockSet;

class Material
{
public:
    enum
    {
        FLAG_USER_ALLOCATED,
        FLAG_TEXTURE_ONLY,
        FLAG_TEXTURE_CLEAN,
        FLAG_MAX
    };

    Material();
    Material(const res::Material* pRes, const ResBlockSet& resBlockSet);
    virtual ~Material();
    virtual void BindAnimation(AnimTransform* pAnimTrans);
    virtual void UnbindAnimation(AnimTransform* pAnimTrans);
    virtual void UnbindAllAnimation();
    virtual void Animate();
    virtual AnimationLink* FindAnimationLink(AnimTransform* pAnimTrans);
    virtual AnimationLink* FindAnimationLink(const AnimResource& animRes);
    virtual void SetAnimationEnable(AnimTransform* pAnimTrans, bool bEnable);
    virtual void SetAnimationEnable(const AnimResource& animRes, bool bEnable);

    void ReserveMem(u8 texMapNum, u8 texSRTNum, u8 texCoordGenNum, u8 tevStageNum = 0, bool allocAlpComp = false, bool allocBlendMode = false);

    /* Get the private Members. */
    u8 GetTexMapCap() const { return u8(m_MemCap.texMap); }
    u8 GetTexSRTCap() const { return u8(m_MemCap.texSRT); }
    u8 GetTexCoordGenCap() const { return u8(m_MemCap.texCoordGen); }
    u8 GetTevStageCap() const { return u8(m_MemCap.tevStage); }
     u8 GetColorElement(u32 colorType) const;
    bool IsAlphaCompareCap() const { return m_MemCap.alpComp != 0; }
    bool IsBlendModeCap() const { return m_MemCap.blendMode != 0; }

    // Name
    const char* GetName() const { return m_Name; }
    void SetName(const char* name);

    // Texture count getters
    u8 GetTexMapNum() const { return u8(m_MemNum.texMap); }
    u8 GetTexCoordGenNum() const { return u8(m_MemNum.texCoordGen); }
    u8 GetTexSRTNum() const { return u8(m_MemNum.texSRT); }
    u8 GetTevStageNum() const { return u8(m_MemNum.tevStage); }

    void SetTexMapNum(u8 num);
    void SetTexCoordGenNum(u8 num);
    void SetTexSRTNum(u8 num);
    void SetTevStageNum(u8 num);
    void SetupGraphics(const DrawInfo& drawInfo,u8 alpha,bool bInitFrameTransform = false);

    // Texture getters/setters
    const TexMap& GetTexMap(u32 idx) const
    {
        return GetTexMapAry()[idx];
    }
    void SetTexMap(u32 idx, const TexMap& value)
    {
        GetTexMapAry()[idx] = value;
        this->SetTextureDirty();
    }
    void SetTexMap(u32 idx, const TextureInfo& value)
    {
        GetTexMapAry()[idx].Set(value);
        this->SetTextureDirty();
    }

    const TexSRT& GetTexSRT(u32 idx) const
    {
        return GetTexSRTAry()[idx];
    }
    void SetTexSRT(u32 idx, const TexSRT& value)
    {
        GetTexSRTAry()[idx] = value;
        this->SetTextureDirty();
    }

    const TexCoordGen& GetTexCoordGen(u32 idx) const
    {
        return GetTexCoordGenAry()[idx];
    }
    void SetTexCoordGen(u32 idx, const TexCoordGen& value)
    {
        GetTexCoordGenAry()[idx] = value;
        this->SetTextureDirty();
    }

    f32 GetTexSRTElement(u32 idx, u32 eleIdx) const
    {
        const f32* const srtAry = &GetTexSRTAry()[idx].translate.x;
        return srtAry[eleIdx];
    }
    void SetTexSRTElement(u32 idx, u32 eleIdx, f32 value)
    {
        f32* const srtAry = &GetTexSRTAry()[idx].translate.x;
        srtAry[eleIdx] = value;
        this->SetTextureDirty();
    }

    // Color getters/setters
    const ut::Color8& GetColor(u32 idx) const
    {
        return m_Colors[idx];
    }
    void SetColor(u32 idx, ut::Color8 value)
    {
        m_Colors[idx] = value;
    }
    u8   GetColorElement(u32 colorType) const;
    void SetColorElement(u32 colorType, u8 value);

    // TevStage getters/setters
    const TevStage& GetTevStage(u32 idx) const
    {
        return GetTevStageAry()[idx];
    }
    void SetTevStage(u32 idx, const TevStage& value)
    {
        GetTevStageAry()[idx] = value;
    }

    // AlphaCompare getters/setters
    const AlphaCompare& GetAlphaCompare() const
    {
        return *GetAlphaComparePtr();
    }
    void SetAlphaCompare(AlphaCompare value)
    {
        *GetAlphaComparePtr() = value;
    }

    // BlendMode getters/setters
    const BlendMode& GetBlendMode() const
    {
        return *GetBlendModePtr();
    }
    void SetBlendMode(BlendMode value)
    {
        *GetBlendModePtr() = value;
    }

    bool GetTextureOnly() const { return internal::TestBit(m_Flag, FLAG_TEXTURE_ONLY); }
    void SetTextureOnly(bool value) { m_Flag = internal::SetBit(m_Flag, FLAG_TEXTURE_ONLY, value); }

    // Animation
    void AddAnimationLink(AnimationLink* pAnimationLink);

    const AnimationList& GetAnimationList() const { return m_AnimList; }
    AnimationList&       GetAnimationList() { return m_AnimList; }

    bool IsUserAllocated() const { return internal::TestBit(m_Flag, FLAG_USER_ALLOCATED); }
    void SetUserAllocated() { internal::SetBit(&m_Flag, FLAG_USER_ALLOCATED, true); }

    bool IsTextureDirty() const { return !internal::TestBit(m_Flag, FLAG_TEXTURE_CLEAN); }
    void SetTextureDirty(bool bDirty = true) { internal::SetBit(&m_Flag, FLAG_TEXTURE_CLEAN, !bDirty); }

protected:
    const TexMap* GetTexMapAry() const;
    TexMap* GetTexMapAry();
    const TexSRT* GetTexSRTAry() const;
    TexSRT* GetTexSRTAry();
    const TexCoordGen* GetTexCoordGenAry() const;
    TexCoordGen* GetTexCoordGenAry();
    const AlphaCompare* GetAlphaComparePtr()const;
    AlphaCompare* GetAlphaComparePtr();
    const BlendMode* GetBlendModePtr() const;
    BlendMode* GetBlendModePtr();
    const TevStage* GetTevStageAry() const;
    TevStage* GetTevStageAry();

private:
    void Init();
    void InitMatMemNums(internal::MatMemNums* ptr);

    AnimationList m_AnimList;
    ut::Color8 m_Colors[MatColorMax];
    internal::MatMemNums m_MemCap;
    internal::MatMemNums m_MemNum;
    void* m_pMem;
    char m_Name[MaterialNameStrMax + 1];
    u8 m_Flag;
    u8 m_Padding[2];

    Material(const Material& other);
    Material& operator=(const Material& other);
};

}
}