#include "pch_studioapi_vk.h"
#include "studiorender/studioapi_vk/vk_studioapi_fence.h"
#include "studiorender/studioapi_vk/vk_studioapi_semaphore.h"
#include "studiorender/studioapi_vk/vk_studioapi_syncmgr.h"

/*
==================
CStudioAPISyncMgrVk::CStudioAPISyncMgrVk
==================
*/
CStudioAPISyncMgrVk::CStudioAPISyncMgrVk()
{
}

/*
==================
CStudioAPISyncMgrVk::CStudioAPISyncMgrVk
==================
*/
CStudioAPISyncMgrVk::~CStudioAPISyncMgrVk()
{
	Shutdown();
}

/*
==================
CStudioAPISyncMgrVk::Init
==================
*/
void CStudioAPISyncMgrVk::Init()
{
}

/*
==================
CStudioAPISyncMgrVk::Shutdown
==================
*/
void CStudioAPISyncMgrVk::Shutdown()
{
	// Release all named semaphores
	for ( auto it = namedSemaphoresDict.begin(), itEnd = namedSemaphoresDict.end(); it != itEnd; ++it )
	{
		ReleaseNamedSemaphore( it->second );
	}
	AssertMsg( namedSemaphoresDict.empty(), "All named semaphores at shutdown of the manager mustn't have any references" );

	// Wait all frames in-flight
	WaitAllFrameInFlights();

	// All fences and semaphores must be released
	AssertMsg( fences.empty() && semaphores.empty(), "All fences and semaphores must be released before shutdown of CStudioAPISyncMgr" );

	// Free allocated memory for free fences and semaphores
	for ( auto it = freeFences.begin(), itEnd = freeFences.end(); it != itEnd; ++it )
	{
		delete *it;
	}
	freeFences.clear();

	for ( auto it = freeSemaphores.begin(), itEnd = freeSemaphores.end(); it != itEnd; ++it )
	{
		delete *it;
	}
	freeSemaphores.clear();
	namedSemaphoresDict.clear();
}

/*
==================
CStudioAPISyncMgrVk::CreateFence
==================
*/
CStudioAPIFenceVk* CStudioAPISyncMgrVk::CreateFence( bool bCreateSignaled /* = false */ )
{
	// The first we search a free fence in the free list
	if ( !freeFences.empty() )
	{
		CStudioAPIFenceVk* pFence = freeFences.front();
		freeFences.pop_front();
		fences.emplace_back( pFence );

		// Set the fence in signaled state if it need
		if ( bCreateSignaled )
		{
			pFence->state = CStudioAPIFenceVk::FENCE_STATE_SIGNALED;
		}
		return pFence;
	}

	// Otherwise create a new one
	return fences.emplace_back( new CStudioAPIFenceVk( bCreateSignaled ) );
}

/*
==================
CStudioAPISyncMgrVk::CreateSemaphore
==================
*/
CStudioAPISemaphoreVk* CStudioAPISyncMgrVk::CreateSemaphore()
{
	// The first we search a free semaphore in the free list
	if ( !freeSemaphores.empty() )
	{
		CStudioAPISemaphoreVk* pSemaphore = freeSemaphores.front();
		freeSemaphores.pop_front();
		semaphores.emplace_back( pSemaphore );
		return pSemaphore;
	}

	// Otherwise create a new one
	return semaphores.emplace_back( new CStudioAPISemaphoreVk() );
}

/*
==================
CStudioAPISyncMgrVk::FindOrCreateNamedSemaphore
==================
*/
CStudioAPINamedSemaphoreVk* CStudioAPISyncMgrVk::FindOrCreateNamedSemaphore( const char* pName )
{
	// The first we search already created the named semaphore in the dictionary
	auto itFind = namedSemaphoresDict.find( pName );
	if ( itFind != namedSemaphoresDict.end() )
	{
		++itFind->second->countReferences;
		return itFind->second;
	}

	// Create a new named semaphore
	CStudioAPINamedSemaphoreVk* pNamedSemaphore = new CStudioAPINamedSemaphoreVk();
	pNamedSemaphore->name						= pName;
	namedSemaphoresDict[pNamedSemaphore->name]	= pNamedSemaphore;

	// Use a free semaphores or create a new one for each frame in-flight
	for ( uint32 index = 0; index < STUDIOAPI_VK_NUM_FRAMES_IN_FLIGHT; ++index )
	{
		CStudioAPISemaphoreVk* pSemaphore = NULL;
		if ( !freeSemaphores.empty() )
		{
			pSemaphore = freeSemaphores.front();
			freeSemaphores.pop_front();
		}
		else
		{
			pSemaphore = new CStudioAPISemaphoreVk();
		}

		pNamedSemaphore->pSemaphores[index] = pSemaphore;
		semaphores.emplace_back( pSemaphore );
	}

	// We are done
	return pNamedSemaphore;
}

/*
==================
CStudioAPISyncMgrVk::ReleaseFence
==================
*/
void CStudioAPISyncMgrVk::ReleaseFence( CStudioAPIFenceVk*& pFence )
{
	// Remove the fence from the used list
	fences.remove( pFence );

	// Add the fence into the pending list to release
	pendingFreeFences[g_StudioAPIVk.GetCurrentFrameInFlight()].emplace_back( pFence );

	// Reset the pointer
	pFence = NULL;
}

/*
==================
CStudioAPISyncMgrVk::ReleaseSemaphore
==================
*/
void CStudioAPISyncMgrVk::ReleaseSemaphore( CStudioAPISemaphoreVk*& pSemaphore )
{
	// Remove the semaphore from the used list
	Assert( pSemaphore != NULL );
	semaphores.remove( pSemaphore );

	// Add the semaphore into the pending list to release
	pendingFreeSemaphores[g_StudioAPIVk.GetCurrentFrameInFlight()].emplace_back( pSemaphore );

	// Reset the pointer
	pSemaphore = NULL;
}

/*
==================
CStudioAPISyncMgrVk::ReleaseNamedSemaphore
==================
*/
void CStudioAPISyncMgrVk::ReleaseNamedSemaphore( CStudioAPINamedSemaphoreVk*& pNamedSemaphore )
{
	// Do nothing if a named semaphore has any references
	// In the case decrease reference count
	if ( pNamedSemaphore->countReferences > 1 )
	{
		--pNamedSemaphore->countReferences;
		return;
	}

	// Add to the pending list to release each semaphore
	for ( uint32 index = 0; index < STUDIOAPI_VK_NUM_FRAMES_IN_FLIGHT; ++index )
	{
		pendingFreeSemaphores[index].emplace_back( pNamedSemaphore->pSemaphores[index] );
	}

	// Remove it from the dictionary
	namedSemaphoresDict.erase( pNamedSemaphore->name );
}

/*
==================
CStudioAPISyncMgrVk::WaitFrameInFlight
==================
*/
void CStudioAPISyncMgrVk::WaitFrameInFlight( uint32 indexFrameInFlight )
{
	// Wait until the current frame in flight will be available
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_WAIT );
	eastl::list<CStudioAPIFenceVk*>& currentFrameInFlightFences = frameInFlightFences[indexFrameInFlight];
	for ( auto it = currentFrameInFlightFences.begin(), itEnd = currentFrameInFlightFences.end(); it != itEnd; ++it )
	{
		// Wait a fence if it isn't signaled
		CStudioAPIFenceVk* pFence = *it;
		if ( !pFence->IsSignaled() )
		{
			bool bResult = pFence->Wait( UINT64_MAX );
			Assert( bResult );
		}

		// Reset the fence
		pFence->Reset();
	}
	currentFrameInFlightFences.clear();

	// Free all pending fences
	eastl::list<CStudioAPIFenceVk*>& currentPendingFreeFences = pendingFreeFences[indexFrameInFlight];
	for ( auto it = currentPendingFreeFences.begin(), itEnd = currentPendingFreeFences.end(); it != itEnd; ++it )
	{
		// Reset a fence and add the one into the list of free fences
		CStudioAPIFenceVk* pFence = *it;
		pFence->Reset();
		freeFences.emplace_back( pFence );
	}
	currentPendingFreeFences.clear();

	// Free all pending semaphores
	eastl::list<CStudioAPISemaphoreVk*>& currentPendingFreeSemaphores = pendingFreeSemaphores[indexFrameInFlight];
	for ( auto it = currentPendingFreeSemaphores.begin(), itEnd = currentPendingFreeSemaphores.end(); it != itEnd; ++it )
	{
		// Reset a semaphore and add the one into the list of free semaphores
		CStudioAPISemaphoreVk* pSemaphore = *it;
		pSemaphore->bSignaled			  = false;
		freeSemaphores.emplace_back( pSemaphore );
	}
	currentPendingFreeSemaphores.clear();
}

/*
==================
VK_UpdateSyncStateBuffer
==================
*/
void VK_UpdateSyncStateBuffer( VkAccessFlags vkDstAccessMask, VkPipelineStageFlags vkDstStageMask, uint32 dstQueueFamilyIndex, studioAPISyncStateBufferVk_t& syncState )
{
	// Split vkDstAccessMask by read and write mask
	VkAccessFlags vkDstReadAccessMask  = vkDstAccessMask & STUDIOAPI_VK_ACCESS_READ_FLAGS;
	VkAccessFlags vkDstWriteAccessMask = vkDstAccessMask & STUDIOAPI_VK_ACCESS_WRITE_FLAGS;
	Assert( ( vkDstAccessMask & ~( STUDIOAPI_VK_ACCESS_KNOWN_FLAGS ) ) == 0 );

	// Save a new owner queue family if early the resource not used
	if ( syncState.ownerQueueFamilyIndex == VK_QUEUE_FAMILY_IGNORED )
	{
		syncState.ownerQueueFamilyIndex = dstQueueFamilyIndex;
	}
	// Otherwise if we do implicit transfer ownership reset all masks and remember the new owner
	else if ( syncState.ownerQueueFamilyIndex != dstQueueFamilyIndex )
	{
		// Remember all scopes for the new queue family
		if ( vkDstReadAccessMask != VK_ACCESS_NONE )
		{
			syncState.readScope.vkAccessFlags	 = vkDstReadAccessMask;
			syncState.readScope.vkPipelineStages = vkDstStageMask;
		}
		else
		{
			syncState.readScope.vkAccessFlags	 = VK_ACCESS_NONE;
			syncState.readScope.vkPipelineStages = VK_PIPELINE_STAGE_NONE;
		}

		if ( vkDstWriteAccessMask != VK_ACCESS_NONE )
		{
			syncState.writeScope.vkAccessFlags	  = vkDstWriteAccessMask;
			syncState.writeScope.vkPipelineStages = vkDstStageMask;
		}
		else
		{
			syncState.writeScope.vkAccessFlags	  = VK_ACCESS_NONE;
			syncState.writeScope.vkPipelineStages = VK_PIPELINE_STAGE_NONE;
		}

		// Remember the new owner and reset syncState.bHasPendingOwnershipTransfer
		Assert( !syncState.bHasPendingOwnershipTransfer );
		syncState.ownerQueueFamilyIndex = dstQueueFamilyIndex;

		// We are done
		return;
	}

	// Update the state for write operations
	bool bNewIsRead	 = vkDstReadAccessMask != VK_ACCESS_NONE;
	bool bNewIsWrite = vkDstWriteAccessMask != VK_ACCESS_NONE;
	if ( bNewIsWrite )
	{
		// Store where write happened
		syncState.writeScope.vkAccessFlags	  = vkDstWriteAccessMask;
		syncState.writeScope.vkPipelineStages = vkDstStageMask;

		// Reset read state
		syncState.readScope.vkAccessFlags	 = VK_ACCESS_NONE;
		syncState.readScope.vkPipelineStages = VK_PIPELINE_STAGE_NONE;
	}

	// Update the state for read operations
	if ( bNewIsRead )
	{
		// Remember this read happened
		syncState.readScope.vkAccessFlags |= vkDstReadAccessMask;
		syncState.readScope.vkPipelineStages |= vkDstStageMask;
	}
}

/*
==================
VK_UpdateSyncStateBufferWithBarrier
==================
*/
bool VK_UpdateSyncStateBufferWithBarrier( VkAccessFlags vkDstAccessMask, VkPipelineStageFlags vkDstStageMask, uint32 dstQueueFamilyIndex, studioAPISyncStateBufferVk_t& syncState, studioAPIBufferMemoryBarrierVk_t& bufferMemoryBarrier, uint32 bufferUsageFlags )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Split vkDstAccessMask by read and write mask
	bool		  bIsNeedBarrier	   = false;
	VkAccessFlags vkDstReadAccessMask  = vkDstAccessMask & STUDIOAPI_VK_ACCESS_READ_FLAGS;
	VkAccessFlags vkDstWriteAccessMask = vkDstAccessMask & STUDIOAPI_VK_ACCESS_WRITE_FLAGS;
	Assert( ( vkDstAccessMask & ~( STUDIOAPI_VK_ACCESS_KNOWN_FLAGS ) ) == 0 );

	// Initialize base fields of a buffer barrier and reset output fields
	bufferMemoryBarrier.vkBufferMemoryBarrier.sType				  = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	bufferMemoryBarrier.vkBufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	bufferMemoryBarrier.vkBufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	bufferMemoryBarrier.vkSrcStageMask							  = VK_PIPELINE_STAGE_NONE;
	bufferMemoryBarrier.vkDstStageMask							  = vkDstStageMask;

	// Save a new owner queue family if early the resource not used
	if ( syncState.ownerQueueFamilyIndex == VK_QUEUE_FAMILY_IGNORED )
	{
		syncState.ownerQueueFamilyIndex = dstQueueFamilyIndex;
	}
	// Otherwise make a barrier if we must do transfer ownership to another queue family
	else if ( syncState.ownerQueueFamilyIndex != dstQueueFamilyIndex )
	{
		// Set source and destination queue family index
		bufferMemoryBarrier.vkBufferMemoryBarrier.srcQueueFamilyIndex = syncState.ownerQueueFamilyIndex;
		bufferMemoryBarrier.vkBufferMemoryBarrier.dstQueueFamilyIndex = dstQueueFamilyIndex;

		// Do transfer ownership only if the buffer hasn't STUDIOAPI_BUFFER_USAGE_FLAG_CONCURRENT
		bIsNeedBarrier = !( bufferUsageFlags & STUDIOAPI_BUFFER_USAGE_FLAG_CONCURRENT );

		// Make the transfer ownership barrier to another queue family if need a barrier
		if ( bIsNeedBarrier && !syncState.bHasPendingOwnershipTransfer )
		{
			bufferMemoryBarrier.vkBufferMemoryBarrier.srcAccessMask = syncState.readScope.vkAccessFlags | syncState.writeScope.vkAccessFlags;
			bufferMemoryBarrier.vkSrcStageMask						= syncState.readScope.vkPipelineStages | syncState.writeScope.vkPipelineStages;
			bufferMemoryBarrier.vkBufferMemoryBarrier.dstAccessMask = VK_ACCESS_NONE;
			bufferMemoryBarrier.vkDstStageMask						= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
			syncState.bHasPendingOwnershipTransfer					= true;
		}
		// Make an acquire ownership barrier in another queue family
		else
		{
			bufferMemoryBarrier.vkBufferMemoryBarrier.srcAccessMask = VK_ACCESS_NONE;
			bufferMemoryBarrier.vkSrcStageMask						= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			bufferMemoryBarrier.vkBufferMemoryBarrier.dstAccessMask = vkDstAccessMask;
			syncState.bHasPendingOwnershipTransfer					= false;

			// Remember all scopes for the new queue family
			if ( vkDstReadAccessMask != VK_ACCESS_NONE )
			{
				syncState.readScope.vkAccessFlags	 = vkDstReadAccessMask;
				syncState.readScope.vkPipelineStages = vkDstStageMask;
			}
			else
			{
				syncState.readScope.vkAccessFlags	 = VK_ACCESS_NONE;
				syncState.readScope.vkPipelineStages = VK_PIPELINE_STAGE_NONE;
			}

			if ( vkDstWriteAccessMask != VK_ACCESS_NONE )
			{
				syncState.writeScope.vkAccessFlags	  = vkDstWriteAccessMask;
				syncState.writeScope.vkPipelineStages = vkDstStageMask;
			}
			else
			{
				syncState.writeScope.vkAccessFlags	  = VK_ACCESS_NONE;
				syncState.writeScope.vkPipelineStages = VK_PIPELINE_STAGE_NONE;
			}

			// Remember the new owner
			syncState.ownerQueueFamilyIndex = dstQueueFamilyIndex;
		}

		// We are done
		return bIsNeedBarrier;
	}

	// Update the state for write operations
	bool bPreviousIsRead  = syncState.readScope.vkAccessFlags != VK_ACCESS_NONE;
	bool bPreviousIsWrite = syncState.writeScope.vkAccessFlags != VK_ACCESS_NONE;
	bool bNewIsRead		  = vkDstReadAccessMask != VK_ACCESS_NONE;
	bool bNewIsWrite	  = vkDstWriteAccessMask != VK_ACCESS_NONE;
	Assert( !syncState.bHasPendingOwnershipTransfer );
	if ( bNewIsWrite )
	{
		// Write is synchronized with previous reads and writes
		if ( bPreviousIsRead || bPreviousIsWrite )
		{
			// Set source stage and access masks
			bufferMemoryBarrier.vkSrcStageMask						= syncState.readScope.vkPipelineStages | syncState.writeScope.vkPipelineStages;
			bufferMemoryBarrier.vkBufferMemoryBarrier.srcAccessMask = syncState.readScope.vkAccessFlags | syncState.writeScope.vkAccessFlags;
			bufferMemoryBarrier.vkBufferMemoryBarrier.dstAccessMask = vkDstReadAccessMask | vkDstWriteAccessMask;
			bIsNeedBarrier											= bufferMemoryBarrier.vkSrcStageMask != VK_PIPELINE_STAGE_NONE;
		}

		// Store where write happened
		syncState.writeScope.vkAccessFlags	  = vkDstWriteAccessMask;
		syncState.writeScope.vkPipelineStages = vkDstStageMask;

		// Reset read state
		syncState.readScope.vkAccessFlags	 = VK_ACCESS_NONE;
		syncState.readScope.vkPipelineStages = VK_PIPELINE_STAGE_NONE;
	}

	// Update the state for read operations
	if ( bNewIsRead )
	{
		// Read is synchronized with previous writes only (if we early not synchronize yet)
		if ( !bNewIsWrite && bPreviousIsWrite )
		{
			// Set source stage and access masks
			bufferMemoryBarrier.vkSrcStageMask						= syncState.writeScope.vkPipelineStages;
			bufferMemoryBarrier.vkBufferMemoryBarrier.srcAccessMask = syncState.writeScope.vkAccessFlags;
			bufferMemoryBarrier.vkBufferMemoryBarrier.dstAccessMask = syncState.readScope.vkAccessFlags | vkDstReadAccessMask;
			bIsNeedBarrier											= bufferMemoryBarrier.vkSrcStageMask != VK_PIPELINE_STAGE_NONE;
		}

		// Check whether this is a new barrier
		// If not we already synchronized, no need to do a barrier
		if ( ( syncState.readScope.vkAccessFlags & vkDstReadAccessMask ) == vkDstReadAccessMask && ( syncState.readScope.vkPipelineStages & vkDstStageMask ) == vkDstStageMask )
		{
			bIsNeedBarrier = false;
		}

		// Remember this read happened
		syncState.readScope.vkAccessFlags |= vkDstReadAccessMask;
		syncState.readScope.vkPipelineStages |= vkDstStageMask;
	}

	return bIsNeedBarrier;
}

/*
==================
VK_UpdateSyncStateImage
==================
*/
void VK_UpdateSyncStateImage( VkImageLayout vkDstImageLayout, VkAccessFlags vkDstAccessMask, VkPipelineStageFlags vkDstStageMask, uint32 dstQueueFamilyIndex, studioAPISyncStateImageVk_t& syncState )
{
	// Split vkDstAccessMask by read and write mask and get a source image layout
	VkAccessFlags vkDstReadAccessMask  = vkDstAccessMask & STUDIOAPI_VK_ACCESS_READ_FLAGS;
	VkAccessFlags vkDstWriteAccessMask = vkDstAccessMask & STUDIOAPI_VK_ACCESS_WRITE_FLAGS;
	VkImageLayout vkSrcImageLayout	   = ( vkDstReadAccessMask == 0 ) ? VK_IMAGE_LAYOUT_UNDEFINED : syncState.vkImageLayout;
	bool		  bIsLayoutTransfer	   = vkDstImageLayout != vkSrcImageLayout;
	Assert( ( vkDstAccessMask & ~( STUDIOAPI_VK_ACCESS_KNOWN_FLAGS ) ) == 0 );

	// Save a new owner queue family if early the resource not used
	if ( syncState.ownerQueueFamilyIndex == VK_QUEUE_FAMILY_IGNORED )
	{
		syncState.ownerQueueFamilyIndex = dstQueueFamilyIndex;
	}
	// Otherwise if we do implicit transfer ownership reset all masks, an image layout and remember the new owner
	else if ( syncState.ownerQueueFamilyIndex != dstQueueFamilyIndex )
	{
		// Remember all scopes for the new queue family
		if ( vkDstReadAccessMask != VK_ACCESS_NONE )
		{
			syncState.readScope.vkAccessFlags	 = vkDstReadAccessMask;
			syncState.readScope.vkPipelineStages = vkDstStageMask;
		}
		else
		{
			syncState.readScope.vkAccessFlags	 = VK_ACCESS_NONE;
			syncState.readScope.vkPipelineStages = VK_PIPELINE_STAGE_NONE;
		}

		if ( vkDstWriteAccessMask != VK_ACCESS_NONE )
		{
			syncState.writeScope.vkAccessFlags	  = vkDstWriteAccessMask;
			syncState.writeScope.vkPipelineStages = vkDstStageMask;
		}
		else
		{
			syncState.writeScope.vkAccessFlags	  = VK_ACCESS_NONE;
			syncState.writeScope.vkPipelineStages = VK_PIPELINE_STAGE_NONE;
		}

		// Remember the new image layout, owner queue family index
		Assert( !syncState.bHasPendingOwnershipTransfer );
		syncState.vkImageLayout			= vkDstImageLayout;
		syncState.ownerQueueFamilyIndex = dstQueueFamilyIndex;

		// We are done
		return;
	}

	// Update the state for write operations and image layout transfer
	bool bNewIsRead	 = vkDstReadAccessMask != VK_ACCESS_NONE;
	bool bNewIsWrite = vkDstWriteAccessMask != VK_ACCESS_NONE;
	if ( bNewIsWrite || bIsLayoutTransfer )
	{
		// Store where write happened
		syncState.writeScope.vkAccessFlags	  = vkDstWriteAccessMask;
		syncState.writeScope.vkPipelineStages = vkDstStageMask;

		// Reset read state
		syncState.readScope.vkAccessFlags	 = VK_ACCESS_NONE;
		syncState.readScope.vkPipelineStages = VK_PIPELINE_STAGE_NONE;

		// Store a new image layout
		syncState.vkImageLayout = vkDstImageLayout;
	}

	// Update the state for read operations
	if ( bNewIsRead )
	{
		// Remember this read happened
		syncState.readScope.vkAccessFlags |= vkDstReadAccessMask;
		syncState.readScope.vkPipelineStages |= vkDstStageMask;
	}
}

/*
==================
VK_UpdateSyncStateImageWithBarrier
==================
*/
bool VK_UpdateSyncStateImageWithBarrier( VkImageLayout vkDstImageLayout, VkAccessFlags vkDstAccessMask, VkPipelineStageFlags vkDstStageMask, uint32 dstQueueFamilyIndex, studioAPISyncStateImageVk_t& syncState, studioAPIImageMemoryBarrierVk_t& imageMemoryBarrier, uint32 textureUsageFlags )
{
	// Split vkDstAccessMask by read and write mask and get a source image layout
	bool		  bIsNeedBarrier	   = false;
	VkAccessFlags vkDstReadAccessMask  = vkDstAccessMask & STUDIOAPI_VK_ACCESS_READ_FLAGS;
	VkAccessFlags vkDstWriteAccessMask = vkDstAccessMask & STUDIOAPI_VK_ACCESS_WRITE_FLAGS;
	VkImageLayout vkSrcImageLayout	   = ( vkDstReadAccessMask == 0 ) ? VK_IMAGE_LAYOUT_UNDEFINED : syncState.vkImageLayout;
	bool		  bIsLayoutTransfer	   = vkDstImageLayout != vkSrcImageLayout;
	Assert( ( vkDstAccessMask & ~( STUDIOAPI_VK_ACCESS_KNOWN_FLAGS ) ) == 0 );

	// Initialize base fields of an image barrier and reset output fields
	imageMemoryBarrier.vkImageMemoryBarrier.sType							= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.vkImageMemoryBarrier.oldLayout						= vkSrcImageLayout;
	imageMemoryBarrier.vkImageMemoryBarrier.newLayout						= vkDstImageLayout;
	imageMemoryBarrier.vkImageMemoryBarrier.srcQueueFamilyIndex				= VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.vkImageMemoryBarrier.dstQueueFamilyIndex				= VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.vkImageMemoryBarrier.subresourceRange.baseMipLevel	= 0;
	imageMemoryBarrier.vkImageMemoryBarrier.subresourceRange.levelCount		= VK_REMAINING_MIP_LEVELS;
	imageMemoryBarrier.vkImageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	imageMemoryBarrier.vkImageMemoryBarrier.subresourceRange.layerCount		= VK_REMAINING_ARRAY_LAYERS;
	imageMemoryBarrier.vkSrcStageMask										= VK_PIPELINE_STAGE_NONE;
	imageMemoryBarrier.vkDstStageMask										= vkDstStageMask;

	// Save a new owner queue family if early the resource not used
	if ( syncState.ownerQueueFamilyIndex == VK_QUEUE_FAMILY_IGNORED )
	{
		syncState.ownerQueueFamilyIndex = dstQueueFamilyIndex;
	}
	// Otherwise make a barrier if we must do transfer ownership to another queue family
	else if ( syncState.ownerQueueFamilyIndex != dstQueueFamilyIndex )
	{
		// Set source and destination queue family index
		imageMemoryBarrier.vkImageMemoryBarrier.srcQueueFamilyIndex = syncState.ownerQueueFamilyIndex;
		imageMemoryBarrier.vkImageMemoryBarrier.dstQueueFamilyIndex = dstQueueFamilyIndex;

		// Do transfer ownership only if the buffer hasn't STUDIOAPI_TEXTURE_USAGE_FLAG_CONCURRENT
		bIsNeedBarrier = !( textureUsageFlags & STUDIOAPI_TEXTURE_USAGE_FLAG_CONCURRENT );

		// Make the transfer ownership barrier to another queue family if need a barrier
		if ( bIsNeedBarrier && !syncState.bHasPendingOwnershipTransfer )
		{
			imageMemoryBarrier.vkImageMemoryBarrier.srcAccessMask = syncState.readScope.vkAccessFlags | syncState.writeScope.vkAccessFlags;
			imageMemoryBarrier.vkSrcStageMask					  = syncState.readScope.vkPipelineStages | syncState.writeScope.vkPipelineStages;
			imageMemoryBarrier.vkImageMemoryBarrier.dstAccessMask = VK_ACCESS_NONE;
			imageMemoryBarrier.vkDstStageMask					  = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
			syncState.bHasPendingOwnershipTransfer				  = true;
		}
		// Make an acquire ownership barrier in another queue family
		else
		{
			imageMemoryBarrier.vkImageMemoryBarrier.srcAccessMask = VK_ACCESS_NONE;
			imageMemoryBarrier.vkSrcStageMask					  = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			imageMemoryBarrier.vkImageMemoryBarrier.dstAccessMask = vkDstAccessMask;
			syncState.bHasPendingOwnershipTransfer				  = false;

			// Remember all scopes for the new queue family
			if ( vkDstReadAccessMask != VK_ACCESS_NONE )
			{
				syncState.readScope.vkAccessFlags	 = vkDstReadAccessMask;
				syncState.readScope.vkPipelineStages = vkDstStageMask;
			}
			else
			{
				syncState.readScope.vkAccessFlags	 = VK_ACCESS_NONE;
				syncState.readScope.vkPipelineStages = VK_PIPELINE_STAGE_NONE;
			}

			if ( vkDstWriteAccessMask != VK_ACCESS_NONE )
			{
				syncState.writeScope.vkAccessFlags	  = vkDstWriteAccessMask;
				syncState.writeScope.vkPipelineStages = vkDstStageMask;
			}
			else
			{
				syncState.writeScope.vkAccessFlags	  = VK_ACCESS_NONE;
				syncState.writeScope.vkPipelineStages = VK_PIPELINE_STAGE_NONE;
			}

			// Remember the new owner and image layout
			syncState.vkImageLayout			= vkDstImageLayout;
			syncState.ownerQueueFamilyIndex = dstQueueFamilyIndex;
		}

		// We are done
		return bIsNeedBarrier;
	}

	// Update the state for write operations and image layout transfer
	bool bPreviousIsRead  = syncState.readScope.vkAccessFlags != VK_ACCESS_NONE;
	bool bPreviousIsWrite = syncState.writeScope.vkAccessFlags != VK_ACCESS_NONE;
	bool bNewIsRead		  = vkDstReadAccessMask != VK_ACCESS_NONE;
	bool bNewIsWrite	  = vkDstWriteAccessMask != VK_ACCESS_NONE;
	Assert( !syncState.bHasPendingOwnershipTransfer );
	if ( bNewIsWrite || bIsLayoutTransfer )
	{
		// Write is synchronized with previous reads and writes or if we do image layout transfer
		if ( bPreviousIsRead || bPreviousIsWrite || bIsLayoutTransfer )
		{
			// Set source stage and access masks
			imageMemoryBarrier.vkSrcStageMask					  = syncState.readScope.vkPipelineStages | syncState.writeScope.vkPipelineStages;
			imageMemoryBarrier.vkImageMemoryBarrier.srcAccessMask = syncState.readScope.vkAccessFlags | syncState.writeScope.vkAccessFlags;
			imageMemoryBarrier.vkImageMemoryBarrier.dstAccessMask = vkDstReadAccessMask | vkDstWriteAccessMask;
			bIsNeedBarrier										  = bIsLayoutTransfer || imageMemoryBarrier.vkSrcStageMask != VK_PIPELINE_STAGE_NONE;

			// Set source stage mask into vkDstStageMask if we need issue barrier, but the stage isn't valid
			if ( bIsNeedBarrier && imageMemoryBarrier.vkSrcStageMask == VK_PIPELINE_STAGE_NONE )
			{
				// NOTE BS yehor.pohuliaka - I don't sure that it is correct
				imageMemoryBarrier.vkSrcStageMask = vkDstStageMask;
			}
		}

		// Store where write happened
		syncState.writeScope.vkAccessFlags	  = vkDstWriteAccessMask;
		syncState.writeScope.vkPipelineStages = vkDstStageMask;

		// Reset read state
		syncState.readScope.vkAccessFlags	 = VK_ACCESS_NONE;
		syncState.readScope.vkPipelineStages = VK_PIPELINE_STAGE_NONE;

		// Store a new image layout
		syncState.vkImageLayout = vkDstImageLayout;
	}

	// Update the state for read operations
	if ( bNewIsRead )
	{
		// Read is synchronized with previous writes only (if we early not synchronize yet)
		if ( !bNewIsWrite && !bIsLayoutTransfer && bPreviousIsWrite )
		{
			// Set source stage and access masks
			imageMemoryBarrier.vkSrcStageMask					  = syncState.writeScope.vkPipelineStages;
			imageMemoryBarrier.vkImageMemoryBarrier.srcAccessMask = syncState.writeScope.vkAccessFlags;
			imageMemoryBarrier.vkImageMemoryBarrier.dstAccessMask = syncState.readScope.vkAccessFlags | vkDstReadAccessMask;
			bIsNeedBarrier										  = imageMemoryBarrier.vkSrcStageMask != VK_PIPELINE_STAGE_NONE;
		}

		// Check whether this is a new barrier
		// If not we already synchronized, no need to do a barrier
		if ( ( syncState.readScope.vkAccessFlags & vkDstReadAccessMask ) == vkDstReadAccessMask && ( syncState.readScope.vkPipelineStages & vkDstStageMask ) == vkDstStageMask && !bIsLayoutTransfer )
		{
			bIsNeedBarrier = false;
		}

		// Remember this read happened
		syncState.readScope.vkAccessFlags |= vkDstReadAccessMask;
		syncState.readScope.vkPipelineStages |= vkDstStageMask;
	}

	return bIsNeedBarrier;
}
