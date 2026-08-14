#pragma once
#include <EASTL/unordered_map.h>

#include "studiorender/studioapi_vk/vk_studioapi_delegates.h"
#include "studiorender/studioapi_vk/vk_studioapi_cmdcontext.h"
#include "studiorender/studioapi_vk/vk_studioapi_pendingdescriptorstate.h"

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
	void SetVertexBufferUP( CStudioAPICmdListVk* pCmdList, uint32 slot, VkBuffer vkBuffer, VkDeviceSize offset );
	void SetIndexBufferUP( CStudioAPICmdListVk* pCmdList, VkBuffer vkBuffer, VkDeviceSize offset, VkIndexType vkIndexType );
	void SetConstantBuffer( CStudioAPICmdListVk* pCmdList, uint32 set, uint32 slot, CStudioAPIBufferVk* pConstantBuffer );
	void SetPushConstants( CStudioAPICmdListVk* pCmdList, byte* pData, uint32 dataSize );
	void SetTexture( CStudioAPICmdListVk* pCmdList, uint32 set, uint32 slot, CStudioAPITextureVk* pTexture );
	void SetSampler( CStudioAPICmdListVk* pCmdList, uint32 set, uint32 slot, CStudioAPISamplerVk* pSampler );
	bool SetRenderPipeline( CStudioAPIRenderPipelineVk* pRenderPipeline, bool bForceReset );

	void BindRenderPipeline( CStudioAPICmdListVk* pCmdList );
	void UpdateDynamicStates( CStudioAPICmdListVk* pCmdList );
	void PrepareForDraw( CStudioAPICmdListVk* pCmdList );
	bool HasIndexBuffer() const;

private:
	struct vertexBuffer_t
	{
		vertexBuffer_t();
		vertexBuffer_t( VkBuffer vkBuffer, VkDeviceSize offset );

		void Clear();
		bool IsValid() const;
		bool operator==( const vertexBuffer_t& right ) const;
		bool operator!=( const vertexBuffer_t& right ) const;

		VkBuffer	 vkBuffer;
		VkDeviceSize offset;
	};

	struct indexBuffer_t
	{
		indexBuffer_t();
		indexBuffer_t( VkBuffer vkBuffer, VkDeviceSize offset, VkIndexType vkIndexType );

		void Clear();
		bool IsValid() const;
		bool operator==( const indexBuffer_t& right ) const;
		bool operator!=( const indexBuffer_t& right ) const;

		VkBuffer	 vkBuffer;
		VkDeviceSize offset;
		VkIndexType	 vkIndexType;
	};

	struct pushConstants_t
	{
		pushConstants_t();
		void Clear();

		uint32 size;
		byte   data[STUDIOAPI_VK_MAX_PUSH_CONSTANT_SIZE];
	};

	struct descriptorStateCache_t
	{
		descriptorStateCache_t()
			: pRenderDescriptorState( NULL )
			, onRenderPipelineDeletedHandle( INVALID_HANDLE )
		{
		}

		CStudioAPIDescriptorStateRenderVk*			  pRenderDescriptorState;
		COnStudioAPIRenderPipelineDeletedVk::handle_t onRenderPipelineDeletedHandle;
	};

	static void OndRenderPipelineDeleted( void* pUserData, CStudioAPIRenderPipelineVk* pRenderPipeline );

	bool																	  bScissorEnabled;
	bool																	  bDirtyVertexBuffers;
	bool																	  bDirtyIndexBuffer;
	bool																	  bDirtyPushConstants;
	VkViewport																  vkViewport;
	VkRect2D																  vkScissor;
	CStudioAPICmdContextVk&													  cmdContext;
	CRefPtr<CStudioAPIRenderPipelineVk>										  pCurrentRenderPipeline;
	CStudioAPIDescriptorStateRenderVk*										  pCurrentRenderDescriptorState;
	indexBuffer_t															  indexBuffer;
	vertexBuffer_t															  vertexBuffers[STUDIOAPI_VK_MAX_VERTEX_ELEMENT_COUNT];
	pushConstants_t															  pushConstants;
	eastl::unordered_map<CStudioAPIRenderPipelineVk*, descriptorStateCache_t> descriptorStatesDict;
};

#include "studiorender/studioapi_vk/vk_studioapi_pendingstate.inl"
