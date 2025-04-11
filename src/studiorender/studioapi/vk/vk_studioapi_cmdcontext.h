/**
 * @file
 * @addtogroup studioapi_vk studioapi_vk
 */

#ifndef VK_STUDIOAPI_CMDCONTEXT_H
#define VK_STUDIOAPI_CMDCONTEXT_H

#include "Volk/volk.h"
#include "core/platform.h"
#include "core/types.h"
#include "studiorender/studioapi/vk/vk_studioapi_renderpass.h"
#include "studiorender/studioapi/vk/vk_studioapi_renderpipeline.h"
#include "studiorender/studioapi/vk/vk_studioapi_framebuffer.h"
#include "studiorender/studioapi/vk/vk_studioapi_buffer.h"
#include "studiorender/studioapi/vk/vk_studioapi_texture.h"
#include "studiorender/studioapi/vk/vk_studioapi_cmdlist.h"

// Forward declarations
class CStudioAPIQueueVk;
class CStudioAPIPendingRenderStateVk;
class CStudioAPIPendingComputeStateVk;

/**
 * @ingroup studioapi_vk
 * @brief Vulkan StudioAPI command context
 * 
 * Tracks pipeline state and records command lists.
 * This is where the actual rendering commands are recorded
 */
class CStudioAPICmdContextVk : public TRefCounted<IStudioAPICmdContext>
{
public:
	/**
	 * @brief Constructor
	 * @param queue					Queue which corresponds the context
	 * @param supportQueueTypes		 Support queue types (see studioAPIVkQueueFlag_t)
	 */
	CStudioAPICmdContextVk( CStudioAPIQueueVk& queue, uint8 supportQueueTypes );

	/**
	 * @brief Destructor
	 */
	~CStudioAPICmdContextVk();

	/**
	 * @brief Initialize the command context
	 */
	void Init();

	/**
	 * @brief Shutdown the command context
	 */
	void Shutdown();

	/**
	 * @brief Begin a frame
	 */
	void BeginFrame();

	/**
	 * @brief End the frame
	 */
	void EndFrame();

	/**
	 * @brief Set a render pipeline
	 * @param pCmdList			StudioAPI command list
	 * @param pRenderPipeline	StudioAPI render pipeline
	 */
	void SetRenderPipeline( CStudioAPICmdListVk* pCmdList, CStudioAPIRenderPipelineVk* pRenderPipeline );

	/**
	 * @brief Set a vertex buffer
	 * @param pCmdList			StudioAPI command list
	 * @param slot				Slot index
	 * @param pVertexBuffer		Vertex buffer
	 * @param offset			Offset in the buffer
	 */
	void SetVertexBuffer( CStudioAPICmdListVk* pCmdList, uint32 slot, CStudioAPIBufferVk* pVertexBuffer, uint64 offset );

	/**
	 * @brief Set a constant buffer
	 * @param pCmdList				StudioAPI command list
	 * @param set					Set index
	 * @param slot					Slot index in the set
	 * @param pConstantBuffer		StudioAPI constant buffer
	 */
	void SetConstantBuffer( CStudioAPICmdListVk* pCmdList, uint32 set, uint32 slot, CStudioAPIBufferVk* pConstantBuffer );

	/**
	 * @brief Set a texture
	 * @param pCmdList		StudioAPI command list
	 * @param set			Set index
	 * @param slot			Slot index in the set
	 * @param pTexture		Texture
	 */
	void SetTexture( CStudioAPICmdListVk* pCmdList, uint32 set, uint32 slot, CStudioAPITextureVk* pTexture );

	/**
	 * @brief Set a sampler
	 * @param pCmdList		StudioAPI command list
	 * @param set			Set index
	 * @param slot			Slot index in the set
	 * @param pSampler		Sampler
	 */
	void SetSampler( CStudioAPICmdListVk* pCmdList, uint32 set, uint32 slot, CStudioAPISamplerVk* pSampler );

	/**
	 * @brief Set viewport
	 * @param pCmdList	StudioAPI command list
	 * @param x			Screen position of viewport by X
	 * @param y			Screen position of viewport by Y
	 * @param width		Viewport width
	 * @param height	Viewport height
	 * @param minDepth	Viewport minimum depth
	 * @param maxDepth	Viewport maximum depth
	 */
	void SetViewport( CStudioAPICmdListVk* pCmdList, float x, float y, float width, float height, float minDepth, float maxDepth );

	/**
	 * @brief Set scissor
	 * @param pCmdList	StudioAPI command list
	 * @param x			Screen position of scissor by X
	 * @param y			Screen position of scissor by Y
	 * @param width		Scissor width
	 * @param height	Scissor height
	 */
	void SetScissor( CStudioAPICmdListVk* pCmdList, int32 x, int32 y, uint32 width, uint32 height );

	/**
	 * @brief Draw
	 * @param pCmdList			StudioAPI command list
	 * @param baseVertexIndex	Base vertex index
	 * @param numVertices		Number vertices to draw
	 * @param numInstances		Number instances to draw
	 */
	void Draw( CStudioAPICmdListVk* pCmdList, uint32 baseVertexIndex, uint32 numVertices, uint32 numInstances = 1 );

	/**
	 * @brief Draw primitives with indexed vertices
	 * @param pCmdList			StudioAPI command list
	 * @param pIndexBuffer		Index buffer
	 * @param baseVertexIndex	Base vertex index
	 * @param baseIndex			Start index in the index buffer
	 * @param numIndices		Number indices to draw
	 * @param numInstances		Number instances to draw
	 */
	void DrawIndexed( CStudioAPICmdListVk* pCmdList, CStudioAPIBufferVk* pIndexBuffer, uint32 baseVertexIndex, uint32 baseIndex, uint32 numIndices, uint32 numInstances = 1 );

	/**
	 * @brief Add pending buffer barriers
	 * @param pCmdList					StudioAPI command list
	 * @param numBufferBarriers			Buffer barriers number
	 * @param pVkBufferMemoryBarriers	Vulkan buffer memory barriers
	 * @param vkSrcStageMask			Vulkan source stage mask
	 * @param vkDstStageMask			Vulkan destination stage mask
	 */
	void AddPendingBufferBarriers( CStudioAPICmdListVk* pCmdList, uint32 numBufferBarriers, const VkBufferMemoryBarrier* pVkBufferMemoryBarriers, VkPipelineStageFlags vkSrcStageMask, VkPipelineStageFlags vkDstStageMask );

	/**
	 * @brief Add pending image barriers
	 * @param pCmdList					StudioAPI command list
	 * @param numImageBarriers			Image barriers number
	 * @param pVkImageMemoryBarrier		Vulkan image memory barriers
	 * @param vkSrcStageMask			Vulkan source stage mask
	 * @param vkDstStageMask			Vulkan destination stage mask
	 */
	void AddPendingImageBarriers( CStudioAPICmdListVk* pCmdList, uint32 numImageBarriers, const VkImageMemoryBarrier* pVkImageMemoryBarrier, VkPipelineStageFlags vkSrcStageMask, VkPipelineStageFlags vkDstStageMask );

	/**
	 * @brief Add pending memory barriers
	 * @param pCmdList					StudioAPI command list
	 * @param numMemoryBarriers			Memory barriers number
	 * @param pVkMemoryBarrier			Vulkan memory barriers
	 * @param vkSrcStageMask			Vulkan source stage mask
	 * @param vkDstStageMask			Vulkan destination stage mask
	 */
	void AddPendingMemoryBarriers( CStudioAPICmdListVk* pCmdList, uint32 numMemoryBarriers, const VkMemoryBarrier* pVkMemoryBarrier, VkPipelineStageFlags vkSrcStageMask, VkPipelineStageFlags vkDstStageMask );

	/**
	 * @brief Flush pending barriers into the command list
	 * @param pCmdList		StudioAPI command list
	 */
	void FlushPendingBarriers( CStudioAPICmdListVk* pCmdList );

	/**
	 * @brief Get queue which corresponds the context
	 * @return Return queue which corresponds the context
	 */
	FORCEINLINE CStudioAPIQueueVk& GetQueue()
	{
		return queue;
	}

	/**
	 * @brief Get the command buffer pool
	 * @return Return the command buffer pool
	 */
	FORCEINLINE CStudioAPICmdBufferPoolVk& GetCmdBufferPool()
	{
		return cmdBufferPools[g_StudioAPIVk.GetCurrentFrameInFlight()];
	}

	/**
	 * @brief Add support of operation types
	 * @param type	Queue type to add
	 */
	FORCEINLINE void AddSupportQueueTypes( uint8 type )
	{
		supportQueueTypes |= type;
	}

	/**
	 * @brief Has support of some queue types
	 * @param queueTypes	Queue types to check (see studioAPIVkQueueFlag_t)
	 * @return Return TRUE if all types are supported by the command context, otherwise FALSE
	 */
	FORCEINLINE bool HasSupportQueueTypes( uint8 types ) const
	{
		return ( supportQueueTypes & types ) == types;
	}

	/**
	 * @brief Get support queue types by the command context
	 * @return Return support queue types by the command context (see studioAPIVkQueueFlag_t)
	 */
	FORCEINLINE uint8 GetSupportQueueTypes() const
	{
		return supportQueueTypes;
	}

	/**
	 * @brief Has support of a queue type
	 * @param queueType		Queue type to check
	 * @return Return TRUE if the queue type is supported by the command context, otherwise FALSE
	 */
	virtual bool HasSupportQueueType( studioAPIQueueType_t queueType ) const override;

	/**
	 * @brief Get the command list batch manager
	 * @return Return the command list batch manager
	 */
	FORCEINLINE CStudioAPICmdListBatchMgrVk& GetCmdListBatchMgr()
	{
		return cmdListBatchMgr;
	}

private:
	/**
	 * @brief Vulkan StudioAPI group of pending pipeline barriers
	 */
	template<typename TType, uint32 maxNumPendingBarriers>
	struct studioAPIPendingPipelineBarrierGroupVk_t
	{
		TType					vkPendingBarriers[maxNumPendingBarriers];	/**< Vulkan pending barriers array */
		uint32					numPendingBarriers;							/**< Pending barriers number */
		VkPipelineStageFlags	vkSrcStageMask;								/**< Vulkan source stage mask */
		VkPipelineStageFlags	vkDstStageMask;								/**< Vulkan destination stage mask */
	};

	uint8																											supportQueueTypes;											/**< Support queue types (see studioAPIVkQueueFlag_t) */
	CStudioAPIQueueVk&																								queue;														/**< Queue which corresponds the context */
	CStudioAPICmdListBatchMgrVk																						cmdListBatchMgr;											/**< Command list batch manager */
	CStudioAPICmdBufferPoolVk																						cmdBufferPools[STUDIOAPI_VK_NUM_FRAMES_IN_FLIGHT];			/**< Command buffer pool for each queue type */
	studioAPIPendingPipelineBarrierGroupVk_t<VkBufferMemoryBarrier, STUDIOAPI_VK_MAX_NUM_PENDING_BUFFER_BARRIERS>	pendingBufferBarriers;										/**< Pending buffer barriers */
	studioAPIPendingPipelineBarrierGroupVk_t<VkImageMemoryBarrier, STUDIOAPI_VK_MAX_NUM_PENDING_IMAGE_BARRIERS>		pendingImageBarriers;										/**< Pending image barriers */
	studioAPIPendingPipelineBarrierGroupVk_t<VkMemoryBarrier, STUDIOAPI_VK_MAX_NUM_PENDING_MEMORY_BARRIERS>			pendingMemoryBarriers;										/**< Pending memory barriers */
	CStudioAPIPendingRenderStateVk*																					pPendingRenderState;										/**< Pending render state */
	CStudioAPIPendingComputeStateVk*																				pPendingComputeState;										/**< Pending compute state */
};

#endif // !VK_STUDIOAPI_CMDCONTEXT_H