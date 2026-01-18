#pragma once
#include "studiorender/studioapi/istudioapi.h"
#include "studiorender/studioapi/istudioapi_cmdcontext.h"
#include "studiorender/studioapi/istudioapi_texture.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStudioAPIRenderPass;
class IStudioAPIFrameBuffer;
class IStudioAPIRenderPipeline;
class IStudioAPIBuffer;
class IStudioAPITexture;
class IStudioAPISampler;
class IStudioAPISwapChain;
struct studioAPIBarrier_t;

//-----------------------------------------------------------------------------
// StudioAPI synchronization with a swap chain flags
//-----------------------------------------------------------------------------
enum studioAPISyncSwapChainFlags_t
{
	STUDIOAPI_SYNC_SWAPCHAIN_FLAGS_ACQUIRE_NEXT_IMAGE = 1 << 0,	 // Set synchronization with acquire of a swap chain image
	STUDIOAPI_SYNC_SWAPCHAIN_FLAGS_PRESENT_TO_IMAGE	  = 1 << 1	 // Set synchronization to present into a swap chain image
};

//-----------------------------------------------------------------------------
// StudioAPI command list interface
//-----------------------------------------------------------------------------
class IStudioAPICmdList : public IStudioAPIResource
{
public:
	virtual ~IStudioAPICmdList() {}

	// Begin/end record commands into the list
	virtual void BeginRecord() = 0;
	virtual void EndRecord()   = 0;

	// Begin/end a render pass
	virtual void BeginRenderPass( IStudioAPIRenderPass* pRenderPass, IStudioAPIFrameBuffer* pFrameBuffer ) = 0;
	virtual void EndRenderPass()																		   = 0;

	virtual void SetRenderPipeline( IStudioAPIRenderPipeline* pRenderPipeline )						   = 0;
	virtual void SetVertexBuffer( uint32 slot, IStudioAPIBuffer* pVertexBuffer, uint64 offset )		   = 0;
	virtual void SetConstantBuffer( uint32 set, uint32 slot, IStudioAPIBuffer* pConstantBuffer )	   = 0;
	virtual void SetTexture( uint32 set, uint32 slot, IStudioAPITexture* pTexture )					   = 0;
	virtual void SetSampler( uint32 set, uint32 slot, IStudioAPISampler* pSampler )					   = 0;
	virtual void SetViewport( float minX, float minY, float minZ, float maxX, float maxY, float maxZ ) = 0;
	virtual void SetScissor( int32 x, int32 y, uint32 width, uint32 height )						   = 0;

	virtual void Draw( uint32 baseVertexIndex, uint32 numVertices, uint32 numInstances = 1 )														 = 0;
	virtual void DrawIndexed( IStudioAPIBuffer* pIndexBuffer, uint32 baseVertexIndex, uint32 baseIndex, uint32 numIndices, uint32 numInstances = 1 ) = 0;
	virtual void Barrier( const studioAPIBarrier_t* pBarriers, uint32 numBarriers, bool bFlush = false )											 = 0;
	virtual void CopyBuffer( IStudioAPIBuffer* pSrcBuffer, uint64 srcOffset, IStudioAPIBuffer* pDstBuffer, uint64 dstOffset, uint64 size )			 = 0;

	// NOTE: To copy textures must be in STUDIOAPI_TEXTURE_LAYOUT_TRANSFER_SRC and STUDIOAPI_TEXTURE_LAYOUT_TRANSFER_DST layout
	virtual void CopyBufferToTexture( IStudioAPIBuffer* pSrcBuffer, uint64 srcOffset, IStudioAPITexture* pDstTexture, uint32 dstMip, const ivec3_t& dstOffset, const ivec3_t& dstSize, uint32 dstStartLayer = 0, uint32 dstNumLayers = -1, uint32 srcRowLength = 0, uint32 srcTextureHeight = 0 ) = 0;
	virtual void CopyTextureToBuffer( IStudioAPITexture* pSrcTexture, uint32 srcMip, const ivec3_t& srcOffset, const ivec3_t& srcSize, IStudioAPIBuffer* pDstBuffer, uint64 dstOffset, uint32 srcStartLayer = 0, uint32 srcNumLayers = -1, uint32 dstRowLength = 0, uint32 dstTextureHeight = 0 ) = 0;
	virtual void CopyTexture( IStudioAPITexture* pSrcTexture, uint32 srcMip, const ivec3_t srcOffset, IStudioAPITexture* pDstTexture, uint32 dstMip, const ivec3_t dstOffset, const ivec3_t size, uint32 srcStartLayer = 0, uint32 dstStartLayer = 0, uint32 numLayers = 1 )					  = 0;

	virtual IStudioAPICmdContext* GetCmdContext() const = 0;
};

//-----------------------------------------------------------------------------
// StudioAPI command list batch interface
//-----------------------------------------------------------------------------
class IStudioAPICmdListBatch : public IStudioAPIResource
{
public:
	virtual ~IStudioAPICmdListBatch() {}

	// Add/remove a command list
	virtual void AddCmdList( IStudioAPICmdList* pCmdList )	  = 0;
	virtual void RemoveCmdList( IStudioAPICmdList* pCmdList ) = 0;
	virtual void RemoveAllCmdLists()						  = 0;

	// Set the synchronization signals
	// NOTE: Only one signal can be used per a frame, otherwise will be undefined behavior
	// NOTE: Only one sync with a swap chain can be used per a frame, otherwise will be undefined behavior (globally)
	virtual void SyncSignal( const char* pSyncName )							   = 0;
	virtual void SyncWait( const char* pSyncName )								   = 0;
	virtual void SyncSwapChain( IStudioAPISwapChain* pSwapChain, uint8 syncFlags ) = 0;

	virtual void RemoveSyncSignal( const char* pSyncName ) = 0;
	virtual void RemoveSyncWait( const char* pSyncName )	= 0;
	virtual void RemoveAllSyncSignals()						= 0;
	virtual void RemoveAllSyncWaits()						= 0;
	virtual void RemoveSyncSwapChain()						= 0;

	virtual IStudioAPICmdContext* GetCmdContext() const	 = 0;
	virtual uint32				  GetNumCmdLists() const = 0;
	virtual IStudioAPICmdList**	  GetCmdLists() const	 = 0;
};