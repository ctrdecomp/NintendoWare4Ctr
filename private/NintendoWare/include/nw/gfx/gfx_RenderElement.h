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

#ifndef NW_GFX_RENDERELEMENT_H_
#define NW_GFX_RENDERELEMENT_H_

#include <nw/gfx/gfx_GfxObject.h>
#include <nw/gfx/gfx_Model.h>
#include <nw/gfx/res/gfx_ResModel.h>
#include <nw/gfx/res/gfx_ResMesh.h>

#include <functional>

namespace nw
{
namespace gfx
{

class Material;
class RenderContext;

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class RenderCommand
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(RenderCommand);

public:

    //
    //
    //
    virtual void Invoke(RenderContext* renderContext) = 0;

protected:
    //
    RenderCommand() {}
    //
    virtual ~RenderCommand() {}
};

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------
template<typename TKey>
class BasicRenderElement
{
public:
    //
    typedef TKey KeyType;

    //
    enum Bit64RenderKeyFormat
    {
        BIT64_LAYER_SHIFT = 56,
        BIT64_LAYER_BIT_SIZE = 8,
        BIT64_LAYER_MASK = (1 << BIT64_LAYER_BIT_SIZE) - 1,

        BIT64_TRANSLUCENCY_KIND_SHIFT = 54,
        BIT64_TRANSLUCENCY_KIND_BIT_SIZE = 2,
        BIT64_TRANSLUCENCY_KIND_MASK = (1 << BIT64_TRANSLUCENCY_KIND_BIT_SIZE) - 1,

        BIT64_COMMAND_FLAG_SHIFT = 53,
        BIT64_COMMAND_FLAG_MASK = 0x1,
        BIT64_COMMAND_BIT_SIZE = 32,
        BIT64_COMMAND_MASK = 0xffffffff,

        BIT64_DEPTH_BIT_SIZE = 24,
        BIT64_DEPTH_MASK = (1 << BIT64_DEPTH_BIT_SIZE) - 1,
        BIT64_DEPTH_MAX_VALUE = BIT64_DEPTH_MASK,

        BIT64_MATERIAL_ID_BIT_SIZE = 29,
        BIT64_MATERIAL_ID_MASK = (1 << BIT64_MATERIAL_ID_BIT_SIZE) - 1,

        // Definition for the material ID.
        BIT64_MATERIAL_RESOURCE_ID_BIT_SIZE = 22,
        BIT64_MATERIAL_RESOURCE_ID_MASK = (1 << BIT64_MATERIAL_RESOURCE_ID_BIT_SIZE) - 1,

        BIT64_MATERIAL_INSTANCE_ID_BIT_SIZE = 3,
        BIT64_MATERIAL_INSTANCE_ID_MASK = (1 << BIT64_MATERIAL_INSTANCE_ID_BIT_SIZE) - 1,

        BIT64_MATERIAL_PRIORITY_BIT_SIZE = 4,
        BIT64_MATERIAL_PRIORITY_MASK = (1 << BIT64_MATERIAL_PRIORITY_BIT_SIZE) - 1,

        // Definition for the simple material ID. Used for the depth priority key, etc.
        BIT64_SIMPLE_MATERIAL_ID_BIT_SIZE = 21,
        BIT64_SIMPLE_MATERIAL_ID_MASK = (1 << BIT64_SIMPLE_MATERIAL_ID_BIT_SIZE) - 1,

        BIT64_SIMPLE_MATERIAL_PRIORITY_BIT_SIZE = 8,
        BIT64_SIMPLE_MATERIAL_PRIORITY_MASK = (1 << BIT64_SIMPLE_MATERIAL_PRIORITY_BIT_SIZE) - 1
    };

    //
    //
    //
    //
    struct Depth24bitQuantizer : public std::unary_function<float, KeyType>
    {
        KeyType operator()(float depth)
        {
            return static_cast<KeyType>(
                depth * static_cast<float>(BIT64_DEPTH_MAX_VALUE))
                & BIT64_DEPTH_MASK;
        }
    };

    //
    //
    //
    //
    struct ReverseDepth24bitQuantizer : public std::unary_function<float, KeyType>
    {
        KeyType operator()(float depth)
        {
            return static_cast<KeyType>(
                (1.0f - depth) * static_cast<float>(BIT64_DEPTH_MAX_VALUE))
                & BIT64_DEPTH_MASK;
        }
    };

    //
    //
    //
    //
    struct ZeroQuantizer : public std::unary_function<float, KeyType>
    {
        KeyType operator()(float depth)
        {
            NW_UNUSED_VARIABLE(depth);
            return 0;
        }
    };

    //
    template<typename TDepthQuantizer = Depth24bitQuantizer>
    class Bit64RenderKey
    {
    private:
        NW_DISALLOW_COPY_AND_ASSIGN(Bit64RenderKey);

    public:
        //----------------------------------------
        //
        //

        //
        Bit64RenderKey() : m_Key(0) {}

        //

        //----------------------------------------
        //
        //

        //
        void SetLayerId(u8 layerId)
        {
            m_Key |= static_cast<KeyType>(layerId & BIT64_LAYER_MASK)
                << BIT64_LAYER_SHIFT;
        }

        //
        void SetPriorityForDetailedMaterial(u8 priority, u32 shift)
        {
            m_Key |=
                static_cast<KeyType>(priority & BIT64_MATERIAL_PRIORITY_MASK)
                    << (shift +
                        BIT64_MATERIAL_RESOURCE_ID_BIT_SIZE +
                        BIT64_MATERIAL_INSTANCE_ID_BIT_SIZE);
        }

        //
        void SetDetailedMaterialIdAndTranslucencyKind(
            const Material* material,
            u32 materialIdShift)
        {
            NW_NULL_ASSERT(material);

            ResMaterial resMaterial = material->GetOriginal();
            NW_ASSERT(resMaterial.IsValid());

            // HACK: Allows an even more valid material ID to be created.
            KeyType id =
                ((reinterpret_cast<KeyType>(material->GetOwnerModel()) >> 2) & BIT64_MATERIAL_INSTANCE_ID_MASK) |
                (static_cast<KeyType>(resMaterial.GetMaterialId() & BIT64_MATERIAL_RESOURCE_ID_MASK) <<
                    BIT64_MATERIAL_INSTANCE_ID_BIT_SIZE);
            m_Key |= (id & BIT64_MATERIAL_ID_MASK) << materialIdShift;

            ResMaterial shadingParametersResMaterial = material->GetShadingParameterResMaterial();
            NW_ASSERT(shadingParametersResMaterial.IsValid());

            ResMaterial::TranslucencyKind translucencyKind =
                shadingParametersResMaterial.GetTranslucencyKind();

            m_Key |= static_cast<KeyType>(translucencyKind & BIT64_TRANSLUCENCY_KIND_MASK)
                << BIT64_TRANSLUCENCY_KIND_SHIFT;
        }

        //
        void SetPriorityForSimpleMaterial(u8 priority, u32 shift)
        {
            m_Key |=
                static_cast<KeyType>(priority & BIT64_SIMPLE_MATERIAL_PRIORITY_MASK)
                    << (shift + BIT64_SIMPLE_MATERIAL_ID_BIT_SIZE);
        }

        //
        void SetSimpleMaterialIdAndTranslucencyKind(
            const Material* material,
            u32 materialIdShift)
        {
            NW_NULL_ASSERT(material);

            ResMaterial resMaterial = material->GetOriginal();
            NW_ASSERT(resMaterial.IsValid());

            m_Key |= static_cast<KeyType>(resMaterial.GetMaterialId() & BIT64_SIMPLE_MATERIAL_ID_MASK)
                << materialIdShift;

            ResMaterial shadingParametersResMaterial = material->GetShadingParameterResMaterial();
            NW_ASSERT(shadingParametersResMaterial.IsValid());

            ResMaterial::TranslucencyKind translucencyKind =
                shadingParametersResMaterial.GetTranslucencyKind();

            m_Key |= (static_cast<KeyType>(translucencyKind)
                & BIT64_TRANSLUCENCY_KIND_MASK) << BIT64_TRANSLUCENCY_KIND_SHIFT;
        }

        //
        void SetTranslucencyKind(ResMaterial::TranslucencyKind translucencyKind)
        {
            m_Key |= (static_cast<KeyType>(translucencyKind) & BIT64_TRANSLUCENCY_KIND_MASK)
                << BIT64_TRANSLUCENCY_KIND_SHIFT;
        }

        //
        void SetDepth(float depth, u32 shift)
        {
            m_Key |= TDepthQuantizer()(depth) << shift;
        }

        //
        void SetCommand(RenderCommand* command)
        {
            m_Key |= static_cast<KeyType>(1) << BIT64_COMMAND_FLAG_SHIFT;
            m_Key |= reinterpret_cast<KeyType>(command);
        }

        //
        const KeyType Key() const { return m_Key; }

        //

    private:
        KeyType m_Key;
        u8 padding[((sizeof(KeyType) % 8) > 0) ? (8 - (sizeof(KeyType) % 8)) : 0]; // Padding to match 8 bytes.
    };

    //----------------------------------------
    //
    //

    //
    explicit BasicRenderElement(const KeyType& key)
    : m_Key(key), m_Model(NULL)
    {
    }

    //
    BasicRenderElement(ResMesh mesh, Model* model)
    : m_Key(0), m_Mesh(mesh), m_Model(model)
    {
    }

    //
    BasicRenderElement(const BasicRenderElement& element)
    : m_Key(element.m_Key), m_Mesh(element.m_Mesh), m_Model(element.m_Model) {}

    //
    ~BasicRenderElement() {}

    //

    //----------------------------------------
    //
    //

    //
    bool IsCommand() const { return this->GetBool(BIT64_COMMAND_FLAG_SHIFT); }

    //
    ResMesh GetMesh() { return m_Mesh; }

    //
    const ResMesh GetMesh() const { return m_Mesh; }

    //
    Model* GetModel() { return m_Model; }

    //
    const Model* GetModel() const { return m_Model; }

    //
    RenderCommand* GetCommand()
    {
        NW_ASSERT(this->IsCommand());
        return reinterpret_cast<RenderCommand*>(
            static_cast<u32>(this->m_Key) & BIT64_COMMAND_MASK);
    }

    //
    const RenderCommand* GetCommand() const
    {
        NW_ASSERT(this->IsCommand());
        return reinterpret_cast<RenderCommand*>(
            static_cast<u32>(this->m_Key) & BIT64_COMMAND_MASK);
    }

    //
    KeyType& Key() { return this->m_Key; }

    //
    const KeyType& Key() const { return this->m_Key; }

    //

private:
    bool GetBool(int shift) const { return (this->m_Key >> shift) & 0x1; }

    KeyType m_Key;
    u8 padding[((sizeof(KeyType) % 8) > 0) ? (8 - (sizeof(KeyType) % 8)) : 0]; // Padding to match 8 bytes.

    ResMesh m_Mesh;
    Model* m_Model;
};

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------
template<typename TKey>
class BasicRenderKeyFactory : public GfxObject
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(BasicRenderKeyFactory);

public:
    //
    typedef TKey KeyType;

    //
    typedef BasicRenderElement<KeyType> RenderElementType;

    //
    virtual KeyType CreateRenderKey(
        const RenderElementType& renderElement,
        float depth,
        u8 layerId) = 0;

    //
    virtual KeyType CreateCommandRenderKey(
        RenderCommand* command,
        ResMaterial::TranslucencyKind translucencyKind,
        u8 priority,
        u8 layerId) = 0;

protected:
    //
    BasicRenderKeyFactory(os::IAllocator* allocator) : GfxObject(allocator) {}

    //
    virtual ~BasicRenderKeyFactory() {}
};

//---------------------------------------------------------------------------
//
//
//
//
//---------------------------------------------------------------------------
template<
typename TKey,
typename TDepthQuantizer = typename BasicRenderElement<TKey>::Depth24bitQuantizer
>
class PriorMaterialRenderKeyFactory : public BasicRenderKeyFactory<TKey>
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(PriorMaterialRenderKeyFactory);

public:
    //
    typedef TKey KeyType;

    //
    typedef BasicRenderElement<KeyType> RenderElementType;

    //
    typedef typename BasicRenderElement<KeyType>::template Bit64RenderKey<TDepthQuantizer> Bit64RenderKeyType;

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    //
    //
    static PriorMaterialRenderKeyFactory* Create(os::IAllocator* allocator)
    {
        void* factoryMemory = allocator->Alloc<PriorMaterialRenderKeyFactory>(1);
        NW_NULL_ASSERT(factoryMemory);
        return new(factoryMemory) PriorMaterialRenderKeyFactory(allocator);
    }

    //

    //
    virtual KeyType CreateRenderKey(
        const RenderElementType& renderElement,
        float depth,
        u8 layerId)
    {
        Bit64RenderKeyType renderKey;

        renderKey.SetDepth(depth, 0);

        const ResMesh mesh = renderElement.GetMesh();
        const Model* model = renderElement.GetModel();

        if (mesh.IsValid() && model)
        {
            const Material* material = model->GetMaterial(mesh.GetMaterialIndex());
            renderKey.SetDetailedMaterialIdAndTranslucencyKind(
                material, RenderElementType::BIT64_DEPTH_BIT_SIZE);
            renderKey.SetPriorityForDetailedMaterial(
                mesh.GetRenderPriority(), RenderElementType::BIT64_DEPTH_BIT_SIZE);
        }

        renderKey.SetLayerId(layerId);

        return renderKey.Key();
    }

    //
    virtual KeyType CreateCommandRenderKey(
        RenderCommand* command,
        ResMaterial::TranslucencyKind translucencyKind,
        u8 priority,
        u8 layerId)
    {
        Bit64RenderKeyType renderKey;

        renderKey.SetLayerId(layerId);
        renderKey.SetPriorityForSimpleMaterial(
            priority, RenderElementType::BIT64_DEPTH_BIT_SIZE);
        renderKey.SetTranslucencyKind(translucencyKind);
        renderKey.SetCommand(command);

        return renderKey.Key();
    }

private:
    PriorMaterialRenderKeyFactory(os::IAllocator* allocator)
    : BasicRenderKeyFactory<KeyType>(allocator)
    {
    }
};

//---------------------------------------------------------------------------
//
//
//
//
//---------------------------------------------------------------------------
template<
typename TKey,
typename TDepthQuantizer = typename BasicRenderElement<TKey>::Depth24bitQuantizer
>
class PriorDepthRenderKeyFactory : public BasicRenderKeyFactory<TKey>
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(PriorDepthRenderKeyFactory);

public:
    //
    typedef TKey KeyType;

    //
    typedef BasicRenderElement<KeyType> RenderElementType;

    //
    typedef typename BasicRenderElement<KeyType>::template Bit64RenderKey<TDepthQuantizer> Bit64RenderKeyType;

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    //
    //
    static PriorDepthRenderKeyFactory* Create(os::IAllocator* allocator)
    {
        void* factoryMemory = allocator->Alloc<PriorDepthRenderKeyFactory>(1);
        NW_NULL_ASSERT(factoryMemory);
        return new(factoryMemory) PriorDepthRenderKeyFactory(allocator);
    }

    //

    //
    virtual KeyType CreateRenderKey(
        const RenderElementType& renderElement,
        float depth,
        u8 layerId)
    {
        Bit64RenderKeyType renderKey;

        renderKey.SetDepth(depth, RenderElementType::BIT64_SIMPLE_MATERIAL_ID_BIT_SIZE);

        const ResMesh mesh = renderElement.GetMesh();
        const Model* model = renderElement.GetModel();

        if (mesh.IsValid() && model)
        {
            const Material* material = model->GetMaterial(mesh.GetMaterialIndex());
            renderKey.SetSimpleMaterialIdAndTranslucencyKind(material, 0);
            renderKey.SetPriorityForSimpleMaterial(
                mesh.GetRenderPriority(), RenderElementType::BIT64_DEPTH_BIT_SIZE);
        }

        renderKey.SetLayerId(layerId);

        return renderKey.Key();
    }

    //
    virtual KeyType CreateCommandRenderKey(
        RenderCommand* command,
        ResMaterial::TranslucencyKind translucencyKind,
        u8 priority,
        u8 layerId)
    {
        Bit64RenderKeyType renderKey;

        renderKey.SetLayerId(layerId);
        renderKey.SetPriorityForSimpleMaterial(
            priority, RenderElementType::BIT64_DEPTH_BIT_SIZE);
        renderKey.SetTranslucencyKind(translucencyKind);
        renderKey.SetCommand(command);

        return renderKey.Key();
    }

private:
    PriorDepthRenderKeyFactory(os::IAllocator* allocator)
    : BasicRenderKeyFactory<KeyType>(allocator)
    {
    }
};

//---------------------------------------------------------------------------
//
//
//
//
//
//
//---------------------------------------------------------------------------
template<
typename TKey,
typename TDepthQuantizer = typename BasicRenderElement<TKey>::Depth24bitQuantizer
>
class TopPriorDepthRenderKeyFactory : public BasicRenderKeyFactory<TKey>
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(TopPriorDepthRenderKeyFactory);

public:
    //
    typedef TKey KeyType;

    //
    typedef BasicRenderElement<KeyType> RenderElementType;

    //
    typedef typename BasicRenderElement<KeyType>::template Bit64RenderKey<TDepthQuantizer> Bit64RenderKeyType;

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    //
    //
    static TopPriorDepthRenderKeyFactory* Create(os::IAllocator* allocator)
    {
        void* factoryMemory = allocator->Alloc<TopPriorDepthRenderKeyFactory>(1);
        NW_NULL_ASSERT(factoryMemory);
        return new(factoryMemory) TopPriorDepthRenderKeyFactory(allocator);
    }

    //

    //
    virtual KeyType CreateRenderKey(
        const RenderElementType& renderElement,
        float depth,
        u8 layerId)
    {
        Bit64RenderKeyType renderKey;

        renderKey.SetDepth(depth,
            RenderElementType::BIT64_SIMPLE_MATERIAL_ID_BIT_SIZE +
            RenderElementType::BIT64_SIMPLE_MATERIAL_PRIORITY_BIT_SIZE);

        const ResMesh mesh = renderElement.GetMesh();
        const Model* model = renderElement.GetModel();

        if (mesh.IsValid() && model)
        {
            const Material* material = model->GetMaterial(mesh.GetMaterialIndex());
            renderKey.SetSimpleMaterialIdAndTranslucencyKind(material, 0);
            renderKey.SetPriorityForSimpleMaterial(mesh.GetRenderPriority(), 0);
        }

        renderKey.SetLayerId(layerId);

        return renderKey.Key();
    }

    //
    virtual KeyType CreateCommandRenderKey(
        RenderCommand* command,
        ResMaterial::TranslucencyKind translucencyKind,
        u8 priority,
        u8 layerId)
    {
        Bit64RenderKeyType renderKey;

        renderKey.SetLayerId(layerId);
        renderKey.SetPriorityForSimpleMaterial(
            priority, RenderElementType::BIT64_DEPTH_BIT_SIZE);
        renderKey.SetTranslucencyKind(translucencyKind);
        renderKey.SetCommand(command);

        return renderKey.Key();
    }

private:
    TopPriorDepthRenderKeyFactory(os::IAllocator* allocator)
    : BasicRenderKeyFactory<KeyType>(allocator)
    {
    }
};

//----------------------------------------
//
//

//
typedef bit64 RenderKeyType;

//
typedef BasicRenderElement<RenderKeyType> RenderElement;

//
typedef BasicRenderKeyFactory<RenderKeyType> RenderKeyFactory;

//---------------------------------------------------------------------------
//
//
//
//
//
//
//
//---------------------------------------------------------------------------
NW_INLINE RenderKeyFactory* CreatePriorMaterialRenderKeyFactory(
    os::IAllocator* allocator)
{
    return PriorMaterialRenderKeyFactory<RenderKeyType>::Create(allocator);
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
NW_INLINE RenderKeyFactory* CreatePriorMaterialReverseDepthRenderKeyFactory(
    os::IAllocator* allocator)
{
    return PriorMaterialRenderKeyFactory<
        RenderKeyType, RenderElement::ReverseDepth24bitQuantizer>::Create(allocator);
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
NW_INLINE RenderKeyFactory* CreatePriorMaterialAndZeroDepthRenderKeyFactory(
    os::IAllocator* allocator)
{
    return PriorMaterialRenderKeyFactory<
        RenderKeyType, RenderElement::ZeroQuantizer>::Create(allocator);
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
NW_INLINE RenderKeyFactory* CreatePriorDepthRenderKeyFactory(
    os::IAllocator* allocator)
{
    return PriorDepthRenderKeyFactory<RenderKeyType>::Create(allocator);
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
NW_INLINE RenderKeyFactory* CreatePriorDepthReverseDepthRenderKeyFactory(
    os::IAllocator* allocator)
{
    return PriorDepthRenderKeyFactory<
        RenderKeyType, RenderElement::ReverseDepth24bitQuantizer>::Create(allocator);
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
NW_INLINE RenderKeyFactory* CreateTopPriorDepthRenderKeyFactory(
    os::IAllocator* allocator)
{
    return TopPriorDepthRenderKeyFactory<RenderKeyType>::Create(allocator);
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
NW_INLINE RenderKeyFactory* CreateTopPriorDepthReverseDepthRenderKeyFactory(
    os::IAllocator* allocator)
{
    return TopPriorDepthRenderKeyFactory<
        RenderKeyType, RenderElement::ReverseDepth24bitQuantizer>::Create(allocator);
}
//

} // namespace gfx
} // namespace nw

#endif // NW_GFX_RENDERELEMENT_H_
