#ifndef VK_STUDIOAPI_SYNCMGR_INL
#define VK_STUDIOAPI_SYNCMGR_INL

/*
==================
studioAPISyncScopeVk_t::operator==
==================
*/
FORCEINLINE bool studioAPISyncScopeVk_t::operator==( const studioAPISyncScopeVk_t& other ) const
{
	// Use integer OR to prevent boolean short circuiting which will be more expensive then these simple integer operations
	return ( ( vkAccessFlags - other.vkAccessFlags ) | ( vkPipelineStages - other.vkPipelineStages ) ) == 0;
}

/*
==================
studioAPISyncScopeVk_t::operator!=
==================
*/
FORCEINLINE bool studioAPISyncScopeVk_t::operator!=( const studioAPISyncScopeVk_t& other ) const
{
	// Use integer OR to prevent boolean short circuiting which will be more expensive then these simple integer operations
	return ( ( vkAccessFlags - other.vkAccessFlags ) | ( vkPipelineStages - other.vkPipelineStages ) ) != 0;
}


/*
==================
studioAPISyncStateBufferVk_t::operator==
==================
*/
FORCEINLINE bool studioAPISyncStateBufferVk_t::operator==( const studioAPISyncStateBufferVk_t& other ) const
{
	// Use integer OR to prevent boolean short circuiting which will be more expensive then these simple integer operations
	return ( ( ( uint32 )bHasPendingOwnershipTransfer - ( uint32 )other.bHasPendingOwnershipTransfer ) |
			 ( uint32 )( !( readScope == other.readScope ) ) |
			 ( uint32 )( !( writeScope == other.writeScope ) ) |
			 ( ownerQueueFamilyIndex - other.ownerQueueFamilyIndex ) ) == 0;
}

/*
==================
studioAPISyncStateBufferVk_t::operator!=
==================
*/
FORCEINLINE bool studioAPISyncStateBufferVk_t::operator!=( const studioAPISyncStateBufferVk_t& other ) const
{
	// Use integer OR to prevent boolean short circuiting which will be more expensive then these simple integer operations
	return ( ( ( uint32 )bHasPendingOwnershipTransfer - ( uint32 )other.bHasPendingOwnershipTransfer ) |
			 ( uint32 )( readScope != other.readScope ) |
			 ( uint32 )( writeScope != other.writeScope ) |
			 ( ownerQueueFamilyIndex - other.ownerQueueFamilyIndex ) ) != 0;
}


/*
==================
studioAPISyncStateImageVk_t::operator==
==================
*/
FORCEINLINE bool studioAPISyncStateImageVk_t::operator==( const studioAPISyncStateImageVk_t& other ) const
{
	// Use integer OR to prevent boolean short circuiting which will be more expensive then these simple integer operations
	return ( ( ( uint32 )bHasPendingOwnershipTransfer - ( uint32 )other.bHasPendingOwnershipTransfer ) |
			 ( ( uint32 )vkImageLayout - ( uint32 )other.vkImageLayout ) |
			 ( uint32 )( !( readScope == other.readScope ) ) |
			 ( uint32 )( !( writeScope == other.writeScope ) ) |
			 ( ownerQueueFamilyIndex - other.ownerQueueFamilyIndex ) ) == 0;
}

/*
==================
studioAPISyncStateImageVk_t::operator!=
==================
*/
FORCEINLINE bool studioAPISyncStateImageVk_t::operator!=( const studioAPISyncStateImageVk_t& other ) const
{
	// Use integer OR to prevent boolean short circuiting which will be more expensive then these simple integer operations
	return ( ( ( uint32 )bHasPendingOwnershipTransfer - ( uint32 )other.bHasPendingOwnershipTransfer ) |
			 ( ( uint32 )vkImageLayout - ( uint32 )other.vkImageLayout ) |
			 ( uint32 )( readScope != other.readScope ) |
			 ( uint32 )( writeScope != other.writeScope ) |
			 ( ownerQueueFamilyIndex - other.ownerQueueFamilyIndex ) ) != 0;
}


/*
==================
CStudioAPISyncMgrVk::WaitAllFrameInFlights
==================
*/
FORCEINLINE void CStudioAPISyncMgrVk::WaitAllFrameInFlights()
{
	for ( uint32 index = 0; index < STUDIOAPI_VK_NUM_FRAMES_IN_FLIGHT; ++index )
	{
		WaitFrameInFlight( index );
	}
}

/*
==================
CStudioAPISyncMgrVk::AddFrameInFlightFence
==================
*/
FORCEINLINE void CStudioAPISyncMgrVk::AddFrameInFlightFence( CStudioAPIFenceVk* pFence, uint32 indexFrameInFlight )
{
	frameInFlightFences[indexFrameInFlight].emplace_back( pFence );
}

#endif // !VK_STUDIOAPI_SYNCMGR_INL
