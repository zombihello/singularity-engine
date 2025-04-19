#ifndef VK_STUDIOAPI_BUFFER_H
#define VK_STUDIOAPI_BUFFER_H

#include "studiorender/studioapi/istudioapi_buffer.h"
#include "studiorender/studioapi/vk/vk_studioapi_device.h"
#include "studiorender/studioapi/vk/vk_studioapi_syncmgr.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CStudioAPICmdListVk;


//-----------------------------------------------------------------------------
// Vulkan StudioAPI buffer
//-----------------------------------------------------------------------------
class CStudioAPIBufferVk : public TRefCounted<IStudioAPIBuffer>
{
public:
	enum
	{
		NUM_BUFFERS = 3
	};

	// IStudioAPIBuffer interface
	virtual uint32 GetUsageFlags() const override;
	virtual uint64 GetSize() const override;
	virtual uint32 GetStride() const override;
	virtual uint8 GetMemoryFlags() const override;

	// NOTE: Before use check memory flags by GetMemoryFlags(). If STUDIOAPI_BUFFER_MEMORY_FLAG_CPU is set then you can use the function,
	// otherwise use IStudioAPICmdList::CopyBuffer
	virtual void MapMemory( uint64 size, uint64 offset, studioAPIMappedBufferData_t& mappedData ) override;
	virtual void UnmapMemory( studioAPIMappedBufferData_t& mappedData ) override;

	// For use the buffer must have STUDIOAPI_BUFFER_USAGE_FLAG_TRANSFER_DST flag 
	virtual void UpdateData( IStudioAPICmdContext* pCmdContext, byte* pData, uint64 dataSize, uint64 offset = 0 ) override;

	CStudioAPIBufferVk( const byte* pData, uint64 dataSize, uint32 dataStride, uint32 usageFlags, const achar* pDebugName = "" );
	~CStudioAPIBufferVk();

	// The function update a synchronization state of the buffer. Useful for for implicit transfer of ownership without barrier
	// and update access with stage masks
	void UpdateSyncState( VkAccessFlags vkDstAccessMask, VkPipelineStageFlags vkDstStageMask, uint32 dstQueueFamilyIndex );

	// The function update a synchronization state of the buffer and make a buffer barrier if it need. Useful for transfer ownership and
	// a buffer barrier if it need. Useful for transfer ownership and
	void UpdateSyncStateWithBarrier( CStudioAPICmdListVk* pCmdList, VkAccessFlags vkDstAccessMask, VkPipelineStageFlags vkDstStageMask, uint32 dstQueueFamilyIndex );

	void SwapCurrentBufferIndex();

	uint64 GetOffset() const;
	VkBuffer GetVkBuffer() const;
	VkIndexType GetVkIndexType() const;
	const studioAPISyncStateBufferVk_t& GetSyncState() const;

private:
	static void OnStudioAPIVkShutdown( void* pUserData );
	static uint32 GetNumBuffersFromUsage( uint32 usageFlags );

	uint32									usageFlags;
	uint8									memoryFlags;
	uint64									size;
	uint32									stride;
	uint8									numBuffers;
	uint8									currentBufferIndex;
	uint64									bufferOffsets[NUM_BUFFERS];
	VkBuffer								vkBuffer;
	VmaAllocation							vmaAllocation;
	studioAPISyncStateBufferVk_t			syncState;
	COnStudioAPIVkShutdown::funcDelegate_t*	pStudioAPIVkShutdownDelegate;
};

#include "studiorender/studioapi/vk/vk_studioapi_buffer.inl"

#endif // !VK_STUDIOAPI_BUFFER_H