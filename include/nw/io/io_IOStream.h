#pragma once

#include <nw/Assert.h>
#include <nw/ut/ut_RuntimeTypeInfo.h>

namespace nw   {
namespace io   {

class IOStream
{
public:
    NW_UT_RUNTIME_TYPEINFO;

    typedef void (*IOStreamCallback)(s32 result, IOStream* stream, void* arg);

    virtual ~IOStream() {}
    virtual bool CanRead() const = 0;
    virtual bool CanWrite() const = 0;
    virtual bool CanAsync() const = 0;
    virtual u32 GetOffsetAlign() const { return 1; }
    virtual u32 GetSizeAlign () const { return 1; }
    virtual u32 GetBufferAlign() const { return 1; }
    virtual s32 Read(void* buf, u32 length);
    virtual bool ReadAsync(void* buf, u32 length, IOStreamCallback callback, void* arg);
    virtual s32 Write(const void* buf, u32 length);
    virtual bool WriteAsync(const void* buf, u32 length, IOStreamCallback callback, void* arg);
    
    s32 WaitAsync() const;

    virtual bool IsBusy() const;
    virtual void Close() = 0;

    bool IsAvailable() const { return m_Available; }

protected:

    IOStream() : 
        m_Available(false),
        m_Callback (NULL),
        m_Arg (NULL) {}


    bool m_Available;
    s32 m_AsyncResult;
    IOStreamCallback m_Callback;
    void* m_Arg;
};

}
}