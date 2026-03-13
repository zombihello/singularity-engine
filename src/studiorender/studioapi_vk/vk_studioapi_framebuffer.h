#pragma once
#include "studiorender/studioapi/istudioapi_framebuffer.h"
#include "studiorender/studioapi_vk/vk_studioapi_device.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CStudioAPIRenderPassVk;

//-----------------------------------------------------------------------------
// Vulkan Studio API frame buffer
//-----------------------------------------------------------------------------
class CStudioAPIFrameBufferVk : public CRefCounted<IStudioAPIFrameBuffer>
{
public:
	// IStudioAPIFrameBuffer interface
	virtual vector2i_t GetSize() const override;

	CStudioAPIFrameBufferVk( const studioAPIFrameBufferCreateInfo_t& createInfo, const char* pDebugName = "" );
	~CStudioAPIFrameBufferVk();

	FORCEINLINE VkFramebuffer		GetVkFrameBuffer() const { return vkFrameBuffer; }
	FORCEINLINE uint8				GetNumClearValues() const { return numClearValues; }
	FORCEINLINE const VkClearValue* GetVkClearValues() const { return vkClearValues; }

private:
	static void OnStudioAPIVkShutdown( void* pUserData );

	vector2i_t						 size;
	VkFramebuffer					 vkFrameBuffer;
	COnStudioAPIVkShutdown::handle_t onStudioAPIVkShutdownHandle;
	uint8							 numClearValues;
	VkClearValue					 vkClearValues[STUDIOAPI_MAX_SIMULTANEOUS_RENDER_TARGETS + 1];
};
