#include "pch_studioapi_vk.h"
#define VMA_IMPLEMENTATION
#include "vma/vk_mem_alloc.h"
#include "studiorender/studioapi_vk/vk_studioapi_memorymgr.h"
#include "studiorender/studioapi_vk/vk_studioapi.h"

// Pending free command buffer size
#define STUDIOAPI_VK_PENDING_FREE_CMDBUFFER_SIZE 10 * 1024 * 1024  // 10MB

// Vulkan memory allocator name
#if STUDIOAPI_VK_GPU_TRACK_MEMORY_ALLOCATION
static const char* s_pVmaName = "Vulkan Memory Allocator";
#endif	// STUDIOAPI_VK_GPU_TRACK_MEMORY_ALLOCATION

/*
==================
CStudioAPIMemoryMgrVk::CStudioAPIMemoryMgrVk
==================
*/
CStudioAPIMemoryMgrVk::CStudioAPIMemoryMgrVk()
#if STUDIOAPI_VK_GPU_TRACK_MEMORY_ALLOCATION
	: memoryUsage( 0 )
	,
#else
	:
#endif	// STUDIOAPI_VK_GPU_TRACK_MEMORY_ALLOCATION
	vmaAllocator( VK_NULL_HANDLE )
{
}

/*
==================
CStudioAPIMemoryMgrVk::~CStudioAPIMemoryMgrVk
==================
*/
CStudioAPIMemoryMgrVk::~CStudioAPIMemoryMgrVk()
{
	Shutdown();
}

/*
==================
CStudioAPIMemoryMgrVk::Init
==================
*/
void CStudioAPIMemoryMgrVk::Init( uint32 vkVersion )
{
	// Initialize Vulkan memory allocator
	AssertMsg( vmaAllocator == VK_NULL_HANDLE, "StudioAPI Vulkan allocator can't be initialized twice!" );
	VmaAllocatorCreateInfo vmaAllocatorCreateInfo = {};
	vmaAllocatorCreateInfo.vulkanApiVersion		  = vkVersion;
	vmaAllocatorCreateInfo.physicalDevice		  = g_StudioAPIVk.GetDevice().GetVkPhysicalDevice();
	vmaAllocatorCreateInfo.device				  = g_StudioAPIVk.GetDevice().GetVkLogicalDevice();
	vmaAllocatorCreateInfo.instance				  = g_StudioAPIVk.GetDevice().GetVkInstance();

#if VMA_DYNAMIC_VULKAN_FUNCTIONS
	VmaVulkanFunctions vmaVulkanFunctions	 = {};
	vmaVulkanFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
	vmaVulkanFunctions.vkGetDeviceProcAddr	 = vkGetDeviceProcAddr;
	vmaAllocatorCreateInfo.pVulkanFunctions	 = &vmaVulkanFunctions;
#endif

	// Create VMA allocator
	STUDIOAPI_VK_VERIFY_RESULT( vmaCreateAllocator( &vmaAllocatorCreateInfo, &vmaAllocator ) );

	// Initialize command buffers for pending free resources
	for ( uint32 frameIdx = 0; frameIdx < STUDIOAPI_VK_NUM_FRAMES_IN_FLIGHT; ++frameIdx )
	{
		cmdBuffer_t& cmdBuffer = pendingFreeCmdBuffers[frameIdx];
		cmdBuffer.pBuffer	   = (byte*)Mem_MallocZero( STUDIOAPI_VK_PENDING_FREE_CMDBUFFER_SIZE );
		cmdBuffer.pWritePtr	   = cmdBuffer.pBuffer;
		cmdBuffer.size		   = STUDIOAPI_VK_PENDING_FREE_CMDBUFFER_SIZE;
		cmdBuffer.numCmds	   = 0;
	}
}

/*
==================
CStudioAPIMemoryMgrVk::Shutdown
==================
*/
void CStudioAPIMemoryMgrVk::Shutdown()
{
	// Execute all pending free command buffers and free memory
	// TODO BS yehor.pohuliaka - Need rework it because time to time the application crash at this action (in Retail configuration)
	for ( uint32 frameIdx = 0; frameIdx < STUDIOAPI_VK_NUM_FRAMES_IN_FLIGHT; ++frameIdx )
	{
		cmdBuffer_t& cmdBuffer = pendingFreeCmdBuffers[frameIdx];
		ExecPendingFreeCmdBuffer( cmdBuffer );
		Mem_Free( cmdBuffer.pBuffer );
		Mem_Memzero( &cmdBuffer, sizeof( cmdBuffer_t ) );
	}

	if ( vmaAllocator != VK_NULL_HANDLE )
	{
		// Destroy VMA allocator
		vmaDestroyAllocator( vmaAllocator );
		vmaAllocator = VK_NULL_HANDLE;
	}

	// Clear other fields
#if STUDIOAPI_VK_GPU_TRACK_MEMORY_ALLOCATION
	memoryUsage = 0;
	allocationDict.clear();
#endif	// STUDIOAPI_VK_GPU_TRACK_MEMORY_ALLOCATION
}

/*
==================
CStudioAPIMemoryMgrVk::AllocateBuffer
==================
*/
VmaAllocation CStudioAPIMemoryMgrVk::AllocateBuffer( const char* pDebugTag, const VkBufferCreateInfo& vkBufferCreateInfo, const VmaAllocationCreateInfo& vmaAllocationCreateInfo, VkBuffer& vkBuffer, VkDeviceSize* pAllocatedSize /* = NULL */ ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Create a buffer
	Assert( vkBufferCreateInfo.size > 0 );
	VmaAllocationInfo vmaAllocationInfo = {};
	VmaAllocation	  vmaAllocation		= VK_NULL_HANDLE;
	VkResult		  vkResult			= vmaCreateBuffer( vmaAllocator, &vkBufferCreateInfo, &vmaAllocationCreateInfo, &vkBuffer, &vmaAllocation, &vmaAllocationInfo );
	if ( vmaAllocation == VK_NULL_HANDLE || vkResult != VK_SUCCESS )
	{
		// Print message about error
		Error( "StudioAPIVk: Failed to allocate GPU buffer (tag: %s)", pDebugTag );
		Error( "StudioAPIVk:\tVMA allocation flags: 0x%X", vmaAllocationCreateInfo.flags );
		Error( "StudioAPIVk:\tRequested size %llu", vkBufferCreateInfo.size );

#if STUDIOAPI_VK_GPU_TRACK_MEMORY_ALLOCATION
		studioAPIMemoryStatsVk_t memoryStats = GetStats();
		Error( "StudioAPIVk:\tGPU memory usage: %llu/%llu", memoryStats.usedMemory, memoryStats.totalAvailableMemory );
#endif	// STUDIOAPI_VK_GPU_TRACK_MEMORY_ALLOCATION

		// Exit from the method
		vkBuffer = VK_NULL_HANDLE;
		if ( pAllocatedSize )
		{
			*pAllocatedSize = 0;
		}
		return VK_NULL_HANDLE;
	}

	// Print information about allocated the GPU buffer and save the size info
	Msg( "StudioAPIVk: Allocated %lluB for GPU buffer (tag: %s, vkBuffer: 0x%p, vmaAllocation: 0x%p)", vmaAllocationInfo.size, pDebugTag, vkBuffer, vmaAllocation );
	if ( pAllocatedSize )
	{
		*pAllocatedSize = vmaAllocationInfo.size;
	}

	// Add to track GPU memory allocation
#if STUDIOAPI_VK_GPU_TRACK_MEMORY_ALLOCATION
	allocationInfo_t& allocationInfo = allocationDict[vmaAllocation];
	allocationInfo.type				 = ALLOCATION_TYPE_BUFFER;
	allocationInfo.size				 = vmaAllocationInfo.size;
	memoryUsage += vmaAllocationInfo.size;
	PROFILER_MEM_ALLOC( (void*)vkBuffer, allocationInfo.size, s_pVmaName );
#endif	// STUDIOAPI_VK_GPU_TRACK_MEMORY_ALLOCATION

	// We are done!
	return vmaAllocation;
}

/*
==================
CStudioAPIMemoryMgrVk::AllocateImage
==================
*/
VmaAllocation CStudioAPIMemoryMgrVk::AllocateImage( const char* pDebugTag, const VkImageCreateInfo& vkImageCreateInfo, const VmaAllocationCreateInfo& vmaAllocationCreateInfo, VkImage& vkImage, VkDeviceSize* pAllocatedSize /* = NULL */ ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Create a image
	VmaAllocationInfo vmaAllocationInfo = {};
	VmaAllocation	  vmaAllocation		= VK_NULL_HANDLE;
	VkResult		  vkResult			= vmaCreateImage( vmaAllocator, &vkImageCreateInfo, &vmaAllocationCreateInfo, &vkImage, &vmaAllocation, &vmaAllocationInfo );
	if ( vmaAllocation == VK_NULL_HANDLE || vkResult != VK_SUCCESS )
	{
		// Print message about error
		Error( "StudioAPIVk: Failed to allocate GPU image (tag: %s)", pDebugTag );
		Error( "StudioAPIVk:\tVMA allocation flags: 0x%X", vmaAllocationCreateInfo.flags );
		Error( "StudioAPIVk:\tRequested size: %ix%ix%i", vkImageCreateInfo.extent.width, vkImageCreateInfo.extent.height, vkImageCreateInfo.extent.depth );
		Error( "StudioAPIVk:\tMips: %i", vkImageCreateInfo.mipLevels );
		Error( "StudioAPIVk:\tLayers: %i", vkImageCreateInfo.arrayLayers );

#if STUDIOAPI_VK_GPU_TRACK_MEMORY_ALLOCATION
		studioAPIMemoryStatsVk_t memoryStats = GetStats();
		Error( "StudioAPIVk:\tGPU memory usage: %llu/%llu", memoryStats.usedMemory, memoryStats.totalAvailableMemory );
#endif	// STUDIOAPI_VK_GPU_TRACK_MEMORY_ALLOCATION

		// Exit from the method
		vkImage = VK_NULL_HANDLE;
		if ( pAllocatedSize )
		{
			*pAllocatedSize = 0;
		}
		return VK_NULL_HANDLE;
	}

	// Print information about allocated the GPU image and save the size info
	Msg( "StudioAPIVk: Allocated %lluB for GPU image (tag: %s, vkImage: 0x%p, vmaAllocation: 0x%p)", vmaAllocationInfo.size, pDebugTag, vkImage, vmaAllocation );
	if ( pAllocatedSize )
	{
		*pAllocatedSize = vmaAllocationInfo.size;
	}

	// Add to track GPU memory allocation
#if STUDIOAPI_VK_GPU_TRACK_MEMORY_ALLOCATION
	allocationInfo_t& allocationInfo = allocationDict[vmaAllocation];
	allocationInfo.type				 = ALLOCATION_TYPE_IMAGE;
	allocationInfo.size				 = vmaAllocationInfo.size;
	memoryUsage += vmaAllocationInfo.size;
	PROFILER_MEM_ALLOC( (void*)vkImage, allocationInfo.size, s_pVmaName );
#endif	// STUDIOAPI_VK_GPU_TRACK_MEMORY_ALLOCATION

	// We are done!
	return vmaAllocation;
}

/*
==================
CStudioAPIMemoryMgrVk::DestroyBuffer
==================
*/
void CStudioAPIMemoryMgrVk::DestroyBuffer( VkBuffer vkBuffer, VmaAllocation vmaAllocation ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Check that VMA allocation and Vulkan handle are valid
	Assert( vkBuffer != VK_NULL_HANDLE );
	Assert( vmaAllocation != VK_NULL_HANDLE );

	// Destroy Vulkan buffer
	Msg( "StudioAPIVk: Destroyed GPU buffer (vkBuffer: 0x%p, vmaAllocation: 0x%p)", vkBuffer, vmaAllocation );
	vmaDestroyBuffer( vmaAllocator, vkBuffer, vmaAllocation );

	// Free information about GPU memory allocation
#if STUDIOAPI_VK_GPU_TRACK_MEMORY_ALLOCATION
	auto it = allocationDict.find( vmaAllocation );
	if ( it != allocationDict.end() )
	{
		memoryUsage -= it->second.size;
		allocationDict.erase( it );
		PROFILER_MEM_FREE( (void*)vkBuffer, s_pVmaName );
	}
	else
	{
		Warning( "StudioAPIVk: Could not find GPU memory allocation (vmaAllocation: 0x%p)", vmaAllocation );
	}
#endif	// STUDIOAPI_VK_GPU_TRACK_MEMORY_ALLOCATION
}

/*
==================
CStudioAPIMemoryMgrVk::DestroyImage
==================
*/
void CStudioAPIMemoryMgrVk::DestroyImage( VkImage vkImage, VmaAllocation vmaAllocation ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Check that VMA allocation and Vulkan handle are valid
	Assert( vkImage != VK_NULL_HANDLE );
	Assert( vmaAllocation != VK_NULL_HANDLE );

	// Destroy Vulkan image
	Msg( "StudioAPIVk: Destroyed GPU image (vkImage: 0x%p, vmaAllocation: 0x%p)", vkImage, vmaAllocation );
	vmaDestroyImage( vmaAllocator, vkImage, vmaAllocation );

	// Free information about GPU memory allocation
#if STUDIOAPI_VK_GPU_TRACK_MEMORY_ALLOCATION
	auto it = allocationDict.find( vmaAllocation );
	if ( it != allocationDict.end() )
	{
		memoryUsage -= it->second.size;
		allocationDict.erase( it );
		PROFILER_MEM_FREE( (void*)vkImage, s_pVmaName );
	}
	else
	{
		Warning( "StudioAPIVk: Could not find GPU memory allocation (vmaAllocation: 0x%p)", vmaAllocation );
	}
#endif	// STUDIOAPI_VK_GPU_TRACK_MEMORY_ALLOCATION
}

/*
==================
CStudioAPIMemoryMgrVk::GetStats
==================
*/
studioAPIMemoryStatsVk_t CStudioAPIMemoryMgrVk::GetStats() const
{
	studioAPIMemoryStatsVk_t studioAPIMemoryStatsVk = {};
#if STUDIOAPI_VK_GPU_TRACK_MEMORY_ALLOCATION
	// Get heap budgets
	const VkPhysicalDeviceMemoryProperties& vkPhysicalDeviceMemoryProperties = g_StudioAPIVk.GetDevice().GetVkMemoryInfo();
	eastl::vector<VmaBudget>				vmaBudgets( vkPhysicalDeviceMemoryProperties.memoryHeapCount );
	vmaGetHeapBudgets( vmaAllocator, vmaBudgets.data() );

	// Calculate total budget
	uint64 totalBudget = 0;
	for ( uint32 budgetIdx = 0; budgetIdx < vkPhysicalDeviceMemoryProperties.memoryHeapCount; ++budgetIdx )
	{
		totalBudget += vmaBudgets[budgetIdx].budget;
	}

	// Calculate allocated buffers and images
	for ( auto it = allocationDict.begin(), itEnd = allocationDict.end(); it != itEnd; ++it )
	{
		switch ( it->second.type )
		{
			// Buffer
		case ALLOCATION_TYPE_BUFFER:
			++studioAPIMemoryStatsVk.bufferAllocationCount;
			studioAPIMemoryStatsVk.bufferAllocationSize += it->second.size;
			break;

			// Image
		case ALLOCATION_TYPE_IMAGE:
			++studioAPIMemoryStatsVk.imageAllocationCount;
			studioAPIMemoryStatsVk.imageAllocationSize += it->second.size;
			break;

		default:
			AssertMsg( false, "Unknown allocation type (0x%X)", it->second.type );
			break;
		}
	}

	// Calculate total allocations and used memory
	studioAPIMemoryStatsVk.allocationCount		= allocationDict.size();
	studioAPIMemoryStatsVk.usedMemory			= memoryUsage;
	studioAPIMemoryStatsVk.totalAvailableMemory = totalBudget;
#endif	// STUDIOAPI_VK_GPU_TRACK_MEMORY_ALLOCATION

	return studioAPIMemoryStatsVk;
}

/*
==================
CStudioAPIMemoryMgrVk::GetBufferAlignmentFromVkUsageFlags
==================
*/
uint64 CStudioAPIMemoryMgrVk::GetBufferAlignmentFromVkUsageFlags( VkBufferUsageFlags vkBufferUsageFlags )
{
	const VkPhysicalDeviceLimits& vkDeviceLimits				 = g_StudioAPIVk.GetDevice().GetVkDeviceLimits();
	bool						  bIsTexelBuffer				 = ( vkBufferUsageFlags & ( VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT ) ) != 0;
	bool						  bIsStorageBuffer				 = ( vkBufferUsageFlags & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT ) != 0;
	bool						  bIsVertexOrIndexBuffer		 = ( vkBufferUsageFlags & ( VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT ) ) != 0;
	bool						  bIsAccelerationStructureBuffer = ( vkBufferUsageFlags & VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR ) != 0;
	bool						  bIsUniformBuffer				 = ( vkBufferUsageFlags & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT ) != 0;

	uint64 alignment = 1;
	if ( bIsTexelBuffer || bIsStorageBuffer )
	{
		alignment = Max( alignment, vkDeviceLimits.minTexelBufferOffsetAlignment );
		alignment = Max( alignment, vkDeviceLimits.minStorageBufferOffsetAlignment );
	}
	else if ( bIsVertexOrIndexBuffer )
	{
		// No alignment restrictions on Vertex or Index buffers, leave it at 1
	}
	else if ( bIsAccelerationStructureBuffer )
	{
		AssertMsg( false, "No support yet" );
	}
	else if ( bIsUniformBuffer )
	{
		alignment = Max( alignment, vkDeviceLimits.minUniformBufferOffsetAlignment );
	}
	else
	{
		AssertMsg( false, "Unknown buffer alignment for vkBufferUsageFlags 0x%X", vkBufferUsageFlags );
	}

	return alignment;
}

/*
==================
CStudioAPIMemoryMgrVk::GetCurrentPendingFreeCmdBuffer
==================
*/
CStudioAPIMemoryMgrVk::cmdBuffer_t& CStudioAPIMemoryMgrVk::GetCurrentPendingFreeCmdBuffer() const
{
	return pendingFreeCmdBuffers[g_StudioAPIVk.GetCurrentFrameInFlight()];
}

/*
==================
CStudioAPIMemoryMgrVk::ExecPendingFreeCmdBuffer
==================
*/
void CStudioAPIMemoryMgrVk::ExecPendingFreeCmdBuffer( cmdBuffer_t& cmdBuffer ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	byte* pReadPtr = cmdBuffer.pBuffer;
	for ( uint32 cmdIdx = 0; cmdIdx < cmdBuffer.numCmds; ++cmdIdx )
	{
		pendingFreeResourceFn_t pFunc = *(pendingFreeResourceFn_t*)pReadPtr;
		pReadPtr += sizeof( pendingFreeResourceFn_t );
		uint32 size = *(uint32*)pReadPtr;
		pReadPtr += sizeof( uint32 );

		pFunc( pReadPtr );
		pReadPtr += size;
	}

	cmdBuffer.pWritePtr = cmdBuffer.pBuffer;
	cmdBuffer.numCmds	= 0;
}
