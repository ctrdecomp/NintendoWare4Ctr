// Filename: lyt_DrawerGraphics.cpp
//
// Project: NintendoWare4Ctr

#include <nw/lyt/lyt_Drawer.h>
#include <nw/lyt/lyt_Layout.h>
#include <nw/lyt/lyt_TextBox.h>
#include <nw/lyt/lyt_Material.h>
#include <nw/lyt/lyt_DrawerCommand.h>
#include <nw/lyt/lyt_DrawInfo.h>
#include <nw/lyt/lyt_GraphicsResource.h>
#include <nw/font/font_DispStringBuffer.h>
#include <nw/font/font_RectDrawerCommand.h>

extern unsigned* __cb_current_command_buffer;

#define TEX_ENV_COLOR_OFFS            3

namespace nw { 
namespace lyt {

void Drawer::SetUpTexEnv(const Material* __restrict pMaterial)
{
    if (pMaterial->GetTextureOnly())
    {
        switch (mCurrentTexEnvType)
        {
        case TEX_ENV_TYPE_0_TEX:
        case TEX_ENV_TYPE_1_TEX:
            break;

        default:
            this->FlushBuffer();
            break;
        }

        return;
    }

    if (pMaterial->IsAlphaCompareCap())
    {
        this->FlushBuffer();
        mAlphaTestEnable = true;

        SetUpAlphaTest(pMaterial);
    }
    else if (mAlphaTestEnable)
    {
        this->FlushBuffer();
        mAlphaTestEnable = false;

        NW_FONT_RECTDRAWER_ADD_SINGLE_COMMAND(PICA_REG_FRAGOP_ALPHA_TEST,PICA_CMD_DATA_FRAGOP_ALPHA_TEST_DISABLE() );
    }


    if (pMaterial->IsBlendModeCap())
    {
        this->FlushBuffer();
        mIsBlendDefault = false;

        this->SetUpBlendMode(pMaterial);
    }
    else if (mIsBlendDefault == false)
    {
        this->FlushBuffer();
        mIsBlendDefault = true;

        const u32 command[] ={
            NW_FONT_COMMAND_SET_BLEND_DEFAULT
        };

        NW_FONT_RECTDRAWER_ADD_COMMAND(command, sizeof(command));
    }


    if (pMaterial->GetTevStageNum() > 0)
    {
        this->FlushBuffer();

        this->SetUpGLTexEnvUser( pMaterial );

        mCurrentTexEnvType = TEX_ENV_TYPE_USER;

        return;
    }

    switch (pMaterial->GetTexMapNum())
    {
    case 0:
    {
        nw::ut::Color8 white = pMaterial->GetColor( INTERPOLATECOLOR_WHITE );

        static u32 command[] NW_LYT_DRAWER_ALIGN_32 ={
            PICA_CMD_DATA_TEX_ENV_SRC(
                PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR,PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT,
                PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT,PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR,
                PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT, PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT),
            PICA_CMD_HEADER_BURSTSEQ(PICA_REG_TEX_ENV5, 5),
            PICA_CMD_DATA_TEX_ENV_OPERAND(
                PICA_DATA_OPE_RGB_SRC_COLOR, PICA_DATA_OPE_RGB_SRC_COLOR, PICA_DATA_OPE_RGB_SRC_COLOR,
                PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_ALPHA ),
            PICA_CMD_DATA_TEX_ENV_COMBINE(
                PICA_DATA_TEX_ENV_COMBINE_MODULATE, PICA_DATA_TEX_ENV_COMBINE_MODULATE),
            PICA_CMD_DATA_TEX_ENV_BUFFER_COLOR_WHITE,
            PICA_CMD_DATA_TEX_ENV_SCALE( PICA_DATA_TEX_ENV_SCALE_1, PICA_DATA_TEX_ENV_SCALE_1 ),
        };

            if (mCurrentTexEnvType != TEX_ENV_TYPE_0_TEX)
            {
                this->FlushBuffer();

                *reinterpret_cast<nw::ut::Color8*>(&command[4]) = white;

                NW_FONT_RECTDRAWER_ADD_COMMAND(command, sizeof(command));

                mCurrentTexEnvType = TEX_ENV_TYPE_0_TEX;
            }
            else if (*reinterpret_cast<nw::ut::Color8*>(&command[4]) != white)
            {
                this->FlushBuffer();

                *reinterpret_cast< nw::ut::Color8*>(&command[4]) = white;

                NW_FONT_RECTDRAWER_ADD_SINGLE_COMMAND(PICA_CMD_HEADER_SINGLE(PICA_REG_TEX_ENV5 + TEX_ENV_COLOR_OFFS),command[4]);
            }

            return;
        }
      case 1 :
      {

            nw::ut::Color8 white = pMaterial->GetColor(INTERPOLATECOLOR_WHITE);
            nw::ut::Color8 black = pMaterial->GetColor(INTERPOLATECOLOR_BLACK);

            static u32 command[] NW_LYT_DRAWER_ALIGN_32 ={
                PICA_CMD_DATA_TEX_ENV_SRC(
                    PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT,
                    PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0,
                    PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT,
                    PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT,
                    PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0,
                    PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT),
                PICA_CMD_HEADER_BURSTSEQ(PICA_REG_TEX_ENV3, 5),
                PICA_CMD_DATA_TEX_ENV_OPERAND(
                    PICA_DATA_OPE_RGB_SRC_COLOR, PICA_DATA_OPE_RGB_ONE_MINUS_SRC_COLOR, PICA_DATA_OPE_RGB_SRC_COLOR,
                    PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_ONE_MINUS_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_ALPHA ),
                PICA_CMD_DATA_TEX_ENV_COMBINE(
                    PICA_DATA_TEX_ENV_COMBINE_MODULATE,
                    PICA_DATA_TEX_ENV_COMBINE_MODULATE),
                PICA_CMD_DATA_TEX_ENV_BUFFER_COLOR_BLACK,
                PICA_CMD_DATA_TEX_ENV_SCALE(PICA_DATA_TEX_ENV_SCALE_1, PICA_DATA_TEX_ENV_SCALE_1),

                PICA_CMD_DATA_TEX_ENV_SRC(
                    PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT,
                    PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0,
                    PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS,
                    PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT,
                    PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0,
                    PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS),
                PICA_CMD_HEADER_BURSTSEQ(PICA_REG_TEX_ENV4, 5),
                PICA_CMD_DATA_TEX_ENV_OPERAND(
                    PICA_DATA_OPE_RGB_SRC_COLOR, PICA_DATA_OPE_RGB_SRC_COLOR, PICA_DATA_OPE_RGB_SRC_COLOR,
                    PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_ALPHA ),
                PICA_CMD_DATA_TEX_ENV_COMBINE(
                    PICA_DATA_TEX_ENV_COMBINE_MULT_ADD_DMP,
                    PICA_DATA_TEX_ENV_COMBINE_MULT_ADD_DMP),
                PICA_CMD_DATA_TEX_ENV_BUFFER_COLOR_WHITE,
                PICA_CMD_DATA_TEX_ENV_SCALE(PICA_DATA_TEX_ENV_SCALE_1, PICA_DATA_TEX_ENV_SCALE_1),

                PICA_CMD_DATA_TEX_ENV_SRC(
                    PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR,
                    PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS,
                    PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS,
                    PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR,
                    PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS,
                    PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS ),
                PICA_CMD_HEADER_BURSTSEQ(PICA_REG_TEX_ENV5, 5),
                PICA_CMD_DATA_TEX_ENV_OPERAND(
                    PICA_DATA_OPE_RGB_SRC_COLOR, PICA_DATA_OPE_RGB_SRC_COLOR, PICA_DATA_OPE_RGB_SRC_COLOR,
                    PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_ALPHA),
                PICA_CMD_DATA_TEX_ENV_COMBINE(
                    PICA_DATA_TEX_ENV_COMBINE_MODULATE,
                    PICA_DATA_TEX_ENV_COMBINE_MODULATE),
                PICA_CMD_DATA_TEX_ENV_BUFFER_COLOR_BLACK,
                PICA_CMD_DATA_TEX_ENV_SCALE(PICA_DATA_TEX_ENV_SCALE_1, PICA_DATA_TEX_ENV_SCALE_1),
            };

            if (mCurrentTexEnvType != TEX_ENV_TYPE_1_TEX)
            {
                this->FlushBuffer();

                *reinterpret_cast<nw::ut::Color8*>(&command[4]) = black;
                *reinterpret_cast<nw::ut::Color8*>(&command[10]) = white;

                NW_FONT_RECTDRAWER_ADD_COMMAND( command, sizeof(command));

                mCurrentTexEnvType = TEX_ENV_TYPE_1_TEX;
            }
            else if (*reinterpret_cast<nw::ut::Color8*>(&command[4]) != black || *reinterpret_cast< nw::ut::Color8* >(&command[10]) != white)
            {
                this->FlushBuffer();

                *reinterpret_cast<nw::ut::Color8*>(&command[4]) = black;
                *reinterpret_cast<nw::ut::Color8*>(&command[10]) = white;

                NW_FONT_RECTDRAWER_ADD_SINGLE_COMMAND(PICA_CMD_HEADER_SINGLE(PICA_REG_TEX_ENV3 + TEX_ENV_COLOR_OFFS),command[4]);

                NW_FONT_RECTDRAWER_ADD_SINGLE_COMMAND(PICA_CMD_HEADER_SINGLE(PICA_REG_TEX_ENV4 + TEX_ENV_COLOR_OFFS),command[10]);
            }

            return;
        }
      case 2 :
      {
            this->FlushBuffer();

            this->SetUpTexEnvType2(pMaterial);

            mCurrentTexEnvType = TEX_ENV_TYPE_2_TEX;

            return;
        }
      case 3 :{
            this->FlushBuffer();

            this->SetUpTexEnvType3(pMaterial);

            mCurrentTexEnvType = TEX_ENV_TYPE_3_TEX;
            return;
        }
    }
}

void Drawer::SetUpTexEnvType2(const Material* __restrict pMaterial)
{
    static u32 command[] NW_LYT_DRAWER_ALIGN_32 ={
        PICA_CMD_DATA_TEX_ENV_SRC(
            PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0,
            PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE1,
            PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT,
            PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0,
            PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE1,
            PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT ),
        PICA_CMD_HEADER_BURSTSEQ( PICA_REG_TEX_ENV3, 5 ),
        PICA_CMD_DATA_TEX_ENV_OPERAND(
            PICA_DATA_OPE_RGB_SRC_COLOR, PICA_DATA_OPE_RGB_SRC_COLOR, PICA_DATA_OPE_RGB_SRC_ALPHA,
            PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_ALPHA ),
        PICA_CMD_DATA_TEX_ENV_COMBINE(
            PICA_DATA_TEX_ENV_COMBINE_INTERPOLATE,
            PICA_DATA_TEX_ENV_COMBINE_INTERPOLATE ),

        PICA_CMD_DATA_TEX_ENV_BUFFER_COLOR_BLACK,
        PICA_CMD_DATA_TEX_ENV_SCALE( PICA_DATA_TEX_ENV_SCALE_1, PICA_DATA_TEX_ENV_SCALE_1 ),

        PICA_CMD_DATA_TEX_ENV_SRC(
            PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT,
            PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS_BUFFER_DMP,
            PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS,
            PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT,
            PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS_BUFFER_DMP,
            PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS ),
        PICA_CMD_HEADER_BURSTSEQ( PICA_REG_TEX_ENV4, 5 ),
        PICA_CMD_DATA_TEX_ENV_OPERAND(
            PICA_DATA_OPE_RGB_SRC_COLOR, PICA_DATA_OPE_RGB_SRC_COLOR, PICA_DATA_OPE_RGB_SRC_COLOR,
            PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_ALPHA ),
        PICA_CMD_DATA_TEX_ENV_COMBINE(
            PICA_DATA_TEX_ENV_COMBINE_INTERPOLATE,
            PICA_DATA_TEX_ENV_COMBINE_INTERPOLATE ),

        PICA_CMD_DATA_TEX_ENV_BUFFER_COLOR_WHITE,
        PICA_CMD_DATA_TEX_ENV_SCALE( PICA_DATA_TEX_ENV_SCALE_1, PICA_DATA_TEX_ENV_SCALE_1 ),

        PICA_CMD_DATA_TEX_ENV_BUFFER_COLOR_BLACK,
        PICA_CMD_HEADER_SINGLE( PICA_REG_TEX_ENV_BUFFER_COLOR ),

        PICA_CMD_DATA_TEX_ENV_SRC(
            PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR,
            PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS,
            PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS,
            PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR,
            PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS,
            PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS ),
        PICA_CMD_HEADER_BURSTSEQ( PICA_REG_TEX_ENV5, 5 ),
        PICA_CMD_DATA_TEX_ENV_OPERAND(
            PICA_DATA_OPE_RGB_SRC_COLOR, PICA_DATA_OPE_RGB_SRC_COLOR, PICA_DATA_OPE_RGB_SRC_COLOR,
            PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_ALPHA ),
        PICA_CMD_DATA_TEX_ENV_COMBINE(
            PICA_DATA_TEX_ENV_COMBINE_MODULATE,
            PICA_DATA_TEX_ENV_COMBINE_MODULATE ),
        PICA_CMD_DATA_TEX_ENV_BUFFER_COLOR_BLACK,
        PICA_CMD_DATA_TEX_ENV_SCALE( PICA_DATA_TEX_ENV_SCALE_1, PICA_DATA_TEX_ENV_SCALE_1 ),

        NW_LYT_CMD_DATA_TEX_ENV_BUF_INPUT_THROUGH,
        PICA_CMD_HEADER_SINGLE_BE( PICA_REG_TEX_ENV_BUFFER_INPUT, 0x2 ) // 0x0e0
    };

    nw::ut::Color8 blend = pMaterial->GetColor(TEVKONSTSEL_K5);
    nw::ut::Color8 white = pMaterial->GetColor(INTERPOLATECOLOR_WHITE);
    nw::ut::Color8 black = pMaterial->GetColor(INTERPOLATECOLOR_BLACK);

    const int cmdPosBlend = 4;
    const int cmdPosWhite = 10;
    const int cmdPosBlack = 12;

    if (mCurrentTexEnvType != TEX_ENV_TYPE_2_TEX)
    {
        *reinterpret_cast< nw::ut::Color8* >( &command[ cmdPosBlend ] ) = blend;
        *reinterpret_cast< nw::ut::Color8* >( &command[ cmdPosWhite ] ) = white;
        *reinterpret_cast< nw::ut::Color8* >( &command[ cmdPosBlack ] ) = black;

        NW_FONT_RECTDRAWER_ADD_COMMAND( command, sizeof( command ) );
    }
    else if (*reinterpret_cast<nw::ut::Color8* >(&command[cmdPosBlend]) != blend || 
            *reinterpret_cast<nw::ut::Color8* >(&command[cmdPosWhite]) != white ||
            *reinterpret_cast<nw::ut::Color8* >(&command[cmdPosBlack]) != black)
            {

        *reinterpret_cast<nw::ut::Color8*>(&command[cmdPosBlend]) = blend;
        *reinterpret_cast<nw::ut::Color8*>(&command[cmdPosWhite]) = white;
        *reinterpret_cast<nw::ut::Color8*>(&command[cmdPosBlack]) = black;

        NW_FONT_RECTDRAWER_ADD_SINGLE_COMMAND(
            PICA_CMD_HEADER_SINGLE( PICA_REG_TEX_ENV3 + TEX_ENV_COLOR_OFFS ), command[cmdPosBlend]);

        NW_FONT_RECTDRAWER_ADD_SINGLE_COMMAND(
            PICA_CMD_HEADER_SINGLE(PICA_REG_TEX_ENV4 + TEX_ENV_COLOR_OFFS), command[cmdPosWhite]);

        NW_FONT_RECTDRAWER_ADD_SINGLE_COMMAND(PICA_CMD_HEADER_SINGLE(PICA_REG_TEX_ENV_BUFFER_COLOR), command[cmdPosBlack]);
    }
}

void Drawer::SetUpTexEnvType3(const Material* __restrict pMaterial)
{
    static u32 command[] NW_LYT_DRAWER_ALIGN_32 ={
        PICA_CMD_DATA_TEX_ENV_SRC(
            PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0,
            PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT,
            PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT,
            PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0,
            PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT,
            PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT ),
        PICA_CMD_HEADER_BURSTSEQ( PICA_REG_TEX_ENV1, 5 ),
        PICA_CMD_DATA_TEX_ENV_OPERAND(
            PICA_DATA_OPE_RGB_SRC_COLOR, PICA_DATA_OPE_RGB_SRC_ALPHA, PICA_DATA_OPE_RGB_SRC_COLOR,
            PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_ALPHA ),
        PICA_CMD_DATA_TEX_ENV_COMBINE(
            PICA_DATA_TEX_ENV_COMBINE_MODULATE,
            PICA_DATA_TEX_ENV_COMBINE_MODULATE ),

        PICA_CMD_DATA_TEX_ENV_BUFFER_COLOR_BLACK,
        PICA_CMD_DATA_TEX_ENV_SCALE( PICA_DATA_TEX_ENV_SCALE_1, PICA_DATA_TEX_ENV_SCALE_1 ),

        PICA_CMD_DATA_TEX_ENV_SRC(
            PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE1,
            PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT,
            PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS,
            PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE1,
            PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT,
            PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS ),
        PICA_CMD_HEADER_BURSTSEQ( PICA_REG_TEX_ENV2, 5 ),
        PICA_CMD_DATA_TEX_ENV_OPERAND(
            PICA_DATA_OPE_RGB_SRC_COLOR, PICA_DATA_OPE_RGB_SRC_B_DMP, PICA_DATA_OPE_RGB_SRC_COLOR,
            PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_B_DMP, PICA_DATA_OPE_ALPHA_SRC_ALPHA ),
        PICA_CMD_DATA_TEX_ENV_COMBINE(
            PICA_DATA_TEX_ENV_COMBINE_MULT_ADD_DMP,
            PICA_DATA_TEX_ENV_COMBINE_MULT_ADD_DMP ),

        PICA_CMD_DATA_TEX_ENV_BUFFER_COLOR_BLACK,
        PICA_CMD_DATA_TEX_ENV_SCALE( PICA_DATA_TEX_ENV_SCALE_1, PICA_DATA_TEX_ENV_SCALE_1 ),

        PICA_CMD_DATA_TEX_ENV_SRC(
            PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE2,
            PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT,
            PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS,
            PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE2,
            PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT,
            PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS ),
        PICA_CMD_HEADER_BURSTSEQ(PICA_REG_TEX_ENV3, 5),
        PICA_CMD_DATA_TEX_ENV_OPERAND(
            PICA_DATA_OPE_RGB_SRC_COLOR, PICA_DATA_OPE_RGB_SRC_G_DMP, PICA_DATA_OPE_RGB_SRC_COLOR,
            PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_G_DMP, PICA_DATA_OPE_ALPHA_SRC_ALPHA ),
        PICA_CMD_DATA_TEX_ENV_COMBINE(
            PICA_DATA_TEX_ENV_COMBINE_MULT_ADD_DMP,
            PICA_DATA_TEX_ENV_COMBINE_MULT_ADD_DMP ),

        PICA_CMD_DATA_TEX_ENV_BUFFER_COLOR_BLACK,
        PICA_CMD_DATA_TEX_ENV_SCALE( PICA_DATA_TEX_ENV_SCALE_1, PICA_DATA_TEX_ENV_SCALE_1 ),

        PICA_CMD_DATA_TEX_ENV_SRC(
            PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT,
            PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS_BUFFER_DMP,
            PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS,
            PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT,
            PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS_BUFFER_DMP,
            PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS ),
        PICA_CMD_HEADER_BURSTSEQ( PICA_REG_TEX_ENV4, 5 ),
        PICA_CMD_DATA_TEX_ENV_OPERAND(
            PICA_DATA_OPE_RGB_SRC_COLOR, PICA_DATA_OPE_RGB_SRC_COLOR, PICA_DATA_OPE_RGB_SRC_COLOR,
            PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_ALPHA ),
        PICA_CMD_DATA_TEX_ENV_COMBINE(
            PICA_DATA_TEX_ENV_COMBINE_INTERPOLATE,
            PICA_DATA_TEX_ENV_COMBINE_INTERPOLATE ),

        PICA_CMD_DATA_TEX_ENV_BUFFER_COLOR_WHITE,
        PICA_CMD_DATA_TEX_ENV_SCALE(PICA_DATA_TEX_ENV_SCALE_1, PICA_DATA_TEX_ENV_SCALE_1),

        PICA_CMD_DATA_TEX_ENV_BUFFER_COLOR_BLACK,
        PICA_CMD_HEADER_SINGLE(PICA_REG_TEX_ENV_BUFFER_COLOR),

        PICA_CMD_DATA_TEX_ENV_SRC(
            PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR,
            PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS,
            PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS,
            PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR,
            PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS,
            PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS ),
        PICA_CMD_HEADER_BURSTSEQ( PICA_REG_TEX_ENV5, 5 ),
        PICA_CMD_DATA_TEX_ENV_OPERAND(
            PICA_DATA_OPE_RGB_SRC_COLOR, PICA_DATA_OPE_RGB_SRC_COLOR, PICA_DATA_OPE_RGB_SRC_COLOR,
            PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_ALPHA ),
        PICA_CMD_DATA_TEX_ENV_COMBINE(PICA_DATA_TEX_ENV_COMBINE_MODULATE,PICA_DATA_TEX_ENV_COMBINE_MODULATE),
        PICA_CMD_DATA_TEX_ENV_BUFFER_COLOR_BLACK,
        PICA_CMD_DATA_TEX_ENV_SCALE(PICA_DATA_TEX_ENV_SCALE_1, PICA_DATA_TEX_ENV_SCALE_1),

        NW_LYT_CMD_DATA_TEX_ENV_BUF_INPUT_THROUGH,
        PICA_CMD_HEADER_SINGLE_BE(PICA_REG_TEX_ENV_BUFFER_INPUT, 0x2)
    };

    nw::ut::Color8 blend = pMaterial->GetColor(TEVKONSTSEL_K5);
    nw::ut::Color8 white = pMaterial->GetColor(INTERPOLATECOLOR_WHITE);
    nw::ut::Color8 black = pMaterial->GetColor(INTERPOLATECOLOR_BLACK);

    const int cmdPosBlend0 = 4;
    const int cmdPosBlend1 = 10;
    const int cmdPosBlend2 = 16;
    const int cmdPosWhite = 22;
    const int cmdPosBlack = 24;

    if (mCurrentTexEnvType != TEX_ENV_TYPE_3_TEX)
    {
        *reinterpret_cast< nw::ut::Color8* >( &command[ cmdPosBlend0 ] ) = blend;
        *reinterpret_cast< nw::ut::Color8* >( &command[ cmdPosBlend1 ] ) = blend;
        *reinterpret_cast< nw::ut::Color8* >( &command[ cmdPosBlend2 ] ) = blend;
        *reinterpret_cast< nw::ut::Color8* >( &command[ cmdPosWhite ] ) = white;
        *reinterpret_cast< nw::ut::Color8* >( &command[ cmdPosBlack ] ) = black;

        NW_FONT_RECTDRAWER_ADD_COMMAND( command, sizeof( command ) );
    }
    else if (*reinterpret_cast<nw::ut::Color8*>(&command[cmdPosBlend0]) != blend ||
            *reinterpret_cast<nw::ut::Color8*>(&command[cmdPosWhite]) != white ||
            *reinterpret_cast<nw::ut::Color8*>(&command[cmdPosBlack]) != black )
            {
        *reinterpret_cast<nw::ut::Color8*>(&command[cmdPosBlend0]) = blend;
        *reinterpret_cast<nw::ut::Color8*>(&command[cmdPosBlend1]) = blend;
        *reinterpret_cast<nw::ut::Color8*>(&command[cmdPosBlend2]) = blend;
        *reinterpret_cast<nw::ut::Color8*>(&command[cmdPosWhite])  = white;
        *reinterpret_cast<nw::ut::Color8*>(&command[cmdPosBlack])  = black;

        NW_FONT_RECTDRAWER_ADD_SINGLE_COMMAND(PICA_CMD_HEADER_SINGLE(PICA_REG_TEX_ENV1 + TEX_ENV_COLOR_OFFS), command[cmdPosBlend0]);

        NW_FONT_RECTDRAWER_ADD_SINGLE_COMMAND(PICA_CMD_HEADER_SINGLE(PICA_REG_TEX_ENV2 + TEX_ENV_COLOR_OFFS), command[cmdPosBlend1]);

        NW_FONT_RECTDRAWER_ADD_SINGLE_COMMAND(PICA_CMD_HEADER_SINGLE(PICA_REG_TEX_ENV3 + TEX_ENV_COLOR_OFFS), command[cmdPosBlend2]);

        NW_FONT_RECTDRAWER_ADD_SINGLE_COMMAND(PICA_CMD_HEADER_SINGLE(PICA_REG_TEX_ENV4 + TEX_ENV_COLOR_OFFS), command[cmdPosWhite]);

        NW_FONT_RECTDRAWER_ADD_SINGLE_COMMAND(PICA_CMD_HEADER_SINGLE(PICA_REG_TEX_ENV_BUFFER_COLOR), command[cmdPosBlack]);
    }
}

void Drawer::SetUpTextures(const Material* __restrict pMaterial, bool addDisableCommand)
{
    static u32 command[] NW_LYT_DRAWER_ALIGN_32 ={

        PICA_CMD_DATA_ZERO(), PICA_CMD_HEADER_SINGLE(PICA_REG_TEXTURE0_SIZE),

        PICA_CMD_DATA_ZERO(), PICA_CMD_HEADER_SINGLE(PICA_REG_TEXTURE0_ADDR1),

        PICA_CMD_DATA_ZERO(), PICA_CMD_HEADER_SINGLE(PICA_REG_TEXTURE0_FORMAT),

        PICA_CMD_DATA_ZERO(), PICA_CMD_HEADER_SINGLE(PICA_REG_TEXTURE0_WRAP_FILTER),

        PICA_CMD_DATA_ZERO(), PICA_CMD_HEADER_SINGLE(PICA_REG_TEXTURE1_SIZE),

        PICA_CMD_DATA_ZERO(), PICA_CMD_HEADER_SINGLE(PICA_REG_TEXTURE1_ADDR),

        PICA_CMD_DATA_ZERO(), PICA_CMD_HEADER_SINGLE(PICA_REG_TEXTURE1_FORMAT),

        PICA_CMD_DATA_ZERO(), PICA_CMD_HEADER_SINGLE(PICA_REG_TEXTURE1_WRAP_FILTER),

        PICA_CMD_DATA_ZERO(), PICA_CMD_HEADER_SINGLE(PICA_REG_TEXTURE2_SIZE),

        PICA_CMD_DATA_ZERO(), PICA_CMD_HEADER_SINGLE(PICA_REG_TEXTURE2_ADDR),

        PICA_CMD_DATA_ZERO(), PICA_CMD_HEADER_SINGLE(PICA_REG_TEXTURE2_FORMAT),

        PICA_CMD_DATA_ZERO(), PICA_CMD_HEADER_SINGLE(PICA_REG_TEXTURE2_WRAP_FILTER),
    };

    enum
    {
        idxWidthHeight = 0,
        idxTextureAddr = 2,
        idxFormat      = 4,
        idxWrapFilter  = 6,
        settingSize    = 8,
        commandSize    = sizeof(u32) * settingSize
    };

    if (pMaterial == NULL)
    {
        command[idxTextureAddr] = PICA_CMD_DATA_ZERO();
        
        NW_ASSERT(mUniformAddrIndex == 0);

        if (addDisableCommand){
            DisableTextures();
        }
        return;
    }

    bool isTexCoordModified = true;
        if (mUniformAddrIndex > 0)
        {
            NW_ASSERT(mPreviousTexEnvType == mCurrentTexEnvType&&  (mCurrentTexEnvType == TEX_ENV_TYPE_0_TEX || mCurrentTexEnvType == TEX_ENV_TYPE_1_TEX)); 
            isTexCoordModified = false;
        }

    const u32 num = ut::Min((u32)pMaterial->GetTexMapNum(), (u32)TexMapMax);

    bool isTexturesModified = false;

    int texbit = 0;
    for (int i = 0; i < num; ++i)
    {
        texbit |= 1 << i;
        
        const TexMap& __restrict texMap = pMaterial->GetTexMap( i );
        u32* __restrict c = &command[ i * settingSize ];

        if (c[idxTextureAddr ] != texMap.GetPhysicalAddress() / 8|| c[idxWrapFilter] != texMap.GetU32WrapFilter())
        {
            c[idxWidthHeight ] = texMap.GetU32WidthHeight();
            c[idxTextureAddr ] = texMap.GetPhysicalAddress() / 8;
            c[idxFormat      ] = texMap.GetU32Format();
            c[idxWrapFilter  ] = texMap.GetU32WrapFilter();
            isTexturesModified = true;
        }
    }

    if (isTexCoordModified || isTexturesModified)
    {
        this->FlushBuffer();
    }

    const u32 texFuncData = PICA_CMD_DATA_TEXTURE_FUNC(PICA_DATA_TEXTURE0_SAMPLER_TYPE_TEXTURE_FALSE,0,0,0,0,0,1) | texbit;

    if (isTexCoordModified)
    {
        NW_FONT_RECTDRAWER_ADD_SINGLE_COMMAND(PICA_CMD_HEADER_SINGLE_BE(PICA_REG_VS_OUT_ATTR_CLK, 0x2), PICA_CMD_DATA_VS_GS_OUT_ATTR_CLK(0,0,0,0,0,0,0) | texbit << 8);

        NW_FONT_RECTDRAWER_ADD_SINGLE_COMMAND(PICA_CMD_HEADER_SINGLE_BE(PICA_REG_TEXTURE_FUNC, 0xB), texFuncData);
    }

    if (isTexCoordModified || isTexturesModified)
    {
        NW_FONT_RECTDRAWER_ADD_SINGLE_COMMAND(PICA_CMD_HEADER_SINGLE_BE(PICA_REG_TEXTURE_FUNC, 0x4), texFuncData);
    }
    
    if (isTexturesModified)
    {
        NW_FONT_RECTDRAWER_ADD_COMMAND(command, commandSize * num);
    }
}

void Drawer::UniformAndDraw(){

    {
    	const u32 size = SetUniformCommand(this->mUniformAddrBuffer, this->mUniformAddrIndex);
        NW_FONT_RECTDRAWER_ADD_COMMAND(this->mUniformAddrBuffer, size);
    }

    if (mUniformMtxIndex > 0)
    {
        Base::AddUniformMtx();
    }

    if (mUniformDataIndex > 0)
    {
        const u32 size = SetUniformCommand(this->mUniformDataBuffer, this->mUniformDataIndex);
        NW_FONT_RECTDRAWER_ADD_COMMAND(this->mUniformDataBuffer, size);
        mUniformDataIndex = 0;
    }
    const int vtxNum = DRAW_VTX_NUM * mUniformAddrIndex;

    const u32 vtxIdxAddrOffset = GetVertexIndexAddressOffset(vtxNum);

    NW_FONT_RECTDRAWER_ADD_SINGLE_COMMAND(PICA_CMD_HEADER_SINGLE(PICA_REG_VS_INT0),PICA_CMD_DATA_VS_INT(this->mTexCoordNum, 0, 0));

    font::internal::SetVertexNumCmd(&__cb_current_command_buffer, vtxIdxAddrOffset, vtxNum);

    NW_FONT_RECTDRAWER_ADD_COMMAND(GetDrawCommands(), GetDrawCommandSize());

    mUniformAddrIndex = 0;
}

void Drawer::SetUpTextBox(const TextBox*  __restrict pTextBox,const Material* __restrict pMaterial,const DrawInfo& drawInfo){
    font::DispStringBuffer* __restrict pStringBuffer = pTextBox->GetDispStringBuffer();

    if (pStringBuffer->IsCommandEmpty())
    {
        return;
    }

    mCurrentTexEnvType = TEX_ENV_TYPE_TEXT;

    mIsBlendDefault = true;

    mAlphaTestEnable = false;

    pTextBox->GetTextGlobalMtx((nw::math::MTX34*)this->mUniformMtx);
    mUniformMtxIndex = 3;
    Base::AddUniformMtx();

    font::WideTextWriter& writer = drawInfo.GetGraphicsResource()->GetTextWriter();
    writer.SetDispStringBuffer(pStringBuffer);

    ut::Color8 topCol = pTextBox->GetTextColor(TEXTCOLOR_TOP);
    ut::Color8 btmCol = pTextBox->GetTextColor(TEXTCOLOR_BOTTOM);
    writer.SetGradationMode(topCol != btmCol ? font::CharWriter::GRADMODE_V: font::CharWriter::GRADMODE_NONE);
    writer.SetTextColor(topCol, btmCol);

    writer.SetColorMapping(pMaterial->GetColor(INTERPOLATECOLOR_BLACK),pMaterial->GetColor(INTERPOLATECOLOR_WHITE));
    writer.SetAlpha(pTextBox->GetGlobalAlpha());

    writer.UseCommandBuffer();

    writer.SetDispStringBuffer(NULL);

    this->SetUpTextures( NULL );
}

void Drawer::SetUpGLTexEnvUser(const Material* __restrict pMaterial){
    const int tevSrc[] =
    {
        PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0,
        PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE1,
        PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE2,
        PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE3,
        PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT,
        PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR,
        PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS,
        PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS_BUFFER_DMP,
    };

    const int tevMode[] =
    {
        PICA_DATA_TEX_ENV_COMBINE_REPLACE,
        PICA_DATA_TEX_ENV_COMBINE_MODULATE,
        PICA_DATA_TEX_ENV_COMBINE_ADD,
        PICA_DATA_TEX_ENV_COMBINE_ADD_SIGNED,
        PICA_DATA_TEX_ENV_COMBINE_INTERPOLATE,
        PICA_DATA_TEX_ENV_COMBINE_SUBTRACT,
        PICA_DATA_TEX_ENV_COMBINE_ADD_MULT_DMP,
        PICA_DATA_TEX_ENV_COMBINE_MULT_ADD_DMP,
    };

    const int tevOpRgb[] =
    {
        PICA_DATA_OPE_RGB_SRC_COLOR,
        PICA_DATA_OPE_RGB_ONE_MINUS_SRC_COLOR,
        PICA_DATA_OPE_RGB_SRC_ALPHA,
        PICA_DATA_OPE_RGB_ONE_MINUS_SRC_ALPHA,
        PICA_DATA_OPE_RGB_SRC_R_DMP,
        PICA_DATA_OPE_RGB_ONE_MINUS_SRC_R_DMP,
        PICA_DATA_OPE_RGB_SRC_G_DMP,
        PICA_DATA_OPE_RGB_ONE_MINUS_SRC_G_DMP,
        PICA_DATA_OPE_RGB_SRC_B_DMP,
        PICA_DATA_OPE_RGB_ONE_MINUS_SRC_B_DMP,
    };

    const int tevOpAlp[] =
    {
        PICA_DATA_OPE_ALPHA_SRC_ALPHA,
        PICA_DATA_OPE_ALPHA_ONE_MINUS_SRC_ALPHA,
        PICA_DATA_OPE_ALPHA_SRC_R_DMP,
        PICA_DATA_OPE_ALPHA_ONE_MINUS_SRC_R_DMP,
        PICA_DATA_OPE_ALPHA_SRC_G_DMP,
        PICA_DATA_OPE_ALPHA_ONE_MINUS_SRC_G_DMP,
        PICA_DATA_OPE_ALPHA_SRC_B_DMP,
        PICA_DATA_OPE_ALPHA_ONE_MINUS_SRC_B_DMP,
    };

    const u32 scale[] =
    {
        PICA_DATA_TEX_ENV_SCALE_1,
        PICA_DATA_TEX_ENV_SCALE_2,
        PICA_DATA_TEX_ENV_SCALE_4,
    };

    const u32 reg[] =
    {
        PICA_REG_TEX_ENV0,
        PICA_REG_TEX_ENV1,
        PICA_REG_TEX_ENV2,
        PICA_REG_TEX_ENV3,
        PICA_REG_TEX_ENV4,
        PICA_REG_TEX_ENV5,
    };

    u32 bufferInput = NW_LYT_CMD_DATA_TEX_ENV_BUF_INPUT_THROUGH;

    u32 stageNum = nw::ut::Min((u32)TEX_ENV_STAGE_NUM, (u32)pMaterial->GetTevStageNum());
    for (int i = 0; i < stageNum; ++i)
    {
        const TevStage& tevStage = pMaterial->GetTevStage(i);

        nw::ut::Color8 rgb = pMaterial->GetColor( tevStage.GetKonstSelRgb() );
        nw::ut::Color8 alpha = pMaterial->GetColor( tevStage.GetKonstSelAlpha() );

        int stageNo = i + ( TEX_ENV_STAGE_NUM - stageNum );

        if (i == 0)
        {

            if ((tevStage.GetSrcRgb0() != TEVSRC_CONSTANT &&
                   tevStage.GetSrcRgb1() != TEVSRC_CONSTANT &&
                   tevStage.GetSrcRgb2() != TEVSRC_CONSTANT) ||
                 (tevStage.GetSrcAlpha0() != TEVSRC_CONSTANT &&
                   tevStage.GetSrcAlpha1() != TEVSRC_CONSTANT &&
                   tevStage.GetSrcAlpha2() != TEVSRC_CONSTANT ) )
                   {
                for (int j = 1; j <= stageNo; ++j)
                {
                    NW_FONT_RECTDRAWER_ADD_SINGLE_COMMAND(
                        PICA_CMD_HEADER_BURSTSEQ( reg[ j ], 5 ),
                        PICA_CMD_DATA_TEX_ENV_SRC(
                            PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS,
                            PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT,
                            PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT,
                            PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS,
                            PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT,
                            PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT ) );

                    static const u32 command[] ={
                        PICA_CMD_DATA_TEX_ENV_OPERAND(
                            PICA_DATA_OPE_RGB_SRC_COLOR,
                            PICA_DATA_OPE_RGB_SRC_COLOR,
                            PICA_DATA_OPE_RGB_SRC_COLOR,
                            PICA_DATA_OPE_ALPHA_SRC_ALPHA,
                            PICA_DATA_OPE_ALPHA_SRC_ALPHA,
                            PICA_DATA_OPE_ALPHA_SRC_ALPHA ),
                        PICA_CMD_DATA_TEX_ENV_COMBINE(
                            PICA_DATA_TEX_ENV_COMBINE_REPLACE,
                            PICA_DATA_TEX_ENV_COMBINE_REPLACE ),
                        PICA_CMD_DATA_TEX_ENV_BUFFER_COLOR_WHITE,
                        PICA_CMD_DATA_TEX_ENV_SCALE(
                            PICA_DATA_TEX_ENV_SCALE_1, PICA_DATA_TEX_ENV_SCALE_1 ),
                    };

                    NW_FONT_RECTDRAWER_ADD_COMMAND( command, sizeof( command ) );
                }

                stageNo = 0;
            }
        }

        if (1 <= i && i <= 4)
        {
            bufferInput |= PICA_CMD_DATA_TEX_ENV_BUFFER_INPUT(
                stageNo,
                ( tevStage.GetSavePrevRgb() ) ?
                    PICA_DATA_TEX_ENV_BUFFER_INPUT_PREVIOUS :
                    PICA_DATA_TEX_ENV_BUFFER_INPUT_PREVIOUS_BUFFER_DMP,
                ( tevStage.GetSavePrevAlpha() ) ?
                    PICA_DATA_TEX_ENV_BUFFER_INPUT_PREVIOUS :
                    PICA_DATA_TEX_ENV_BUFFER_INPUT_PREVIOUS_BUFFER_DMP );
        }

        const u32 command[] ={
            PICA_CMD_DATA_TEX_ENV_SRC(
                tevSrc[ tevStage.GetSrcRgb0() ],   tevSrc[ tevStage.GetSrcRgb1() ],  tevSrc[ tevStage.GetSrcRgb2() ],
                tevSrc[ tevStage.GetSrcAlpha0() ], tevSrc[ tevStage.GetSrcAlpha1()], tevSrc[ tevStage.GetSrcAlpha2() ] ),
            PICA_CMD_HEADER_BURSTSEQ( reg[ stageNo ], 5 ),
            PICA_CMD_DATA_TEX_ENV_OPERAND(
                tevOpRgb[ tevStage.GetOperandRgb0() ],   tevOpRgb[ tevStage.GetOperandRgb1() ],   tevOpRgb[ tevStage.GetOperandRgb2() ],
                tevOpAlp[ tevStage.GetOperandAlpha0() ], tevOpAlp[ tevStage.GetOperandAlpha1() ], tevOpAlp[ tevStage.GetOperandAlpha2() ] ),
            PICA_CMD_DATA_TEX_ENV_COMBINE( tevMode[ tevStage.GetCombineRgb() ], tevMode[ tevStage.GetCombineAlpha() ] ),
            NW_LYT_CMD_DATA_COLOR_ALPHA( rgb, alpha ),
            PICA_CMD_DATA_TEX_ENV_SCALE( scale[ tevStage.GetScaleRgb() ], scale[ tevStage.GetScaleAlpha() ] ),
        };

        NW_FONT_RECTDRAWER_ADD_COMMAND( command, sizeof( command ) );
    }

    {
        nw::ut::Color8 black = pMaterial->GetColor( TEVKONSTSEL_BUFFER );

        NW_FONT_RECTDRAWER_ADD_SINGLE_COMMAND(
            PICA_CMD_HEADER_SINGLE( PICA_REG_TEX_ENV_BUFFER_COLOR ),
            NW_LYT_CMD_DATA_COLOR(black) );

        NW_FONT_RECTDRAWER_ADD_SINGLE_COMMAND(
            PICA_CMD_HEADER_SINGLE_BE( PICA_REG_TEX_ENV_BUFFER_INPUT, 0x2 ),
            bufferInput );
    }
}

void Drawer::SetUpAlphaTest(const Material* __restrict pMaterial)
{
    const AlphaCompare& __restrict alphaCompare = pMaterial->GetAlphaCompare();
    register f32 refValue = alphaCompare.GetRef() * 255.f + 0.5f;

    const u32 alphaTest[] ={
        PICA_DATA_ALPHA_TEST_NEVER,
        PICA_DATA_ALPHA_TEST_LESS,
        PICA_DATA_ALPHA_TEST_LEQUAL,
        PICA_DATA_ALPHA_TEST_EQUAL,
        PICA_DATA_ALPHA_TEST_NOTEQUAL,
        PICA_DATA_ALPHA_TEST_GEQUAL,
        PICA_DATA_ALPHA_TEST_GREATER,
        PICA_DATA_ALPHA_TEST_ALWAYS,
    };

    NW_FONT_RECTDRAWER_ADD_SINGLE_COMMAND(PICA_CMD_HEADER_SINGLE(PICA_REG_FRAGOP_ALPHA_TEST),PICA_CMD_DATA_FRAGOP_ALPHA_TEST(true, alphaTest[alphaCompare.GetFunc()], (u8)refValue));
}

void Drawer::SetUpBlendMode(const Material* __restrict pMaterial)
{
    const BlendMode& blendMode = pMaterial->GetBlendMode();

    if (blendMode.GetBlendOp() == BLENDOP_DISABLE && blendMode.GetLogicOp() == LOGICOP_DISABLE)
    {
        u32 command[] =	{
            NW_FONT_COMMAND_SET_BLEND_FUNC(PICA_DATA_BLEND_EQUATION_ADD,PICA_DATA_BLEND_FUNC_ONE,PICA_DATA_BLEND_FUNC_ZERO)
        };

        NW_FONT_RECTDRAWER_ADD_COMMAND( command, sizeof( command ) );
    }
    else if ( blendMode.GetBlendOp() != BLENDOP_DISABLE )
    {

        const u32 blendOp[] ={
            0,
            PICA_DATA_BLEND_EQUATION_ADD,
            PICA_DATA_BLEND_EQUATION_SUBTRACT,
            PICA_DATA_BLEND_EQUATION_REVERSE_SUBTRACT,
        };

        const u32 srcFactor[] ={
            PICA_DATA_BLEND_FUNC_ZERO,
            PICA_DATA_BLEND_FUNC_ONE,
            PICA_DATA_BLEND_FUNC_DST_COLOR,
            PICA_DATA_BLEND_FUNC_ONE_MINUS_DST_COLOR,
            PICA_DATA_BLEND_FUNC_SRC_ALPHA,
            PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_ALPHA,
            PICA_DATA_BLEND_FUNC_DST_ALPHA,
            PICA_DATA_BLEND_FUNC_ONE_MINUS_DST_ALPHA,
        };

        const u32 dstFactor[] ={
            PICA_DATA_BLEND_FUNC_ZERO,
            PICA_DATA_BLEND_FUNC_ONE,
            PICA_DATA_BLEND_FUNC_SRC_COLOR,
            PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_COLOR,
            PICA_DATA_BLEND_FUNC_SRC_ALPHA,
            PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_ALPHA,
            PICA_DATA_BLEND_FUNC_DST_ALPHA,
            PICA_DATA_BLEND_FUNC_ONE_MINUS_DST_ALPHA,
        };

        const u32 command[] ={
            NW_FONT_COMMAND_SET_BLEND_FUNC(blendOp[blendMode.GetBlendOp()],srcFactor[ blendMode.GetSrcFactor()],dstFactor[blendMode.GetDstFactor()]),
        };
        NW_FONT_RECTDRAWER_ADD_COMMAND(command, sizeof(command));
    }
    else if ( blendMode.GetLogicOp() != LOGICOP_DISABLE )
    {

        const u32 logicOp[] ={
            0 , // DISABLE
            PICA_DATA_LOGIC_NOOP,
            PICA_DATA_LOGIC_CLEAR,
            PICA_DATA_LOGIC_SET,
            PICA_DATA_LOGIC_COPY,
            PICA_DATA_LOGIC_COPY_INVERTED,
            PICA_DATA_LOGIC_INVERT,
            PICA_DATA_LOGIC_AND,
            PICA_DATA_LOGIC_NAND,
            PICA_DATA_LOGIC_OR,
            PICA_DATA_LOGIC_NOR,
            PICA_DATA_LOGIC_XOR,
            PICA_DATA_LOGIC_EQUIV,
            PICA_DATA_LOGIC_AND_REVERSE,
            PICA_DATA_LOGIC_AND_INVERTED,
            PICA_DATA_LOGIC_OR_REVERSE,
            PICA_DATA_LOGIC_OR_INVERTED,
        };

        const u32 command[] ={
            NW_LYT_COMMAND_SET_LOGIC_OP( logicOp[ blendMode.GetLogicOp() ] ),
        };
        NW_FONT_RECTDRAWER_ADD_COMMAND( command, sizeof( command ) );
    }
}

}
}