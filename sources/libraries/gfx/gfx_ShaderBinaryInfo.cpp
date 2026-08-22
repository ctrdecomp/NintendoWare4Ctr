#include <nw/gfx/gfx_CommandUtil.h>
#include <nw/gfx/gfx_ShaderBinaryInfo.h>

namespace nw{
namespace gfx{
    
void ShaderBinaryInfo::AnalyzeBinary(){
    const u32* binary = this->mpShaderBinary;

    ++binary;

    mExeImageCount = *binary;
    ++binary;

    for (int i = 0; i < mExeImageCount; ++i){
        mExeImageInfo[i] = reinterpret_cast< const ExeImageInfo* >((u8*)this->mpShaderBinary + *binary);
        
        if (mExeImageInfo[i]->isGeometryShader){
            ++mGeometryShaderCount;
        }
        
        ++binary;
    }

    const u32* packageInfo = binary;

    ++binary;
    ++binary;

    this->mpInstruction = static_cast<const u32*>(nw::ut::AddOffsetToPtr(packageInfo, *binary));
    ++binary;

    this->mInstructionCount = *binary;
    ++binary;

    const u32* swizzle = static_cast<const u32*>(nw::ut::AddOffsetToPtr(packageInfo, *binary));
    ++binary;

    mSwizzleCount = *binary;
    ++binary;

    for (int i = 0; i < mSwizzleCount; i++){
        mSwizzle[i] = swizzle[i * 2];
    }
}

s32 ShaderBinaryInfo::GetCommonCommandSize() const{
    s32 size = 0;

    size += sizeof(u32) * 2;
    
    size += GetSwizzleCommandSize();
    size += GetProgramCommandSize();
    
    return size;
}

s32 ShaderBinaryInfo::GetShaderProgramCommandSize( s32 vertexIndex, s32 geometryIndex ) const{
    s32 size = 0;
    
    size += 2 * sizeof(u32);
    
    size += this->GetConstRegCommandSize( vertexIndex );
    
    if (geometryIndex >= 0){
        size += this->GetConstRegCommandSize( geometryIndex );
    }
    
    size += this->GetOutAttrCommandSize( vertexIndex, geometryIndex );
    
    return size;
}

s32 ShaderBinaryInfo::GetProgramCommandSize() const{
    enum { VS_INSTRUCTION_MAX = 512 };
    
    u32 vertexInstructionCount = nw::ut::Min(this->mInstructionCount, u32(VS_INSTRUCTION_MAX));
    
    s32 size = 0;
    
    size += 2 * sizeof(u32);
    size += this->GetLoadCommandSize( vertexInstructionCount );
    size += 2 * sizeof(u32);

    if (this->GetGeometryShaderCount() > 0){
        size += 2 * sizeof(u32);
        size += this->GetLoadCommandSize(this->mInstructionCount);
        size += 2 * sizeof(u32);
    }
    
    return size;
}

s32 ShaderBinaryInfo::GetSwizzleCommandSize() const{
    s32 size = 0;
    
    size += 2 * sizeof(u32);
    size += this->GetLoadCommandSize(this->mSwizzleCount);
    
    if (this->GetGeometryShaderCount() > 0){
        size += 2 * sizeof(u32);
        size += this->GetLoadCommandSize(this->mSwizzleCount);
    }
    
    return size;
}

s32 ShaderBinaryInfo::GetPrepareCommandSize() const{
    return 0;
}

s32 ShaderBinaryInfo::GetConstRegCommandSize(s32 shaderIndex) const{
    s32 size = 0;
    const s32 INT_COMMAND_SIZE = 2;
    const s32 FLOAT_COMMAND_SIZE = 6;
    
    bool isGeometry = this->IsGeometryShader(shaderIndex);

    const ExeImageInfo* exeInfo = mExeImageInfo[shaderIndex];

    struct SetupInfo { 
        u16 type; 
        u16 index; 
        u32 value[4]; 
    };

    enum { TYPE_BOOL = 0, TYPE_INT = 1, TYPE_FLOAT = 2 };
    
    const SetupInfo* setupInfoTable = static_cast<const SetupInfo*>(nw::ut::AddOffsetToPtr(exeInfo, exeInfo->setupOffset));

    for (int i = 0; i < exeInfo->setupCount; ++i){
        const SetupInfo& info = setupInfoTable[i];

        switch ( info.type )
        {
        case TYPE_BOOL:
            break;

        case TYPE_INT:
            size += INT_COMMAND_SIZE * sizeof(u32);
            break;

        case TYPE_FLOAT:
            size += FLOAT_COMMAND_SIZE * sizeof(u32);
            break;
        }
    }
    
    return size;
}

s32 ShaderBinaryInfo::GetOutAttrCommandSize(s32 vertexIndex, s32 geometryIndex) const{
    NW_UNUSED_VARIABLE(vertexIndex);
    
    const s32 GEOMETRY_SETTING_COMMAND_SIZE = 8;
    const s32 SHADER_PROGRAM_COMMAND_SIZE = 24;
    const s32 GEOMETRY_COMMAND_SIZE = 6;
    
    s32 size = 0;
    
    bool hasGeometry = geometryIndex >= 0;
    u32 geometrySettingCommandSize = (GEOMETRY_SETTING_COMMAND_SIZE - (hasGeometry ? 0 : 2)) * sizeof(u32);
    
    size += geometrySettingCommandSize;
    size += SHADER_PROGRAM_COMMAND_SIZE * sizeof(u32);
    
    if (hasGeometry){
        size += GEOMETRY_COMMAND_SIZE * sizeof(u32);
    }
    
    return size;
}

s32 ShaderBinaryInfo::BuildCommonCommand(u32* bufferAddress, u32 bufferSize) const{
    SafeBuffer buffer(bufferAddress, bufferSize);
    
    if (this->GetGeometryShaderCount() > 0){
        this->PutEnableMirroringShaderSetting(buffer, false);
    }
    else{
        this->PutEnableMirroringShaderSetting(buffer, true);
    }
    
    this->BuildSwizzleCommand(buffer);
    this->BuildProgramCommand(buffer);
    
    return buffer.UsedSize();
}

void ShaderBinaryInfo::BuildProgramCommand( SafeBuffer& buffer ) const{ 
    enum { VS_INSTRUCTION_MAX = 512 };
    
    u32 vertexInstructionCount = nw::ut::Min(this->mInstructionCount, u32(VS_INSTRUCTION_MAX));
    
    const u32 VS_COMMAND[] = {
        0, internal::MakeCommandHeader(PICA_REG_VS_PROG_ADDR, 1, false, 0xF)
    };
    
    buffer.Write( &VS_COMMAND[0], sizeof(VS_COMMAND) );

    this->PutLoadCommand(buffer,PICA_REG_VS_PROG_DATA0,&this->mpInstruction[0],vertexInstructionCount);
    
    const u32 VS_RENEWAL_COMMAND[] = {
        1, internal::MakeCommandHeader(PICA_REG_VS_PROG_RENEWAL_END, 1, false, 0xF)
    };
    
    buffer.Write(&VS_RENEWAL_COMMAND[0], sizeof(VS_RENEWAL_COMMAND));
    
    if (this->GetGeometryShaderCount() > 0){
        const u32 GS_COMMAND[] = {
            0, internal::MakeCommandHeader(PICA_REG_GS_PROG_ADDR, 1, false, 0xF)
        };
        
        buffer.Write( &GS_COMMAND[0], sizeof(GS_COMMAND) );

        this->PutLoadCommand(buffer, PICA_REG_GS_PROG_DATA0,&this->mpInstruction[0],this->mInstructionCount);
        
        const u32 GS_RENEWAL_COMMAND[] = {
            1,internal::MakeCommandHeader(PICA_REG_GS_PROG_RENEWAL_END, 1, false, 0xF)
        };
        
        buffer.Write(&GS_RENEWAL_COMMAND[0], sizeof(GS_RENEWAL_COMMAND));
    }
}

void ShaderBinaryInfo::BuildSwizzleCommand(SafeBuffer& buffer) const{
    
    const u32 COMMAND[] = {
        0, internal::MakeCommandHeader(PICA_REG_VS_PROG_SWIZZLE_ADDR, 1, false, 0xF)
    };
    
    buffer.Write(&COMMAND[0], sizeof(COMMAND));
    
    NW_ASSERT(this->mSwizzleCount > 0);

    this->PutLoadCommand(buffer,PICA_REG_VS_PROG_SWIZZLE_DATA0,&this->mSwizzle[0],this->mSwizzleCount);
    
    if (this->GetGeometryShaderCount() > 0){
        const u32 GS_COMMAND[] = {
            0, internal::MakeCommandHeader( PICA_REG_GS_PROG_SWIZZLE_ADDR, 1, false, 0xF )
        };

        buffer.Write(&GS_COMMAND[0], sizeof(GS_COMMAND));
        
        NW_ASSERT(this->mSwizzleCount > 0);

        this->PutLoadCommand(buffer,PICA_REG_GS_PROG_SWIZZLE_DATA0,&this->mSwizzle[0],this->mSwizzleCount );
    }
}

s32 ShaderBinaryInfo::BuildShaderProgramCommand(s32 vertexIndex, s32 geometryIndex, u32* bufferAddress, u32 bufferSize) const{
    SafeBuffer buffer(bufferAddress, bufferSize);
    
    NW_ASSERT(! this->IsGeometryShader( vertexIndex ));
    NW_ASSERT(0 <= vertexIndex && vertexIndex < this->GetShaderCount());
    NW_ASSERT(geometryIndex < 0 || this->IsGeometryShader(geometryIndex));
    NW_ASSERT(geometryIndex < this->GetShaderCount());
    
    if (geometryIndex < 0){
        this->PutEnableMirroringShaderSetting(buffer, true);
    }
    else{
        this->PutEnableMirroringShaderSetting(buffer, false);
    }

    this->BuildConstRegCommand( buffer, vertexIndex );
    
    if (geometryIndex >= 0){
        this->BuildConstRegCommand( buffer, geometryIndex );
    }
    
    this->BuildOutAttrCommand(buffer, vertexIndex, geometryIndex);
    
    return buffer.UsedSize();
}

void ShaderBinaryInfo::BuildConstRegCommand(SafeBuffer& buffer, s32 shaderIndex) const{
    bool isGeometry = this->IsGeometryShader(shaderIndex);
    
    u32 regFloat    = PICA_REG_VS_FLOAT_ADDR;
    u32 regInteger  = PICA_REG_VS_INT0;

    if (isGeometry){
        regFloat    = PICA_REG_GS_FLOAT_ADDR;
        regInteger  = PICA_REG_GS_INT0;
    }

    const ExeImageInfo* exeInfo = this->mExeImageInfo[shaderIndex];

    struct SetupInfo { 
        u16 type; 
        u16 index; 
        u32 value[4]; 
    };

    enum { TYPE_BOOL = 0, TYPE_INT = 1, TYPE_FLOAT = 2 };
    
    const SetupInfo* setupInfoTable = 
        static_cast<const SetupInfo*>(nw::ut::AddOffsetToPtr( exeInfo, exeInfo->setupOffset) );

    for (int i = 0; i < exeInfo->setupCount; ++i){
        const SetupInfo& info = setupInfoTable[ i ];
        const u32* value = info.value;

        switch ( info.type ){
        case TYPE_BOOL:
            break;

        case TYPE_INT:{
                const u32 COMMAND[] ={
                    value[0], internal::MakeCommandHeader(regInteger + info.index, 1, false, 0xF)
                };
                
                buffer.Write( &COMMAND[0], sizeof(COMMAND) );
            }
            break;

        case TYPE_FLOAT:{
                const u32 COMMAND[] = {
                    info.index,
                    internal::MakeCommandHeader(regFloat, 4, true, 0xF),
                    (value[ 3 ] <<  8 & 0xffffff00) | (value[2] >> 16 & 0x000000ff),
                    (value[ 2 ] << 16 & 0xffff0000) | (value[1] >>  8 & 0x0000ffff),
                    (value[ 1 ] << 24 & 0xff000000) | (value[0] >>  0 & 0x00ffffff),
                    0,
                };
                
                buffer.Write( &COMMAND[0], sizeof(COMMAND) );
            }
            break;
        }
    }
}

void ShaderBinaryInfo::BuildOutAttrCommand(SafeBuffer& buffer, s32 vertexIndex, s32 geometryIndex) const{
    bool hasGeometry = geometryIndex >= 0;
    
    u32 vertexOutputMask;
    u32 vertexOutputNum;
    u32 shaderOutputMask;
    u32 shaderOutputNum;
    u32 shaderOutputMap[7];
    u32 clockControl = 0x01030703;
    u32 vertexEntry;
    u32 geometryInputNum;
    u32 geometryEntry;
    bool isTextureOutput = false;
    
    this->GetOutputRegisterNum(vertexIndex, &vertexOutputNum, &vertexOutputMask);
    this->GetShaderOutputRegisterNum(vertexIndex, geometryIndex, &shaderOutputNum, &shaderOutputMask);
    this->GetShaderOutputRegisterMap(vertexIndex, geometryIndex, &shaderOutputMap[0]);
    vertexEntry = this->GetEntryAddress(vertexIndex);
    
    u32 GEOMETRY_SETTING_COMMAND[] = {
        0x00000000, 0x000f0252, 
        0x00000000, 0x00010254, 
        0x00000000, 0x00080229, 
        0x00000000, 0x00020289, 
    };
    
    if (hasGeometry){
        enum { IDX_REG_252 = 0, IDX_REG_254 = 2, IDX_REG_229 = 4, IDX_REG_289 = 6 };
        
        geometryInputNum = vertexOutputNum;
        geometryEntry = this->GetEntryAddress( geometryIndex );
        
        u32 geometryMode = this->GetGeometryDataMode( geometryIndex );
        
        GEOMETRY_SETTING_COMMAND[IDX_REG_252] |= geometryMode & 0x3;
        
        switch (geometryMode){
        case 0:
            break;
        case 1:{
                GEOMETRY_SETTING_COMMAND[IDX_REG_229] |= 1u << 31;
                GEOMETRY_SETTING_COMMAND[IDX_REG_289] |= 1 << 8;
                
                u32 geomMainVertexNum = this->GetGeometryMainVertexNum(geometryIndex);
                
                GEOMETRY_SETTING_COMMAND[IDX_REG_254] |= geomMainVertexNum;
            }
            break;
        case 2:{
                GEOMETRY_SETTING_COMMAND[IDX_REG_289] |= 1 << 8;
                
                u32 geomPatchSize = this->GetGeometryPatchSize(geometryIndex);
                u32 geomStartIndex = this->GetGeometryStartIndex(geometryIndex);
                
                GEOMETRY_SETTING_COMMAND[IDX_REG_252] |= (geomPatchSize - 1) << 8;
                GEOMETRY_SETTING_COMMAND[IDX_REG_252] |= (vertexOutputNum - 1) << 12;
                GEOMETRY_SETTING_COMMAND[IDX_REG_252] |= geomStartIndex << 16;
                GEOMETRY_SETTING_COMMAND[IDX_REG_252] |= 1 << 24;
            }
            break;
        default:
            NW_FATAL_ERROR("Invalid geometry mode");
        }
    }

    const u32 SHADER_PROGRAM_COMMAND[] ={
        vertexOutputMask,    0x000f02bd,
        vertexOutputNum - 1, 0x000f0251,
        vertexOutputNum - 1, 0x000f024a,
        shaderOutputNum - 1, 0x0001025e,
        shaderOutputNum,     0x000f004f,
        shaderOutputMap[0],  0x806f0050,
        shaderOutputMap[1],  shaderOutputMap[2],
        shaderOutputMap[3],  shaderOutputMap[4],
        shaderOutputMap[5],  shaderOutputMap[6],
        0x00000001, 0x000f0064,
        clockControl,        0x000f006f,
        0x7fff0000 | vertexEntry, 0x000f02ba,
    };

    u32 geometrySettingCommandSize = sizeof(GEOMETRY_SETTING_COMMAND) - sizeof(u32) * (hasGeometry ? 0 : 2);
    buffer.Write(&GEOMETRY_SETTING_COMMAND[0], geometrySettingCommandSize);
    buffer.Write(&SHADER_PROGRAM_COMMAND[0], sizeof(SHADER_PROGRAM_COMMAND));
    
    if (hasGeometry){
        const u32 GEOMETRY_COMMAND[] ={
            shaderOutputMask, 0x000f028d,
            0x08000000 | (geometryInputNum - 1), 0x00090289,
            0x7fff0000 | geometryEntry, 0x000f028a,
        };
        
        buffer.Write( &GEOMETRY_COMMAND[0], sizeof(GEOMETRY_COMMAND) );
    }
}

void ShaderBinaryInfo::PutEnableMirroringShaderSetting(SafeBuffer& buffer, bool enableMirroring) const{
    const u32 COMMAND[] ={
        enableMirroring ? 0 : 1, internal::MakeCommandHeader(PICA_REG_VS_COM_MODE, 1, false, 0x1)
    };
    
    buffer.Write(&COMMAND[0], sizeof(COMMAND));
}

void ShaderBinaryInfo::PutLoadCommand(SafeBuffer& buffer, u32 regAddr, const u32* src, u32 count) const{
    enum { WRITE_MAX = 128 };

    u32 restCount = count;
    u32 index = 0;
    
    while (true){
        u32 countPerCommand = nw::ut::Min(restCount, u32(WRITE_MAX));
        
        const u32 COMMAND[] = {
            src[index], internal::MakeCommandHeader( regAddr, countPerCommand, false, 0xF)
        };
        
        buffer.Write(&COMMAND[0], sizeof(COMMAND));
        buffer.Write(&src[index + 1], (countPerCommand - 1) * sizeof(u32));
        
        if ((countPerCommand % 2) == 0){
            buffer.Write(0);
        }
        
        index += countPerCommand;
        restCount -= countPerCommand;
        
        if (restCount == 0){
            break;
        }
    }
}

s32 ShaderBinaryInfo::GetLoadCommandSize(u32 count) const{
    enum { WRITE_MAX = 128 };
    
    s32 commandSize = 0;
    u32 restCount = count;
    
    while (true){
        u32 countPerCommand = nw::ut::Min( restCount, u32(WRITE_MAX) );
        
        commandSize += 2 * sizeof(u32);
        commandSize += (countPerCommand - 1) * sizeof(u32);
        
        if ((countPerCommand % 2) == 0){
            commandSize += sizeof(u32);
        }
        
        restCount -= countPerCommand;
        
        if (restCount == 0){
            break;
        }
    }
    
    return commandSize;
}

}
}
