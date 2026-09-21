// Filename: gfx_ResMesh.cpp
//
// Project: NintendoWare4Ctr

#include <nw/gfx/res/gfx_ResMesh.h>
#include <nw/gfx/res/gfx_ResModel.h>
#include <nw/gfx/res/gfx_ResGraphicsFile.h>
#include <nw/gfx/gfx_ActivateCommand.h>
#include <nw/gfx/gfx_ShaderUniforms.h>
#include <nw/ut/ut_Float24.h>
#include <nn/gx.h>

namespace nw {
namespace gfx {
namespace res {

namespace {

    enum
    {
        BONE_INDEX_SCALE_INDEX = -2
    };

    s32 QueryScaleCommandIndex(ResVertexAttribute::VertexAttributeUsage usage)
    {
        enum
        {
            BASE_INDEX = 2
        };

        switch (usage)
        {
        case ResVertexAttribute::USAGE_POSITION:            return BASE_INDEX + 4;
        case ResVertexAttribute::USAGE_NORMAL:              return BASE_INDEX + 3;
        case ResVertexAttribute::USAGE_TANGENT:             return BASE_INDEX + 2;
        case ResVertexAttribute::USAGE_COLOR:               return BASE_INDEX + 0;
        case ResVertexAttribute::USAGE_TEXTURECOODINATE0:   return BASE_INDEX + 8;
        case ResVertexAttribute::USAGE_TEXTURECOODINATE1:   return BASE_INDEX + 7;
        case ResVertexAttribute::USAGE_TEXTURECOODINATE2:   return BASE_INDEX + 6;
        case ResVertexAttribute::USAGE_BONEWEIGHT:          return BASE_INDEX + 5;
        case ResVertexAttribute::USAGE_BONEINDEX:           return BONE_INDEX_SCALE_INDEX;
        case ResVertexAttribute::USAGE_USERATTRIBUTE0:
        case ResVertexAttribute::USAGE_USERATTRIBUTE1:
        case ResVertexAttribute::USAGE_USERATTRIBUTE2:
        default: break;
        }

        return -1;
    }

    void ResSeparateDataShape_SetupVertexIrScale(ResMesh mesh, ResSeparateDataShape shape, ResShaderProgramDescription shaderProgramDesc)
    {
        enum { ATTRIBUTE_SCALES_VECTOR_COUNT = 3 };

        u32* command = &mesh.ref().m_IrScaleCommand[0];

        mesh.ref().m_Flags &= ~(ResMesh::FLAG_HAS_VERTEX_ALPHA | ResMesh::FLAG_HAS_BONE_WEIGHT_W);
        std::memset(command, 0, sizeof(mesh.ref().m_IrScaleCommand));

        command[0] = VERTEX_SHADER_UNIFORM_IRSCALE_INDEX | 0x80000000;
        command[1] = internal::MakeCommandHeader(PICA_REG_VS_FLOAT_ADDR, 1, false, 0xF);
        command[3] = internal::MakeCommandHeader(PICA_REG_VS_FLOAT0, 2 * 4, false, 0xF);

        internal::ResVertexAttributeIterator iter = internal::ResVertexAttributeIterator::Begin( shape );

        ResVertexAttributeArray::iterator end = shape.GetVertexAttributes().end();
        for ( ; iter.IsValid(); ++iter)
        {
            ResVertexAttribute attribute = *iter;

            int index = shaderProgramDesc.GetAttributeIndices(attribute.GetUsage());

            if (index < 0)
            {
                continue;
            }

            u8 dimension = 0;
            ResVertexAttribute::VertexAttributeUsage usage = ResVertexAttribute::USAGE_NONE;

            if (attribute.GetFlags() & ResVertexAttributeData::FLAG_VERTEX_PARAM)
            {
                ResVertexParamAttribute constantVertexAttribute(attribute.ptr());

                dimension = constantVertexAttribute.GetDimension();

                usage = static_cast<ResVertexAttribute::VertexAttributeUsage>(constantVertexAttribute.GetUsage());
                s32 commandIndex = QueryScaleCommandIndex(usage);
                if (commandIndex >= 0)
                {
                    command[commandIndex] = ut::Float32::Float32ToBits32(1.0f);
                }
            }
            else
            {
                ResVertexStream vertexStream(attribute.ptr());

                dimension = vertexStream.GetDimension();

                usage = static_cast<ResVertexAttribute::VertexAttributeUsage>(vertexStream.GetUsage());
                f32 scale = vertexStream.GetScale();
                s32 commandIndex = QueryScaleCommandIndex(usage);
                if (commandIndex >= 0)
                {
                    command[commandIndex] = ut::Float32::Float32ToBits32(scale);
                }
                NW_ASSERT(!((commandIndex == BONE_INDEX_SCALE_INDEX) && (scale != 1.0f)));
            }

            if (dimension == 4)
            {
                if (usage == ResVertexAttribute::USAGE_COLOR)
                {
                    mesh.ref().m_Flags |= ResMesh::FLAG_HAS_VERTEX_ALPHA;
                }
                else if (usage == ResVertexAttribute::USAGE_BONEWEIGHT)
                {
                    mesh.ref().m_Flags |= ResMesh::FLAG_HAS_BONE_WEIGHT_W;
                }
            }
        }
    }
}

Result ResMesh::Setup(ResModel owner, os::IAllocator* allocator, ResGraphicsFile graphicsFile)
{
    NW_UNUSED_VARIABLE(graphicsFile);

    Result result = RESOURCE_RESULT_OK;

    if (allocator == NULL) { allocator = CommandCacheManager::GetAllocator(); }

    ResShape    shape      = owner.GetShapes(this->GetShapeIndex());
    ResMaterial material   = owner.GetMaterials(this->GetMaterialIndex());
    ResBinaryShader shader = material.GetShader().Dereference();
    s32 shaderIndex = material.GetShaderProgramDescriptionIndex();

    NW_ASSERT(shader.IsValid());

    ResShaderProgramDescription shaderProgramDesc = shader.GetDescriptions( shaderIndex );

    switch (shape.GetTypeInfo())
    {
    case ResSeparateDataShape::TYPE_INFO:
        {
            ResSeparateDataShape separateShape = ResStaticCast<ResSeparateDataShape>(shape);

            s32 activateCommandSize   = internal::CalcSetupActivateVertexAttributeCommandSize(separateShape, shaderProgramDesc);
            s32 deactivateCommandSize = internal::CalcSetupDeactivateVertexAttributeCommandSize(separateShape, shaderProgramDesc);

            void* buffer = allocator->Alloc( activateCommandSize + deactivateCommandSize, 4 );

            internal::CommandBufferInfo bufferInfo(buffer, activateCommandSize + deactivateCommandSize);

            internal::SetupVertexAttributeCommand(bufferInfo, separateShape, shaderProgramDesc);
            NW_ASSERT(bufferInfo.GetCurrentSize() == activateCommandSize);

            internal::SetupDeactivateVertexAttributeCommand(bufferInfo, separateShape, shaderProgramDesc);
            NW_ASSERT(bufferInfo.GetCurrentSize() == activateCommandSize + deactivateCommandSize);

            ref().m_ActivateCommandCache = buffer;
            ref().m_ActivateCommandCacheSize = activateCommandSize;

            ref().m_DeactivateCommandCache = nw::ut::AddOffsetToPtr( ref().m_ActivateCommandCache, activateCommandSize );
            ref().m_DeactivateCommandCacheSize = deactivateCommandSize;

            ref().m_CommandAllocator = allocator;

            ResSeparateDataShape_SetupVertexIrScale( *this, separateShape, shaderProgramDesc );
        }
        break;

    case ResParticleShape::TYPE_INFO:
        {
        }
        break;
    }

    if (result.IsSuccess())
    {
        this->EnableFlags(ResMesh::FLAG_HAS_BEEN_SETUP);
    }

    return result;
}

void ResMesh::Cleanup()
{
    this->DisableFlags(ResMesh::FLAG_HAS_BEEN_SETUP);

    if (ref().m_ActivateCommandCache != NULL)
    {
        ref().m_CommandAllocator->Free(ref().m_ActivateCommandCache);

        ref().m_ActivateCommandCache = NULL;
        ref().m_ActivateCommandCacheSize = 0;

        if (ref().m_DeactivateCommandCache != NULL)
        {
            ref().m_DeactivateCommandCache = NULL;
            ref().m_DeactivateCommandCacheSize = 0;
        }
    }

    NW_ASSERT(ref().m_DeactivateCommandCache == NULL);
}

} // namespace res
} // namespace gfx
} // namespace nw