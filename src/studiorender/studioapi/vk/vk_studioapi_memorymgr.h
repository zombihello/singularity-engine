/**
 * @file
 * @addtogroup studioapi_vk studioapi_vk
 */

#ifndef VK_STUDIOAPI_MEMORYMGR_H
#define VK_STUDIOAPI_MEMORYMGR_H

#include <list>

#include "vma/vk_mem_alloc.h"
#include "studiorender/studioapi/vk/vk_helpers.h"

/**
 * @ingroup studioapi_vk
 * @brief Should do GPU track memory allocation by Vulkan StudioAPI
 */
#define STUDIOAPI_VK_GPU_TRACK_MEMORY_ALLOCATION		!RETAIL

/**
 * @ingroup studioapi_vk
 * @brief Vulkan StudioAPI memory statistics
 */
struct studioAPIMemoryStatsVk_t
{
	uint64		usedMemory;					/**< Used memory */
	uint64		totalAvailableMemory;		/**< Total available memory */
	uint64		allocationCount;			/**< Allocation count */
	uint64		bufferAllocationSize;		/**< Buffer allocation size */
	uint64		bufferAllocationCount;		/**< Buffer allocation count */
	uint64		imageAllocationSize;		/**< Image allocation size */
	uint64		imageAllocationCount;		/**< Image allocation count */
};

/**
 * @ingroup studioapi_vk
 * @brief Vulkan StudioAPI memory manager
 */
class CStudioAPIMemoryMgrVk
{
public:
	/**
	 * @brief Constructor
	 */
	CStudioAPIMemoryMgrVk();

	/**
	 * @brief Destructor
	 */
	~CStudioAPIMemoryMgrVk();

	/**
	 * @brief Initialize the allocator
	 * @param vkVersion		Vulkan API version
	 */
	void Init( uint32 vkVersion );

	/**
	 * @brief Shutdown the allocator
	 */
	void Shutdown();

	/**
	 * @brief Allocate a buffer
	 * @param pDebugTag					Debug tag
	 * @param vkBufferCreateInfo		Buffer create information
	 * @param vmaMemoryUsage			VMA memory usage type
	 * @param vmaAllocationCreateInfo	VMA allocation create info
	 * @param vkBuffer					Output the created buffer. If failed will be VK_NULL_HANDLE
	 * @param pAllocatedSize			Output allocated size, optional
	 * @return Return VMA allocation handle. If failed return VK_NULL_HANDLE
	 */
	VmaAllocation AllocateBuffer( const achar* pDebugTag, const VkBufferCreateInfo& vkBufferCreateInfo, const VmaAllocationCreateInfo& vmaAllocationCreateInfo, VkBuffer& vkBuffer, VkDeviceSize* pAllocatedSize = NULL ) const;

	/**
	 * @brief Allocate an image
	 * @param pDebugTag					Debug tag
	 * @param vkImageCreateInfo			Image create information
	 * @param vmaAllocationCreateInfo	VMA allocation create info
	 * @param vkImage					Output the created image. If failed will be VK_NULL_HANDLE
	 * @param pAllocatedSize			Output allocated size, optional
	 * @return Return VMA allocation handle. If failed return VK_NULL_HANDLE
	 */
	VmaAllocation AllocateImage( const achar* pDebugTag, const VkImageCreateInfo& vkImageCreateInfo, const VmaAllocationCreateInfo& vmaAllocationCreateInfo, VkImage& vkImage, VkDeviceSize* pAllocatedSize = NULL ) const;

	/**
	 * @brief Destroy the buffer and free the allocation
	 * @param vkBuffer			Buffer to free
	 * @param vmaAllocation		Allocation to free
	 */
	void DestroyBuffer( VkBuffer vkBuffer, VmaAllocation vmaAllocation ) const;

	/**
	 * @brief Destroy the image and free the allocation
	 * @param vkImage			Image to free
	 * @param vmaAllocation		Allocation to free
	 */
	void DestroyImage( VkImage vkImage, VmaAllocation vmaAllocation ) const;

	/**
	 * @brief Free a resource
	 * The function place the lambda to pending list and later will be called to free a resource
	 * @param freeLambda		Lambda will be called to free a resource
	 */
	template<typename TFreeLambda>
	FORCEINLINE void FreeResource( TFreeLambda&& freeLambda ) const
	{
		cmdBuffer_t&		currentCmdBuffer = GetCurrentPendingFreeCmdBuffer();
		auto				pendingFreeCmdFunc = []( void* pUserData )
		{
			auto	pFunc = ( TFreeLambda* )pUserData;
			( *pFunc )();
			pFunc->~TFreeLambda();
		};

		AssertMsg( currentCmdBuffer.pWritePtr - currentCmdBuffer.pBuffer + 
				sizeof( pendingFreeResourceFn_t ) + sizeof( uint32 ) +
				sizeof( TFreeLambda ) < currentCmdBuffer.size, "Not enough memory in the command buffer to write the command" );
		*( pendingFreeResourceFn_t* )currentCmdBuffer.pWritePtr = pendingFreeCmdFunc;
		currentCmdBuffer.pWritePtr += sizeof( pendingFreeResourceFn_t );

		*( uint32* )currentCmdBuffer.pWritePtr = sizeof( TFreeLambda );
		currentCmdBuffer.pWritePtr += sizeof( uint32 );

		new( currentCmdBuffer.pWritePtr ) TFreeLambda( std::forward<TFreeLambda>( ( TFreeLambda&& )freeLambda ) );
		currentCmdBuffer.pWritePtr += sizeof( TFreeLambda );
		++currentCmdBuffer.numCmds;
	}

	/**
	 * @brief Free pending resources at a frame-in-flight
	 * @param indexFrameInFlight	Frame in-flight index, must be from 0 to STUDIOAPI_VK_NUM_FRAMES_IN_FLIGHT
	 */
	FORCEINLINE void FreePendingResources( uint32 indexFrameInFlight ) const
	{
		ExecPendingFreeCmdBuffer( pendingFreeCmdBuffers[indexFrameInFlight] );
	}

	/**
	 * @brief Map memory
	 * @param vmaAllocation		Allocation to map memory
	 * @return Return mapped data of the allocation
	 */
	template<typename TType>
	FORCEINLINE TType* MapMemory( VmaAllocation vmaAllocation ) const
	{
		TType*		pMappedMemory = NULL;
		STUDIOAPI_VK_VERIFY_RESULT( vmaMapMemory( vmaAllocator, vmaAllocation, ( void** )&pMappedMemory ) );
		return pMappedMemory;
	}

	/**
	 * @brief Unmap memory
	 * @param vmaAllocation		Allocation to unmap memory
	 */
	FORCEINLINE void UnmapMemory( VmaAllocation vmaAllocation ) const
	{
		vmaUnmapMemory( vmaAllocator, vmaAllocation );
	}

	/**
	 * @brief Get the memory statistics
	 * @return Return the memory statistics
	 */
	studioAPIMemoryStatsVk_t GetStats() const;

	/**
	 * @brief Get buffer alignment from Vulkan usage flags
	 * @param vkBufferUsageFlags	Vulkan buffer usage flags
	 * @return Return buffer alignment from Vulkan usage flags
	 */
	static uint64 GetBufferAlignmentFromVkUsageFlags( VkBufferUsageFlags vkBufferUsageFlags );

private:
	/**
	 * @brief Pending free resource function type
	 */
	typedef void ( *pendingFreeResourceFn_t )( void* pUserData );

	/**
	 * @brief Allocation type
	 */
	enum allocationType_t
	{
		ALLOCATION_TYPE_NONE,		/**< Unknown */
		ALLOCATION_TYPE_BUFFER,		/**< Buffer */
		ALLOCATION_TYPE_IMAGE		/**< Image */
	};

	/**
	 * @brief Information about allocation
	 */
	struct allocationInfo_t
	{
		allocationType_t	type;	/**< Allocation type */
		uint64				size;	/**< Allocated size */
	};

	/**
	 * @brief Command buffer
	 */
	struct cmdBuffer_t
	{
		byte*		pBuffer;	/**< Buffer memory */
		byte*		pWritePtr;	/**< Pointer to write data */
		uint32		size;		/**< Buffer size */
		uint32		numCmds;	/**< Commands number */
	};

	/**
	 * @brief Get pending free command buffer for current frame-in-flight
	 * @return Return pending free command buffer for current frame-in-flight
	 */
	cmdBuffer_t& GetCurrentPendingFreeCmdBuffer() const;

	/**
	 * @brief Execute pending free command buffer
	 * @param cmdBuffer		Command buffer to execute
	 */
	void ExecPendingFreeCmdBuffer( cmdBuffer_t& cmdBuffer ) const;

#if STUDIOAPI_VK_GPU_TRACK_MEMORY_ALLOCATION
	mutable uint64													memoryUsage;												/**< Memory usage of all heaps */
	mutable std::unordered_map<VmaAllocation, allocationInfo_t>		allocationDict;												/**< Allocation dictionary */
#endif // STUDIOAPI_VK_GPU_TRACK_MEMORY_ALLOCATION
	VmaAllocator													vmaAllocator;												/**< VMA allocator */
	mutable cmdBuffer_t												pendingFreeCmdBuffers[STUDIOAPI_VK_NUM_FRAMES_IN_FLIGHT];	/**< Command buffer for pending free resources for each frame-in-flight */
};

#endif // !VK_STUDIOAPI_MEMORYMGR_H