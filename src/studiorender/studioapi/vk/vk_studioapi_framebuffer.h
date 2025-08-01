#pragma once
#include "studiorender/studioapi/istudioapi_framebuffer.h"
#include "studiorender/studioapi/vk/vk_studioapi_device.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CStudioAPIRenderPassVk;


//-----------------------------------------------------------------------------
// Vulkan Studio API frame buffer
//-----------------------------------------------------------------------------
class CStudioAPIFrameBufferVk : public TRefCounted<IStudioAPIFrameBuffer>
{
public:
	// IStudioAPIFrameBuffer interface
	virtual ivec2_t GetSize() const override;

	CStudioAPIFrameBufferVk( const studioAPIFrameBufferCreateInfo_t& createInfo, const achar* pDebugName = "" );
	~CStudioAPIFrameBufferVk();

	FORCEINLINE VkFramebuffer GetVkFrameBuffer() const			{ return vkFrameBuffer;	}
	FORCEINLINE uint8 GetNumClearValues() const					{ return numClearValues; }
	FORCEINLINE const VkClearValue* GetVkClearValues() const	{ return vkClearValues;	}

private:
	static void OnStudioAPIVkShutdown( void* pUserData );

	ivec2_t									size;
	VkFramebuffer							vkFrameBuffer;
	COnStudioAPIVkShutdown::funcDelegate_t*	pStudioAPIVkShutdownDelegate;
	uint8									numClearValues;
	VkClearValue							vkClearValues[STUDIOAPI_MAX_SIMULTANEOUS_RENDER_TARGETS + 1];
};