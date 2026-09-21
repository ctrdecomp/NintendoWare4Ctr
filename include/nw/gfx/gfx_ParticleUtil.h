#pragma once

#include <nw/types.h>
#include <nw/ut/ut_MoveArray.h>

#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <limits>
#include <functional>
#include <algorithm>

#include <nw/gfx/res/gfx_ResParticleModel.h>
#include <nw/gfx/res/gfx_ResParticleSet.h>

namespace nw {
namespace gfx {

class SceneNode;
class ParticleContext;

namespace res {
    class ResParticleInitializer;
    struct ResParticleInitializerData;
    class ResParticleUpdater;
    struct ResParticleUpdaterData;
} // namespace res

class ParticleUtil
{
public:
    static void SetupParticleObject(nw::ut::MoveArray<SceneNode*>* sceneNodeArray,ParticleContext* context);

    static void ResolveParticleObject(nw::ut::MoveArray<SceneNode*>* sceneNodeArray);

    static ResParticleInitializerData* DuplicateResParticleInitializer(const ResParticleInitializer* src,nw::os::IAllocator* allocator);

    static void GetMemorySizeForDuplicateResParticleInitializerInternal(nw::os::MemorySizeCalculator* pSize,const ResParticleInitializer* src);

    static ResParticleUpdaterData* DuplicateResParticleUpdater(const ResParticleUpdater* src,nw::os::IAllocator* allocator);

    static void GetMemorySizeForDuplicateResParticleUpdaterInternal(nw::os::MemorySizeCalculator* pSize,const ResParticleUpdater* src);
};

struct ParticleSetCompare : public std::binary_function<ParticleSet*, ParticleSet*, bool>
{
    bool operator() (const ParticleSet* lhs,const ParticleSet* rhs);
};

class ParticleSetIsBufferFlushEnabledSetter
{
public:
    ParticleSetIsBufferFlushEnabledSetter(bool flag): 
        m_Flag(flag) {}

    void operator()(ResModel resModel) const
    {
        ResParticleModel resParticleModel = ResDynamicCast<ResParticleModel>(resModel);
        if (resParticleModel.IsValid())
        {
            resParticleModel.ForeachParticleSet(ParticleSetIsBufferFlushEnabledSetter(m_Flag));
        }
    }

    void operator()(ResParticleModel resParticleModel) const
    {
        if (resParticleModel.IsValid())
        {
            resParticleModel.ForeachParticleSet(ParticleSetIsBufferFlushEnabledSetter(m_Flag));
        }
    }

    void operator()(ResParticleSet resParticleSet) const
    {
        resParticleSet.SetIsBufferFlushEnabled(m_Flag);
    }

private:
    bool m_Flag;
};

}
}