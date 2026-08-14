#pragma once

/*
==================
CStudioAPIBufferVk::UpdateSyncState
==================
*/
FORCEINLINE void CStudioAPIBufferVk::UpdateSyncState( VkAccessFlags vkDstAccessMask, VkPipelineStageFlags vkDstStageMask, uint32 dstQueueFamilyIndex )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
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
	alloc.offset	   = size * currentBufferIndex;
}

/*
==================
CStudioAPIBufferVk::GetOffset
==================
*/
FORCEINLINE uint64 CStudioAPIBufferVk::GetOffset() const
{
	return alloc.offset;
}

/*
==================
CStudioAPIBufferVk::GetVkBuffer
==================
*/
FORCEINLINE VkBuffer CStudioAPIBufferVk::GetVkBuffer() const
{
	Assert( IsValidAlloc() );
	return alloc.vkBuffer;
}

/*
==================
CStudioAPIBufferVk::GetVkIndexType
==================
*/
FORCEINLINE VkIndexType CStudioAPIBufferVk::GetVkIndexType() const
{
	Assert( stride == sizeof( uint16 ) || stride == sizeof( uint32 ) );
	return stride == sizeof( uint32 ) ? VK_INDEX_TYPE_UINT32 : VK_INDEX_TYPE_UINT16;
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
