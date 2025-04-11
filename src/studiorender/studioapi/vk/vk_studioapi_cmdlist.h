/**
 * @file
 * @addtogroup studioapi_vk studioapi_vk
 */

#ifndef VK_STUDIOAPI_CMDBLIST_H
#define VK_STUDIOAPI_CMDBLIST_H

#include "studiorender/studioapi/istudioapi_cmdlist.h"

// Forward declarations
class CStudioAPICmdContextVk;
class CStudioAPICmdBufferVk;
class CStudioAPISwapChainVk;
class CStudioAPISemaphoreVk;
class CStudioAPIFenceVk;
class CStudioAPISwapChainVk;
class CStudioAPICmdListBatchMgrVk;
class CStudioAPINamedSemaphoreVk;

/**
 * @ingroup studioapi_vk
 * @brief Vulkan StudioAPI command list state
 */
enum studioAPICmdListStateVk_t
{
	STUDIOAPI_VK_CMDLIST_STATE_READY_FOR_RECORD,		/**< The command list ready for to begin record */
	STUDIOAPI_VK_CMDLIST_STATE_HAS_BEGUN_RECORD,		/**< The command list is inside record */
	STUDIOAPI_VK_CMDLIST_STATE_IS_INSIDE_RENDER_PASS,	/**< The command list is inside a render pass */
	STUDIOAPI_VK_CMDLIST_STATE_HAS_ENDED_RECORD			/**< The command list has ended record */
};

/**
 * @ingroup studioapi_vk
 * @brief Vulkan StudioAPI command list
 */
class CStudioAPICmdListVk : public TRefCounted<IStudioAPICmdList>
{
public:
	friend CStudioAPICmdListBatchMgrVk;

	/**
	 * @brief Constructor
	 * @param pCmdContext	Command context which the command list corresponds to 
	 */
	CStudioAPICmdListVk( CStudioAPICmdContextVk* pCmdContext );

	/**
	 * @brief Destructor
	 */
	~CStudioAPICmdListVk();

	/**
	 * @brief Begin record commands into the list
	 */
	virtual void BeginRecord() override;

	/**
	 * @brief End record commands into the list
	 */
	virtual void EndRecord() override;

	/**
	 * @brief Begin a render pass
	 * @param pRenderPass		StudioAPI render pass to begin
	 * @param pFrameBuffer		StudioAPI frame buffer to draw
	 */
	virtual void BeginRenderPass( IStudioAPIRenderPass* pRenderPass, IStudioAPIFrameBuffer* pFrameBuffer ) override;

	/**
	 * @brief End the render pass
	 */
	virtual void EndRenderPass() override;

	/**
	 * @brief Set a render pipeline
	 * @param pRenderPipeline	StudioAPI render pipeline
	 */
	virtual void SetRenderPipeline( IStudioAPIRenderPipeline* pRenderPipeline ) override;

	/**
	 * @brief Set a vertex buffer
	 * @param slot				Slot index
	 * @param pVertexBuffer		Vertex buffer
	 * @param offset			Offset in the buffer
	 */
	virtual void SetVertexBuffer( uint32 slot, IStudioAPIBuffer* pVertexBuffer, uint64 offset ) override;

	/**
	 * @brief Set a constant buffer
	 * @param set					Set index
	 * @param slot					Slot index in the set
	 * @param pConstantBuffer		Constant buffer
	 */
	virtual void SetConstantBuffer( uint32 set, uint32 slot, IStudioAPIBuffer* pConstantBuffer ) override;

	/**
	 * @brief Set viewport
	 * @param minX		Min x
	 * @param minY		Min y
	 * @param minZ		Min z
	 * @param maxX		Max x
	 * @param maxY		Max y
	 * @param maxZ		Max z
	 */
	virtual void SetViewport( float minX, float minY, float minZ, float maxX, float maxY, float maxZ ) override;

	/**
	 * @brief Set scissor
	 * @param x			Screen position of scissor by X
	 * @param y			Screen position of scissor by Y
	 * @param width		Scissor width
	 * @param height	Scissor height
	 */
	virtual void SetScissor( int32 x, int32 y, uint32 width, uint32 height ) override;

	/**
	 * @brief Set a texture
	 * @param set			Set index
	 * @param slot			Slot index in the set
	 * @param pTexture		Texture
	 */
	virtual void SetTexture( uint32 set, uint32 slot, IStudioAPITexture* pTexture ) override;

	/**
	 * @brief Set a sampler
	 * @param set			Set index
	 * @param slot			Slot index in the set
	 * @param pSampler		Sampler
	 */
	virtual void SetSampler( uint32 set, uint32 slot, IStudioAPISampler* pSampler ) override;

	/**
	 * @brief Draw primitives
	 * @param baseVertexIndex	Base vertex index
	 * @param numVertices		Number vertices to draw
	 * @param numInstances		Number instances to draw
	 */
	virtual void Draw( uint32 baseVertexIndex, uint32 numVertices, uint32 numInstances = 1 ) override;

	/**
	 * @brief Draw primitives with indexed vertices
	 * @param pIndexBuffer		Index buffer
	 * @param baseVertexIndex	Base vertex index
	 * @param baseIndex			Start index in the index buffer
	 * @param numIndices		Number indices to draw
	 * @param numInstances		Number instances to draw
	 */
	virtual void DrawIndexed( IStudioAPIBuffer* pIndexBuffer, uint32 baseVertexIndex, uint32 baseIndex, uint32 numIndices, uint32 numInstances = 1 ) override;

	/**
	 * @brief Place a barrier into the list
	 * @param pBarriers		StudioAPI barriers
	 * @param numBarriers	Barriers number
	 * @param bFlush		Is need flush all pending barriers
	 */
	virtual void Barrier( const studioAPIBarrier_t* pBarriers, uint32 numBarriers, bool bFlush = false ) override;

	/**
	 * @brief Copy a buffer to another buffer
	 * @param pSrcBuffer	Source buffer
	 * @param srcOffset		Starting offset in bytes from the start of pSrc
	 * @param pDstBuffer	Destination buffer
	 * @param dstOffset		Starting offset in bytes from the start of pDst
	 * @param size			Number of bytes to copy
	 */
	virtual void CopyBuffer( IStudioAPIBuffer* pSrcBuffer, uint64 srcOffset, IStudioAPIBuffer* pDstBuffer, uint64 dstOffset, uint64 size ) override;

	/**
	 * @brief Copy a buffer to a texture
	 * @warning To copy the pDstTexture must be in STUDIOAPI_TEXTURE_LAYOUT_TRANSFER_DST layout
	 *
	 * @param pSrcBuffer						Source buffer
	 * @param srcOffset							Offset in the source buffer
	 * @param pDstTexture						Destination texture
	 * @param dstMip							Destination mip level
	 * @param dstOffset							Offset in the destination mip level
	 * @param dstSize							Size in the destination mip level
	 * @param dstStartLayer						Start layer in the destination texture
	 * @param dstNumLayers						Layers number in the destination texture. -1 it is mean whole texture's layers
	 * @param srcRowLength, srcTextureHeight	Specify in texels a subregion of a larger two- or three-dimensional image in buffer memory, and control the addressing calculations. If either of these values is zero, that aspect of the buffer memory is considered to be tightly packed according to the dstSize
	 */
	virtual void CopyBufferToTexture( IStudioAPIBuffer* pSrcBuffer, uint64 srcOffset, IStudioAPITexture* pDstTexture, uint32 dstMip, const ivec3_t& dstOffset, const ivec3_t& dstSize, uint32 dstStartLayer = 0, uint32 dstNumLayers = -1, uint32 srcRowLength = 0, uint32 srcTextureHeight = 0 ) override;

	/**
	 * @brief Copy a texture to a buffer
	 * @warning To copy the pSrcTexture must be in STUDIOAPI_TEXTURE_LAYOUT_TRANSFER_SRC layout
	 *
	 * @param pSrcTexture						Source texture
	 * @param srcMip							Source mip level
	 * @param srcOffset							Offset in the source mip level
	 * @param srcSize							Size in the source mip level
	 * @param pDstBuffer						Destination buffer
	 * @param dstOffset							Offset in the destination buffer
	 * @param srcStartLayer						Start layer in the source texture
	 * @param srcNumLayers						Layers number in the source texture. -1 it is mean whole texture's layers
	 * @param dstRowLength, dstTextureHeight	Specify in texels a subregion of a larger two- or three-dimensional image in buffer memory, and control the addressing calculations. If either of these values is zero, that aspect of the buffer memory is considered to be tightly packed according to the srcSize
	 */
	virtual void CopyTextureToBuffer( IStudioAPITexture* pSrcTexture, uint32 srcMip, const ivec3_t& srcOffset, const ivec3_t& srcSize, IStudioAPIBuffer* pDstBuffer, uint64 dstOffset, uint32 srcStartLayer = 0, uint32 srcNumLayers = -1, uint32 dstRowLength = 0, uint32 dstTextureHeight = 0 ) override;

	/**
	 * @brief Copy a texture to another texture
	 * @warning To copy the pSrcTexture must be in STUDIOAPI_TEXTURE_LAYOUT_TRANSFER_SRC and pDstTexture must be in STUDIOAPI_TEXTURE_LAYOUT_TRANSFER_DST layout
	 *
	 * @param pSrcTexture			Source texture
	 * @param srcMip				Source mip level
	 * @param srcOffset				Offset in the source mip level
	 * @param pDstTexture			Destination texture
	 * @param dstMip				Destination mip level
	 * @param dstOffset				Offset in the destination mip level
	 * @param size					Size in texels to copy
	 * @param srcStartLayer			Start layer in the source texture
	 * @param dstStartLayer			Start layer in the destination texture
	 * @param numLayers				Layers number to copy
	 */
	virtual void CopyTexture( IStudioAPITexture* pSrcTexture, uint32 srcMip, const ivec3_t srcOffset, IStudioAPITexture* pDstTexture, uint32 dstMip, const ivec3_t dstOffset, const ivec3_t size, uint32 srcStartLayer = 0, uint32 dstStartLayer = 0, uint32 numLayers = 1 ) override;

	/**
	 * @brief Mark has dynamic states
	 */
	FORCEINLINE void MarkHasDynamicStates()
	{
		bNeedsDynamicStateSet = false;
	}

	/**
	 * @brief Mark has a pipeline
	 */
	FORCEINLINE void MarkHasPipeline()
	{
		bHasPipeline = true;
	}

	/**
	 * @brief Mark has a viewport
	 * @param vkViewport	Vulkan viewport
	 */
	FORCEINLINE void MarkHasViewport( VkViewport vkViewport )
	{
		vkCurrentViewport = vkViewport;
		bHasViewport = true;
	}

	/**
	 * @brief Mark has a scissor
	 * @param vkScissor		Vulkan scissor
	 */
	FORCEINLINE void MarkHasScissor( VkRect2D vkScissor )
	{
		vkCurrentScissor = vkScissor;
		bHasScissor = true;
	}

	/**
	 * @brief Is needs dynamic states to set
	 * @return Return TRUE if the command list needs dynamic states to set, otherwise FALSE
	 */
	FORCEINLINE bool IsNeedsDynamicStateSet() const
	{
		return bNeedsDynamicStateSet;
	}

	/**
	 * @brief Has a pipeline
	 * @return Return TRUE if the command list has a pipeline, otherwise FALSE
	 */
	FORCEINLINE bool HasPipeline() const
	{
		return bHasPipeline;
	}

	/**
	 * @brief Get current set Vulkan viewport
	 * @return Return current set Vulkan viewport. If isn't set return zeroed struct
	 */
	FORCEINLINE VkViewport GetVkCurrentViewport() const
	{
		return vkCurrentViewport;
	}

	/**
	 * @brief Has a viewport
	 * @return Return TRUE if the command list has a viewport, otherwise FALSE
	 */
	FORCEINLINE bool HasViewport() const
	{
		return bHasViewport;
	}

	/**
	 * @brief Get current set Vulkan scissor
	 * @return Return current set Vulkan scissor. If isn't set return zeroed struct
	 */
	FORCEINLINE VkRect2D GetVkCurrentScissor() const
	{
		return vkCurrentScissor;
	}

	/**
	 * @brief Has a scissor
	 * @return Return TRUE if the command list has a scissor, otherwise FALSE
	 */
	FORCEINLINE bool HasScissor() const
	{
		return bHasScissor;
	}

	/**
	 * @brief Get the command context which the list corresponded
	 * @return Return the command context which the list corresponded
	 */
	virtual IStudioAPICmdContext* GetCmdContext() const override;

	/**
	 * @brief Get the command list state
	 * @return Return the command list state
	 */
	FORCEINLINE studioAPICmdListStateVk_t GetState() const
	{
		return state;
	}

	/**
	 * @brief Get the current command buffer which use in the current record
	 * @return Return the current command buffer which use in the current record. If the list hasn't begun record return NULL
	 */
	FORCEINLINE CStudioAPICmdBufferVk* GetCmdBuffer() const
	{
		return pCmdBuffer;
	}

private:
	bool								bNeedsDynamicStateSet;				/**< Is needs dynamic states to set */
	bool								bHasPipeline;						/**< Has a pipeline or not */
	bool								bHasViewport;						/**< Has a viewport or not */
	bool								bHasScissor;						/**< Has a scissor or not */
	studioAPICmdListStateVk_t			state;								/**< The command list state */
	VkViewport							vkCurrentViewport;					/**< Current Vulkan viewport that set in the list */
	VkRect2D							vkCurrentScissor;					/**< Current Vulkan scissor that set in the list */
	TRefPtr<CStudioAPICmdContextVk>		pCmdContext;						/**< The command context which the command list corresponds to */
	CStudioAPICmdBufferVk*				pCmdBuffer;							/**< The current command buffer for record */
};

/**
 * @ingroup studioapi_vk
 * @brief Vulkan StudioAPI command list batch
 */
class CStudioAPICmdListBatchVk : public TRefCounted<IStudioAPICmdListBatch>
{
public:
	friend CStudioAPICmdListBatchMgrVk;

	/**
	 * @brief Constructor
	 * @param pCmdContext	Command context which the command list batch corresponds to 
	 */
	CStudioAPICmdListBatchVk( CStudioAPICmdContextVk* pCmdContext );

	/**
	 * @brief Destructor
	 */
	~CStudioAPICmdListBatchVk();

	/**
	 * @brief Add into the batch a command list
	 * @param pCmdList		StudioAPI command list to add
	 */
	virtual void AddCmdList( IStudioAPICmdList* pCmdList ) override;

	/**
	 * @brief Remove the command list from the batch
	 * @param pCmdList		StudioAPI command list to remove
	 */
	virtual void RemoveCmdList( IStudioAPICmdList* pCmdList ) override;

	/**
	 * @brief Remove all command lists from the batch
	 */
	virtual void RemoveAllCmdLists() override;

	/**
	 * @brief Set the synchronization signal
	 * @warning Only one signal can be used per a frame,
	 * otherwise will be undefined behavior
	 * @param pSyncName		Synchronization name
	 */
	virtual void SyncSignal( const achar* pSyncName ) override;

	/**
	 * @brief Set the synchronization wait of the signal
	 * @warning Only one wait signal can be used per a frame,
	 * otherwise will be undefined behavior
	 * @param pSyncName		Synchronization name
	 */
	virtual void SyncWait( const achar* pSyncName ) override;

	/**
	 * @brief Set synchronization with the swap chain
	 * @warning Only one sync with a swap chain can be used per a frame,
	 * otherwise will be undefined behavior (globally)
	 *
	 * @param pSwapChain	StudioAPI swap chain. If the pointer is NULL the batch will be unsynced from a swap chain
	 * @param syncFlags		Synchronization flags with the swap chain. Can't be zero if pSwapChain isn't NULL! (see studioAPISyncSwapChainFlags_t)
	 */
	virtual void SyncSwapChain( IStudioAPISwapChain* pSwapChain, uint8 syncFlags ) override;

	/**
	 * @brief Remove the synchronization signal
	 * @param pSyncName		Synchronization name
	 */
	virtual void RemoveSyncSignal( const achar* pSyncName ) override;

	/**
	 * @brief Remove the synchronization wait
	 * @param pSyncName		Synchronization name
	 */
	virtual void RemoveSyncWait( const achar* pSyncName ) override;

	/**
	 * @brief Remove all synchronization signals
	 */
	virtual void RemoveAllSyncSignals() override;

	/**
	 * @brief Remove all synchronization wait of signals
	 */
	virtual void RemoveAllSyncWaits() override;

	/**
	 * @brief Remove the synchronization with the swap chain
	 */
	virtual void RemoveSyncSwapChain() override;

	/**
	 * @brief Get the command context which the batch corresponded
	 * @return Return the command context which the batch corresponded
	 */
	virtual IStudioAPICmdContext* GetCmdContext() const override;

	/**
	 * @brief Get command lists number in the batch
	 * @return Return command lists number in the batch
	 */
	virtual uint32 GetNumCmdLists() const override;

	/**
	 * @brief Get command lists in the batch
	 * @return Return command lists in the batch. If the one hasn't any command list return NULL
	 */
	virtual IStudioAPICmdList** GetCmdLists() const override;

private:
	uint8										syncSwapChainFlags;	/**< Synchronization with the swap chain flags (see studioAPISyncSwapChainFlags_t) */
	CStudioAPISwapChainVk*						pSyncSwapChain;		/**< Synchronization with the swap chain */
	TRefPtr<CStudioAPICmdContextVk>				pCmdContext;		/**< The command context which the command list corresponds to */
	std::list<CStudioAPINamedSemaphoreVk*>		syncSignals;		/**< Synchronization signal */
	std::list<CStudioAPINamedSemaphoreVk*>		syncWaits;			/**< Synchronization wait signal */
	std::vector<TRefPtr<CStudioAPICmdListVk>>	cmdLists;			/**< Command lists in the batch */
};

/**
 * @ingroup studioapi_vk
 * @brief Vulkan StudioAPI command list batch manager
 */
class CStudioAPICmdListBatchMgrVk
{
public:
	/**
	 * @brief Constructor
	 * @param cmdContext		Command context which the manager corresponds to
	 */
	CStudioAPICmdListBatchMgrVk( CStudioAPICmdContextVk& cmdContext );

	/**
	 * @brief Destructor
	 */
	~CStudioAPICmdListBatchMgrVk();

	/**
	 * @brief Initialize the manager
	 */
	void Init();

	/**
	 * @brief Shutdown the manager
	 */
	void Shutdown();

	/**
	 * @brief Submit a command list batch
	 * @param pCmdListBatch		StudioAPI command list batch to submit
	 * @param bWait				Is need wait the command list batch when it will be done on GPU
	 * @param waitTime			Wait time in nanoseconds. Set 0 for infinity wait
	 */
	void Submit( CStudioAPICmdListBatchVk* pCmdListBatch, bool bWait = false, uint64 waitTime = 0 );

	/**
	 * @brief Wait the command list batch when it will be done on GPU
	 * @param waitTime		Wait time in nanoseconds. Set 0 for infinity wait
	 * @return Return TRUE if the command list batch has been done, otherwise FALSE
	 */
	bool Wait( CStudioAPICmdListBatchVk* pCmdListBatch, uint64 waitTime = 0 );

	/**
	 * @brief Clear submitted batches at current frame in-flight
	 */
	void ClearSubmittedBatches();

private:
	/**
	 * @brief Submitted batches dictionary type
	 */
	typedef std::unordered_map<TRefPtr<CStudioAPICmdListBatchVk>, CStudioAPIFenceVk*, TRefPtr<CStudioAPICmdListBatchVk>::hashFunction_t>	submittedBatchesDict_t;

	/**
	 * @brief Grab all Vulkan semaphores for synchronization signals from the batch
	 * The function grabs all Vulkan synchronization signals from the batch and set they to signal state
	 * 
	 * @param pCmdListBatch		StudioAPI command list batch
	 * @param vkSyncSignals		Output array with all Vulkan semaphores for synchronization signals from the batch
	 */
	void GrabVkSyncSignalsFromBatch( CStudioAPICmdListBatchVk* pCmdListBatch, std::vector<VkSemaphore>& vkSyncSignals ) const;

	/**
	 * @brief Get all Vulkan semaphores for synchronization signals from the batch
	 * The function grabs all Vulkan synchronization wait of signals from the batch and set they to unsignal state
	 * 
	 * @param pCmdListBatch			StudioAPI command list batch
	 * @param vkSyncSignals			Output array with all Vulkan semaphores for synchronization signals from the batch
	 * @param vkSyncWaitStageMasks	Output array with all Vulkan wait stage masks for vkSyncSignals
	 */
	void GrabVkSyncWaitsFromBatch( CStudioAPICmdListBatchVk* pCmdListBatch, std::vector<VkSemaphore>& vkSyncWaits, std::vector<VkPipelineStageFlags>& vkSyncWaitStageMasks ) const;

	/**
	 * @brief Get all Vulkan command buffers from the batch
	 * The function grabs all Vulkan command buffers from the batch and reset the batch's command lists
	 * 
	 * @param pCmdListBatch		StudioAPI command list batch
	 * @param vkCmdBuffers		Output array with all Vulkan command buffers from the batch
	 */
	void GrabVkCmdBuffersFromBatch( CStudioAPICmdListBatchVk* pCmdListBatch, std::vector<VkCommandBuffer>& vkCmdBuffers ) const;

	CStudioAPICmdContextVk&		cmdContext;				/**< Command context which the manager corresponds to */
	submittedBatchesDict_t		submittedBatchesDict;	/**< Submitted batches at the current frame in-flight dictionary */
};

#endif // !VK_STUDIOAPI_CMDBLIST_H