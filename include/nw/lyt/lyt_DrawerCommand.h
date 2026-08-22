#pragma once

#define NW_LYT_CMD_DATA_COLOR( color8 ) \
    ( color8.r | color8.g << 8 | color8.b << 16 | color8.a << 24 )

#define NW_LYT_CMD_DATA_COLOR_ALPHA( color8, alpha8 ) \
    ( color8.r | color8.g << 8 | color8.b << 16 | alpha8.a << 24 )

#define NW_LYT_CMD_DATA_TEX_ENV_BUF_INPUT_THROUGH \
    PICA_CMD_DATA_ZERO()

#define NW_LYT_COMMAND_SET_LOGIC_OP( logicOp ) \
    PICA_CMD_DATA_COLOR_OPERATION( \
        PICA_DATA_FRAGOP_MODE_DMP, \
        PICA_DATA_ENABLE_COLOR_LOGIC_OP ), \
    PICA_CMD_HEADER_SINGLE( PICA_REG_COLOR_OPERATION ), \
    PICA_CMD_DATA_LOGIC_OP( logicOp ), \
    PICA_CMD_HEADER_SINGLE( PICA_REG_LOGIC_OP )