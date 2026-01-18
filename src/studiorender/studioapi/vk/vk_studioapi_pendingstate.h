#pragma once
#include <unordered_map>

#include "studiorender/studioapi/vk/vk_studioapi_delegates.h"
#include "studiorender/studioapi/vk/vk_studioapi_cmdcontext.h"
#include "studiorender/studioapi/vk/vk_studioapi_pendingdescriptorstate.h"

//-----------------------------------------------------------------------------
// All the current Vulkan compute pipeline states in use
//-----------------------------------------------------------------------------
class CStudioAPIPendingComputeStateVk
{
public:
	CStudioAPIPendingComputeStateVk( CStudioAPICmdContextVk& cmdContext );

	void Reset();
	void PrepareForDispatch( CStudioAPICmdListVk* pCmdList );

private:
	CStudioAPICmdContextVk& cmdContext;
};

//-----------------------------------------------------------------------------
// All the current Vulkan render pipeline states in use
//-----------------------------------------------------------------------------
class CStudioAPIPendingRenderStateVk
{
public:
	CStudioAPIPendingRenderStateVk( CStudioAPICmdContextVk& cmdContext );
	~CStudioAPIPendingRenderStateVk();

	void Reset();

	void SetViewport( float x, float y, float width, float height, float minDepth, float maxDepth );
	void SetScissor( bool bEnable, int32 x, int32 y, uint32 width, uint32 height );
	void SetVertexBuffer( CStudioAPICmdListVk* pCmdList, uint32 slot, CStudioAPIBufferVk* pBuffer, uint64 offset );
	void SetIndexBuffer( CStudioAPICmdListVk* pCmdList, CStudioAPIBufferVk* pBuffer, uint64 offset );
	void SetConstantBuffer( CStudioAPICmdListVk* pCmdList, uint32 set, uint32 slot, CStudioAPIBufferVk* pConstantBuffer );
	void SetTexture( CStudioAPICmdListVk* pCmdList, uint32 set, uint32 slot, CStudioAPITextureVk* pTexture );
	void SetSampler( CStudioAPICmdListVk* pCmdList, uint32 set, uint32 slot, CStudioAPISamplerVk* pSampler );
	bool SetRenderPipeline( CStudioAPIRenderPipelineVk* pRenderPipeline, bool bForceReset );

	void BindRenderPipeline( CStudioAPICmdListVk* pCmdList );
	void UpdateDynamicStates( CStudioAPICmdListVk* pCmdList );
	void PrepareForDraw( CStudioAPICmdListVk* pCmdList );

private:
	struct vertexBuffer_t
	{
		vertexBuffer_t()
			: offset( 0 )
		{
		}

		void Clear();

		TRefPtr<CStudioAPIBufferVk> pBuffer;
		VkDeviceSize				offset;
	};

	struct indexBuffer_t
	{
		indexBuffer_t()
			: offset( 0 )
		{
		}

		void Clear();

		TRefPtr<CStudioAPIBufferVk> pBuffer;
		VkDeviceSize				offset;
	};

	struct descriptorStateCache_t
	{
		descriptorStateCache_t()
			: pRenderDescriptorState( NULL )
			, pRenderPipelineDeletedDelegate( NULL )
		{
		}

		CStudioAPIDescriptorStateRenderVk*					 pRenderDescriptorState;
		COnStudioAPIRenderPipelineDeletedVk::funcDelegate_t* pRenderPipelineDeletedDelegate;
	};

	static void OndRenderPipelineDeleted( void* pUserData, CStudioAPIRenderPipelineVk* pRenderPipeline );

	bool																	bScissorEnabled;
	bool																	bDirtyVertexBuffers;
	bool																	bDirtyIndexBuffer;
	VkViewport																vkViewport;
	VkRect2D																vkScissor;
	CStudioAPICmdContextVk&													cmdContext;
	TRefPtr<CStudioAPIRenderPipelineVk>										pCurrentRenderPipeline;
	CStudioAPIDescriptorStateRenderVk*										pCurrentRenderDescriptorState;
	indexBuffer_t															indexBuffer;
	vertexBuffer_t															vertexBuffers[STUDIOAPI_VK_MAX_VERTEX_ELEMENT_COUNT];
	std::unordered_map<CStudioAPIRenderPipelineVk*, descriptorStateCache_t> descriptorStatesDict;
};

#include "studiorender/studioapi/vk/vk_studioapi_pendingstate.inl"