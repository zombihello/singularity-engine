#pragma once
#include "Volk/volk.h"
#include "tier0/defines.h"
#include "tier0/types.h"
#include "studiorender/studioapi_vk/vk_studioapi_renderpass.h"
#include "studiorender/studioapi_vk/vk_studioapi_renderpipeline.h"
#include "studiorender/studioapi_vk/vk_studioapi_framebuffer.h"
#include "studiorender/studioapi_vk/vk_studioapi_buffer.h"
#include "studiorender/studioapi_vk/vk_studioapi_texture.h"
#include "studiorender/studioapi_vk/vk_studioapi_cmdlist.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CStudioAPIQueueVk;
class CStudioAPIPendingRenderStateVk;
class CStudioAPIPendingComputeStateVk;

//-----------------------------------------------------------------------------
// Vulkan StudioAPI command context
//
// Tracks pipeline state and records command lists.
// This is where the actual rendering commands are recorded
//-----------------------------------------------------------------------------
class CStudioAPICmdContextVk : public TRefCounted<IStudioAPICmdContext>
{
public:
	// IStudioAPICmdContext interface
	virtual bool HasSupportQueueType( studioAPIQueueType_t queueType ) const override;

	CStudioAPICmdContextVk( CStudioAPIQueueVk& queue, uint8 supportQueueTypes );
	~CStudioAPICmdContextVk();

	// Initialize and shutdown functions
	void Init();
	void Shutdown();

	// Begin/end a frame
	void BeginFrame();
	void EndFrame();

	// Set resources
	void SetRenderPipeline( CStudioAPICmdListVk* pCmdList, CStudioAPIRenderPipelineVk* pRenderPipeline );
	void SetVertexBuffer( CStudioAPICmdListVk* pCmdList, uint32 slot, CStudioAPIBufferVk* pVertexBuffer, uint64 offset );
	void SetConstantBuffer( CStudioAPICmdListVk* pCmdList, uint32 set, uint32 slot, CStudioAPIBufferVk* pConstantBuffer );
	void SetTexture( CStudioAPICmdListVk* pCmdList, uint32 set, uint32 slot, CStudioAPITextureVk* pTexture );
	void SetSampler( CStudioAPICmdListVk* pCmdList, uint32 set, uint32 slot, CStudioAPISamplerVk* pSampler );
	void SetViewport( CStudioAPICmdListVk* pCmdList, float x, float y, float width, float height, float minDepth, float maxDepth );
	void SetScissor( CStudioAPICmdListVk* pCmdList, int32 x, int32 y, uint32 width, uint32 height );

	// Draw commands
	void Draw( CStudioAPICmdListVk* pCmdList, uint32 baseVertexIndex, uint32 numVertices, uint32 numInstances = 1 );
	void DrawIndexed( CStudioAPICmdListVk* pCmdList, CStudioAPIBufferVk* pIndexBuffer, uint32 baseVertexIndex, uint32 baseIndex, uint32 numIndices, uint32 numInstances = 1 );

	// Barriers
	void AddPendingBufferBarriers( CStudioAPICmdListVk* pCmdList, uint32 numBufferBarriers, const VkBufferMemoryBarrier* pVkBufferMemoryBarriers, VkPipelineStageFlags vkSrcStageMask, VkPipelineStageFlags vkDstStageMask );
	void AddPendingImageBarriers( CStudioAPICmdListVk* pCmdList, uint32 numImageBarriers, const VkImageMemoryBarrier* pVkImageMemoryBarrier, VkPipelineStageFlags vkSrcStageMask, VkPipelineStageFlags vkDstStageMask );
	void AddPendingMemoryBarriers( CStudioAPICmdListVk* pCmdList, uint32 numMemoryBarriers, const VkMemoryBarrier* pVkMemoryBarrier, VkPipelineStageFlags vkSrcStageMask, VkPipelineStageFlags vkDstStageMask );
	void FlushPendingBarriers( CStudioAPICmdListVk* pCmdList );

	void  AddSupportQueueTypes( uint8 type );
	bool  HasSupportQueueTypes( uint8 types ) const;
	uint8 GetSupportQueueTypes() const;

	CStudioAPIQueueVk&			 GetQueue();
	CStudioAPICmdBufferPoolVk&	 GetCmdBufferPool();
	CStudioAPICmdListBatchMgrVk& GetCmdListBatchMgr();

private:
	// Vulkan StudioAPI group of pending pipeline barriers
	template<typename TType, uint32 maxNumPendingBarriers>
	struct studioAPIPendingPipelineBarrierGroupVk_t
	{
		TType				 vkPendingBarriers[maxNumPendingBarriers];
		uint32				 numPendingBarriers;
		VkPipelineStageFlags vkSrcStageMask;
		VkPipelineStageFlags vkDstStageMask;
	};

	uint8																										  supportQueueTypes;
	CStudioAPIQueueVk&																							  queue;
	CStudioAPICmdListBatchMgrVk																					  cmdListBatchMgr;
	CStudioAPICmdBufferPoolVk																					  cmdBufferPools[STUDIOAPI_VK_NUM_FRAMES_IN_FLIGHT];
	studioAPIPendingPipelineBarrierGroupVk_t<VkBufferMemoryBarrier, STUDIOAPI_VK_MAX_NUM_PENDING_BUFFER_BARRIERS> pendingBufferBarriers;
	studioAPIPendingPipelineBarrierGroupVk_t<VkImageMemoryBarrier, STUDIOAPI_VK_MAX_NUM_PENDING_IMAGE_BARRIERS>	  pendingImageBarriers;
	studioAPIPendingPipelineBarrierGroupVk_t<VkMemoryBarrier, STUDIOAPI_VK_MAX_NUM_PENDING_MEMORY_BARRIERS>		  pendingMemoryBarriers;
	CStudioAPIPendingRenderStateVk*																				  pPendingRenderState;
	CStudioAPIPendingComputeStateVk*																			  pPendingComputeState;
};

#include "studiorender/studioapi_vk/vk_studioapi_cmdcontext.inl"