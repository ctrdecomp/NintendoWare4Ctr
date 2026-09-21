// Filename: lyt_Material.cpp
//
// Project: NintendoWare4Ctr

#include <nw/lyt/lyt_Common.h>
#include <nw/lyt/lyt_DrawInfo.h>
#include <nw/lyt/lyt_GraphicsResource.h>
#include <nw/lyt/lyt_Material.h>
#include <nw/lyt/lyt_Layout.h>
#include <nw/lyt/lyt_Resources.h>
#include <nw/lyt/lyt_Animation.h>
#include <nw/lyt/lyt_ResourceAccessor.h>
#include <nw/lyt/lyt_Util.h>

namespace nw{
namespace lyt{
namespace{

const u32 DefaultBlackColor = 0x00000000;
const u32 DefaultWhiteColor = 0xffffffff;

void InitTexSRT(TexSRT* texSRTs, u32 num)
{
    for (u32 i = 0; i < num; ++i)
    {
        texSRTs[i].translate = res::Vec2(0.f, 0.f);
        texSRTs[i].rotate    = 0.f;
        texSRTs[i].scale     = res::Vec2(1.f, 1.f);
    }
}

u32 CalcOffsetTexSRTAry(const internal::MatMemNums& bitNums)
{
    return sizeof(TexMap) * bitNums.texMap;
}

u32 CalcOffsetTexCoordGenAry(const internal::MatMemNums& bitNums)
{
    return CalcOffsetTexSRTAry(bitNums) + sizeof(TexSRT) * bitNums.texSRT;
}

u32 CalcOffsetGetAlphaCompare(const internal::MatMemNums& bitNums)
{
    return CalcOffsetTexCoordGenAry(bitNums) + sizeof(TexCoordGen) * bitNums.texCoordGen;
}

u32 CalcOffsetBlendMode(const internal::MatMemNums& bitNums)
{
    return CalcOffsetGetAlphaCompare(bitNums) + sizeof(AlphaCompare) * bitNums.alpComp;
}

u32 CalcOffsetTevStageAry(const internal::MatMemNums& bitNums)
{
    return CalcOffsetBlendMode(bitNums) + sizeof(BlendMode) * bitNums.blendMode;
}

static void LoadTexCoordMatrix(GraphicsResource& graphicsResource, int texIdx, f32 (*v)[3])
{
    GLuint program = graphicsResource.GetGlProgram();

    GLint loc = graphicsResource.GetUniformLocation(graphicsResource.UNIFORM_uTexMtx0 + texIdx);
    glUniform4f(loc + 0, v[0][0], v[0][1], 0.0f, v[0][2]);
    glUniform4f(loc + 1, v[1][0], v[1][1], 0.0f, v[1][2]);
}

struct TexEnvSetting
{
    u8 /*TevMode*/ m_ModeRgb;
    u8 /*TevMode*/ m_ModeAlpha;
    u8 /*TevSrc*/ m_SrcRgb0;
    u8 /*TevSrc*/ m_SrcRgb1;
    u8 /*TevSrc*/ m_SrcRgb2;
    u8 /*TevSrc*/ m_SrcAlpha0;
    u8 /*TevSrc*/ m_SrcAlpha1;
    u8 /*TevSrc*/ m_SrcAlpha2;
    u8 /*TevOpRgb*/ m_OpRgb0;
    u8 /*TevOpRgb*/ m_OpRgb1;
    u8 /*TevOpRgb*/ m_OpRgb2;
    u8 /*TevOpAlp*/ m_OpAlpha0;
    u8 /*TevOpAlp*/ m_OpAlpha1;
    u8 /*TevOpAlp*/ m_OpAlpha2;
    u8 /*TevScale*/ m_ScaleRgb;
    u8 /*TevScale*/ m_ScaleAlpha;

    void SetTexEnv(GraphicsResource& gres, int i) const
    {
        internal::GL::SetTevCombineRgb(gres, i, TevMode(m_ModeRgb));
        internal::GL::SetTevCombineAlpha(gres, i, TevMode(m_ModeAlpha));
        internal::GL::SetTevSrcRgb(gres,i,TevSrc(m_SrcRgb0),TevSrc(m_SrcRgb1),TevSrc(m_SrcRgb2));
        internal::GL::SetTevSrcAlpha(gres,i,TevSrc(m_SrcAlpha0),TevSrc(m_SrcAlpha1),TevSrc(m_SrcAlpha2));
        internal::GL::SetTevOperandRgb(gres,i,TevOpRgb(m_OpRgb0),TevOpRgb(m_OpRgb1),TevOpRgb(m_OpRgb2));
        internal::GL::SetTevOperandAlpha(gres,i,TevOpAlp(m_OpAlpha0),TevOpAlp(m_OpAlpha1),TevOpAlp(m_OpAlpha2));
        internal::GL::SetTevScaleRgb(gres, i, TevScale(m_ScaleRgb));
        internal::GL::SetTevScaleAlpha(gres, i, TevScale(m_ScaleAlpha));
    }
};

}

Material::Material()
{
    this->Init();

    std::memset(this->m_Name, 0, sizeof(m_Name));
}

Material::Material(const res::Material* pRes,const ResBlockSet& resBlockSet)
{
    this->Init();
    this->SetName(pRes->name);

    // constant color
    for (int i = 0; i < MatColorMax; ++i)
    {
        this->m_Colors[i] = pRes->colors[i];
    }

    u32 resOffs = sizeof(res::Material);

    const res::TexMap *const pResTexMap = internal::ConvertOffsToPtr<res::TexMap>(pRes, resOffs);
    resOffs += sizeof(res::TexMap) * pRes->resNum.GetTexMapNum();

    const TexSRT *const pResTexSRTs = internal::ConvertOffsToPtr<TexSRT>(pRes, resOffs);
    resOffs += sizeof(TexSRT) * pRes->resNum.GetTexSRTNum();

    const TexCoordGen *const pResTexCoordGens = internal::ConvertOffsToPtr<TexCoordGen>(pRes, resOffs);
    resOffs += sizeof(TexCoordGen) * pRes->resNum.GetTexCoordGenNum();

    const TevStage *const pResTevStages = internal::ConvertOffsToPtr<TevStage>(pRes, resOffs);
    resOffs += sizeof(TevStage) * pRes->resNum.GetTevStageNum();

    const AlphaCompare *const pResAlphaCompare = internal::ConvertOffsToPtr<AlphaCompare>(pRes, resOffs);
    resOffs += pRes->resNum.HasAlphaCompare()? sizeof(AlphaCompare) : 0;

    const BlendMode *const pResBlendMode = internal::ConvertOffsToPtr<BlendMode>(pRes, resOffs);
    resOffs += pRes->resNum.HasBlendMode()? sizeof(BlendMode) : 0;

    const u8 texMapNum = ut::Min(pRes->resNum.GetTexMapNum(), u8(TexMapMax));
    const u8 texSRTNum = ut::Min(pRes->resNum.GetTexSRTNum(), u8(TexMapMax));
    const u8 texCoordGenNum = ut::Min(pRes->resNum.GetTexCoordGenNum(), u8(TexMapMax));
    const u8 tevStageNum = ut::Min(pRes->resNum.GetTevStageNum(), u8(TevStageMax));
    const bool allocAlpComp = pRes->resNum.HasAlphaCompare();
    const bool allocBlendMode = pRes->resNum.HasBlendMode();

    this->ReserveMem(texMapNum,texSRTNum,texCoordGenNum,tevStageNum,allocAlpComp,allocBlendMode);
    this->SetTextureOnly(pRes->resNum.IsTextureOnly());

    if (mpMem)
    {
        this->SetTexMapNum(texMapNum);
        if (texMapNum > 0)
        {

            const res::Texture *const pResTextures = internal::ConvertOffsToPtr<res::Texture>(resBlockSet.pTextureList, sizeof(*resBlockSet.pTextureList));
            TexMap *const texMaps = GetTexMapAry();

            u8 di = 0;
            for (u8 si = 0; si < texMapNum; ++si)
            {
                int texIdx = pResTexMap[si].texIdx;
                const char* fileName = internal::ConvertOffsToPtr<char>(pResTextures, pResTextures[pResTexMap[si].texIdx].nameStrOffset);

                TextureInfo texInfo = resBlockSet.pResAccessor->GetTexture(fileName);
                texMaps[di].Set(texInfo);
                texMaps[di].SetWrapMode(pResTexMap[si].GetWarpModeS(), pResTexMap[si].GetWarpModeT());
                texMaps[di].SetFilter(pResTexMap[si].GetMinFilter(), pResTexMap[si].GetMagFilter());
                ++di;
            }
        }

        TexSRT* texSRTs = GetTexSRTAry();
        SetTexSRTNum(texSRTNum);
        for (int i = 0; i < texSRTNum; ++i)
        {
            texSRTs[i].translate = pResTexSRTs[i].translate;
            texSRTs[i].rotate    = pResTexSRTs[i].rotate;
            texSRTs[i].scale     = pResTexSRTs[i].scale;
        }

        TexCoordGen* texCoordGens = this->GetTexCoordGenAry();
        this->SetTexCoordGenNum(texCoordGenNum);
        for (unsigned int i = 0; i < m_MemNum.texCoordGen; ++i)
        {
            texCoordGens[i] = pResTexCoordGens[i];
        }

        if (tevStageNum > 0)
        {
            this->SetTevStageNum(tevStageNum);
            TevStage *const tevStages = this->GetTevStageAry();
            for (int i = 0; i < tevStageNum; ++i)
            {
                tevStages[i] = pResTevStages[i];
            }
        }

        if (allocAlpComp)
        {
            *this->GetAlphaComparePtr() = *pResAlphaCompare;
        }

        if (allocBlendMode)
        {
            *this->GetBlendModePtr() = *pResBlendMode;
        }
    }
}

Material::~Material()
{
    this->UnbindAllAnimation();

    if (mpMem)
    {
        Layout::FreeMemory(this->mpMem);
        mpMem = 0;
    }
}

void Material::Init()
{
    this->m_Colors[INTERPOLATECOLOR_BLACK] = ut::Color8(DefaultBlackColor);
    this->m_Colors[INTERPOLATECOLOR_WHITE] = ut::Color8(DefaultWhiteColor);
    this->m_Colors[2] = DefaultWhiteColor;

    InitMatMemNums(&this->m_MemCap);
    InitMatMemNums(&this->m_MemNum);

    m_Flag = 0;

    mpMem = 0;
}

void Material::InitMatMemNums(internal::MatMemNums* ptr)
{
    ptr->texMap = 0;
    ptr->texSRT = 0;
    ptr->texCoordGen = 0;
    ptr->tevStage = 0;
    ptr->alpComp = 0;
    ptr->blendMode = 0;
}

void Material::ReserveMem(u8 texMapNum,u8 texSRTNum,u8 texCoordGenNum,u8 tevStageNum,bool allocAlpComp,bool allocBlendMode)
{

    const unsigned int alpCompNum = allocAlpComp ? 1: 0;
    const unsigned int blendModeNum = allocBlendMode ? 1: 0;

    if ( m_MemCap.texMap < texMapNum
      || m_MemCap.texSRT < texSRTNum
      || m_MemCap.texCoordGen < texCoordGenNum
      || m_MemCap.tevStage < tevStageNum
      || m_MemCap.alpComp < alpCompNum
      || m_MemCap.blendMode < blendModeNum 
    )
    {
        if (mpMem)
        {
            Layout::FreeMemory(this->mpMem);
            mpMem = 0;

            InitMatMemNums(&this->m_MemCap);
            InitMatMemNums(&this->m_MemNum);
        }

        mpMem = Layout::AllocMemory(sizeof(TexMap) * texMapNum + sizeof(TexSRT) * texSRTNum +
            sizeof(TexCoordGen) * texCoordGenNum + sizeof(AlphaCompare) * alpCompNum +
            sizeof(BlendMode) * blendModeNum + sizeof(TevStage) * tevStageNum);

        if (mpMem)
        {
            m_MemCap.texMap = texMapNum;
            m_MemCap.texSRT = texSRTNum;
            m_MemCap.texCoordGen = texCoordGenNum;
            m_MemCap.tevStage = tevStageNum;
            m_MemCap.alpComp = alpCompNum;
            m_MemCap.blendMode = blendModeNum;

            m_MemNum.texSRT = m_MemCap.texSRT;
            InitTexSRT(GetTexSRTAry(), this->m_MemNum.texSRT);

            m_MemNum.alpComp = m_MemCap.alpComp;
            if (m_MemNum.alpComp)
            {
                *this->GetAlphaComparePtr() = AlphaCompare();
            }

            m_MemNum.blendMode = m_MemCap.blendMode;
            if (m_MemNum.blendMode)
            {
                *this->GetBlendModePtr() = BlendMode();
            }
        }
    }
}

const TexMap* Material::GetTexMapAry() const
{
    return internal::ConvertOffsToPtr<TexMap>(this->mpMem, 0);
}

TexMap* Material::GetTexMapAry()
{
    return internal::ConvertOffsToPtr<TexMap>(this->mpMem, 0);
}

const TexSRT* Material::GetTexSRTAry() const
{
    return internal::ConvertOffsToPtr<TexSRT>(this->mpMem, CalcOffsetTexSRTAry(this->m_MemCap));
}

TexSRT* Material::GetTexSRTAry()
{
    return internal::ConvertOffsToPtr<TexSRT>(this->mpMem, CalcOffsetTexSRTAry(this->m_MemCap));
}

const TexCoordGen* Material::GetTexCoordGenAry() const
{
    return internal::ConvertOffsToPtr<TexCoordGen>(this->mpMem, CalcOffsetTexCoordGenAry(this->m_MemCap));
}

TexCoordGen* Material::GetTexCoordGenAry()
{
    return internal::ConvertOffsToPtr<TexCoordGen>(this->mpMem, CalcOffsetTexCoordGenAry(this->m_MemCap));
}

const AlphaCompare* Material::GetAlphaComparePtr() const
{
    return internal::ConvertOffsToPtr<AlphaCompare>(this->mpMem, CalcOffsetGetAlphaCompare(this->m_MemCap));
}

AlphaCompare* Material::GetAlphaComparePtr()
{
    return internal::ConvertOffsToPtr<AlphaCompare>(this->mpMem, CalcOffsetGetAlphaCompare(this->m_MemCap));
}

const BlendMode* Material::GetBlendModePtr() const
{
    return internal::ConvertOffsToPtr<BlendMode>(this->mpMem, CalcOffsetBlendMode(this->m_MemCap));
}

BlendMode* Material::GetBlendModePtr()
{
    return internal::ConvertOffsToPtr<BlendMode>(this->mpMem, CalcOffsetBlendMode(this->m_MemCap));
}

const TevStage* Material::GetTevStageAry() const
{
    return internal::ConvertOffsToPtr<TevStage>(this->mpMem, CalcOffsetTevStageAry(this->m_MemCap));
}

TevStage* Material::GetTevStageAry()
{
    return internal::ConvertOffsToPtr<TevStage>(this->mpMem, CalcOffsetTevStageAry(this->m_MemCap));
}

void Material::SetName(const char* name)
{
    ut::strcpy(this->m_Name, sizeof(m_Name), name);
}

void Material::SetTexMapNum(u8 num)
{
    if (num > 0)
    {
        TexMap *const texMaps = GetTexMapAry();
        for (u32 i = m_MemNum.texMap; i < num; ++i)
        {
            new (&texMaps[i]) TexMap();
        }
    }
    m_MemNum.texMap = num;

    this->SetTextureDirty();
}

void Material::SetTexSRTNum(u8 num)
{
    if (num > 0)
    {

        TexSRT *const texSRTs = GetTexSRTAry();
        for (u32 i = m_MemNum.texSRT; i < num; ++i)
        {
            new (&texSRTs[i]) TexSRT();
        }
    }

    m_MemNum.texSRT = num;

    this->SetTextureDirty();
}

void Material::SetTexCoordGenNum(u8 num)
{
    if (num > 0)
    {
        TexCoordGen *const texCoordGens = GetTexCoordGenAry();
        for (u32 i = m_MemNum.texCoordGen; i < num; ++i)
        {
            new (&texCoordGens[i]) TexCoordGen();
        }
    }

    m_MemNum.texCoordGen = num;

    this->SetTextureDirty();
}

void Material::SetTevStageNum(u8 num)
{
    if (num > 0)
    {
        TevStage *const tevStages = GetTevStageAry();
        for (u32 i = m_MemNum.tevStage; i < num; ++i)
        {
            new (&tevStages[i]) TevStage();
        }
    }

    m_MemNum.tevStage = num;
}

u8 Material::GetColorElement(u32 colorType) const
{
    if (colorType < ANIMTARGET_MATCOLOR_MAX)
    {
        const u32 idx = (colorType - ANIMTARGET_MATCOLOR_BUFFER_R) / 4;
        switch ((colorType - ANIMTARGET_MATCOLOR_BUFFER_R) % 4)
        {
        case 0: 
            return this->m_Colors[idx].r;
        case 1: 
            return this->m_Colors[idx].g;
        case 2: 
            return this->m_Colors[idx].b;
        case 3: 
            return this->m_Colors[idx].a;
        }
    }

    return ut::Color8::ELEMENT_MAX;
}

void Material::SetColorElement(u32 colorType, u8 value)
{
    if (colorType < ANIMTARGET_MATCOLOR_MAX)
    {

        const u32 idx = (colorType - ANIMTARGET_MATCOLOR_BUFFER_R) / 4;
        switch ((colorType - ANIMTARGET_MATCOLOR_BUFFER_R) % 4)
        {
        case 0: 
            this->m_Colors[idx].r = value; break;
        case 1: 
            this->m_Colors[idx].g = value; break;
        case 2: 
            this->m_Colors[idx].b = value; break;
        case 3: 
            this->m_Colors[idx].a = value; break;
        }
    }
}

void Material::SetupGraphics(const DrawInfo& drawInfo,u8 alpha,bool bInitFrameTransform)
{
    Layout* layout = drawInfo.GetLayout();
    GraphicsResource& graphicsResource = *drawInfo.GetGraphicsResource();

    if (!this->GetTextureOnly())
    {
        GLuint program = graphicsResource.GetGlProgram();

        graphicsResource.UseProgram(program);

        f32 scale = 1.0f / 255.0f;

        if (bInitFrameTransform)
        {
            GLint loc = graphicsResource.GetUniformLocation(graphicsResource.UNIFORM_uFrameSpec);
            glUniform4f(loc, (f32)TEXTUREFLIP_NONE, 0.0f, 0.0f, 0.0f);
        }

        glUniform4f(graphicsResource.GetUniformLocation(graphicsResource.UNIFORM_uColor),
            scale * 1.0f,
            scale * 1.0f,
            scale * 1.0f,
            scale * scale * alpha);

        if (m_MemNum.texCoordGen > 0)
        {
            u32 i = 0;
            const u32 num = ut::Min(this->m_MemNum.texCoordGen, u32(TexMapMax));
            for (; i < num; ++i)
            {
                const TexCoordGen& texCoordGen = this->GetTexCoordGen(i);
                graphicsResource.SetTexCoordSrc(int(i), texCoordGen.GetTexGenSrc());
            }
            for (; i < TexMapMax; ++i)
            {
                graphicsResource.SetTexCoordSrc(int(i), -1);
            }
        }
    }

    if (m_MemNum.texSRT > 0)
    {
        const u32 num = ut::Min(ut::Min(this->m_MemNum.texSRT, u32(this->GetTexMapNum())), u32(TexMapMax));
        for (u32 i = 0; i < num; ++i)
        {
            const TexSRT& texSRT = this->GetTexSRT(i);
            const TexMap& texMap = this->GetTexMap(i);

            math::MTX23 texMtx;
            lyt::CalcTextureMtx(&texMtx, texSRT, texMap);

            LoadTexCoordMatrix(graphicsResource, i, texMtx.m);
        }
    }

    if (m_MemNum.texMap > 0)
    {
        static const GLint wrapMode[] ={
            GL_CLAMP_TO_EDGE,
            GL_REPEAT,
            GL_MIRRORED_REPEAT,
        };

        u32 i = 0;
        const u32 num = ut::Min(this->m_MemNum.texMap, u32(TexMapMax));
        for (; i < num; ++i)
        {
            const TexMap& texMap = this->GetTexMap(i);

            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, texMap.GetTextureObject());
            internal::GL::SetTextureSamplerType(graphicsResource, i, GL_TEXTURE_2D);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode[texMap.GetWrapModeS()]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode[texMap.GetWrapModeT()]);
            internal::GL::SetTexMinFilter(graphicsResource, texMap.GetMinFilter());
            internal::GL::SetTexMagFilter(graphicsResource, texMap.GetMagFilter());

            if (i == 0)
            {
                GLint loc = graphicsResource.GetUniformLocation(graphicsResource.UNIFORM_uRcpTexSize0);
                glUniform4f(loc, 1.0f / texMap.GetWidth(), 1.0f / texMap.GetHeight(), 0.0f, 0.0f);
            }

            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, 0.0f);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, -1000);
        }
        
        for (; i < internal::TexUnitMax; ++i)
        {
            internal::GL::SetTextureSamplerType(graphicsResource, i, GL_FALSE);
        }
    }

    //  Textures / Tev
    if (!this->GetTextureOnly())
    {
        internal::GL::SetTevBufferColor(graphicsResource, this->GetColor(TEVKONSTSEL_BUFFER));

        u32 i = 0;
        if (m_MemNum.tevStage > 0)
        {
            graphicsResource.SetTexEnvAuto(false);

            for (; i < ut::Min(this->m_MemNum.tevStage, (u32)internal::TexEnvUnitMax); ++i)
            {
                const TevStage& tevStage = this->GetTevStage(i);
                internal::GL::SetTevCombineRgb(graphicsResource, i, tevStage.GetCombineRgb());

                internal::GL::SetTevCombineAlpha(graphicsResource, i, tevStage.GetCombineAlpha());

                internal::GL::SetTevSrcRgb(graphicsResource, i, tevStage.GetSrcRgb0(), tevStage.GetSrcRgb1(), tevStage.GetSrcRgb2());

                internal::GL::SetTevSrcAlpha(graphicsResource, i, tevStage.GetSrcAlpha0(), tevStage.GetSrcAlpha1(), tevStage.GetSrcAlpha2());

                internal::GL::SetTevOperandRgb(graphicsResource, i, tevStage.GetOperandRgb0(), tevStage.GetOperandRgb1(), tevStage.GetOperandRgb2());

                internal::GL::SetTevOperandAlpha(graphicsResource, i, tevStage.GetOperandAlpha0(), tevStage.GetOperandAlpha1(), tevStage.GetOperandAlpha2());

                internal::GL::SetTevScaleRgb(graphicsResource, i, tevStage.GetScaleRgb());

                internal::GL::SetTevScaleAlpha(graphicsResource, i, tevStage.GetScaleAlpha());

                internal::GL::SetTevConstRgba(graphicsResource, i, this->GetColor(tevStage.GetKonstSelRgb()), this->GetColor(tevStage.GetKonstSelAlpha()));

                if (0 < i && i <= 4)
                {
                    internal::GL::SetTevBufferInput(graphicsResource, i, tevStage.GetSavePrevRgb(), tevStage.GetSavePrevAlpha());
                }
            }
        }
        else{
            bool setTexEnv = !graphicsResource.GetTexEnvAuto() ||
              graphicsResource.GetNumTexMap() != m_MemNum.texMap;

            graphicsResource.SetTexEnvAuto(true, this->m_MemNum.texMap);

            switch (m_MemNum.texMap)
            {
            case 0:

                internal::GL::SetTevConstRgba(graphicsResource, i, this->GetColor(TEVKONSTSEL_K0));
                if (setTexEnv)
                {
                    static const TexEnvSetting setting ={
                        TEVMODE_MODULATE,
                        TEVMODE_MODULATE,
                        TEVSRC_CONSTANT, TEVSRC_PRIMARY, TEVSRC_PRIMARY,
                        TEVSRC_CONSTANT, TEVSRC_PRIMARY, TEVSRC_PRIMARY,
                        TEVOPRGB_RGB, TEVOPRGB_RGB, TEVOPRGB_RGB,
                        TEVOPALP_ALPHA, TEVOPALP_ALPHA, TEVOPALP_ALPHA,
                        TEVSCALE_1,
                        TEVSCALE_1,
                    };

                    setting.SetTexEnv(graphicsResource, i);
                }

                ++i;
                break;

            case 1:
                internal::GL::SetTevConstRgba(graphicsResource, i, this->GetColor(TEVKONSTSEL_K0));

                if (setTexEnv)
                {
                    static const TexEnvSetting setting =
                    {
                        TEVMODE_MODULATE,
                        TEVMODE_MODULATE,
                        TEVSRC_CONSTANT, TEVSRC_TEXTURE0, TEVSRC_TEXTURE0,
                        TEVSRC_CONSTANT, TEVSRC_TEXTURE0, TEVSRC_TEXTURE0,
                        TEVOPRGB_RGB, TEVOPRGB_RGB, TEVOPRGB_RGB,
                        TEVOPALP_ALPHA, TEVOPALP_ALPHA, TEVOPALP_ALPHA,
                        TEVSCALE_1,
                        TEVSCALE_1,
                    };

                    setting.SetTexEnv(graphicsResource, i);
                }

                ++i;

                internal::GL::SetTevConstRgba(graphicsResource, i, this->GetColor(TEVKONSTSEL_BUFFER));

                if (setTexEnv)
                {
                    static const TexEnvSetting setting = {
                        TEVMODE_MULT_ADD,
                        TEVMODE_MULT_ADD,
                        TEVSRC_CONSTANT, TEVSRC_TEXTURE0, TEVSRC_PREVIOUS,
                        TEVSRC_CONSTANT, TEVSRC_TEXTURE0, TEVSRC_PREVIOUS,
                        TEVOPRGB_RGB, TEVOPRGB_INV_RGB, TEVOPRGB_RGB,
                        TEVOPALP_ALPHA, TEVOPALP_INV_ALPHA, TEVOPALP_ALPHA,
                        TEVSCALE_1,
                        TEVSCALE_1,
                    };

                    setting.SetTexEnv(graphicsResource, i);
                }

                ++i;

                if (setTexEnv)
                {
                    static const TexEnvSetting setting ={
                        TEVMODE_MODULATE,
                        TEVMODE_MODULATE,
                        TEVSRC_PRIMARY, TEVSRC_PREVIOUS, TEVSRC_PREVIOUS,
                        TEVSRC_PRIMARY, TEVSRC_PREVIOUS, TEVSRC_PREVIOUS,
                        TEVOPRGB_RGB, TEVOPRGB_RGB, TEVOPRGB_RGB,
                        TEVOPALP_ALPHA, TEVOPALP_ALPHA, TEVOPALP_ALPHA,
                        TEVSCALE_1,
                        TEVSCALE_1,
                    };

                    setting.SetTexEnv(graphicsResource, i);
                }

                ++i;
                break;
            case 2:
                internal::GL::SetTevConstRgba(graphicsResource, i, this->GetColor(TEVKONSTSEL_K5));

                if (setTexEnv)
                {
                    static const TexEnvSetting setting ={
                        TEVMODE_INTERPOLATE,
                        TEVMODE_INTERPOLATE,
                        TEVSRC_TEXTURE0, TEVSRC_TEXTURE1, TEVSRC_CONSTANT,
                        TEVSRC_TEXTURE0, TEVSRC_TEXTURE1, TEVSRC_CONSTANT,
                        TEVOPRGB_RGB, TEVOPRGB_RGB, TEVOPRGB_ALPHA,
                        TEVOPALP_ALPHA, TEVOPALP_ALPHA, TEVOPALP_ALPHA,
                        TEVSCALE_1,
                        TEVSCALE_1,
                    };

                    setting.SetTexEnv(graphicsResource, i);
                }

                ++i;

                internal::GL::SetTevConstRgba(graphicsResource, i, this->GetColor(TEVKONSTSEL_K0));

                if (setTexEnv)
                {
                    static const TexEnvSetting setting = {
                        TEVMODE_MODULATE,
                        TEVMODE_MODULATE,
                        TEVSRC_CONSTANT, TEVSRC_PREVIOUS, TEVSRC_PREVIOUS,
                        TEVSRC_CONSTANT, TEVSRC_PREVIOUS, TEVSRC_PREVIOUS,
                        TEVOPRGB_RGB, TEVOPRGB_RGB, TEVOPRGB_RGB,
                        TEVOPALP_ALPHA, TEVOPALP_ALPHA, TEVOPALP_ALPHA,
                        TEVSCALE_1,
                        TEVSCALE_1,
                    };

                    setting.SetTexEnv(graphicsResource, i);
                }

                ++i;

                if (setTexEnv)
                {
                    static const TexEnvSetting setting ={
                        TEVMODE_MODULATE,
                        TEVMODE_MODULATE,
                        TEVSRC_PRIMARY, TEVSRC_PREVIOUS, TEVSRC_PREVIOUS,
                        TEVSRC_PRIMARY, TEVSRC_PREVIOUS, TEVSRC_PREVIOUS,
                        TEVOPRGB_RGB, TEVOPRGB_RGB, TEVOPRGB_RGB,
                        TEVOPALP_ALPHA, TEVOPALP_ALPHA, TEVOPALP_ALPHA,
                        TEVSCALE_1,
                        TEVSCALE_1,
                    };

                    setting.SetTexEnv(graphicsResource, i);
                }
                ++i;
                break;
            case 3:
                if (setTexEnv)
                {
                    internal::GL::SetTevConstRgba(graphicsResource, i, ut::Color8(0, 0, 0, 255 / 2));

                    static const TexEnvSetting setting ={
                        TEVMODE_INTERPOLATE,
                        TEVMODE_INTERPOLATE,
                        TEVSRC_TEXTURE0, TEVSRC_TEXTURE1, TEVSRC_CONSTANT,
                        TEVSRC_TEXTURE0, TEVSRC_TEXTURE1, TEVSRC_CONSTANT,
                        TEVOPRGB_RGB, TEVOPRGB_RGB, TEVOPRGB_ALPHA,
                        TEVOPALP_ALPHA, TEVOPALP_ALPHA, TEVOPALP_ALPHA,
                        TEVSCALE_1,
                        TEVSCALE_1,
                    };

                    setting.SetTexEnv(graphicsResource, i);
                }

                ++i;

                if (setTexEnv)
                {
                    internal::GL::SetTevConstRgba(graphicsResource, i, ut::Color8(0, 0, 0, 255 / 3));

                    static const TexEnvSetting setting ={
                        TEVMODE_INTERPOLATE,
                        TEVMODE_INTERPOLATE,
                        TEVSRC_TEXTURE2, TEVSRC_PREVIOUS, TEVSRC_CONSTANT,
                        TEVSRC_TEXTURE2, TEVSRC_PREVIOUS, TEVSRC_CONSTANT,
                        TEVOPRGB_RGB, TEVOPRGB_RGB, TEVOPRGB_ALPHA,
                        TEVOPALP_ALPHA, TEVOPALP_ALPHA, TEVOPALP_ALPHA,
                        TEVSCALE_1,
                        TEVSCALE_1,
                    };
                    setting.SetTexEnv(graphicsResource, i);
                }

                ++i;

                if (setTexEnv)
                {
                    static const TexEnvSetting setting ={
                        TEVMODE_MODULATE,
                        TEVMODE_MODULATE,
                        TEVSRC_PRIMARY, TEVSRC_PREVIOUS, TEVSRC_PREVIOUS,
                        TEVSRC_PRIMARY, TEVSRC_PREVIOUS, TEVSRC_PREVIOUS,
                        TEVOPRGB_RGB, TEVOPRGB_RGB, TEVOPRGB_RGB,
                        TEVOPALP_ALPHA, TEVOPALP_ALPHA, TEVOPALP_ALPHA,
                        TEVSCALE_1,
                        TEVSCALE_1,
                    };

                    setting.SetTexEnv(graphicsResource, i);
                }

                ++i;
                break;
            }
        }
        const u32 numPrevTexEnv = graphicsResource.GetNumTexEnv();
        graphicsResource.SetNumTexEnv(i);

        for (; i < numPrevTexEnv; ++i)
        {
            internal::GL::SetTevCombineRgb(graphicsResource, i, TEVMODE_REPLACE);
            internal::GL::SetTevCombineAlpha(graphicsResource, i, TEVMODE_REPLACE);
            internal::GL::SetTevSrcRgb(graphicsResource, i, TEVSRC_PREVIOUS, TEVSRC_PREVIOUS, TEVSRC_PREVIOUS);
            internal::GL::SetTevSrcAlpha(graphicsResource, i, TEVSRC_PREVIOUS, TEVSRC_PREVIOUS, TEVSRC_PREVIOUS);
            internal::GL::SetTevOperandRgb(graphicsResource, i, TEVOPRGB_RGB, TEVOPRGB_RGB, TEVOPRGB_RGB);
            internal::GL::SetTevOperandAlpha(graphicsResource, i, TEVOPALP_ALPHA, TEVOPALP_ALPHA, TEVOPALP_ALPHA);
            internal::GL::SetTevScaleRgb(graphicsResource, i, TEVSCALE_1);
            internal::GL::SetTevScaleAlpha(graphicsResource, i, TEVSCALE_1);
        }
    }

    if (IsAlphaCompareCap())
    {
        const AlphaCompare& alphaCompare = this->GetAlphaCompare();
        internal::GL::SetEnableAlphaTest(graphicsResource, true);
        internal::GL::SetAlphaRefValue(graphicsResource, alphaCompare.GetRef());
        internal::GL::SetAlphaTestFunc(graphicsResource,  alphaCompare.GetFunc());
    }
    else{
        internal::GL::SetEnableAlphaTest(graphicsResource, false);
    }

    if (!this->GetTextureOnly())
    {
        if (IsBlendModeCap())
        {

            static const GLint blendOp[] ={
                0,GL_FUNC_ADD,GL_FUNC_SUBTRACT,GL_FUNC_REVERSE_SUBTRACT,
            };

            static const GLint srcFactor[] ={
                GL_ZERO,
                GL_ONE,
                GL_DST_COLOR,
                GL_ONE_MINUS_DST_COLOR,
                GL_SRC_ALPHA,
                GL_ONE_MINUS_SRC_ALPHA,
                GL_DST_ALPHA,
                GL_ONE_MINUS_DST_ALPHA,
            };

            static const GLint dstFactor[] ={
                GL_ZERO,
                GL_ONE,
                GL_SRC_COLOR,
                GL_ONE_MINUS_SRC_COLOR,
                GL_SRC_ALPHA,
                GL_ONE_MINUS_SRC_ALPHA,
                GL_DST_ALPHA,
                GL_ONE_MINUS_DST_ALPHA,
            };

            static const GLint logicOp[] ={
                0 ,
                GL_NOOP,
                GL_CLEAR,
                GL_SET,
                GL_COPY,
                GL_COPY_INVERTED,
                GL_INVERT,
                GL_AND,
                GL_NAND,
                GL_OR,
                GL_NOR,
                GL_XOR,
                GL_EQUIV,
                GL_AND_REVERSE,
                GL_AND_INVERTED,
                GL_OR_REVERSE,
                GL_OR_INVERTED,
            };

            const BlendMode& blendMode = this->GetBlendMode();

            if (blendMode.GetBlendOp() == BLENDOP_DISABLE)
            {
                glDisable(GL_BLEND);
            }
            else{
                glEnable(GL_BLEND);
                glBlendFunc(srcFactor[blendMode.GetSrcFactor()], dstFactor[blendMode.GetDstFactor()]);
                glBlendEquation(blendOp[blendMode.GetBlendOp()]);
            }

            if (blendMode.GetLogicOp() == LOGICOP_DISABLE)
            {
                glDisable(GL_COLOR_LOGIC_OP);
            }
            else{
                glEnable(GL_COLOR_LOGIC_OP);
                glLogicOp(logicOp[blendMode.GetLogicOp()]);
            }
        }
        else{
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glDisable(GL_COLOR_LOGIC_OP);
        }
    }
}

void Material::BindAnimation(AnimTransform* pAnimTrans)
{
    pAnimTrans->Bind(this);
}

void Material::UnbindAnimation(AnimTransform* pAnimTrans)
{
    internal::UnbindAnimationLink(&this->m_AnimList, pAnimTrans);
}

void Material::UnbindAllAnimation()
{
    this->UnbindAnimation(0);
}

void Material::Animate()
{
    for (AnimationList::Iterator it = this->m_AnimList.GetBeginIter(); it != this->m_AnimList.GetEndIter(); ++it)
    {
        if (it->IsEnable())
        {
            AnimTransform* animTrans = it->GetAnimTransform();
            animTrans->Animate(it->GetIndex(), this);
        }
    }
}

void Material::AddAnimationLink(AnimationLink* pAnimationLink)
{
    this->m_AnimList.PushBack(pAnimationLink);
}

AnimationLink* Material::FindAnimationLink(AnimTransform* pAnimTrans)
{
    return internal::FindAnimationLink(&this->m_AnimList, pAnimTrans);
}

AnimationLink* Material::FindAnimationLink(const AnimResource& animRes)
{
    return internal::FindAnimationLink(&this->m_AnimList, animRes);
}

void Material::SetAnimationEnable(AnimTransform* pAnimTrans,bool bEnable)
{
    if (AnimationLink* pAnimLink= this->FindAnimationLink(pAnimTrans))
    {
        pAnimLink->SetEnable(bEnable);
    }
}

void Material::SetAnimationEnable(const AnimResource& animRes,bool bEnable)
{
    if (AnimationLink* pAnimLink = this->FindAnimationLink(animRes))
    {
        pAnimLink->SetEnable(bEnable);
    }
}

}
}