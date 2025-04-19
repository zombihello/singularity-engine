#ifndef ISTUDIOAPI_SWAPCHAIN_H
#define ISTUDIOAPI_SWAPCHAIN_H

#include "stdlib/math/math.h"
#include "stdlib/delegate.h"
#include "studiorender/studioapi/istudioapi_framebuffer.h"
#include "studiorender/studioapi/istudioapi_renderpass.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStudioAPISwapChain;


//-----------------------------------------------------------------------------
// StudioAPI swap chain image interface
//-----------------------------------------------------------------------------
class IStudioAPISwapChainImage
{
public:
	virtual IStudioAPISwapChain* GetSwapChain() const = 0;
	virtual uint32 GetIndex() const = 0;
};


//-----------------------------------------------------------------------------
// StudioAPI swap chain interface
//-----------------------------------------------------------------------------
class IStudioAPISwapChain : public IStudioAPIResource
{
public:
	DECLARE_MULTICAST_DELEGATE_INTERFACE( IOnReCreated, IStudioAPISwapChain* /* pStudioAPISwapChain */, bool /* bChangedImageFormat */ );

	virtual ~IStudioAPISwapChain() {}

	virtual void Resize( uint32 width, uint32 height ) = 0;
	virtual bool AcquireNextImage() = 0;
	virtual bool Present() = 0;
	virtual bool ReCreate() = 0;

	virtual bool IsUseVSync() const = 0;
	virtual bool IsValid() const = 0;
	virtual ivec2_t GetSize() const = 0;
	virtual uint32 GetCurrentImageIndex() const = 0;
	virtual IStudioAPISwapChainImage* GetCurrentImage() const = 0;
	virtual uint32 GetNumImages() const = 0;
	virtual IStudioAPISwapChainImage* GetImage( uint32 index ) const = 0;
	virtual windowHandle_t GetWindowHandle() const = 0;
	
	virtual IOnReCreated* OnReCreated() const = 0;
};

#endif // !ISTUDIOAPI_SWAPCHAIN_H