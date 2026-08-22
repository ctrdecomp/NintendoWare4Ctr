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

class OffScreenBuffer : public IRenderTarget{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(OffScreenBuffer);

public:
    NW_UT_RUNTIME_TYPEINFO;

    virtual const Description& GetDescription() const { return mDescription; }
    virtual const FrameBufferObject& GetBufferObject() const { return mBackBufferObject; }

private:
    OffScreenBuffer(nw::os::IAllocator* pAllocator, const Description& description, ResPixelBasedTexture resTexture);
    virtual ~OffScreenBuffer();

    FrameBufferObject mBackBufferObject;
    void* mActivateCommand;
    Description mDescription;
    ResTexture mTexture;

    friend class IRenderTarget;
};

}
}