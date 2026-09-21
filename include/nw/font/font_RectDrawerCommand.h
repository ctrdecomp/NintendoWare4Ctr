#pragma once

#define NW_FONT_ADD_COMMANDS_PTR( ptr, command, size )  \
    std::memcpy( ptr, command, size );                  \
    (ptr) += (size) >> 2

#define NW_FONT_ADD_SINGLE_COMMAND_PTR( ptr, header, data ) \
    *(ptr)++ = (data);                                      \
    *(ptr)++ = (header)

#define NW_FONT_RECTDRAWER_ADD_COMMAND( command, size )         \
    std::memcpy( __cb_current_command_buffer, command, size );  \
    __cb_current_command_buffer += (size) >> 2

#define NW_FONT_RECTDRAWER_ADD_SINGLE_COMMAND( header, data )   \
    *__cb_current_command_buffer++ = (data);                    \
    *__cb_current_command_buffer++ = (header)

#define NW_FONT_CMD_BEGIN_COMMAND_BLOCK                                 \
    {                                                                   \
        const u32 commands[] = {

#define NW_FONT_CMD_END_COMMAND_BLOCK                                   \
        };                                                              \
        NW_FONT_RECTDRAWER_ADD_COMMAND( commands, sizeof(commands) );   \
    }

enum
{
    NW_FONT_CMD_CULL_FACE_DISABLE,
    NW_FONT_CMD_CULL_FACE_FRONT,
    NW_FONT_CMD_CULL_FACE_BACK,

    NW_FONT_CMD_CULL_FACE_MASK = 0x3
};

namespace nw {
namespace font {

struct ColorBufferInfo
{
    u16 width;
    u16 height;
    u8 depth;
};

}
}