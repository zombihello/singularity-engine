/**
 * @file
 * @addtogroup studioapi_vk studioapi_vk
 */

#ifndef VK_STUDIOAPI_RENDERBUFFER_H
#define VK_STUDIOAPI_RENDERBUFFER_H

#include "studiorender/studioapi/istudioapi_framebuffer.h"
#include "studiorender/studioapi/vk/vk_studioapi_device.h"

// Forward declarations
class CStudioAPIRenderPassVk;

/**
 * @ingroup studioapi_vk
 * @brief Vulkan studio api frame buffer
 */
class CStudioAPIFrameBufferVk : public TRefCounted<IStudioAPIFrameBuffer>
{
public:
	/*
	 * @brief Constructor
	 * @param createInfo	Information to create the frame buffer
	 * @param pDebugName	Debug name
	 */
	CStudioAPIFrameBufferVk( const studioAPIFrameBufferCreateInfo_t& createInfo, const achar* pDebugName = "" );

	/**
	 * @brief Destructor
	 */
	~CStudioAPIFrameBufferVk();

	/**
	 * @brief Get the frame buffer image size
	 * @return Return the frame buffer size
	 */
	virtual ivec2_t GetSize() const override;

	/**
	 * @brief Get the Vulkan frame buffer
	 * @return Return the Vulkan frame buffer. If the frame buffer isn't created return VK_NULL_HANDLE
	 */
	FORCEINLINE VkFramebuffer GetVkFrameBuffer() const
	{
		return vkFrameBuffer;
	}

	/**
	 * @brief Get clear values number
	 * @return Return clear values number
	 */
	FORCEINLINE uint8 GetNumClearValues() const
	{
		return numClearValues;
	}

	/**
	 * @brief Get clear values
	 * @return Return clear values
	 */
	FORCEINLINE const VkClearValue* GetVkClearValues() const
	{
		return vkClearValues;
	}

private:
	/**
	 * @brief Delegate called when StudioAPI Vulkan is shutdown
	 * @param pUserData		Pointer to CStudioAPIFrameBufferVk
	 */
	static void OnStudioAPIVkShutdown( void* pUserData );

	ivec2_t									size;															/**< Frame buffer image size */
	VkFramebuffer							vkFrameBuffer;													/**< Vulkan frame buffer */
	COnStudioAPIVkShutdown::funcDelegate_t*	pStudioAPIVkShutdownDelegate;									/**< Delegate handle 'on StudioAPI Vulkan shutdown' */
	uint8									numClearValues;													/**< Clear values for each render target */
	VkClearValue							vkClearValues[STUDIOAPI_MAX_SIMULTANEOUS_RENDER_TARGETS + 1];	/**< Vulkan clear values for each render target */
};

#endif // !VK_STUDIOAPI_RENDERBUFFER_H