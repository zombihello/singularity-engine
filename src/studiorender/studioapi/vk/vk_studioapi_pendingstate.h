/**
 * @file
 * @addtogroup studioapi_vk studioapi_vk
 */

#ifndef VK_STUDIOAPI_PENDINGSTATE_H
#define VK_STUDIOAPI_PENDINGSTATE_H

#include <unordered_map>

#include "studiorender/studioapi/vk/vk_studioapi_delegates.h"
#include "studiorender/studioapi/vk/vk_studioapi_cmdcontext.h"
#include "studiorender/studioapi/vk/vk_studioapi_pendingdescriptorstate.h"

/**
 * @ingroup studioapi_vk
 * @brief All the current Vulkan compute pipeline states in use
 */
class CStudioAPIPendingComputeStateVk
{
public:
	/**
	 * @brief Constructor
	 * @param cmdContext	Command context
	 */
	CStudioAPIPendingComputeStateVk( CStudioAPICmdContextVk& cmdContext );

	/**
	 * @brief Reset the pending state
	 */
	FORCEINLINE void Reset()
	{}

	/**
	 * @brief Prepare for dispatch
	 * @param pCmdList	StudioAPI command list
	 */
	void PrepareForDispatch( CStudioAPICmdListVk* pCmdList );

private:
	CStudioAPICmdContextVk&		cmdContext;		/**< Command context */
};

/**
 * @ingroup studioapi_vk
 * @brief All the current Vulkan render pipeline states in use
 */
class CStudioAPIPendingRenderStateVk
{
public:
	/**
	 * @brief Constructor
	 * @param cmdContext	Command context
	 */
	CStudioAPIPendingRenderStateVk( CStudioAPICmdContextVk& cmdContext );

	/**
	 * @brief Destructor
	 */
	~CStudioAPIPendingRenderStateVk();

	/**
	 * @brief Reset the pending state
	 */
	void Reset();

	/**
	 * @brief Set viewport
	 * @param x			Screen position of viewport by X
	 * @param y			Screen position of viewport by Y
	 * @param width		Viewport width
	 * @param height	Viewport height
	 * @param minDepth	Viewport minimum depth
	 * @param maxDepth	Viewport maximum depth
	 */
	FORCEINLINE void SetViewport( float x, float y, float width, float height, float minDepth, float maxDepth )
	{
		// Update viewport
		vkViewport.x			= x;
		vkViewport.y			= y;
		vkViewport.width		= width;
		vkViewport.height		= height;
		vkViewport.minDepth		= minDepth;
		vkViewport.maxDepth		= maxDepth;

		// Update scissor
		SetScissor( false, ( int32 )x, ( int32 )y, ( uint32 )width, ( uint32 )height );
	}

	/**
	 * @brief Set scissor
	 * @param bEnable	Is enable scissor
	 * @param x			Screen position of scissor by X
	 * @param y			Screen position of scissor by Y
	 * @param width		Scissor width
	 * @param height	Scissor height
	 */
	FORCEINLINE void SetScissor( bool bEnable, int32 x, int32 y, uint32 width, uint32 height )
	{
		if ( bEnable )
		{
			vkScissor.offset.x		= x;
			vkScissor.offset.y		= y;
			vkScissor.extent.width	= width;
			vkScissor.extent.height	= height;
		}
		else
		{
			vkScissor.offset.x		= ( int32 )vkViewport.x;
			vkScissor.offset.y		= ( int32 )vkViewport.y;
			vkScissor.extent.width	= ( uint32 )vkViewport.width;
			vkScissor.extent.height	= ( uint32 )vkViewport.height;
		}

		bScissorEnabled = bEnable;
	}

	/**
	 * @brief Set a vertex buffer
	 * @param pCmdList		StudioAPI command list
	 * @param slot			Slot index
	 * @param pBuffer		StudioAPI vertex buffer
	 * @param offset		Offset in the buffer
	 */
	void SetVertexBuffer( CStudioAPICmdListVk* pCmdList, uint32 slot, CStudioAPIBufferVk* pBuffer, uint64 offset );

	/**
	 * @brief Set an index buffer
	 * @param pCmdList		StudioAPI command list
	 * @param pBuffer		StudioAPI index buffer
	 * @param offset		Offset in the buffer
	 */
	void SetIndexBuffer( CStudioAPICmdListVk* pCmdList, CStudioAPIBufferVk* pBuffer, uint64 offset );

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
	 * @brief Set a render pipeline
	 * @param pRenderPipeline	StudioAPI render pipeline
	 * @param bForceReset		Is need force reset
	 * @return Return TRUE if the render pipeline has been changed, otherwise FALSE
	 */
	bool SetRenderPipeline( CStudioAPIRenderPipelineVk* pRenderPipeline, bool bForceReset );

	/**
	 * @brief Bind the render pipeline
	 * @param pCmdList	StudioAPI command list
	 */
	FORCEINLINE void BindRenderPipeline( CStudioAPICmdListVk* pCmdList )
	{
		vkCmdBindPipeline( pCmdList->GetCmdBuffer()->GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pCurrentRenderPipeline->GetVkPipeline() );
		pCmdList->MarkHasPipeline();
	}

	/**
	 * @brief Update dynamic states
	 * @param pCmdList	StudioAPI command list
	 */
	void UpdateDynamicStates( CStudioAPICmdListVk* pCmdList );

	/**
	 * @brief Prepare for draw
	 * @param pCmdList	StudioAPI command list
	 */
	void PrepareForDraw( CStudioAPICmdListVk* pCmdList );

private:
	/**
	 * @brief Vertex buffer that set as source of vertices
	 */
	struct vertexBuffer_t
	{
		/**
		 * @brief Constructor
		 */
		vertexBuffer_t()
			: offset( 0 )
		{}

		/**
		 * @brief Clear
		 */
		FORCEINLINE void Clear()
		{
			pBuffer		= NULL;
			offset		= 0;
		}

		TRefPtr<CStudioAPIBufferVk>			pBuffer;		/**< StudioAPI buffer */
		VkDeviceSize						offset;			/**< Offset in the buffer */
	};

	/**
	 * @brief Index buffer that set as source of indices
	 */
	struct indexBuffer_t
	{
		/**
		 * @brief Constructor
		 */
		indexBuffer_t()
			: offset( 0 )
		{}

		/**
		 * @brief Clear
		 */
		FORCEINLINE void Clear()
		{
			pBuffer		= NULL;
			offset		= 0;
		}

		TRefPtr<CStudioAPIBufferVk>	pBuffer;	/**< StudioAPI buffer */
		VkDeviceSize				offset;		/**< Offset in the buffer */
	};

	/**
	 * @brief Descriptor state cache
	 */
	struct descriptorStateCache_t
	{
		/**
		 * @brief Constructor
		 */
		descriptorStateCache_t()
			: pRenderDescriptorState( NULL )
			, pRenderPipelineDeletedDelegate( NULL )
		{}

		CStudioAPIDescriptorStateRenderVk*						pRenderDescriptorState;			/**< Render descriptor state */
		COnStudioAPIRenderPipelineDeletedVk::funcDelegate_t*	pRenderPipelineDeletedDelegate;	/**< Delegate handle 'on StudioAPI Vulkan render pipeline deleted' */
	};

	/**
	 * @brief Event called when a render pipeline is deleted
	 * @param pUserData			Pointer to CStudioAPIPendingRenderStateVk
	 * @param pRenderPipeline	StudioAPI render pipeline
	 */
	static void OndRenderPipelineDeleted( void* pUserData, CStudioAPIRenderPipelineVk* pRenderPipeline );

	bool																		bScissorEnabled;										/**< Is enabled scissor */
	bool																		bDirtyVertexBuffers;									/**< Is dirty vertex buffers */
	bool																		bDirtyIndexBuffer;										/**< Is dirty index buffer */
	VkViewport																	vkViewport;												/**< Pending Vulkan viewport */
	VkRect2D																	vkScissor;												/**< Pending Vulkan scissor */
	CStudioAPICmdContextVk&														cmdContext;												/**< Command context */
	TRefPtr<CStudioAPIRenderPipelineVk>											pCurrentRenderPipeline;									/**< Current render pipeline */
	CStudioAPIDescriptorStateRenderVk*											pCurrentRenderDescriptorState;							/**< Render descriptor state */
	indexBuffer_t																indexBuffer;											/**< Pending index buffer */
	vertexBuffer_t																vertexBuffers[STUDIOAPI_VK_MAX_VERTEX_ELEMENT_COUNT];	/**< Pending vertex buffers */
	std::unordered_map<CStudioAPIRenderPipelineVk*, descriptorStateCache_t>		descriptorStatesDict;									/**< Render descriptor states dictionary */
};

#endif // !VK_STUDIOAPI_PENDINGSTATE_H