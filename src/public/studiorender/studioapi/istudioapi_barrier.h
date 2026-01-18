#pragma once
#include "studiorender/studioapi/istudioapi_cmdcontext.h"
#include "studiorender/studioapi/istudioapi_texture.h"
#include "studiorender/studioapi/istudioapi_buffer.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStudioAPIBuffer;
class IStudioAPISwapChainImage;

//-----------------------------------------------------------------------------
// Barrier types
//-----------------------------------------------------------------------------
enum studioAPIBarrierType_t
{
	STUDIOAPI_BARRIER_TYPE_MEMORY,	 // UAV accesses
	STUDIOAPI_BARRIER_TYPE_TEXTURE,	 // Texture layout transition
	STUDIOAPI_BARRIER_TYPE_BUFFER	 // Buffer state transition
};

//-----------------------------------------------------------------------------
// Memory barrier
//-----------------------------------------------------------------------------
struct studioAPIMemoryBarrier_t
{
	IStudioAPIResource* pResource;
};

//-----------------------------------------------------------------------------
// Texture barrier
//-----------------------------------------------------------------------------
struct studioAPITextureBarrier_t
{
	union
	{
		IStudioAPITexture*		  pTexture;	 // StudioAPI texture, used only when bSwapChain is FALSE
		IStudioAPISwapChainImage* pImage;	 // StudioAPI swap chain image, used only when bSwapChain is TRUE
	};
	uint32					 mip;
	uint32					 layer;
	studioAPITextureLayout_t layout;
	studioAPIQueueType_t	 ownerQueueType;
	bool					 bSwapChain;  // Is the barrier for a swap chain image
};

//-----------------------------------------------------------------------------
// Buffer barrier
//-----------------------------------------------------------------------------
struct studioAPIBufferBarrier_t
{
	IStudioAPIBuffer*	   pBuffer;
	studioAPIBufferState_t state;
	studioAPIQueueType_t   ownerQueueType;
};

//-----------------------------------------------------------------------------
// StudioAPI barrier to pipeline synchronization
//-----------------------------------------------------------------------------
struct studioAPIBarrier_t
{
	studioAPIBarrierType_t type;
	union
	{
		studioAPIMemoryBarrier_t  memory;
		studioAPITextureBarrier_t texture;
		studioAPIBufferBarrier_t  buffer;
	};
};

//-----------------------------------------------------------------------------
// Functions to make a StudioAPI barrier
//-----------------------------------------------------------------------------
studioAPIBarrier_t StudioAPI_MakeMemoryBarrier( IStudioAPIResource* pResource );
studioAPIBarrier_t StudioAPI_MakeBufferBarrier( IStudioAPIBuffer* pBuffer, studioAPIBufferState_t state, studioAPIQueueType_t ownerQueueType );
studioAPIBarrier_t StudioAPI_MakeTextureBarrier( IStudioAPITexture* pTexture, studioAPITextureLayout_t layout, studioAPIQueueType_t ownerQueueType, uint32 mip = -1, uint32 layer = -1 );
studioAPIBarrier_t StudioAPI_MakeTextureBarrier( IStudioAPISwapChainImage* pImage, studioAPITextureLayout_t layout, studioAPIQueueType_t ownerQueueType );

#include "studiorender/studioapi/istudioapi_barrier.inl"