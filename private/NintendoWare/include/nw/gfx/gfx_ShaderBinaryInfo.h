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

#ifndef NW_GFX_SHADERBINARYINFO_H_
#define NW_GFX_SHADERBINARYINFO_H_

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <nw/types.h>
#include <nw/ut/ut_Inlines.h>
#include <nw/gfx/gfx_CommandUtil.h>

#ifdef NW_PLATFORM_CTR
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 2530 // padding added to end of struct.
#endif
#endif
// The driver level for shaders for parsing, etc., of shader binaries.

namespace nw
{
namespace gfx
{

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ShaderBinaryInfo
{
private:
    class SafeBuffer;

public:
    // Symbol type
    enum SymbolType
    {
        SYMBOL_TYPE_INVALID,  //
        SYMBOL_TYPE_INPUT,    //
        SYMBOL_TYPE_FLOAT,    //
        SYMBOL_TYPE_INT,      //
        SYMBOL_TYPE_BOOL      //
    };

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    ShaderBinaryInfo( const void* shaderBinary )
     : m_pShaderBinary( reinterpret_cast<const u32*>(shaderBinary) ),
       m_ExeImageCount( 0 ),
       m_pInstruction( NULL ),
       m_InstructionCount( 0 ),
       m_SwizzleCount( 0 ),
       m_GeometryShaderCount( 0 )
    {
        for (int i = 0; i < EXE_IMAGE_MAX; ++i)
        {
            m_ExeImageInfo[ i ] = NULL;
        }

        // The m_Swizzle clear process does not occur outside of debug builds.
    #if defined(NW_DEBUG)
        for (int i = 0; i < SWIZZLE_PATTERN_MAX; ++i)
        {
            m_Swizzle[ i ] = 0;
        }
    #endif
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    s32 GetCommonCommandSize() const;

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    s32 BuildCommonCommand( u32* bufferAddress, u32 bufferSize ) const;

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    s32 BuildCommonCommand() const
    {
        u32* currentBuffer = static_cast<u32*>( internal::NWGetCurrentCmdBuffer() );
        u32* cmdBufferEnd  = static_cast<u32*>( internal::NWGetCmdBufferEnd() );
        u32   bufferSize = ut::GetOffsetFromPtr( currentBuffer, cmdBufferEnd );

        s32 result = this->BuildCommonCommand( currentBuffer, bufferSize );

        if ( result > 0 )
        {
            internal::NWForwardCurrentCmdBuffer( result );
        }

        return result;
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    s32 GetShaderProgramCommandSize( s32 vertexIndex, s32 geometryIndex ) const;

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    s32 BuildShaderProgramCommand( s32 vertexIndex, s32 geometryIndex, u32* bufferAddress, u32 bufferSize ) const;

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    s32 BuildShaderProgramCommand( s32 vertexIndex, s32 geometryIndex ) const
    {
        u32* currentBuffer = static_cast<u32*>( internal::NWGetCurrentCmdBuffer() );
        u32* cmdBufferEnd  = static_cast<u32*>( internal::NWGetCmdBufferEnd() );
        u32   bufferSize = ut::GetOffsetFromPtr( currentBuffer, cmdBufferEnd );

        s32 result = this->BuildShaderProgramCommand( vertexIndex, geometryIndex, currentBuffer, bufferSize );

        if ( result > 0 )
        {
            internal::NWForwardCurrentCmdBuffer( result );
        }

        return result;
    }

    //---------------------------------------------------------------------------
    //
    //

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    bool IsGeometryShader(s32 shaderIndex) const
    {
        const ExeImageInfo* exeInfo = this->GetShaderProgramInfo(shaderIndex);

        return exeInfo->isGeometryShader ? true : false;
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    // NOTE: Because of a compiler bug, pair isn't always correctly returned, so make it noinline.
    __declspec(noinline) ::std::pair<s32, SymbolType>
    SearchUniformIndex(s32 shaderIndex, const char* name) const
    {
        enum { BEGIN_INPUT = 0, BEGIN_FLOAT = 16, BEGIN_INT = 112, BEGIN_BOOL = 120, END_SYMBOL = 136 };

        // Output attribute information
        struct BindSymbolInfo
        {
            u32 nameIndex;
            u32 regIndex;
        };

        const ExeImageInfo* exeInfo = this->GetShaderProgramInfo(shaderIndex);

        const BindSymbolInfo* bindSymbolInfoTable = static_cast<const BindSymbolInfo*>( ut::AddOffsetToPtr(exeInfo, exeInfo->bindSymbolOffset) );
        const char* stringTable = static_cast<const char*>( ut::AddOffsetToPtr(exeInfo, exeInfo->stringOffset) );

        SymbolType symbolType  = SYMBOL_TYPE_INVALID;
        s32        symbolIndex = -1;

        u32 nameLen = std::strlen( name );
        for (int i = 0; i < static_cast<int>(exeInfo->bindSymbolCount); ++i)
        {
            const BindSymbolInfo& info = bindSymbolInfoTable[ i ];

            const char* symbolName = &stringTable[ info.nameIndex ];

            if ( std::strncmp( name, symbolName, nameLen ) != 0 ) { continue; }
            if ( symbolName[ nameLen ] != '\0' && symbolName[ nameLen ] != '.' ) { continue; }

            symbolIndex = static_cast<s32>( info.regIndex & 0x0000ffff );

            if ( END_SYMBOL <= symbolIndex )      { symbolIndex = -1; }
            else if (BEGIN_BOOL <= symbolIndex  ) { symbolType = SYMBOL_TYPE_BOOL;  symbolIndex = symbolIndex - BEGIN_BOOL;  }
            else if (BEGIN_INT <= symbolIndex   ) { symbolType = SYMBOL_TYPE_INT;   symbolIndex = symbolIndex - BEGIN_INT;   }
            else if (BEGIN_FLOAT <= symbolIndex ) { symbolType = SYMBOL_TYPE_FLOAT; symbolIndex = symbolIndex - BEGIN_FLOAT; }
            else                                  { symbolType = SYMBOL_TYPE_INPUT; symbolIndex = symbolIndex; }

            break;
        }

        return std::make_pair(symbolIndex, symbolType);
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    const char*
    SearchUniformIndex(s32 shaderIndex, SymbolType symbolType, s32 index) const
    {
        enum { BEGIN_INPUT = 0, BEGIN_FLOAT = 16, BEGIN_INT = 112, BEGIN_BOOL = 120, END_SYMBOL = 136 };

        // Output attribute information
        struct BindSymbolInfo
        {
            u32 nameIndex;
            u32 regIndex;
        };

        const ExeImageInfo* exeInfo = this->GetShaderProgramInfo(shaderIndex);

        const BindSymbolInfo* bindSymbolInfoTable
            = static_cast<const BindSymbolInfo*>( ut::AddOffsetToPtr( exeInfo, exeInfo->bindSymbolOffset ) );
        const char* stringTable = static_cast<const char*>( ut::AddOffsetToPtr( exeInfo, exeInfo->stringOffset ) );

        s32 targetIndex = 0;

        switch ( symbolType )
        {
        case SYMBOL_TYPE_INPUT: { targetIndex = BEGIN_INPUT + index; } break;
        case SYMBOL_TYPE_FLOAT: { targetIndex = BEGIN_FLOAT + index; } break;
        case SYMBOL_TYPE_INT  : { targetIndex = BEGIN_INT   + index; } break;
        case SYMBOL_TYPE_BOOL : { targetIndex = BEGIN_BOOL  + index; } break;
        default: NW_FATAL_ERROR("Unknown symbolType");
        }

        for (int i = 0; i < static_cast<int>(exeInfo->bindSymbolCount); ++i)
        {
            const BindSymbolInfo& info = bindSymbolInfoTable[ i ];

            s32 symbolIndex = static_cast<s32>( info.regIndex & 0x0000ffff );

            if (symbolIndex == targetIndex)
            {
                return &stringTable[ info.nameIndex ];
            }
        }

        return NULL;
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    int SearchBinadSymbolCount(s32 shaderIndex, SymbolType symbolType) const
    {
        enum
        {
            BEGIN_INPUT = 0,   END_INPUT = 15,
            BEGIN_FLOAT = 16,  END_FLOAT = 111,
            BEGIN_INT   = 112, END_INT   = 119,
            BEGIN_BOOL  = 120, END_BOOL  = 135,
            END_SYMBOL  = 136
        };

        // Output attribute information
        struct BindSymbolInfo
        {
            u32 nameIndex;
            u32 regIndex;
        };

        const ExeImageInfo* exeInfo = this->GetShaderProgramInfo(shaderIndex);

        const BindSymbolInfo* bindSymbolInfoTable =
            static_cast<const BindSymbolInfo*>(
                ut::AddOffsetToPtr( exeInfo, exeInfo->bindSymbolOffset )
            );

        s32 count = 0;

        for (int i = 0; i < static_cast<int>(exeInfo->bindSymbolCount); ++i)
        {
            const BindSymbolInfo& info = bindSymbolInfoTable[ i ];

            s32 symbolIndex = static_cast<s32>( info.regIndex & 0x0000ffff );

            switch ( symbolType )
            {
            case SYMBOL_TYPE_INPUT:
                if (BEGIN_INPUT <= symbolIndex && symbolIndex <= END_INPUT)
                {
                    ++count;
                }
                break;
            case SYMBOL_TYPE_FLOAT:
                if (BEGIN_FLOAT <= symbolIndex && symbolIndex <= END_FLOAT)
                {
                    ++count;
                }
                break;
            case SYMBOL_TYPE_INT:
                if (BEGIN_INT <= symbolIndex && symbolIndex <= END_INT)
                {
                    ++count;
                }
                break;
            case SYMBOL_TYPE_BOOL:
                if (BEGIN_BOOL <= symbolIndex && symbolIndex <= END_BOOL)
                {
                    ++count;
                }
                break;
            default: NW_FATAL_ERROR("Unknown symbolType");
            }
        }

        return count;
    }


    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    u32 GetBoolConstant(s32 shaderIndex) const
    {
        enum { TYPE_BOOL = 0, TYPE_INT = 1, TYPE_FLOAT = 2 };

        const ExeImageInfo* exeInfo = this->GetShaderProgramInfo(shaderIndex);

        // Constant register information
        struct SetupInfo
        {
            u16 type;
            u16 index;
            u32 value[4];
        };

        const SetupInfo* setupInfoTable =
            static_cast<const SetupInfo*>( ut::AddOffsetToPtr( exeInfo, exeInfo->setupOffset) );

        u32 boolMap = 0;

        for ( int i = 0; i < static_cast<int>(exeInfo->setupCount); ++i )
        {
            const SetupInfo& info = setupInfoTable[ i ];
            const u32* value = info.value;

            if (info.type == TYPE_BOOL)
            {
                boolMap |= (value[ 0 ] & 0x1) << info.index;
            }
        }

        return boolMap;
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    u32 GetInputRegisterNum(s32 shaderIndex) const
    {
        enum { MAX_INPUT = 16 }; // Up to 16 geometry shaders.

        const ExeImageInfo* exeInfo = this->GetShaderProgramInfo( shaderIndex );

        u32 mask = exeInfo->inputMask;
        s32 count = 0;

        for (int i = 0; i < 16; ++i)
        {
            if (mask & (1 << i))
            {
                ++count;
            }
        }

        return count;
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    u32 GetOutputRegisterNum(s32 shaderIndex) const
    {
        const ExeImageInfo* exeInfo = this->GetShaderProgramInfo(shaderIndex);

        u32 mask = exeInfo->outputMask;
        s32 count = 0;

        for (int i = 0; i < 16; ++i)
        {
            if (mask & (1 << i))
            {
                ++count;
            }
        }

        return count;
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void GetOutputRegisterNum(s32 shaderIndex, u32* pNum, u32* pMask) const
    {
        const ExeImageInfo* exeInfo = this->GetShaderProgramInfo(shaderIndex);

        u32 mask = exeInfo->outputMask;
        s32 count = 0;

        for (int i = 0; i < 16; ++i)
        {
            if (mask & (1 << i))
            {
                ++count;
            }
        }

        if (pNum)  { *pNum  = u32(count); }
        if (pMask) { *pMask = u32(mask); }
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void GetShaderOutputRegisterNum(s32 vertexIndex, s32 geometryIndex, u32* pNum, u32* pMask) const
    {
        const ExeImageInfo* exeInfo = NULL;

        if (geometryIndex >= 0)
        {
            exeInfo= this->GetShaderProgramInfo(geometryIndex);
        }
        else
        {
            exeInfo = this->GetShaderProgramInfo(vertexIndex);
        }

        NW_NULL_ASSERT( exeInfo );

        u32 mask = exeInfo->outputMask;
        s32 count = 0;

        for (int i = 0; i < 16; ++i)
        {
            if (mask & (1 << i))
            {
                ++count;
            }
        }

        if (pNum)  { *pNum  = u32(count); }
        if (pMask) { *pMask = u32(mask); }
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void GetShaderOutputRegisterMap(s32 vertexIndex, s32 geometryIndex, u32 pOutputFormat[7]) const
    {
        // For the time being moved from tugal. Confirmation needed.

        enum { OUT_ATTR_INDEX_MAX = 7, OUT_ATTR_DIMENTION_MAX = 4, VS_OUT_ATTR_INDEX_MAX = 16 };

        u32 outNum  = 0;
        u32 useTex  = 0;
        u32 clock   = 0;
        u32 outMask = 0;

        NW_UNUSED_VARIABLE( useTex );

        // Output attribute information
        struct OutmapInfo
        {
            u16 type;
            u16 index;
            u16 mask;
            u16 reserve;
        };

        enum { OUTPUT_REG_NUM = 7 };
        const ExeImageInfo* exeInfo = NULL;

        if (geometryIndex >= 0)
        {
            exeInfo = this->GetShaderProgramInfo(geometryIndex);
        }
        else
        {
            exeInfo = this->GetShaderProgramInfo(vertexIndex);
        }

        NW_NULL_ASSERT( exeInfo );

        const OutmapInfo* outmapInfoTable = static_cast<const OutmapInfo*>( ut::AddOffsetToPtr( exeInfo, exeInfo->outmapOffset ) );

        if ( pOutputFormat )
        {
            for ( int outputIndex = 0; outputIndex < OUTPUT_REG_NUM; ++outputIndex )
            {
                pOutputFormat[ outputIndex ] = 0x1f1f1f1f;

                for ( int i = 0; i < static_cast<int>(exeInfo->outmapCount); ++i )
                {
                    u32 c = 0;
                    const OutmapInfo& outmapInfo = outmapInfoTable[ i ];

                    for ( int j = 0; outmapInfo.index == outputIndex && j <  OUT_ATTR_DIMENTION_MAX; ++j )
                    {
                        if ( ( outmapInfo.mask & ( 1 << j ) ) == 0 ) { continue; }

                        int value = 0x1f;
                        switch ( outmapInfo.type )
                        {
                        case 0 :
                            {
                                value = 0x00 + c++;
                                if (c == 2) { clock |= 1 <<  0; }
                            }
                            break; // position
                        case 1 :
                            {
                                value = 0x04 + c++;
                                clock |= 1 << 24;
                            }
                            break; // Quaternion
                        case 2 :
                            {
                                value = 0x08 + c++;
                                clock |= 1 <<  1;
                            }
                            break; // color
                        case 3 :
                            {
                                if (c < 2) { value = 0x0c + c++; }
                                useTex = 1;
                                clock |= 1 <<  8;
                            }
                            break; // texcoord0
                        case 4 :
                            {
                                value = 0x10;
                                useTex = 1;
                                clock |= 3 << 16;
                            }
                            break; // texcoord0w
                        case 5 :
                            {
                                if (c < 2) { value = 0x0e + c++; }
                                useTex = 1;
                                clock |= 1 <<  9;
                            }
                            break; // texcoord1
                        case 6 :
                            {
                                if (c < 2) { value = 0x16 + c++; }
                                useTex = 1;
                                clock |= 1 << 10;
                            }
                            break; // texcoord2
                        case 8 :
                            {
                                if (c < 3) { value = 0x12 + c++; }
                                clock |= 1 << 24;
                            }
                            break; // view
                        }

                        pOutputFormat[ outputIndex ] = pOutputFormat[ outputIndex ] & ~( 0xff << ( j * 8 ) ) | value << ( j * 8 );
                    }
                }

                if (pOutputFormat[ outputIndex ] != 0x1f1f1f1f)
                {
                    outMask |= ( 1 << outputIndex );
                    ++outNum;
                }
            }
        }
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    u32 GetEntryAddress(s32 shaderIndex) const
    {
        const ExeImageInfo* exeInfo = this->GetShaderProgramInfo(shaderIndex);

        return exeInfo->mainAddress;
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    u32 GetGeometryDataMode(s32 shaderIndex) const
    {
        NW_ASSERT( this->IsGeometryShader( shaderIndex ) );

        const ExeImageInfo* exeInfo = this->GetShaderProgramInfo(shaderIndex);

        return exeInfo->gsDataMode;
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    u32 GetGeometryMainVertexNum(s32 shaderIndex) const
    {
        NW_ASSERT( this->IsGeometryShader( shaderIndex ) );

        const ExeImageInfo* exeInfo = this->GetShaderProgramInfo(shaderIndex);

        return exeInfo->gsPatchSize;
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    u32 GetGeometryPatchSize(s32 shaderIndex) const
    {
        NW_ASSERT( this->IsGeometryShader( shaderIndex ) );

        const ExeImageInfo* exeInfo = this->GetShaderProgramInfo(shaderIndex);

        return exeInfo->gsVertexNum;
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    u32 GetGeometryStartIndex(s32 shaderIndex) const
    {
        NW_ASSERT( this->IsGeometryShader( shaderIndex ) );

        const ExeImageInfo* exeInfo = this->GetShaderProgramInfo(shaderIndex);

        return exeInfo->gsVertexStartIndex;
    }

    //

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    void AnalyzeBinary();

private:
    enum
    {
        EXE_IMAGE_MAX = 32,
        SWIZZLE_PATTERN_MAX = 128,
        DUMMY_DATA_NUM_251 = 10,
        DUMMY_DATA_NUM_200 = 30,
        PADDING_DATA = 0xead0fead
    };

    // The header structure for the shader binary's execution image information.
    struct ExeImageInfo
    {
        u32 signature;
        u16 version;
        u8  isGeometryShader;
        u8  outputMaps;
        u32 mainAddress;
        u32 endAddress;
        u16 inputMask;
        u16 outputMask;
        u8  gsDataMode;
        u8  gsVertexStartIndex;
        u8  gsPatchSize;
        u8  gsVertexNum;
        u32 setupOffset;
        u32 setupCount;
        u32 labelOffset;
        u32 labelCount;
        u32 outmapOffset;
        u32 outmapCount;
        u32 bindSymbolOffset;
        u32 bindSymbolCount;
        u32 stringOffset;
        u32 stringCount;
    };

    const u32* m_pShaderBinary;
    u32        m_ExeImageCount;
    const ExeImageInfo* m_ExeImageInfo[ EXE_IMAGE_MAX ];
    const u32* m_pInstruction;
    u32        m_InstructionCount;
    u32        m_Swizzle[ SWIZZLE_PATTERN_MAX ];
    u32        m_SwizzleCount;
    s32        m_GeometryShaderCount;

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    const ExeImageInfo* GetShaderProgramInfo( s32 shaderIndex ) const
    {
        NW_ASSERT( 0 <= shaderIndex && shaderIndex < static_cast<s32>(m_ExeImageCount) );

        return m_ExeImageInfo[ shaderIndex ];
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    s32 GetShaderCount() const
    {
        return m_ExeImageCount;
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    s32 GetGeometryShaderCount() const
    {
        return m_GeometryShaderCount;
    }

    // Program load command generation
    void BuildProgramCommand( SafeBuffer& buffer ) const;

    // Swizzle pattern load command generation
    void BuildSwizzleCommand( SafeBuffer& buffer ) const;

    // Generate geometry shader use settings commands
    void BuildPrepareCommand( SafeBuffer& buffer ) const;

    // Constant register command generation
    void BuildConstRegCommand( SafeBuffer& buffer, s32 shaderIndex ) const;

    // Shader output attribute command generation
    void BuildOutAttrCommand( SafeBuffer& buffer, s32 vertexIndex, s32 geometryIndex ) const;

    // Get the program load command size
    s32 GetProgramCommandSize() const;
    // Get the swizzle pattern load command size
    s32 GetSwizzleCommandSize() const;
    // Get the geometry shader use settings command size
    s32 GetPrepareCommandSize() const;
    // Get the constant register command size
    s32 GetConstRegCommandSize( s32 shaderIndex ) const;
    // Get the shader output attribute command size
    s32 GetOutAttrCommandSize( s32 vertexIndex, s32 geometryIndex ) const;

    // Get the data transfer command size
    s32 GetLoadCommandSize( u32 count ) const;

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void PutLoadCommand( SafeBuffer& buffer, u32 regAddr, const u32* src, u32 count ) const;

    // Enable the shared shader mirror mode from the vertex shader.
    void PutEnableMirroringShaderSetting( SafeBuffer& buffer, bool enableMirroring ) const;

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    class SafeBuffer
    {
    public:
        typedef SafeBuffer Self;
        //---------------------------------------------------------------------------
        //
        //
        //
        //
        //---------------------------------------------------------------------------
        SafeBuffer(u32* start, s32 size)
         : m_StartAddress( start ),
           m_CurrentAddress( start ),
           m_EndAddress( static_cast<u32*>( ut::AddOffsetToPtr(start, size) ) ),
           m_IsFinished( false ) {}

        const u32* StartAddress() const { return m_StartAddress; }
        const u32* EndAddress() const { return m_EndAddress; }
        const u32* CurrentAddress() const { return m_EndAddress; }
        s32  BufferSize() const { return ut::GetOffsetFromPtr(m_StartAddress, m_EndAddress); }
        s32  UsedSize() const { return ut::GetOffsetFromPtr(m_StartAddress, m_CurrentAddress); }

        bool IsFinished() const { return m_IsFinished; }

        operator const u32*() const { return m_CurrentAddress; }
        Self& operator++() { return this->MoveAddress(sizeof(u32)); }
        Self  operator++(int) { Self tmp = *this; (void)this->MoveAddress(sizeof(u32)); return tmp; }
        Self& operator+=(s32 count) { return this->MoveAddress(count * sizeof(u32)); }

        //---------------------------------------------------------------------------
        //
        //
        //
        //
        //
        //
        //
        //---------------------------------------------------------------------------
        bool VerifyWriteSize(int size)
        {
            if (ut::AddOffsetToPtr(m_CurrentAddress, size) <= m_EndAddress)
            {
                return true;
            }
            else
            {
                m_IsFinished = true;
                return false;
            }
        }

        //---------------------------------------------------------------------------
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //---------------------------------------------------------------------------
        s32  Write(const u32* src, s32 size)
        {
            if (size <= 0) { return -1; }
            if (!this->VerifyWriteSize(size)) { return -1; }
            nw::os::MemCpy(m_CurrentAddress, src, size);
            this->MoveAddress(size);

            return size;
        }

        s32 Write(u32 src)
        {
            if (!this->VerifyWriteSize(sizeof(u32))) { return -1; }
            *m_CurrentAddress = src;
            ++m_CurrentAddress;
            return sizeof(u32);
        }

    private:
        u32* m_StartAddress;
        u32* m_CurrentAddress;
        u32* m_EndAddress;
        bool m_IsFinished;

        Self& MoveAddress(s32 size)
        {
            m_CurrentAddress = static_cast<u32*>( ut::AddOffsetToPtr(m_CurrentAddress, size) );
            NW_ASSERT(m_CurrentAddress <= m_EndAddress);
            return *this;
        }
    };
};

} // namespace gfx
} // namespace nw
#ifdef NW_PLATFORM_CTR
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 2530 // padding added to end of struct.
#endif
#endif

#endif // NW_GFX_SHADERBINARYINFO_H_
