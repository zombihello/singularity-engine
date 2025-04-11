/**
 * @file
 * @addtogroup studioapi studioapi
 */

#ifndef ISTUDIOAPI_BARRIER_H
#define ISTUDIOAPI_BARRIER_H

#include "studiorender/studioapi/istudioapi_cmdcontext.h"
#include "studiorender/studioapi/istudioapi_texture.h"
#include "studiorender/studioapi/istudioapi_buffer.h"

// Forward declarations
class IStudioAPIBuffer;
class IStudioAPISwapChainImage;

/**
 * @ingroup studioapi
 * @brief StudioAPI barrier type
 */
enum studioAPIBarrierType_t
{
	STUDIOAPI_BARRIER_TYPE_MEMORY,		/**< UAV accesses */
	STUDIOAPI_BARRIER_TYPE_TEXTURE,		/**< Texture layout transition */
	STUDIOAPI_BARRIER_TYPE_BUFFER		/**< Buffer state transition */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI memory barrier
 */
struct studioAPIMemoryBarrier_t
{
	IStudioAPIResource*			pResource;			/**< StudioAPI resource */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI texture barrier
 */
struct studioAPITextureBarrier_t
{
	union
	{
		IStudioAPITexture*			pTexture;			/**< StudioAPI texture, used only when bSwapChain is FALSE */
		IStudioAPISwapChainImage*	pImage;				/**< StudioAPI swap chain image, used only when bSwapChain is TRUE */
	};
	uint32							mip;				/**< Mipmap level */
	uint32							layer;				/**< Array layer */
	studioAPITextureLayout_t		layout;				/**< Destination texture layout */
	studioAPIQueueType_t			ownerQueueType;		/**< Destination queue type which will be have the ownership */
	bool							bSwapChain;			/**< Is the barrier for a swap chain image */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI buffer barrier
 */
struct studioAPIBufferBarrier_t
{
	IStudioAPIBuffer*			pBuffer;			/**< StudioAPI buffer */
	studioAPIBufferState_t		state;				/**< Destination buffer state */
	studioAPIQueueType_t		ownerQueueType;		/**< Destination queue type which will be have the ownership */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI barrier to pipeline synchronization
 */
struct studioAPIBarrier_t
{
	studioAPIBarrierType_t			type;		/**< Barrier type */
	union
	{
		studioAPIMemoryBarrier_t	memory;		/**< Memory barrier */
		studioAPITextureBarrier_t	texture;	/**< Texture barrier */
		studioAPIBufferBarrier_t	buffer;		/**< Buffer barrier */
	};
};

/**
 * @ingroup studioapi
 * @brief Make a StudioAPI memory barrier
 * @param pResource			StudioAPI resource
 * @return Return created a StudioAPI memory barrier
 */
FORCEINLINE studioAPIBarrier_t StudioAPI_MakeMemoryBarrier( IStudioAPIResource* pResource )
{
	studioAPIBarrier_t			barrier;
	barrier.type				= STUDIOAPI_BARRIER_TYPE_MEMORY;
	barrier.memory.pResource	= pResource;
	return barrier;
}

/**
 * @ingroup studioapi
 * @brief Make a StudioAPI buffer barrier
 * 
 * @param pBuffer			StudioAPI buffer
 * @param state				Destination buffer state
 * @param ownerQueueType	Destination queue type which will be have the ownership
 * @return Return created a StudioAPI buffer barrier
 */
FORCEINLINE studioAPIBarrier_t StudioAPI_MakeBufferBarrier( IStudioAPIBuffer* pBuffer, studioAPIBufferState_t state, studioAPIQueueType_t ownerQueueType )
{
	studioAPIBarrier_t				barrier;
	barrier.type					= STUDIOAPI_BARRIER_TYPE_BUFFER;
	barrier.buffer.pBuffer			= pBuffer;
	barrier.buffer.state			= state;
	barrier.buffer.ownerQueueType	= ownerQueueType;
	return barrier;
}

/**
 * @ingroup studioapi
 * @brief Make a StudioAPI texture barrier
 * 
 * @param pTexture			StudioAPI texture
 * @param layout			Destination texture layout
 * @param ownerQueueType	Destination queue type which will be have the ownership
 * @param mip				Mipmap level. If it is -1 will be used all mipmap levels
 * @param layer				Array layer. If it is -1 will be used all array layers
 * @return Return created a StudioAPI texture barrier
 */
FORCEINLINE studioAPIBarrier_t StudioAPI_MakeTextureBarrier( IStudioAPITexture* pTexture, studioAPITextureLayout_t layout, studioAPIQueueType_t ownerQueueType, uint32 mip = -1, uint32 layer = -1 )
{
	studioAPIBarrier_t				barrier;
	barrier.type					= STUDIOAPI_BARRIER_TYPE_TEXTURE;
	barrier.texture.pTexture		= pTexture;
	barrier.texture.mip				= mip;
	barrier.texture.layer			= layer;
	barrier.texture.layout			= layout;
	barrier.texture.ownerQueueType	= ownerQueueType;
	barrier.texture.bSwapChain		= false;
	return barrier;
}

/**
 * @ingroup studioapi
 * @brief Make a StudioAPI texture barrier
 * 
 * @param pImage			StudioAPI swap chain image
 * @param layout			Destination texture layout
 * @param ownerQueueType	Destination queue type which will be have the ownership
 * @return Return created a StudioAPI swap chain image barrier
 */
FORCEINLINE studioAPIBarrier_t StudioAPI_MakeTextureBarrier( IStudioAPISwapChainImage* pImage, studioAPITextureLayout_t layout, studioAPIQueueType_t ownerQueueType )
{
	studioAPIBarrier_t				barrier;
	barrier.type					= STUDIOAPI_BARRIER_TYPE_TEXTURE;
	barrier.texture.pImage			= pImage;
	barrier.texture.mip				= -1;
	barrier.texture.layer			= -1;
	barrier.texture.layout			= layout;
	barrier.texture.ownerQueueType	= ownerQueueType;
	barrier.texture.bSwapChain		= true;
	return barrier;
}

#endif // !ISTUDIOAPI_BARRIER_H