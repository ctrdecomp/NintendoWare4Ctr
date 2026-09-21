#include <nw/gfx/gfx_IRenderTarget.h>
#include <nw/ut/ut_Preprocessor.h>

namespace nw{
namespace os{
    class IAllocator;
}
namespace gfx{

class OnScreenBuffer : public IRenderTarget
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(OnScreenBuffer);

public:
    NW_UT_RUNTIME_TYPEINFO;

    virtual const Description& GetDescription() const { return m_Description; }

    virtual const FrameBufferObject& GetBufferObject() const { return m_BackBufferObject; }

private:
    OnScreenBuffer(nw::os::IAllocator* pAllocator, const Description& description);
    
    void InitializeGl();
    void InitizlizeManulally();

    virtual ~OnScreenBuffer();

    u32 m_ColorBuffer;
    u32 m_DepthBuffer;
    FrameBufferObject m_BackBufferObject;
    Description m_Description;

    friend class IRenderTarget;
};

}
}