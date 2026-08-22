/*---------------------------------------------------------------------------*
  Project:  NintendoWare

  Copyright (C)Nintendo/HAL Laboratory, Inc.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary
  information of Nintendo and/or its licensed developers and are protected by
  national and international copyright laws. They may not be disclosed to third
  parties or copied or duplicated in any form, in whole or in part, without the
  prior written consent of Nintendo.

  The content herein is highly confidential and should be handled accordingly.
 *---------------------------------------------------------------------------*/
#ifndef NW_GFX_DISPLAYLIST_H_
#define NW_GFX_DISPLAYLIST_H_

#include <nw/types.h>
#include <nw/ut/ut_Inlines.h>
#include <nw/gfx/gfx_GlImplement.h>
#include <nw/dev.h>

#include <gles2/gl2.h>
#include <gles2/gl2ext.h>
#include <nn/gx.h>
#include <cstring>

namespace nw {
namespace gfx {
namespace internal {


//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class GpuCommand
{
public:

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    GpuCommand()
     : m_CurrentBuffer( NULL ),
       m_StartAddress( NULL ),
       m_CurrentHeader( NULL ),
       m_Size(0)
    {}

    //--------------------------------------------------------------------------
    //
    //
    //
    //--------------------------------------------------------------------------
    void Begin(void* addr)
    {
        NW_NULL_ASSERT( addr );

        m_CurrentBuffer = reinterpret_cast<u32*>(addr);
        m_StartAddress  = reinterpret_cast<u32*>(addr);
    }

    //--------------------------------------------------------------------------
    //
    //
    //
    //
    //--------------------------------------------------------------------------
    void WriteHeader(u32 regAddr, u8 byteEnable = 0xf)
    {
        this->Align8();

        m_CurrentHeader = m_CurrentBuffer + 1;
        *m_CurrentHeader = internal::MakeCommandHeader(regAddr, 1, false, byteEnable);
    }

    //--------------------------------------------------------------------------
    //
    //
    //
    //
    //--------------------------------------------------------------------------
    void WriteHeaderIncremental(u32 regAddr, u8 byteEnable = 0xf)
    {
        this->Align8();

        m_CurrentHeader = m_CurrentBuffer + 1;
        *m_CurrentHeader = internal::MakeCommandHeader(regAddr, 1, true, byteEnable);
    }

    //--------------------------------------------------------------------------
    //
    //
    //
    //--------------------------------------------------------------------------
    void WriteData(u32 data)
    {
        *m_CurrentBuffer = data;

        if (m_Size == 0)
        {
            m_Size = 2;
            m_CurrentBuffer += 2;
        }
        else
        {
            m_Size += 1;
            m_CurrentBuffer += 1;
        }
    }

    //--------------------------------------------------------------------------
    //
    //
    //
    //
    //--------------------------------------------------------------------------
    void WriteData(const void* data, int count)
    {
        NW_NULL_ASSERT( data );
        NW_ASSERT( count > 0 );

        const u32* pData = reinterpret_cast<const u32*>(data);
        int copyCount = count;

        if (m_Size == 0)
        {
            *m_CurrentBuffer = *pData;
            pData += 1;
            copyCount -= 1;
            m_CurrentBuffer += 2;
            m_Size = 2;
        }

        if (copyCount > 0)
        {
            nw::os::MemCpy(m_CurrentBuffer, pData, copyCount * sizeof(u32));
            m_Size += copyCount;
            m_CurrentBuffer += copyCount;
        }
    }

    //--------------------------------------------------------------------------
    //
    //
    //
    //--------------------------------------------------------------------------
    u32 End()
    {
        this->Align8();

        return reinterpret_cast<u32>(m_CurrentBuffer) - reinterpret_cast<u32>(m_StartAddress);
    }

private:

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    void Align8()
    {
        enum
        {
            SIZE_SHIFT = 20,
            SIZE_WIDTH = 8
        };

        // Save the amount of data in the header.
        if (m_Size > 0)
        {
            NW_NULL_ASSERT( m_CurrentHeader );

            // The amount of data, excluding the header, is m_Size - 1. Since count - 1 is stored to the register, -2.
            *m_CurrentHeader |= ut::internal::MakeBits(m_Size - 2, SIZE_WIDTH, SIZE_SHIFT);
            m_CurrentHeader = NULL;
        }

        // Insert alignment.
        if ((m_Size % 2) == 1)
        {
            *m_CurrentBuffer = 0x0;
            m_CurrentBuffer += 1;
        }

        m_Size = 0;
    }

    u32* m_CurrentBuffer;
    u32* m_StartAddress;
    u32* m_CurrentHeader;
    u32  m_Size;
};


} // namespace internal
} // namespace gfx
} // namespace nw

#endif // NW_GFX_DISPLAYLIST_H_
