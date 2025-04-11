/**
 * @file
 * @addtogroup studioapi_vk studioapi_vk
 */

#ifndef VK_STUDIOAPI_SYNCMGR_H
#define VK_STUDIOAPI_SYNCMGR_H

// Forward declarations
class CStudioAPIFenceVk;
class CStudioAPISemaphoreVk;
class CStudioAPINamedSemaphoreVk;

/**
 * @ingroup studioapi_vk
 *  @brief Vulkan StudioAPI buffer barrier
 */
struct studioAPIBufferMemoryBarrierVk_t
{
	VkBufferMemoryBarrier	vkBufferMemoryBarrier;	/**< Vulkan buffer memory barrier */
	VkPipelineStageFlags	vkDstStageMask;			/**< Vulkan destination pipeline stage mask */
	VkPipelineStageFlags	vkSrcStageMask;			/**< Vulkan source pipeline stage mask */
};


/**
 * @ingroup studioapi_vk
 *  @brief Vulkan StudioAPI image barrier
 */
struct studioAPIImageMemoryBarrierVk_t
{
	VkImageMemoryBarrier	vkImageMemoryBarrier;	/**< Vulkan image memory barrier */
	VkPipelineStageFlags	vkDstStageMask;			/**< Vulkan destination pipeline stage mask */
	VkPipelineStageFlags	vkSrcStageMask;			/**< Vulkan source pipeline stage mask */
};


/**
 * @ingroup studioapi_vk
 * @brief Vulkan StudioAPI synchronization scope
 */
struct studioAPISyncScopeVk_t
{
	/**
	 * @brief Constructor
	 */
	studioAPISyncScopeVk_t()
		: vkAccessFlags( VK_ACCESS_NONE )
		, vkPipelineStages( VK_PIPELINE_STAGE_NONE )
	{}

	/**
	 * @brief Constructor
	 * @param vkAccessFlags		Vulkan access flags
	 * @param vkPipelineStages	Vulkan pipeline stages
	 */
	studioAPISyncScopeVk_t( VkAccessFlags vkAccessFlags, VkPipelineStageFlags vkPipelineStages )
		: vkAccessFlags( vkAccessFlags )
		, vkPipelineStages( vkPipelineStages )
	{}

	/**
	 * @brief Override operator ==
	 * @param other		Other synchronization scope
	 * @return Return TRUE if both is equal, otherwise FALSE
	 */
	FORCEINLINE bool operator==( const studioAPISyncScopeVk_t& other ) const
	{
		// Use integer OR to prevent boolean short circuiting which will be more expensive then these simple integer operations
		return ( ( vkAccessFlags - other.vkAccessFlags ) | ( vkPipelineStages - other.vkPipelineStages ) ) == 0;
	}

	/**
	 * @brief Override operator !=
	 * @param other		Other synchronization scope
	 * @return Return TRUE if both isn't equal, otherwise FALSE
	 */
	FORCEINLINE bool operator!=( const studioAPISyncScopeVk_t& other ) const
	{
		// Use integer OR to prevent boolean short circuiting which will be more expensive then these simple integer operations
		return ( ( vkAccessFlags - other.vkAccessFlags ) | ( vkPipelineStages - other.vkPipelineStages ) ) != 0;
	}

	VkAccessFlags			vkAccessFlags;			/**< Vulkan access flags */
	VkPipelineStageFlags	vkPipelineStages;		/**< Vulkan pipeline stages */
};


/**
 * @ingroup studioapi_vk
 * @brief Vulkan StudioAPI buffer synchronization state
 */
struct studioAPISyncStateBufferVk_t
{
public:
	/**
	 * @brief Constructor
	 */
	studioAPISyncStateBufferVk_t()
		: bHasPendingOwnershipTransfer( false )
		, ownerQueueFamilyIndex( VK_QUEUE_FAMILY_IGNORED )
	{}

	/**
	 * @brief Constructor
	 * @param readScope				Read scope
	 * @param writeScope			Write scope
	 * @param ownerQueueFamilyIndex	Queue family index which own of a resource
	 */
	studioAPISyncStateBufferVk_t( const studioAPISyncScopeVk_t& readScope, const studioAPISyncScopeVk_t& writeScope, uint32 ownerQueueFamilyIndex )
		: bHasPendingOwnershipTransfer( false )
		, readScope( readScope )
		, writeScope( writeScope )
		, ownerQueueFamilyIndex( ownerQueueFamilyIndex )
	{}

	/**
	 * @brief Override operator ==
	 * @param other		Other synchronization scope
	 * @return Return TRUE if both is equal, otherwise FALSE
	 */
	FORCEINLINE bool operator==( const studioAPISyncStateBufferVk_t& other ) const
	{
		// Use integer OR to prevent boolean short circuiting which will be more expensive then these simple integer operations
		return ( ( ( uint32 )bHasPendingOwnershipTransfer - ( uint32 )other.bHasPendingOwnershipTransfer ) |
				 ( uint32 )( !( readScope == other.readScope ) ) |
				 ( uint32 )( !( writeScope == other.writeScope ) ) |
				 ( ownerQueueFamilyIndex - other.ownerQueueFamilyIndex ) ) == 0;
	}

	/**
	 * @brief Override operator !=
	 * @param other		Other synchronization scope
	 * @return Return TRUE if both isn't equal, otherwise FALSE
	 */
	FORCEINLINE bool operator!=( const studioAPISyncStateBufferVk_t& other ) const
	{
		// Use integer OR to prevent boolean short circuiting which will be more expensive then these simple integer operations
		return ( ( ( uint32 )bHasPendingOwnershipTransfer - ( uint32 )other.bHasPendingOwnershipTransfer ) |
				 ( uint32 )( readScope != other.readScope ) |
				 ( uint32 )( writeScope != other.writeScope ) |
				 ( ownerQueueFamilyIndex - other.ownerQueueFamilyIndex ) ) != 0;
	}

	bool					bHasPendingOwnershipTransfer;	/**< Has pending ownership transfer */
	studioAPISyncScopeVk_t	readScope;						/**< Read scope */
	studioAPISyncScopeVk_t	writeScope;						/**< Write scope */
	uint32					ownerQueueFamilyIndex;			/**< Queue family index which own of a resource */
};


/**
 * @ingroup studioapi_vk
 * @brief Vulkan StudioAPI image synchronization state
 */
struct studioAPISyncStateImageVk_t
{
	/**
	 * @brief Constructor
	 */
	studioAPISyncStateImageVk_t()
		: bHasPendingOwnershipTransfer( false )
		, vkImageLayout( VK_IMAGE_LAYOUT_UNDEFINED )
		, ownerQueueFamilyIndex( VK_QUEUE_FAMILY_IGNORED )
	{}

	/**
	 * @brief Constructor
	 * @param vkImageLayout			Vulkan image layout
	 * @param readScope				Read scope
	 * @param writeScope			Write scope
	 * @param ownerQueueFamilyIndex	Queue family index which own of a resource
	 */
	studioAPISyncStateImageVk_t( VkImageLayout vkImageLayout, const studioAPISyncScopeVk_t& readScope, const studioAPISyncScopeVk_t& writeScope, uint32 ownerQueueFamilyIndex )
		: bHasPendingOwnershipTransfer( false )
		, vkImageLayout( vkImageLayout )
		, readScope( readScope )
		, writeScope( writeScope )
		, ownerQueueFamilyIndex( ownerQueueFamilyIndex )
	{}

	/**
	 * @brief Override operator ==
	 * @param other		Other synchronization scope
	 * @return Return TRUE if both is equal, otherwise FALSE
	 */
	FORCEINLINE bool operator==( const studioAPISyncStateImageVk_t& other ) const
	{
		// Use integer OR to prevent boolean short circuiting which will be more expensive then these simple integer operations
		return ( ( ( uint32 )bHasPendingOwnershipTransfer - ( uint32 )other.bHasPendingOwnershipTransfer ) |
				 ( ( uint32 )vkImageLayout - ( uint32 )other.vkImageLayout ) |
				 ( uint32 )( !( readScope == other.readScope ) ) |
				 ( uint32 )( !( writeScope == other.writeScope ) ) |
				 ( ownerQueueFamilyIndex - other.ownerQueueFamilyIndex ) ) == 0;
	}

	/**
	 * @brief Override operator !=
	 * @param other		Other synchronization scope
	 * @return Return TRUE if both isn't equal, otherwise FALSE
	 */
	FORCEINLINE bool operator!=( const studioAPISyncStateImageVk_t& other ) const
	{
		// Use integer OR to prevent boolean short circuiting which will be more expensive then these simple integer operations
		return ( ( ( uint32 )bHasPendingOwnershipTransfer - ( uint32 )other.bHasPendingOwnershipTransfer ) |
				 ( ( uint32 )vkImageLayout - ( uint32 )other.vkImageLayout ) |
				 ( uint32 )( readScope != other.readScope ) |
				 ( uint32 )( writeScope != other.writeScope ) |
				 ( ownerQueueFamilyIndex - other.ownerQueueFamilyIndex ) ) != 0;
	}

	bool					bHasPendingOwnershipTransfer;	/**< Has pending ownership transfer */
	VkImageLayout			vkImageLayout;					/**< Vulkan image layout */
	studioAPISyncScopeVk_t	readScope;						/**< Read scope */
	studioAPISyncScopeVk_t	writeScope;						/**< Write scope */
	uint32					ownerQueueFamilyIndex;			/**< Queue family index which own of a resource */
};


/**
 * @ingroup studioapi_vk
 * @brief Vulkan StudioAPI synchronization manager
 */
class CStudioAPISyncMgrVk
{
public:
	/**
	 * @brief Constructor
	 */
	CStudioAPISyncMgrVk();

	/**
	 * @brief Destructor
	 */
	~CStudioAPISyncMgrVk();

	/**
	 * @brief Initialize the manager
	 */
	void Init();

	/**
	 * @brief Shutdown the manager
	 */
	void Shutdown();

	/**
	 * @brief Wait a frame in-flight
	 * The function waits when a frame in-flight will be available and
	 * free all pending semaphores and fences of the frame
	 * 
	 * @param indexFrameInFlight	Frame in-flight index, must be from 0 to STUDIOAPI_VK_NUM_FRAMES_IN_FLIGHT
	 */
	void WaitFrameInFlight( uint32 indexFrameInFlight );

	/**
	 * @brief Wait all frame in-flights
	 */
	FORCEINLINE void WaitAllFrameInFlights()
	{
		for ( uint32 index = 0; index < STUDIOAPI_VK_NUM_FRAMES_IN_FLIGHT; ++index )
		{
			WaitFrameInFlight( index );
		}
	}

	/**
	 * @brief Create a new fence
	 * @param bCreateSignaled	Is need create the fence in signaled state
	 * @return Return created a new fence
	 */
	CStudioAPIFenceVk* CreateFence( bool bCreateSignaled = false );

	/**
	 * @brief Create a new semaphore
	 * @return Return create a new semaphore
	 */
	CStudioAPISemaphoreVk* CreateSemaphore();

	/**
	 * @brief Find a named semaphore of create a new one
	 * @param pName		The semaphore name
	 * @return Return found the named semaphore or created a new one
	 */
	CStudioAPINamedSemaphoreVk* FindOrCreateNamedSemaphore( const achar* pName );

	/**
	 * @brief Release the fence
	 * The function places the fence into a pending fences to free and will be released
	 * at same index of frame in-flight later
	 * 
	 * @param pFence	The fence to release. Will be set to NULL
	 */
	void ReleaseFence( CStudioAPIFenceVk*& pFence );

	/**
	 * @brief Release the semaphore
	 * The function places the semaphore into a pending semaphore to free and will be released
	 * at same index of frame in-flight later
	 * 
	 * @param pSemaphore	The semaphore to release. Will be set to NULL
	 */
	void ReleaseSemaphore( CStudioAPISemaphoreVk*& pSemaphore );

	/**
	 * @brief Release the named semaphore
	 * The function places the named semaphore into a pending named semaphore to free and will be released
	 * at same index of frame in-flight later
	 * 
	 * @param pNamedSemaphore	The named semaphore to release. Will be set to NULL
	 */
	void ReleaseNamedSemaphore( CStudioAPINamedSemaphoreVk*& pNamedSemaphore );

	/**
	 * @brief Add a fence for a frame in-flight
	 * @param pFence				Fence
	 * @param indexFrameInFlight	Frame in-flight index, must be from 0 to STUDIOAPI_VK_NUM_FRAMES_IN_FLIGHT
	 */
	FORCEINLINE void AddFrameInFlightFence( CStudioAPIFenceVk* pFence, uint32 indexFrameInFlight )
	{
		frameInFlightFences[indexFrameInFlight].emplace_back( pFence );
	}

private:
	std::list<CStudioAPIFenceVk*>										fences;														/**< Fences */
	std::list<CStudioAPISemaphoreVk*>									semaphores;													/**< Semaphores */
	std::list<CStudioAPIFenceVk*>										frameInFlightFences[STUDIOAPI_VK_NUM_FRAMES_IN_FLIGHT];		/**< Frame in-flight fences (per a frame in-flight) */
	std::list<CStudioAPIFenceVk*>										pendingFreeFences[STUDIOAPI_VK_NUM_FRAMES_IN_FLIGHT];		/**< Pending to free fence per a frame in-flight */
	std::list<CStudioAPISemaphoreVk*>									pendingFreeSemaphores[STUDIOAPI_VK_NUM_FRAMES_IN_FLIGHT];	/**< Pending to free semaphores per a frame in-flight */
	std::list<CStudioAPIFenceVk*>										freeFences;													/**< Free fences */
	std::list<CStudioAPISemaphoreVk*>									freeSemaphores;												/**< Free semaphores */
	std::unordered_map<std::string_view, CStudioAPINamedSemaphoreVk*>	namedSemaphoresDict;										/**< Named semaphores dictionary */
};


/**
 * @ingroup studioapi_vk
 * @brief Update a synchronization state of a buffer
 * The function update a synchronization state of a buffer. Useful for
 * for implicit transfer of ownership without barrier and update access with stage masks
 * 
 * @param vkDstAccessMask		Destination Vulkan access mask
 * @param vkDstStageMask		Destination Vulkan pipeline stage mask
 * @param dstQueueFamilyIndex	Destination queue family which will be owner of the resource
 * @param syncState				A sync state to update
 */
void VK_UpdateSyncStateBuffer( VkAccessFlags vkDstAccessMask, VkPipelineStageFlags vkDstStageMask, uint32 dstQueueFamilyIndex, studioAPISyncStateBufferVk_t& syncState );

/**
 * @ingroup studioapi_vk
 * @brief Update a synchronization state and make a buffer barrier
 * The function update a synchronization state of a buffer and make
 * a buffer barrier if it need. Useful for transfer ownership and
 * make barriers for the buffer
 * 
 * @param vkDstAccessMask			Destination Vulkan access mask
 * @param vkDstStageMask			Destination Vulkan pipeline stage mask
 * @param dstQueueFamilyIndex		Destination queue family which will be owner of the resource
 * @param syncState					A sync state to update
 * @param bufferMemoryBarrier		An output StudioAPI buffer memory barrier (valid only if the barrier is need)
 * @param bufferUsageFlags			Buffer usage flags of the buffer which the barrier will be created for
 * @return Return TRUE if the barrier is need, otherwise FALSE
 */
bool VK_UpdateSyncStateBufferWithBarrier( VkAccessFlags vkDstAccessMask, VkPipelineStageFlags vkDstStageMask, uint32 dstQueueFamilyIndex, studioAPISyncStateBufferVk_t& syncState, studioAPIBufferMemoryBarrierVk_t& bufferMemoryBarrier, uint32 bufferUsageFlags );

/**
 * @ingroup studioapi_vk
 * @brief Update a synchronization state of an image
 * The function update a synchronization state of an texture. Useful for
 * for implicit transfer of ownership without barrier, change an image layout 
 * and update access with stage masks
 *
 * @param vkDstImageLayout		Destination image layout
 * @param vkDstAccessMask		Destination Vulkan access mask
 * @param vkDstStageMask		Destination Vulkan pipeline stage mask
 * @param dstQueueFamilyIndex	Destination queue family which will be owner of the resource
 * @param syncState				A sync state to update
 */
void VK_UpdateSyncStateImage( VkImageLayout vkDstImageLayout, VkAccessFlags vkDstAccessMask, VkPipelineStageFlags vkDstStageMask, uint32 dstQueueFamilyIndex, studioAPISyncStateImageVk_t& syncState );

/**
 * @ingroup studioapi_vk
 * @brief Update a synchronization state and make an image barrier
 * The function update a synchronization state of an image and make
 * an image barrier if it need. Useful for transfer ownership, change
 * an image layout and make barriers for the texture
 *
 * @param vkDstImageLayout			Destination image layout
 * @param vkDstAccessMask			Destination Vulkan access mask
 * @param vkDstStageMask			Destination Vulkan pipeline stage mask
 * @param dstQueueFamilyIndex		Destination queue family which will be owner of the resource
 * @param syncState					A sync state to update
 * @param imageMemoryBarrier		An output StudioAPI image memory barrier (valid only if the barrier is need)
 * @param textureUsageFlags			Texture usage flags of the texture which the barrier will be created for
 * @return Return TRUE if the barrier is need, otherwise FALSE
 */
bool VK_UpdateSyncStateImageWithBarrier( VkImageLayout vkDstImageLayout, VkAccessFlags vkDstAccessMask, VkPipelineStageFlags vkDstStageMask, uint32 dstQueueFamilyIndex, studioAPISyncStateImageVk_t& syncState, studioAPIImageMemoryBarrierVk_t& imageMemoryBarrier, uint32 textureUsageFlags );

#endif // !VK_STUDIOAPI_SYNCMGR_H