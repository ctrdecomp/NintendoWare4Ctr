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


#ifndef NW_GFX_SCENEHELPER_H_
#define NW_GFX_SCENEHELPER_H_

#include <nw/gfx/gfx_SceneNode.h>
#include <nw/ut/ut_MoveArray.h>

namespace nw
{
namespace gfx
{

//
class SceneHelper
{
private:
    SceneHelper();
    NW_DISALLOW_COPY_AND_ASSIGN(SceneHelper);

public:
    //
    //
    //
    //
    //
    //
    template<typename TIterator>
    static void ResolveReference(TIterator begin, TIterator end)
    {
        for (TIterator parent = begin; parent != end; ++parent)
        {
            for (TIterator child = begin; child != end; ++child)
            {
                NW_NULL_ASSERT(ut::DynamicCast<SceneNode*>(*child));
                ResolveReferenceImpl(*parent, *child);
            }
        }
    }

    //
    template<typename TIterator>
    static void ResolveReference(const std::pair<TIterator, TIterator>& range)
    {
        ResolveReference<TIterator>(range.first, range.second);
    }

    //
    template<typename TNode, int TSize>
    static void ResolveReference(TNode (&nodes)[TSize])
    {
        ResolveReference<TNode*>(nodes, nodes + TSize);
    }

    //
    template<typename TNodeArray>
    static void ResolveReference(TNodeArray& nodes)
    {
        ResolveReference<TNodeArray::iterator>(nodes.begin(), nodes.end());
    }

    //
    //
    //
    //
    //
    //
    //
    //
    template <typename TIterator, typename TFunction>
    static void
    ForeachRootNodes(TIterator begin, TIterator end, TFunction function)
    {
        for (TIterator iter = begin; iter != end; ++iter)
        {
            if ((*iter)->GetParent() == NULL)
            {
                function(*iter);
            }
        }
    }

    //----------------------------------------
    //
    //

    //
    static float CalculateDepth(
        const math::VEC3& localPosition,
        const math::MTX34& worldMatrix,
        const Camera& camera)
    {
        math::VEC3 position;
        math::VEC3Transform(&position, &worldMatrix, &localPosition);
        math::VEC3Transform(&position, &camera.ViewMatrix(), &position);
        const math::MTX44& projection = camera.ProjectionMatrix();
        float z =
            projection.f._20 * position.x +
            projection.f._21 * position.y +
            projection.f._22 * position.z +
            projection.f._23;
        float w =
            projection.f._30 * position.x +
            projection.f._31 * position.y +
            projection.f._32 * position.z +
            projection.f._33;
        // The Z depth in the clipping coordinate system would have a range of 0 to -w so convert that so it has a range from 0 to 1.0.
        // 
        return ut::Clamp(-z / w, 0.0f, 1.0f);
    }

    //
    static float CalculateDepth(
        const math::MTX34& worldMatrix,
        const Camera& camera)
    {
        math::VEC3 position(worldMatrix.GetColumn(3));
        math::VEC3Transform(&position, &camera.ViewMatrix(), &position);
        const math::MTX44& projection = camera.ProjectionMatrix();
        float z =
            projection.f._20 * position.x +
            projection.f._21 * position.y +
            projection.f._22 * position.z +
            projection.f._23;
        float w =
            projection.f._30 * position.x +
            projection.f._31 * position.y +
            projection.f._32 * position.z +
            projection.f._33;
        // The Z depth in the clipping coordinate system would have a range of 0 to -w so convert that so it has a range from 0 to 1.0.
        // 
        return ut::Clamp(-z / w, 0.0f, 1.0f);
    }

    //

private:
    // Internal implementation of scene tree parsing.
    static void ResolveReferenceImpl(SceneNode* parent, SceneNode* child);
};

//
class AttachNode
{
public:
    //
    //
    //
    AttachNode(nw::gfx::SceneNode* parent)
    : m_Parent(parent)
    {}

    //
    //
    //
    template <typename TNode>
    void operator() (TNode* node)
    {
        NW_POINTER_ASSERT(m_Parent);
        bool result = m_Parent->AttachChild(node);
        NW_ASSERTMSG(result, "Failed to attach child");
    }

private:
    nw::gfx::SceneNode* m_Parent;
};

//
class TryAttachNode
{
public:
    //
    //
    //
    //
    //
    //
    //
    TryAttachNode(nw::gfx::SceneNode* parent, u32* pFailureCount = NULL)
        : m_Parent(parent)
        , m_pFailureCount(pFailureCount)
    {}

    //
    //
    //
    //
    //
    //
    template <typename TNode>
    void operator() (TNode* node)
    {
        NW_POINTER_ASSERT(m_Parent);
        bool result = m_Parent->AttachChild(node);
        if (!result && m_pFailureCount != NULL)
        {
            (*m_pFailureCount)++;
        }
    }

private:
    nw::gfx::SceneNode* m_Parent;
    u32* m_pFailureCount;
};

} // namespace gfx
} // namespace nw

#endif // NW_GFX_SCENEHELPER_H_
