/**
 * @file
 * @addtogroup studioapi_vk studioapi_vk
 */

#ifndef VK_STUDIOAPI_BUFFER_H
#define VK_STUDIOAPI_BUFFER_H

#include "studiorender/studioapi/istudioapi_buffer.h"
#include "studiorender/studioapi/vk/vk_studioapi_device.h"
#include "studiorender/studioapi/vk/vk_studioapi_syncmgr.h"

// Forward declarations
class CStudioAPICmdListVk;

/**
 * @ingroup studioapi_vk
 * @brief Vulkan StudioAPI buffer
 */
class CStudioAPIBufferVk : public TRefCounted<IStudioAPIBuffer>
{
public:
	enum
	{
		NUM_BUFFERS = 3		/**< Maximum buffer count */
	};

	/*
	 * @brief Constructor
	 * @param pData					Buffer data
	 * @param dataSize				Data size in bytes
	 * @param dataStride			Data stride
	 * @param usageFlags			Usage flags (see studioAPIBufferUsageFlag_t)
	 * @param pDebugName			Debug name
	 */
	CStudioAPIBufferVk( const byte* pData, uint64 dataSize, uint32 dataStride, uint32 usageFlags, const achar* pDebugName = "" );

	/**
	 * @brief Destructor
	 */
	~CStudioAPIBufferVk();

	/**
	 * @brief Get usage flags
	 * @return Return usage flags (see studioAPIBufferUsageFlag_t)
	 */
	virtual uint32 GetUsageFlags() const override;

	/**
	 * @brief Get buffer size
	 * @return Return buffer size
	 */
	virtual uint64 GetSize() const override;

	/**
	 * @brief Get buffer stride
	 * @return Return buffer stride
	 */
	virtual uint32 GetStride() const override;

	/**
	 * @brief Get buffer memory flags
	 * @return Return buffer memory flags (see studioAPIBufferMemoryFlag_t)
	 */
	virtual uint8 GetMemoryFlags() const override;

	/**
	 * @brief Map memory of the buffer data
	 * @warning Before use check memory flags by GetMemoryFlags(). If STUDIOAPI_BUFFER_MEMORY_FLAG_CPU is set then you can use the function,
	 * otherwise use IStudioAPICmdList::CopyBuffer
	 *
	 * @param size			Data size to map
	 * @param offset		Offset in the buffer
	 * @param mappedData	Output mapped data
	 */
	virtual void MapMemory( uint64 size, uint64 offset, studioAPIMappedBufferData_t& mappedData ) override;

	/**
	 * @brief Unmap memory of the buffer data
	 * @warning Before use check memory flags by GetMemoryFlags(). If STUDIOAPI_BUFFER_MEMORY_FLAG_CPU is set then you can use the function,
	 * otherwise use IStudioAPICmdList::CopyBuffer
	 *
	 * @param mappedData	Mapped data to unmap
	 */
	virtual void UnmapMemory( studioAPIMappedBufferData_t& mappedData ) override;

	/**
	 * @brief Update a data in the buffer
	 * @warning For use the buffer must have STUDIOAPI_BUFFER_USAGE_FLAG_TRANSFER_DST flag 
	 * 
	 * @param pCmdContext	Command context
	 * @param pData			A new buffer data
	 * @param dataSize		Data size
	 * @param offset		Buffer offset
	 */
	virtual void UpdateData( IStudioAPICmdContext* pCmdContext, byte* pData, uint64 dataSize, uint64 offset = 0 ) override;

	/**
	 * @brief Update a synchronization state of the buffer
	 * The function update a synchronization state of the buffer. Useful for
	 * for implicit transfer of ownership without barrier and update access with stage masks
	 * 
	 * @param vkDstAccessMask		Destination Vulkan access mask
	 * @param vkDstStageMask		Destination Vulkan pipeline stage mask
	 * @param dstQueueFamilyIndex	Destination queue family which will be owner of the resource
	 */
	FORCEINLINE void UpdateSyncState( VkAccessFlags vkDstAccessMask, VkPipelineStageFlags vkDstStageMask, uint32 dstQueueFamilyIndex )
	{
		PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
		VK_UpdateSyncStateBuffer( vkDstAccessMask, vkDstStageMask, dstQueueFamilyIndex, syncState );
	}

	/**
	 * @brief Update a synchronization state and make a buffer barrier
	 * The function update a synchronization state of the buffer and make
	 * a buffer barrier if it need. Useful for transfer ownership and
	 * make barriers for the buffer
	 * 
	 * @param pCmdList				A command list which will be placed a barrier
	 * @param vkDstAccessMask		Destination Vulkan access mask
	 * @param vkDstStageMask		Destination Vulkan pipeline stage mask
	 * @param dstQueueFamilyIndex	Destination queue family which will be owner of the resource
	 */
	void UpdateSyncStateWithBarrier( CStudioAPICmdListVk* pCmdList, VkAccessFlags vkDstAccessMask, VkPipelineStageFlags vkDstStageMask, uint32 dstQueueFamilyIndex );

	/**
	 * @brief Swap the current buffer index
	 */
	FORCEINLINE void SwapCurrentBufferIndex()
	{
		currentBufferIndex = ( currentBufferIndex + 1 ) % numBuffers;
	}

	/**
	 * @brief Get buffer offset
	 * @return Return the buffer offset
	 */
	FORCEINLINE uint64 GetOffset() const
	{
		return bufferOffsets[currentBufferIndex];
	}

	/**
	 * @brief Get Vulkan buffer
	 * @return Return Vulkan buffer. If the buffer hasn't been created return VK_NULL_HANDLE
	 */
	FORCEINLINE VkBuffer GetVkBuffer() const
	{
		return vkBuffer;
	}

	/**
	 * @brief Get Vulkan index type
	 * @return Return Vulkan index type
	 */
	FORCEINLINE VkIndexType GetVkIndexType() const
	{
		return stride == 4 ? VK_INDEX_TYPE_UINT32 : VK_INDEX_TYPE_UINT16;
	}

	/**
	 * @brief Get synchronization state
	 * @return Return synchronization state
	 */
	FORCEINLINE const studioAPISyncStateBufferVk_t& GetSyncState() const
	{
		return syncState;
	}

private:
	/**
	 * @brief Delegate called when StudioAPI Vulkan is shutdown
	 * @param pUserData		Pointer to CStudioAPIBufferVk
	 */
	static void OnStudioAPIVkShutdown( void* pUserData );

	/**
	 * @brief Get number of buffers from usage flags
	 * @param usageFlags	Usage flags
	 * @return Return the number of buffers from usage flags
	 */
	FORCEINLINE static uint32 GetNumBuffersFromUsage( uint32 usageFlags )
	{
		return usageFlags & STUDIOAPI_BUFFER_USAGE_FLAG_DYNAMIC ? NUM_BUFFERS : 1;
	}

	uint32									usageFlags;						/**< Usage flags (see studioAPIBufferUsageFlag_t) */
	uint8									memoryFlags;					/**< Memory flags (see studioAPIBufferMemoryFlag_t) */
	uint64									size;							/**< Buffer size */
	uint32									stride;							/**< Data stride */
	uint8									numBuffers;						/**< Number of buffers */
	uint8									currentBufferIndex;				/**< Current buffer index */
	uint64									bufferOffsets[NUM_BUFFERS];		/**< Offset used for binding a buffer */
	VkBuffer								vkBuffer;						/**< Vulkan buffer */
	VmaAllocation							vmaAllocation;					/**< VMA allocation */
	studioAPISyncStateBufferVk_t			syncState;						/**< Synchronization state */
	COnStudioAPIVkShutdown::funcDelegate_t*	pStudioAPIVkShutdownDelegate;	/**< Delegate handle 'on StudioAPI Vulkan shutdown' */
};

#endif // !VK_STUDIOAPI_BUFFER_H