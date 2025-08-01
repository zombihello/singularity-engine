#pragma once

/*
==================
StudioAPI_MakeMemoryBarrier
==================
*/
FORCEINLINE studioAPIBarrier_t StudioAPI_MakeMemoryBarrier( IStudioAPIResource* pResource )
{
	studioAPIBarrier_t			barrier = {};
	barrier.type				= STUDIOAPI_BARRIER_TYPE_MEMORY;
	barrier.memory.pResource	= pResource;
	return barrier;
}

/*
==================
StudioAPI_MakeBufferBarrier
==================
*/
FORCEINLINE studioAPIBarrier_t StudioAPI_MakeBufferBarrier( IStudioAPIBuffer* pBuffer, studioAPIBufferState_t state, studioAPIQueueType_t ownerQueueType )
{
	studioAPIBarrier_t				barrier = {};
	barrier.type					= STUDIOAPI_BARRIER_TYPE_BUFFER;
	barrier.buffer.pBuffer			= pBuffer;
	barrier.buffer.state			= state;
	barrier.buffer.ownerQueueType	= ownerQueueType;
	return barrier;
}

/*
==================
StudioAPI_MakeTextureBarrier
==================
*/
FORCEINLINE studioAPIBarrier_t StudioAPI_MakeTextureBarrier( IStudioAPITexture* pTexture, studioAPITextureLayout_t layout, studioAPIQueueType_t ownerQueueType, uint32 mip /*= -1*/, uint32 layer /*= -1*/ )
{
	studioAPIBarrier_t				barrier = {};
	barrier.type					= STUDIOAPI_BARRIER_TYPE_TEXTURE;
	barrier.texture.pTexture		= pTexture;
	barrier.texture.mip				= mip;
	barrier.texture.layer			= layer;
	barrier.texture.layout			= layout;
	barrier.texture.ownerQueueType	= ownerQueueType;
	barrier.texture.bSwapChain		= false;
	return barrier;
}

/*
==================
StudioAPI_MakeTextureBarrier
==================
*/
FORCEINLINE studioAPIBarrier_t StudioAPI_MakeTextureBarrier( IStudioAPISwapChainImage* pImage, studioAPITextureLayout_t layout, studioAPIQueueType_t ownerQueueType )
{
	studioAPIBarrier_t				barrier = {};
	barrier.type					= STUDIOAPI_BARRIER_TYPE_TEXTURE;
	barrier.texture.pImage			= pImage;
	barrier.texture.mip				= -1;
	barrier.texture.layer			= -1;
	barrier.texture.layout			= layout;
	barrier.texture.ownerQueueType	= ownerQueueType;
	barrier.texture.bSwapChain		= true;
	return barrier;
}