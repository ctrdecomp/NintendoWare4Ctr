#pragma once

#include <new>

#include <nw/os/os_Memory.h>
#include <nw/ut/ut_Inlines.h>
#include <nw/ut/ut_LinkList.h>

#include <nw/lyt/lyt_Animation.h>
#include <nw/lyt/lyt_Types.h>

namespace nw {
namespace ut {
struct Rect;
}

namespace font {
template<typename CharType>
class TagProcessorBase;
}

namespace lyt {

namespace res {
struct AnimShareInfoList;
}

class ResourceAccessor;
struct ResBlockSet;
class Pane;
class GroupContainer;
class DrawInfo;

typedef ut::LinkList<AnimTransform, offsetof(AnimTransform, mLink)> AnimTransformList;

class Layout{
public:

    static nw::os::IAllocator* GetAllocator() { return s_pAllocator; }
    static nw::os::IAllocator* GetDeviceMemoryAllocator() { return s_pDeviceMemoryAllocator; }
    static void SetAllocator(nw::os::IAllocator* pAllocator);
    static void SetDeviceMemoryAllocator(nw::os::IAllocator* pAllocator);
    static void* AllocMemory(u32 size, u8 alignment = 4);
    static void* AllocDeviceMemory(u32 size, u8 alignment = 4);
    static void FreeMemory(void* mem);
    static void FreeDeviceMemory(void* mem);

    Layout();
    virtual ~Layout();

    virtual bool Build(const void* lytResBuf, ResourceAccessor* pResAcsr);

#ifdef NW_LYT_DMPGL_ENABLED
    static void SetLayoutDrawEnable(bool enable){ sLayoutDrawEnable = enable; }
    static bool GetLayoutDrawEnable(){
#ifdef NW_PLATFORM_CTR
        return sLayoutDrawEnable;
#else
        return true;
#endif
    }
#endif

    // AnimTransform
    virtual AnimTransform* CreateAnimTransform();
    virtual AnimTransform* CreateAnimTransform(const void* animResBuf, ResourceAccessor* pResAcsr);
    virtual AnimTransform* CreateAnimTransform(const AnimResource& animRes, ResourceAccessor* pResAcsr);
    virtual void DeleteAnimTransform(AnimTransform* pAnimTransform);
    virtual void BindAnimation(AnimTransform* pAnimTrans);
    virtual void UnbindAnimation(AnimTransform* pAnimTrans);
    virtual void UnbindAllAnimation();
    virtual bool BindAnimationAuto(const AnimResource& animRes, ResourceAccessor* pResAcsr);
    virtual void SetAnimationEnable(AnimTransform* pAnimTrans, bool bEnable);
    virtual void Animate(u32 option = 0);

    virtual void CalculateMtx(const DrawInfo& drawInfo);
    virtual void Draw(const DrawInfo& drawInfo);

    // Getters
    Pane*           GetRootPane()       const { return mpRootPane; }
    GroupContainer* GetGroupContainer() const { return mpGroupContainer; }
    const Size&     GetLayoutSize()     const { return mLayoutSize; }
    const ut::Rect  GetLayoutRect()     const;

    virtual void SetTagProcessor(font::TagProcessorBase<wchar_t>* pTagProcessor);

    const AnimTransformList& GetAnimTransformList() const { return mAnimTransList; }
    AnimTransformList&       GetAnimTransformList()       { return mAnimTransList; }

    // Object creation
    template<typename T>
    static T* NewObj(){
        if (void* pMem = Layout::AllocMemory(sizeof(T)))
            return new (pMem) T();
        return 0;
    }

    template<typename T, typename Param1>
    static T* NewObj(Param1 param1){
        if (void* pMem = Layout::AllocMemory(sizeof(T)))
            return new (pMem) T(param1);
        return 0;
    }

    template<typename T, typename Param1, typename Param2>
    static T* NewObj(Param1 param1, Param2 param2){
        if (void* pMem = Layout::AllocMemory(sizeof(T)))
            return new (pMem) T(param1, param2);
        return 0;
    }

    template<typename T, typename Param1, typename Param2, typename Param3>
    static T* NewObj(Param1 param1, Param2 param2, Param3 param3){
        if (void* pMem = Layout::AllocMemory(sizeof(T)))
            return new (pMem) T(param1, param2, param3);
        return 0;
    }

    template<typename T, typename Param1, typename Param2, typename Param3, typename Param4>
    static T* NewObj(Param1 param1, Param2 param2, Param3 param3, Param4 param4){
        if (void* pMem = Layout::AllocMemory(sizeof(T)))
            return new (pMem) T(param1, param2, param3, param4);
        return 0;
    }

    template<typename T>
    static T* NewArray(u32 num){
        void* pMem = AllocMemory(sizeof(T) * num);
        if (!pMem) return 0;
        T* const objAry = static_cast<T*>(pMem);
        for (u32 i = 0; i < num; ++i)
            new (&objAry[i]) T();
        return objAry;
    }

    template<typename T>
    static void DeleteObj(T* pObj){
        if (pObj){
            pObj->~T();
            FreeMemory(pObj);
        }
    }

    template<typename T>
    static void DeleteArray(T objAry[], u32 num){
        if (objAry){
            for (u32 i = 0; i < num; ++i)
                objAry[i].~T();
            FreeMemory(objAry);
        }
    }

    template<typename T>
    static void DeletePrimArray(T objAry[]){
        if (objAry)
            FreeMemory(objAry);
    }

protected:
    virtual Pane* BuildPaneObj(s32 kind, const void* pBlock, const ResBlockSet& resBlockSet);

    void SetRootPane(Pane* pPane)                        { mpRootPane = pPane; }
    void SetGroupContainer(GroupContainer* pGroupContainer) { mpGroupContainer = pGroupContainer; }
    void SetLayoutSize(const Size& size)                 { mLayoutSize = size; }

protected:
    static nw::os::IAllocator* s_pAllocator;
    static nw::os::IAllocator* s_pDeviceMemoryAllocator;
    static bool                s_LayoutDrawEnable;

    AnimTransformList mAnimTransList;
    Pane*             mpRootPane;
    GroupContainer*   mpGroupContainer;
    Size              mLayoutSize;

private:
    Layout(const Layout& other);
    Layout& operator=(const Layout& other);
};

} // namespace lyt
} // namespace nw