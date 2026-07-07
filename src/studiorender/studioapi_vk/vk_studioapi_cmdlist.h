#pragma once
#include "studiorender/studioapi/istudioapi_cmdlist.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CStudioAPICmdContextVk;
class CStudioAPICmdBufferVk;
class CStudioAPISwapChainVk;
class CStudioAPISemaphoreVk;
class CStudioAPIFenceVk;
class CStudioAPISwapChainVk;
class CStudioAPICmdListBatchMgrVk;
class CStudioAPINamedSemaphoreVk;

//-----------------------------------------------------------------------------
// Vulkan StudioAPI command list
//-----------------------------------------------------------------------------
enum studioAPICmdListStateVk_t
{
	STUDIOAPI_VK_CMDLIST_STATE_READY_FOR_RECORD,
	STUDIOAPI_VK_CMDLIST_STATE_HAS_BEGUN_RECORD,
	STUDIOAPI_VK_CMDLIST_STATE_IS_INSIDE_RENDER_PASS,
	STUDIOAPI_VK_CMDLIST_STATE_HAS_ENDED_RECORD
};

class CStudioAPICmdListVk : public CRefCounted<IStudioAPICmdList>
{
public:
	friend CStudioAPICmdListBatchMgrVk;

	// IStudioAPICmdList interface
	// Begin/end record commands into the list
	virtual void BeginRecord() override;
	virtual void EndRecord() override;

	// Begin/end a render pass
	virtual void BeginRenderPass( IStudioAPIRenderPass* pRenderPass, IStudioAPIFrameBuffer* pFrameBuffer ) override;
	virtual void EndRenderPass() override;

	virtual void SetRenderPipeline( IStudioAPIRenderPipeline* pRenderPipeline ) override;
	virtual void SetVertexBuffer( uint32 slot, IStudioAPIBuffer* pVertexBuffer, uint64 offset ) override;
	virtual void SetIndexBuffer( IStudioAPIBuffer* pIndexBuffer, uint64 offset ) override;
	virtual void SetConstantBuffer( uint32 set, uint32 slot, IStudioAPIBuffer* pConstantBuffer ) override;
	virtual void SetTexture( uint32 set, uint32 slot, IStudioAPITexture* pTexture ) override;
	virtual void SetSampler( uint32 set, uint32 slot, IStudioAPISampler* pSampler ) override;
	virtual void SetViewport( float minX, float minY, float minZ, float maxX, float maxY, float maxZ ) override;
	virtual void SetScissor( int32 x, int32 y, uint32 width, uint32 height ) override;

	virtual void Draw( uint32 baseVertexIndex, uint32 numVertices, uint32 numInstances = 1 ) override;
	virtual void DrawIndexed( uint32 baseVertexIndex, uint32 baseIndex, uint32 numIndices, uint32 numInstances = 1 ) override;
	virtual void Barrier( const studioAPIBarrier_t* pBarriers, uint32 numBarriers, bool bFlush = false ) override;
	virtual void CopyBuffer( IStudioAPIBuffer* pSrcBuffer, uint64 srcOffset, IStudioAPIBuffer* pDstBuffer, uint64 dstOffset, uint64 size ) override;

	// NOTE: To copy textures must be in STUDIOAPI_TEXTURE_LAYOUT_TRANSFER_SRC and STUDIOAPI_TEXTURE_LAYOUT_TRANSFER_DST layout
	virtual void CopyBufferToTexture( IStudioAPIBuffer* pSrcBuffer, uint64 srcOffset, IStudioAPITexture* pDstTexture, uint32 dstMip, const vector3i_t& dstOffset, const vector3i_t& dstSize, uint32 dstStartLayer = 0, uint32 dstNumLayers = -1, uint32 srcRowLength = 0, uint32 srcTextureHeight = 0 ) override;
	virtual void CopyTextureToBuffer( IStudioAPITexture* pSrcTexture, uint32 srcMip, const vector3i_t& srcOffset, const vector3i_t& srcSize, IStudioAPIBuffer* pDstBuffer, uint64 dstOffset, uint32 srcStartLayer = 0, uint32 srcNumLayers = -1, uint32 dstRowLength = 0, uint32 dstTextureHeight = 0 ) override;
	virtual void CopyTexture( IStudioAPITexture* pSrcTexture, uint32 srcMip, const vector3i_t srcOffset, IStudioAPITexture* pDstTexture, uint32 dstMip, const vector3i_t dstOffset, const vector3i_t size, uint32 srcStartLayer = 0, uint32 dstStartLayer = 0, uint32 numLayers = 1 ) override;

	virtual IStudioAPICmdContext* GetCmdContext() const override;

	CStudioAPICmdListVk( CStudioAPICmdContextVk* pCmdContext );
	~CStudioAPICmdListVk();

	void MarkHasDynamicStates();
	void MarkHasPipeline();
	void MarkHasViewport( VkViewport vkViewport );
	void MarkHasScissor( VkRect2D vkScissor );

	bool IsNeedsDynamicStateSet() const;
	bool HasPipeline() const;
	bool HasViewport() const;
	bool HasScissor() const;

	VkViewport				  GetVkCurrentViewport() const;
	VkRect2D				  GetVkCurrentScissor() const;
	studioAPICmdListStateVk_t GetState() const;
	CStudioAPICmdBufferVk*	  GetCmdBuffer() const;

private:
	bool							bNeedsDynamicStateSet;
	bool							bHasPipeline;
	bool							bHasViewport;
	bool							bHasScissor;
	studioAPICmdListStateVk_t		state;
	VkViewport						vkCurrentViewport;
	VkRect2D						vkCurrentScissor;
	CRefPtr<CStudioAPICmdContextVk> pCmdContext;
	CStudioAPICmdBufferVk*			pCmdBuffer;
};

//-----------------------------------------------------------------------------
// Vulkan StudioAPI command list batch
//-----------------------------------------------------------------------------
class CStudioAPICmdListBatchVk : public CRefCounted<IStudioAPICmdListBatch>
{
public:
	friend CStudioAPICmdListBatchMgrVk;

	// IStudioAPICmdListBatch interface
	// Add/remove a command list
	virtual void AddCmdList( IStudioAPICmdList* pCmdList ) override;
	virtual void RemoveCmdList( IStudioAPICmdList* pCmdList ) override;
	virtual void RemoveAllCmdLists() override;

	// Set the synchronization signals
	// NOTE: Only one signal can be used per a frame, otherwise will be undefined behavior
	// NOTE: Only one sync with a swap chain can be used per a frame, otherwise will be undefined behavior (globally)
	virtual void SyncSignal( const char* pSyncName ) override;
	virtual void SyncWait( const char* pSyncName ) override;
	virtual void SyncSwapChain( IStudioAPISwapChain* pSwapChain, uint8 syncFlags ) override;

	virtual void RemoveSyncSignal( const char* pSyncName ) override;
	virtual void RemoveSyncWait( const char* pSyncName ) override;
	virtual void RemoveAllSyncSignals() override;
	virtual void RemoveAllSyncWaits() override;
	virtual void RemoveSyncSwapChain() override;

	virtual IStudioAPICmdContext* GetCmdContext() const override;
	virtual uint32				  GetNumCmdLists() const override;
	virtual IStudioAPICmdList**	  GetCmdLists() const override;

	CStudioAPICmdListBatchVk( CStudioAPICmdContextVk* pCmdContext );
	~CStudioAPICmdListBatchVk();

private:
	uint8										syncSwapChainFlags;
	CStudioAPISwapChainVk*						pSyncSwapChain;
	CRefPtr<CStudioAPICmdContextVk>				pCmdContext;
	eastl::list<CStudioAPINamedSemaphoreVk*>	syncSignals;
	eastl::list<CStudioAPINamedSemaphoreVk*>	syncWaits;
	eastl::vector<CRefPtr<CStudioAPICmdListVk>> cmdLists;
};

//-----------------------------------------------------------------------------
// Vulkan StudioAPI command list batch manager
//-----------------------------------------------------------------------------
class CStudioAPICmdListBatchMgrVk
{
public:
	CStudioAPICmdListBatchMgrVk( CStudioAPICmdContextVk& cmdContext );
	~CStudioAPICmdListBatchMgrVk();

	void Init();
	void Shutdown();

	void Submit( CStudioAPICmdListBatchVk* pCmdListBatch, bool bWait = false, uint64 waitTime = 0 );
	bool Wait( CStudioAPICmdListBatchVk* pCmdListBatch, uint64 waitTime = 0 );
	void ClearSubmittedBatches();

private:
	typedef eastl::unordered_map<CRefPtr<CStudioAPICmdListBatchVk>, CStudioAPIFenceVk*, CRefPtr<CStudioAPICmdListBatchVk>::hashFunction_t> submittedBatchesDict_t;

	void GrabVkSyncSignalsFromBatch( CStudioAPICmdListBatchVk* pCmdListBatch, eastl::vector<VkSemaphore>& vkSyncSignals ) const;
	void GrabVkSyncWaitsFromBatch( CStudioAPICmdListBatchVk* pCmdListBatch, eastl::vector<VkSemaphore>& vkSyncWaits, eastl::vector<VkPipelineStageFlags>& vkSyncWaitStageMasks ) const;
	void GrabVkCmdBuffersFromBatch( CStudioAPICmdListBatchVk* pCmdListBatch, eastl::vector<VkCommandBuffer>& vkCmdBuffers ) const;

	CStudioAPICmdContextVk& cmdContext;
	submittedBatchesDict_t	submittedBatchesDict;
};

#include "studiorender/studioapi_vk/vk_studioapi_cmdlist.inl"