#include <nw/gfx/gfx_IRenderTarget.h>
#include <nw/ut/ut_Preprocessor.h>
#include <nw/gfx/res/gfx_ResTexture.h>
#include <nw/gfx/gfx_FrameBuffer.h>

namespace nw{
namespace os{
    class IAllocator;
}
namespace gfx
{

class OffScreenBuffer : public IRenderTarget
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(OffScreenBuffer);

public:
    NW_UT_RUNTIME_TYPEINFO;

    virtual const Description& GetDescription() const { return m_Description; }
    virtual const FrameBufferObject& GetBufferObject() const { return m_BackBufferObject; }

private:
    OffScreenBuffer(nw::os::IAllocator* pAllocator, const Description& description, ResPixelBasedTexture resTexture);
    virtual ~OffScreenBuffer();

    FrameBufferObject m_BackBufferObject;
    void* m_ActivateCommand;
    Description m_Description;
    ResTexture m_Texture;

    friend class IRenderTarget;
};

}
}