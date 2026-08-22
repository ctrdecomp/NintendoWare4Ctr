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

#ifndef NW_GFX_RENDERQUEUE_H_
#define NW_GFX_RENDERQUEUE_H_

#include <nw/gfx/gfx_RenderElement.h>

#include <nw/ut/ut_MoveArray.h>
#include <nw/ut/ut_Preprocessor.h>
#include <nw/ut/ut_TypeTraits.h>
#include <nw/gfx/gfx_Camera.h>
#include <nw/gfx/gfx_Model.h>
#include <nw/gfx/gfx_SkeletalModel.h>
#include <nw/gfx/gfx_SceneHelper.h>

namespace nw
{
namespace gfx
{

//---------------------------------------------------------------------------
//
//
//
//
//
//---------------------------------------------------------------------------
template<
    typename TElement,
    typename TElementList,
    typename TElementKeyFactory = BasicRenderKeyFactory<typename TElement::KeyType>
>
class BasicRenderQueue  : public GfxObject
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(BasicRenderQueue);
    NW_STATIC_ASSERT(!ut::IsArray<TElement>::value);

public:
    NW_UT_RUNTIME_TYPEINFO;

    static const u8 PRIORITY_END = 0xff; //

    typedef TElement ElementType;
    typedef TElementList ElementListType;
    typedef TElementKeyFactory ElementKeyFactoryType;
    typedef typename TElement::KeyType ElementKeyType;

    typedef typename TElementList::reference reference;
    typedef typename TElementList::difference_type difference_type;
    typedef typename TElementList::value_type value_type;
    typedef typename TElementList::iterator iterator;
    typedef typename TElementList::const_iterator const_iterator;

#if defined(_MSC_VER) && _MSC_VER <= 1201
    typedef std::reverse_iterator<iterator, TElement> reverse_iterator;
    typedef std::reverse_iterator<const_iterator, TElement> const_reverse_iterator;
#else
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
#endif

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    struct IsCalculatingOnlyLayer1Functor
    {
        IsCalculatingOnlyLayer1Functor(Model* model, ResMesh mesh)
        {
            isCalculating =
                model->GetRenderLayerId(mesh) == ResMaterial::TRANSLUCENCY_KIND_LAYER1;
        }

        IsCalculatingOnlyLayer1Functor(ResMaterial::TranslucencyKind renderLayer)
        {
            isCalculating = renderLayer ==  ResMaterial::TRANSLUCENCY_KIND_LAYER1;
        }

        bool operator ()() const
        {
            return isCalculating;
        }

        bool isCalculating;
    };

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    struct AlwaysCalculatingFunctor
    {
        AlwaysCalculatingFunctor(Model* model, ResMesh mesh)
        { NW_UNUSED_VARIABLE(model); NW_UNUSED_VARIABLE(mesh); }

        AlwaysCalculatingFunctor(ResMaterial::TranslucencyKind renderLayer)
        { NW_UNUSED_VARIABLE(renderLayer); }

        bool operator ()() const
        {
            return true;
        }
    };

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    struct CalculateDepthFunctor
    {
        typedef Model ModelType;

        CalculateDepthFunctor(ModelType* model)
        : model(model)
        {
            NW_NULL_ASSERT(model);
            this->resModel = model->GetResModel();
            NW_ASSERT(resModel.IsValid());
        }

        template<typename IsCalculating>
        float operator ()(ResMesh mesh, const Camera& camera, IsCalculating isCalculating) const
        {
            if (isCalculating())
            {
                ResShape shape = resModel.GetShapes(mesh.GetShapeIndex());
                NW_ASSERT(shape.IsValid());

                return SceneHelper::CalculateDepth(
                    shape.GetCenterPosition(), model->WorldMatrix(), camera);
            }
            else
            {
                return 0.0f;
            }
        }

        ModelType* model;
        ResModel resModel;
    };

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    struct CalculateDepthOfSkeletalModelFunctor
    {
        typedef SkeletalModel ModelType;

        CalculateDepthOfSkeletalModelFunctor(ModelType* model)
        : model(model)
        {
            NW_NULL_ASSERT(model);
            this->resModel = model->GetResModel();
            NW_ASSERT(resModel.IsValid());
        }

        template<typename IsCalculating>
        float operator ()(ResMesh mesh, const Camera& camera, IsCalculating isCalculating) const
        {
            if (isCalculating())
            {
                float depth;
                ResShape shape = resModel.GetShapes(mesh.GetShapeIndex());
                NW_ASSERT(shape.IsValid());

                // When there is one primitive, the bone matrix is used
                if (shape.GetPrimitiveSetsCount() == 1)
                {
                    ResPrimitiveSet primitiveSet = shape.GetPrimitiveSets(0);
                    s32 boneIndex = primitiveSet.GetBoneIndexTable(0);
                    Skeleton::MatrixPose& pose = this->model->GetSkeleton()->WorldMatrixPose();
                    depth = SceneHelper::CalculateDepth(
                        shape.GetCenterPosition(), *pose.GetMatrix(boneIndex), camera);
                }
                else
                {
                    depth = SceneHelper::CalculateDepth(
                        shape.GetCenterPosition(), model->WorldMatrix(), camera);
                }

                return depth;
            }
            else
            {
                return 0.0f;
            }
        }

        ModelType* model;
        ResModel resModel;
    };

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    template<typename ModelType, typename CalculateDepth, typename IsCalculating>
    struct BasicEnqueueModelFunctor : public std::unary_function<ResMesh, void>
    {
        BasicEnqueueModelFunctor(
            BasicRenderQueue* renderQueue,
            ModelType* model,
            u8 layerId,
            const Camera& camera)
        : calculateDepth(model),
          renderQueue(renderQueue),
          camera(camera),
          layerId(layerId) {}

        result_type operator()(argument_type mesh)
        {
            NW_ASSERT(mesh.IsValid());
            if (calculateDepth.model->IsMeshVisible(mesh))
            {
                float depth = calculateDepth(mesh, camera, IsCalculating(calculateDepth.model, mesh));
                renderQueue->EnqueueMesh(mesh, calculateDepth.model, depth, layerId);
            }
        }

        CalculateDepth calculateDepth;
        BasicRenderQueue* renderQueue;
        const Camera& camera;
        u8 layerId;
        u8 padding[3];
    };

    //
    typedef BasicEnqueueModelFunctor<Model, CalculateDepthFunctor, AlwaysCalculatingFunctor>
        EnqueueModelFunctor;

    //
    typedef BasicEnqueueModelFunctor<SkeletalModel, CalculateDepthOfSkeletalModelFunctor, AlwaysCalculatingFunctor>
        EnqueueSkeletalModelFunctor;

    //
    //
    typedef BasicEnqueueModelFunctor<Model, CalculateDepthFunctor, IsCalculatingOnlyLayer1Functor>
        FastEnqueueModelFunctor;

    //
    //
    typedef BasicEnqueueModelFunctor<SkeletalModel, CalculateDepthOfSkeletalModelFunctor, IsCalculatingOnlyLayer1Functor>
        FastEnqueueSkeletalModelFunctor;

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    template<typename ModelType, typename CalculateDepth, typename IsCalculating>
    struct BasicEnqueueModelTranslucentModelBaseFunctor : public std::unary_function<ResMesh, void>
    {
        BasicEnqueueModelTranslucentModelBaseFunctor(
            BasicRenderQueue* renderQueue,
            ModelType* model,
            u8 layerId,
            const Camera& camera)
        : calculateDepth(model),
          renderQueue(renderQueue),
          camera(camera),
          layerId(layerId)
        {
            NW_NULL_ASSERT(renderQueue);

            modelDepth = SceneHelper::CalculateDepth(model->WorldMatrix(), camera);
        }

        result_type operator()(argument_type mesh)
        {
            NW_ASSERT(mesh.IsValid());
            if (calculateDepth.model->IsMeshVisible(mesh))
            {
                ResMaterial::TranslucencyKind translucencyKind =
                    calculateDepth.model->GetRenderLayerId(mesh);

                float depth = (translucencyKind == ResMaterial::TRANSLUCENCY_KIND_OPAQUE)
                    ? calculateDepth(mesh, camera, IsCalculating(translucencyKind))
                    : modelDepth;

                ElementType* enqueuedElement = renderQueue->EnqueueElement(
                    ElementType(mesh, calculateDepth.model), translucencyKind, depth, layerId);
                NW_WARNING(enqueuedElement != NULL, "Failed to enqueue element");
            }
        }

        CalculateDepth calculateDepth;
        BasicRenderQueue* renderQueue;
        const Camera& camera;
        float modelDepth;
        u8 layerId;
        u8 padding[3];
    };

    //
    typedef BasicEnqueueModelTranslucentModelBaseFunctor<Model, CalculateDepthFunctor, AlwaysCalculatingFunctor>
        EnqueueModelTranslucentModelBaseFunctor;

    //
    typedef BasicEnqueueModelTranslucentModelBaseFunctor<SkeletalModel, CalculateDepthOfSkeletalModelFunctor, AlwaysCalculatingFunctor>
        EnqueueSkeletalModelTranslucentModelBaseFunctor;

    //
    //
    typedef BasicEnqueueModelTranslucentModelBaseFunctor<Model, CalculateDepthFunctor, IsCalculatingOnlyLayer1Functor>
        FastEnqueueModelTranslucentModelBaseFunctor;

    //
    //
    typedef BasicEnqueueModelTranslucentModelBaseFunctor<SkeletalModel, CalculateDepthOfSkeletalModelFunctor, IsCalculatingOnlyLayer1Functor>
        FastEnqueueSkeletalModelTranslucentModelBaseFunctor;

    //----------------------------------------
    //
    //

    //
    struct ReportFunctor : public std::unary_function<void, reference>
    {
        ReportFunctor() : count(0) {}

        //
        void operator() (reference element)
        {
#ifdef NW_RELEASE
            NW_UNUSED_VARIABLE(element);
#else
            if (element.IsCommand())
            {
                NW_DEV_LOG("%3d : Command(%x)\n",
                    count,
                    reinterpret_cast<u32>(element.GetCommand()));
            }
            else
            {
                const ResMesh mesh = element.GetMesh();
                const ResModel model = element.GetModel()->GetResModel();
                const ResMaterial material =
                    element.GetModel()->GetMaterial(mesh.GetMaterialIndex())->GetOriginal();
                NW_DEV_LOG("%3d : Model(%s), Mesh(%s), Material(%s)\n",
                    count,
                    model.GetName(),
                    mesh.GetName(),
                    material.GetName());
            }

            ++count;
#endif
        }

        int count;
    };

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    class Builder
    {
    public:
        //
        Builder() : m_IsFixedSizeMemory(true), m_MaxRenderElements(64) {}

        //
        ~Builder() {}

        //
        //
        //
        //
        //
        Builder& IsFixedSizeMemory(bool isFixedSizeMemory)
        {
            m_IsFixedSizeMemory = isFixedSizeMemory;
            return *this;
        }

        //
        Builder& MaxRenderElements(int max) { m_MaxRenderElements = max; return *this; }

        //
        //
        //
        //
        //
        //
        BasicRenderQueue* Create(os::IAllocator* allocator)
        {
            void* queueMemory = allocator->Alloc<BasicRenderQueue>(1);
            NW_NULL_ASSERT(queueMemory);
            BasicRenderQueue* queue;
            if (m_IsFixedSizeMemory)
            {
                queue = new(queueMemory) BasicRenderQueue(allocator, m_MaxRenderElements);
            }
            else
            {
                queue = new(queueMemory) BasicRenderQueue(allocator);
            }
            return queue;
        }

    private:
        bool m_IsFixedSizeMemory;
        int m_MaxRenderElements;
    };

    //

    //----------------------------------------
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
    ElementType* EnqueueElement(
        const ElementType& element,
        ResMaterial::TranslucencyKind translucencyKind,
        float depth,
        u8 layerId)
    {
        bool pushed = this->m_List.push_back(element);
        if (!pushed)
        {
            return NULL;
        }

        ElementType* back = &m_List.back();

        if (this->m_KeyCachingState == UNUSE_CACHED_KEY ||
            translucencyKind == ResMaterial::TRANSLUCENCY_KIND_LAYER1)
        {
            RenderKeyFactory* keyFactory = this->GetKeyFactory(translucencyKind);
            back->Key() = keyFactory->CreateRenderKey(*back, depth, layerId);
        }
        else
        {
            if (ut::CheckFlag<u32, u32>(back->GetMesh().GetFlags(), ResMesh::FLAG_VALID_RENDER_KEY_CACHE))
            {
                // The cached key is used.
                back->Key() = back->GetMesh().GetRenderKeyCache();
            }
            else
            {
                // Create the key and cache it.
                RenderKeyFactory* keyFactory = this->GetKeyFactory(translucencyKind);
                back->Key() = keyFactory->CreateRenderKey(*back, depth, layerId);

                back->GetMesh().SetRenderKeyCache(back->Key());
                back->GetMesh().SetFlags(ut::EnableFlag<u32, u32>(
                    back->GetMesh().GetFlags(), ResMesh::FLAG_VALID_RENDER_KEY_CACHE));
            }
        }

        return back;
    }

    //
    //
    //
    //
    //
    //
    //
    ElementType* EnqueueMesh(
        ResMesh mesh,
        Model* model,
        float depth,
        u8 layerId)
    {
        NW_ASSERT(mesh.IsValid());
        NW_NULL_ASSERT(model);

        ElementType* enqueuedElement = EnqueueElement(
            ElementType(mesh, model), model->GetRenderLayerId(mesh), depth, layerId);
        NW_WARNING(enqueuedElement != NULL, "Failed to enqueue element");

        return enqueuedElement;
    }

    //
    //
    //
    //
    //
    //
    void EnqueueModel(
        Model* model,
        u8 layerId,
        const Camera& camera)
    {
        NW_NULL_ASSERT(model);
        gfx::ResMeshArray meshs = model->GetResMeshes();
        std::for_each(
            meshs.begin(),
            meshs.end(),
            EnqueueModelFunctor(this, model, layerId, camera));
    }

    //
    //
    //
    //
    //
    //
    void EnqueueSkeletalModel(
        SkeletalModel* model,
        u8 layerId,
        const Camera& camera)
    {
        NW_NULL_ASSERT(model);
        gfx::ResMeshArray meshs = model->GetResMeshes();
        std::for_each(
            meshs.begin(),
            meshs.end(),
            EnqueueSkeletalModelFunctor(this, model, layerId, camera));
    }

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
    //
    //
    //
    ElementType* EnqueueCommand(
        RenderCommand* command,
        ResMaterial::TranslucencyKind translucencyKind,
        u8 priority,
        u8 layerId)
    {
        RenderKeyFactory* keyFactory = this->GetKeyFactory(translucencyKind);

        bool pushed = this->m_List.push_back(
            ElementType(keyFactory->CreateCommandRenderKey(
                command, translucencyKind, priority, layerId)));

        if (pushed)
        {
            return &this->m_List.back();
        }
        else
        {
            return NULL;
        }
    }

    //
    //
    //
    //
    void Reset(bool cacheEnabled = false)
    {
        this->m_List.clear();
        this->m_KeyCachingState = (cacheEnabled) ? USE_CACHED_KEY : UNUSE_CACHED_KEY;
    }

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
    //
    //
    //
    //
    //
    //
    //
    void Reset(
        ElementKeyFactoryType* opacityKeyFactory,
        ElementKeyFactoryType* translucentKeyFactory,
        ElementKeyFactoryType* additiveKeyFactory,
        ElementKeyFactoryType* subtractionKeyFactory,
        bool cacheEnabled = false)
    {
        this->Reset(cacheEnabled);

        if (opacityKeyFactory != NULL)
        {
            this->m_OpacityKeyFactory = opacityKeyFactory;
        }
        if (translucentKeyFactory != NULL)
        {
            this->m_TranslucentKeyFactory = translucentKeyFactory;
        }
        if (additiveKeyFactory != NULL)
        {
            this->m_AdditiveKeyFactory = additiveKeyFactory;
        }
        if (subtractionKeyFactory != NULL)
        {
            this->m_SubtractionKeyFactory = subtractionKeyFactory;
        }
    }

    //

    //----------------------------------------
    //
    //

    //
    int Size() const
    {
        return m_List.size();
    }

    //
    bool Empty() const
    {
        return m_List.empty();
    }

    //
    ElementType& Peek()
    {
        return m_List.front();
    }

    //
    const ElementType& Peek() const
    {
        return m_List.front();
    }

    //
    iterator Begin()
    {
        return m_List.begin();
    }

    //
    const_iterator Begin() const
    {
        return m_List.begin();
    }

    //
    iterator End()
    {
        return m_List.end();
    }

    //
    const_iterator End() const
    {
        return m_List.end();
    }

    //
    reverse_iterator ReverseBegin()
    {
        return m_List.rbegin();
    }

    //
    const_reverse_iterator ReverseBegin() const
    {
        return m_List.rbegin();
    }

    //
    reverse_iterator ReverseEnd()
    {
        return m_List.rend();
    }

    //
    const_reverse_iterator ReverseEnd() const
    {
        return m_List.rend();
    }

    //

private:
    explicit BasicRenderQueue(os::IAllocator* allocator)
    : GfxObject(allocator),
      m_List(allocator),
      m_OpacityKeyFactory(NULL),
      m_TranslucentKeyFactory(NULL),
      m_AdditiveKeyFactory(NULL),
      m_SubtractionKeyFactory(NULL),
      m_KeyCachingState(UNUSE_CACHED_KEY) {}

    BasicRenderQueue(os::IAllocator* allocator, int maxRenderElements)
    : GfxObject(allocator),
      m_List(maxRenderElements, allocator),
      m_OpacityKeyFactory(NULL),
      m_TranslucentKeyFactory(NULL),
      m_AdditiveKeyFactory(NULL),
      m_SubtractionKeyFactory(NULL),
      m_KeyCachingState(UNUSE_CACHED_KEY) {}

    virtual ~BasicRenderQueue() {}

    ElementKeyFactoryType* GetKeyFactory(ResMaterial::TranslucencyKind translucencyKind)
    {
        ElementKeyFactoryType* keyFactory = NULL;
        switch (translucencyKind)
        {
        case ResMaterial::TRANSLUCENCY_KIND_LAYER0:
            keyFactory = this->m_OpacityKeyFactory;
            break;
        case ResMaterial::TRANSLUCENCY_KIND_LAYER1:
            keyFactory = this->m_TranslucentKeyFactory;
            break;
        case ResMaterial::TRANSLUCENCY_KIND_LAYER2:
            keyFactory = this->m_SubtractionKeyFactory;
            break;
        case ResMaterial::TRANSLUCENCY_KIND_LAYER3:
            keyFactory = this->m_AdditiveKeyFactory;
            break;
        default: NW_FAILSAFE_IF(false) { keyFactory = this->m_OpacityKeyFactory; } break;
        }
        NW_NULL_ASSERT(keyFactory);
        return keyFactory;
    }

    TElementList m_List;
    ElementKeyFactoryType* m_OpacityKeyFactory;
    ElementKeyFactoryType* m_TranslucentKeyFactory;
    ElementKeyFactoryType* m_AdditiveKeyFactory;
    ElementKeyFactoryType* m_SubtractionKeyFactory;

    enum KeyCachingState
    {
        UNUSE_CACHED_KEY,
        USE_CACHED_KEY
    };

    KeyCachingState m_KeyCachingState;
    u8 padding[3];
};

//
typedef BasicRenderQueue<
    BasicRenderElement<RenderKeyType>,
    ut::MoveArray<BasicRenderElement<RenderKeyType> > >
        RenderQueue;

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct RenderElementCompare
: public std::binary_function<RenderElement, RenderElement, bool>
{
    bool operator() (
        const RenderElement& lhs,
        const RenderElement& rhs)
    {
        return lhs.Key() < rhs.Key();
    }
};

} // namespace gfx
} // namespace nw

#endif // NW_GFX_RENDERQUEUE_H_
