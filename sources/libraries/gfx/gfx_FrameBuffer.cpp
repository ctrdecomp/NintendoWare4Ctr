#include <gles2/gl2.h>
#include <nn/gx.h>
#include <nw/gfx/gfx_CommandUtil.h>
#include <nw/gfx/gfx_FrameBuffer.h>
#include <nw/gfx/gfx_GlImplement.h>
#include <nw/ut/ut_Inlines.h>

namespace nw {
namespace gfx {


static inline u32 ColorToRGBA8(const nw::ut::FloatColor& color)
{
    
    u8 red = static_cast<u8>( 0.5f + nw::ut::Clamp(color.r, 0.0f, 1.0f) * 255.f );
    u8 green = static_cast<u8>( 0.5f + nw::ut::Clamp(color.g, 0.0f, 1.0f) * 255.f );
    u8 blue = static_cast<u8>( 0.5f + nw::ut::Clamp(color.b, 0.0f, 1.0f) * 255.f );
    u8 alpha = static_cast<u8>( 0.5f + nw::ut::Clamp(color.a, 0.0f, 1.0f) * 255.f );

    return (alpha) | (blue << 8) | (green << 16) | (red << 24);
}


static inline u16 ColorToRGB5A1(const nw::ut::FloatColor& color)
{
    u8 red = static_cast<u8>( 0.5f + nw::ut::Clamp(color.r, 0.0f, 1.0f) * 31.f );
    u8 green = static_cast<u8>( 0.5f + nw::ut::Clamp(color.g, 0.0f, 1.0f) * 31.f );
    u8 blue = static_cast<u8>( 0.5f + nw::ut::Clamp(color.b, 0.0f, 1.0f) * 31.f );
    u8 alpha = static_cast<u8>( 0.5f + nw::ut::Clamp(color.a, 0.0f, 1.0f) * 1.f );
    
    return static_cast<u16>( (alpha) | (blue << 1) | (green << 6) | (red << 11) );
}

static inline u16 ColorToRGB565(const nw::ut::FloatColor& color)
{
    u8 red = static_cast<u8>( 0.5f + nw::ut::Clamp(color.r, 0.0f, 1.0f) * 31.f );
    u8 green = static_cast<u8>( 0.5f + nw::ut::Clamp(color.g, 0.0f, 1.0f) * 63.f );
    u8 blue = static_cast<u8>( 0.5f + nw::ut::Clamp(color.b, 0.0f, 1.0f) * 31.f );
    
    return static_cast<u16>( (blue) | (green << 5) | (red << 11) );
}

static inline u16 ColorToRGBA4(const nw::ut::FloatColor& color)
{
    u8 red = static_cast<u8>( 0.5f + nw::ut::Clamp(color.r, 0.0f, 1.0f) * 15.f );
    u8 green = static_cast<u8>( 0.5f + nw::ut::Clamp(color.g, 0.0f, 1.0f) * 15.f );
    u8 blue = static_cast<u8>( 0.5f + nw::ut::Clamp(color.b, 0.0f, 1.0f) * 15.f );
    u8 alpha = static_cast<u8>( 0.5f + nw::ut::Clamp(color.a, 0.0f, 1.0f) * 15.f );
    
    return static_cast<u16>( (alpha) | (blue << 4) | (green << 8) | (red << 12) ); 
}

void FrameBufferObject::SetDescription(const Description& description) 
{
    m_Description = description;
    
    if (description.fboID == 0)
    {
        this->SetFboID(description.fboID);
    }
    else{
        m_Description = description;
    }
}

void FrameBufferObject::SetFboID(GLuint fboID)
{
    m_Description.fboID = fboID;
    
    if (fboID != 0)
    {
        internal::GetFrameBufferState(this->m_Description.fboID, &this->m_Description.colorAddress, &this->m_Description.depthAddress);
    }
}

void FrameBufferObject::ActivateBuffer() const
{
    const FrameBufferObject::Description& description = this->GetDescription();
    
        NW_ASSERT(description.height <= 1024);
        NW_ASSERT(description.width <= 1024);
        
        u32 depthFormat = 0;
        u32 colorFormat = 0;
        
        switch (description.depthFormat)
        {
        case GL_DEPTH_COMPONENT16:     depthFormat = 0; break;
        case GL_DEPTH_COMPONENT24_OES: depthFormat = 2; break;
        case GL_DEPTH24_STENCIL8_EXT:  depthFormat = 3; break;
        default:
            NW_ASSERTMSG( description.depthAddress == NULL, "Unknown depth format" );
        }
        
        switch (description.colorFormat)
        {
        case GL_RGBA8_OES: colorFormat = (0 << 16) | 0x2; break;
        case GL_GAS_DMP:   colorFormat = (0 << 16) | 0x2; break;
        case GL_RGB5_A1:   colorFormat = (2 << 16); break;
        case GL_RGB565:    colorFormat = (3 << 16); break;
        case GL_RGBA4:     colorFormat = (4 << 16); break;
        default:
            NW_ASSERTMSG( description.colorAddress == NULL, "Unknown color format" );
        }
        
        if (description.depthAddress == NULL)
        {
            u32 COMMAND[] = {
                0x1,
                internal::MakeCommandHeader(PICA_REG_COLOR_DEPTH_BUFFER_CLEAR1, 1, false, 0xf),
                0x1,
                internal::MakeCommandHeader(PICA_REG_COLOR_DEPTH_BUFFER_CLEAR0, 1, false, 0xf),
                
                colorFormat,
                internal::MakeCommandHeader( PICA_REG_RENDER_BUFFER_COLOR_MODE, 1, false, 0xf),
                
                (description.colorAddress != NULL) ? (nngxGetPhysicalAddr(description.colorAddress) >> 3) : 0,
                internal::MakeCommandHeader(PICA_REG_RENDER_BUFFER_COLOR_ADDR, 1, false, 0xf),
                
                description.width | ((description.height - 1) << 12) | 0x01000000,
                internal::MakeCommandHeader( PICA_REG_RENDER_BUFFER_RESOLUTION0, 1, false, 0xf ),
                
                description.width | ((description.height - 1) << 12) | 0x01000000,
                internal::MakeCommandHeader( PICA_REG_RENDER_BUFFER_RESOLUTION1, 1, false, 0xf ),
                
                (description.useBlock32)? 1 : 0,
                internal::MakeCommandHeader( PICA_REG_RENDER_BLOCK_FORMAT, 1, false, 0xf ),
            };
            
            internal::NWUseCmdlist<sizeof(COMMAND)>( &COMMAND[0] );
        }
        else{
            u32 COMMAND[] = {
                0x1,
                internal::MakeCommandHeader( PICA_REG_COLOR_DEPTH_BUFFER_CLEAR1, 1, false, 0xf ),
                0x1,
                internal::MakeCommandHeader( PICA_REG_COLOR_DEPTH_BUFFER_CLEAR0, 1, false, 0xf ),
                
                (description.depthAddress != NULL) ? (nngxGetPhysicalAddr( description.depthAddress ) >> 3) : 0,
                internal::MakeCommandHeader( PICA_REG_RENDER_BUFFER_DEPTH_ADDR, 3, true, 0xf ),
                (description.colorAddress != NULL) ? (nngxGetPhysicalAddr( description.colorAddress ) >> 3) : 0,
                description.width | ((description.height - 1) << 12) | 0x01000000,
                
                description.width | ((description.height - 1) << 12) | 0x01000000,
                internal::MakeCommandHeader( PICA_REG_RENDER_BUFFER_RESOLUTION1, 1, false, 0xf ),
                
                depthFormat,
                internal::MakeCommandHeader( PICA_REG_RENDER_BUFFER_DEPTH_MODE, 1, false, 0xf ),
                
                colorFormat,
                internal::MakeCommandHeader( PICA_REG_RENDER_BUFFER_COLOR_MODE, 1, false, 0xf ),
                
                (description.useBlock32)? 1 : 0,
                internal::MakeCommandHeader( PICA_REG_RENDER_BLOCK_FORMAT, 1, false, 0xf ),
            };
        internal::NWUseCmdlist<sizeof(COMMAND)>( &COMMAND[0] );
    }
}

void FrameBufferObject::ClearBuffer(u32 mask,const nw::ut::FloatColor& clearColor, f32 clearDepth, u8 clearStencil) const
{
    const FrameBufferObject::Description& description = this->GetDescription();
    
    u32 colorAddress = 0;
    u32 depthAddress = 0;
    u32 colorWidth = 0;
    u32 depthWidth = 0;
    u32 clearColorHW = 0;
    u32 clearDepthHW = 0;
    u32 colorSize = 0;
    u32 depthSize = 0;
    
    if (mask & CLEAR_MASK_COLOR)
    {
        colorAddress = description.colorAddress;
        
        switch (description.colorFormat)
        {
        case GL_RGBA8_OES:{
                colorWidth = 32;
                clearColorHW = ColorToRGBA8( clearColor );
            }
            break;
        case GL_GAS_DMP:{
                colorWidth = 32; 
                clearColorHW = ColorToRGBA8( clearColor );
            }
            break;
        case GL_RGB5_A1:{
                colorWidth = 16; 
                clearColorHW = ColorToRGB5A1( clearColor );
                clearColorHW = (clearColorHW << 16) | clearColorHW;
            }
            break;
        case GL_RGB565:{
                colorWidth = 16; 
                clearColorHW = ColorToRGB565( clearColor );
                clearColorHW = (clearColorHW << 16) | clearColorHW;
            }
            break;
        case GL_RGBA4:{
                colorWidth = 16; 
                clearColorHW = ColorToRGBA4( clearColor );
                clearColorHW = (clearColorHW << 16) | clearColorHW;
            }
            break;
        default:
            NW_FATAL_ERROR("Unknown color format");
        }
        
        colorSize = description.height * description.width * colorWidth / 8;
    }
    
    if (mask & CLEAR_MASK_DEPTH)
    {
        depthAddress = description.depthAddress;
        
        if (depthAddress == 0)
        {
            depthWidth = 0;
            clearDepthHW = 0;
        }
        else{
            switch (description.depthFormat)
            {
            case GL_DEPTH_COMPONENT16:{
                    depthWidth = 16; 
                    clearDepthHW = static_cast<u16>( nw::ut::Clamp(clearDepth, 0.0f, 1.0f) * static_cast<f32>(0xFFFF) );
                    clearDepthHW = (clearDepthHW << 16) | clearDepthHW;
                }
                break;
            case GL_DEPTH_COMPONENT24_OES:{
                    depthWidth = 24; 
                    clearDepthHW = static_cast<u32>( nw::ut::Clamp(clearDepth, 0.0f, 1.0f) * static_cast<f32>(0xFFFFFF) );
                }
                break;
            case GL_DEPTH24_STENCIL8_EXT:{
                    depthWidth = 32; 
                    clearDepthHW = static_cast<u32>( nw::ut::Clamp(clearDepth, 0.0f, 1.0f) * static_cast<f32>(0xFFFFFF) );
                    clearDepthHW |= clearStencil << 24;
                }
                break;
            default:
                NW_FATAL_ERROR("Unknown depth format");
            }
        }
        
        depthSize = description.height * description.width * depthWidth / 8;
    }
    
    internal::nwgfxClear(colorAddress, colorSize, clearColorHW, colorWidth,depthAddress, depthSize, clearDepthHW, depthWidth);
}


}
}