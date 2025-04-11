/**
 * @file
 * @addtogroup studioapi studioapi
 */

#ifndef ISTUDIOAPI_CMDLIST_H
#define ISTUDIOAPI_CMDLIST_H

#include "studiorender/studioapi/istudioapi.h"
#include "studiorender/studioapi/istudioapi_cmdcontext.h"
#include "studiorender/studioapi/istudioapi_texture.h"

// Forward declarations
class IStudioAPIRenderPass;
class IStudioAPIFrameBuffer;
class IStudioAPIRenderPipeline;
class IStudioAPIBuffer;
class IStudioAPITexture;
class IStudioAPISampler;
class IStudioAPISwapChain;
struct studioAPIBarrier_t;

/**
 * @ingroup studioapi
 * @brief StudioAPI synchronization with a swap chain flags
 */
enum studioAPISyncSwapChainFlags_t
{
	STUDIOAPI_SYNC_SWAPCHAIN_FLAGS_ACQUIRE_NEXT_IMAGE	= 1 << 0,	/**< Set synchronization with acquire of a swap chain image */
	STUDIOAPI_SYNC_SWAPCHAIN_FLAGS_PRESENT_TO_IMAGE		= 1 << 1	/**< Set synchronization to present into a swap chain image */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI command list interface
 */
class IStudioAPICmdList : public IStudioAPIResource
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~IStudioAPICmdList() {}

	/**
	 * @brief Begin record commands into the list
	 */
	virtual void BeginRecord() = 0;

	/**
	 * @brief End record commands into the list
	 */
	virtual void EndRecord() = 0;

	/**
	 * @brief Begin a render pass
	 * @param pRenderPass		StudioAPI render pass to begin
	 * @param pFrameBuffer		StudioAPI frame buffer to draw
	 */
	virtual void BeginRenderPass( IStudioAPIRenderPass* pRenderPass, IStudioAPIFrameBuffer* pFrameBuffer ) = 0;

	/**
	 * @brief End the render pass
	 */
	virtual void EndRenderPass() = 0;

	/**
	 * @brief Set a render pipeline
	 * @param pRenderPipeline	StudioAPI render pipeline
	 */
	virtual void SetRenderPipeline( IStudioAPIRenderPipeline* pRenderPipeline ) = 0;

	/**
	 * @brief Set a vertex buffer
	 * @param slot				Slot index
	 * @param pVertexBuffer		Vertex buffer
	 * @param offset			Offset in the buffer
	 */
	virtual void SetVertexBuffer( uint32 slot, IStudioAPIBuffer* pVertexBuffer, uint64 offset ) = 0;

	/**
	 * @brief Set a constant buffer
	 * @param set					Set index
	 * @param slot					Slot index in the set
	 * @param pConstantBuffer		Constant buffer
	 */
	virtual void SetConstantBuffer( uint32 set, uint32 slot, IStudioAPIBuffer* pConstantBuffer ) = 0;

	/**
	 * @brief Set a texture
	 * @param set			Set index
	 * @param slot			Slot index in the set
	 * @param pTexture		Texture
	 */
	virtual void SetTexture( uint32 set, uint32 slot, IStudioAPITexture* pTexture ) = 0;

	/**
	 * @brief Set a sampler
	 * @param set			Set index
	 * @param slot			Slot index in the set
	 * @param pSampler		Sampler
	 */
	virtual void SetSampler( uint32 set, uint32 slot, IStudioAPISampler* pSampler ) = 0;

	/**
	 * @brief Set viewport
	 * @param minX		Min x
	 * @param minY		Min y
	 * @param minZ		Min z
	 * @param maxX		Max x
	 * @param maxY		Max y
	 * @param maxZ		Max z
	 */
	virtual void SetViewport( float minX, float minY, float minZ, float maxX, float maxY, float maxZ ) = 0;

	/**
	 * @brief Set scissor
	 * @param x			Screen position of scissor by X
	 * @param y			Screen position of scissor by Y
	 * @param width		Scissor width
	 * @param height	Scissor height
	 */
	virtual void SetScissor( int32 x, int32 y, uint32 width, uint32 height ) = 0;

	/**
	 * @brief Draw primitives
	 * @param baseVertexIndex	Base vertex index
	 * @param numVertices		Number vertices to draw
	 * @param numInstances		Number instances to draw
	 */
	virtual void Draw( uint32 baseVertexIndex, uint32 numVertices, uint32 numInstances = 1 ) = 0;

	/**
	 * @brief Draw primitives with indexed vertices
	 * @param pIndexBuffer		Index buffer
	 * @param baseVertexIndex	Base vertex index
	 * @param baseIndex			Start index in the index buffer
	 * @param numIndices		Number indices to draw
	 * @param numInstances		Number instances to draw
	 */
	virtual void DrawIndexed( IStudioAPIBuffer* pIndexBuffer, uint32 baseVertexIndex, uint32 baseIndex, uint32 numIndices, uint32 numInstances = 1 ) = 0;

	/**
	 * @brief Place a barrier into the list
	 * @param pBarriers		StudioAPI barriers
	 * @param numBarriers	Barriers number
	 * @param bFlush		Is need flush all pending barriers
	 */
	virtual void Barrier( const studioAPIBarrier_t* pBarriers, uint32 numBarriers, bool bFlush = false ) = 0;

	/**
	 * @brief Copy a buffer to another buffer
	 * @param pSrcBuffer	Source buffer
	 * @param srcOffset		Starting offset in bytes from the start of pSrc
	 * @param pDstBuffer	Destination buffer
	 * @param dstOffset		Starting offset in bytes from the start of pDst
	 * @param size			Number of bytes to copy
	 */
	virtual void CopyBuffer( IStudioAPIBuffer* pSrcBuffer, uint64 srcOffset, IStudioAPIBuffer* pDstBuffer, uint64 dstOffset, uint64 size ) = 0;

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
	virtual void CopyBufferToTexture( IStudioAPIBuffer* pSrcBuffer, uint64 srcOffset, IStudioAPITexture* pDstTexture, uint32 dstMip, const ivec3_t& dstOffset, const ivec3_t& dstSize, uint32 dstStartLayer = 0, uint32 dstNumLayers = -1, uint32 srcRowLength = 0, uint32 srcTextureHeight = 0 ) = 0;

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
	virtual void CopyTextureToBuffer( IStudioAPITexture* pSrcTexture, uint32 srcMip, const ivec3_t& srcOffset, const ivec3_t& srcSize, IStudioAPIBuffer* pDstBuffer, uint64 dstOffset, uint32 srcStartLayer = 0, uint32 srcNumLayers = -1, uint32 dstRowLength = 0, uint32 dstTextureHeight = 0 ) = 0;

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
	virtual void CopyTexture( IStudioAPITexture* pSrcTexture, uint32 srcMip, const ivec3_t srcOffset, IStudioAPITexture* pDstTexture, uint32 dstMip, const ivec3_t dstOffset, const ivec3_t size, uint32 srcStartLayer = 0, uint32 dstStartLayer = 0, uint32 numLayers = 1 ) = 0;

	/**
	 * @brief Get the command context which the list corresponded
	 * @return Return the command context which the list corresponded
	 */
	virtual IStudioAPICmdContext* GetCmdContext() const = 0;
};

/**
 * @ingroup studioapi
 * @brief StudioAPI command list batch
 */
class IStudioAPICmdListBatch : public IStudioAPIResource
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~IStudioAPICmdListBatch() {}

	/**
	 * @brief Add into the batch a command list
	 * @param pCmdList		StudioAPI command list to add
	 */
	virtual void AddCmdList( IStudioAPICmdList* pCmdList ) = 0;

	/**
	 * @brief Remove the command list from the batch
	 * @param pCmdList		StudioAPI command list to remove
	 */
	virtual void RemoveCmdList( IStudioAPICmdList* pCmdList ) = 0;

	/**
	 * @brief Remove all command lists from the batch
	 */
	virtual void RemoveAllCmdLists() = 0;

	/**
	 * @brief Set the synchronization signal
	 * @warning Only one signal can be used per a frame, 
	 * otherwise will be undefined behavior
	 * @param pSyncName		Synchronization name
	 */
	virtual void SyncSignal( const achar* pSyncName ) = 0;

	/**
	 * @brief Set the synchronization wait of the signal
	 * @warning Only one wait signal can be used per a frame,
	 * otherwise will be undefined behavior
	 * @param pSyncName		Synchronization name
	 */
	virtual void SyncWait( const achar* pSyncName ) = 0;

	/**
	 * @brief Set synchronization with the swap chain
	 * @warning Only one sync with a swap chain can be used per a frame,
	 * otherwise will be undefined behavior (globally)
	 * 
	 * @param pSwapChain	StudioAPI swap chain. If the pointer is NULL the batch will be unsynced from a swap chain
	 * @param syncFlags		Synchronization flags with the swap chain. Can't be zero if pSwapChain isn't NULL! (see studioAPISyncSwapChainFlags_t)
	 */
	virtual void SyncSwapChain( IStudioAPISwapChain* pSwapChain, uint8 syncFlags ) = 0;

	/**
	 * @brief Remove the synchronization signal
	 * @param pSyncName		Synchronization name
	 */
	virtual void RemoveSyncSignal( const achar* pSyncName ) = 0;

	/**
	 * @brief Remove the synchronization wait
	 * @param pSyncName		Synchronization name
	 */
	virtual void RemoveSyncWait( const achar* pSyncName ) = 0;

	/**
	 * @brief Remove all synchronization signals
	 */
	virtual void RemoveAllSyncSignals() = 0;

	/**
	 * @brief Remove all synchronization wait of signals
	 */
	virtual void RemoveAllSyncWaits() = 0;

	/**
	 * @brief Remove the synchronization with the swap chain
	 */
	virtual void RemoveSyncSwapChain() = 0;

	/**
	 * @brief Get the command context which the batch corresponded
	 * @return Return the command context which the batch corresponded
	 */
	virtual IStudioAPICmdContext* GetCmdContext() const = 0;

	/**
	 * @brief Get command lists number in the batch
	 * @return Return command lists number in the batch
	 */
	virtual uint32 GetNumCmdLists() const = 0;

	/**
	 * @brief Get command lists in the batch
	 * @return Return command lists in the batch. If the one hasn't any command list return NULL
	 */
	virtual IStudioAPICmdList** GetCmdLists() const = 0;
};

#endif // !ISTUDIOAPI_CMDLIST_H