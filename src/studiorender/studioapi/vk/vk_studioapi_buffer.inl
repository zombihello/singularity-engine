#ifndef VK_STUDIOAPI_BUFFER_INL
#define VK_STUDIOAPI_BUFFER_INL

/*
==================
CStudioAPIBufferVk::UpdateSyncState
==================
*/
FORCEINLINE void CStudioAPIBufferVk::UpdateSyncState( VkAccessFlags vkDstAccessMask, VkPipelineStageFlags vkDstStageMask, uint32 dstQueueFamilyIndex )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
	VK_UpdateSyncStateBuffer( vkDstAccessMask, vkDstStageMask, dstQueueFamilyIndex, syncState );
}

/*
==================
CStudioAPIBufferVk::SwapCurrentBufferIndex
==================
*/
FORCEINLINE void CStudioAPIBufferVk::SwapCurrentBufferIndex()
{
	currentBufferIndex = ( currentBufferIndex + 1 ) % numBuffers;
}

/*
==================
CStudioAPIBufferVk::GetOffset
==================
*/
FORCEINLINE uint64 CStudioAPIBufferVk::GetOffset() const
{
	return bufferOffsets[currentBufferIndex];
}

/*
==================
CStudioAPIBufferVk::GetVkBuffer
==================
*/
FORCEINLINE VkBuffer CStudioAPIBufferVk::GetVkBuffer() const
{
	return vkBuffer;
}

/*
==================
CStudioAPIBufferVk::GetVkIndexType
==================
*/
FORCEINLINE VkIndexType CStudioAPIBufferVk::GetVkIndexType() const
{
	return stride == 4 ? VK_INDEX_TYPE_UINT32 : VK_INDEX_TYPE_UINT16;
}

/*
==================
CStudioAPIBufferVk::GetSyncState
==================
*/
FORCEINLINE const studioAPISyncStateBufferVk_t& CStudioAPIBufferVk::GetSyncState() const
{
	return syncState;
}

/*
==================
CStudioAPIBufferVk::GetNumBuffersFromUsage
==================
*/
FORCEINLINE uint32 CStudioAPIBufferVk::GetNumBuffersFromUsage( uint32 usageFlags )
{
	return usageFlags & STUDIOAPI_BUFFER_USAGE_FLAG_DYNAMIC ? NUM_BUFFERS : 1;
}

#endif // !VK_STUDIOAPI_BUFFER_INL
