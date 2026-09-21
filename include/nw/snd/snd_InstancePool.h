#ifndef NW_SND_INSTANCE_POOL_H_
#define NW_SND_INSTANCE_POOL_H_

#include <new>
#include <nw/snd/snd_Global.h>

namespace nw { 
namespace snd { 
namespace internal {

class PoolImpl
{
public:
    PoolImpl(): 
        m_pNext(NULL)
        {
    }

protected:
    u32 CreateImpl(void* buffer, size_t size, u32 objSize);
    void DestroyImpl();
    int CountImpl() const;
    void* AllocImpl();
    void FreeImpl(void* ptr);

private:
    PoolImpl* m_pNext;
};

template <typename T>
class InstancePool : private PoolImpl
{
public:
    u32 Create(void* buffer, unsigned long size)
{
        u32 objSize =
            (sizeof(T) > sizeof(InstancePool<T>*))
                ? sizeof(T)
                : sizeof(InstancePool<T>*);
        return CreateImpl(buffer, size, objSize);
    }

    void Destroy()
    {
        DestroyImpl();
    }

    int Count() const
    {
        return CountImpl();
    }

    T* Alloc()
    {
        void *ptr = AllocImpl();
        if (ptr == NULL)
            return NULL;
        return new (ptr) T();
    }

    void Free(T* obj)
    {
        if (obj == NULL)
            return;
        obj->~T();
        FreeImpl(obj);
    }
};

} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_INSTANCE_POOL_H_
