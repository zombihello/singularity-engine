/**
 * @file
 * @addtogroup studiorender studiorender
 */

#ifndef STUDIO_VIEWPORT_H
#define STUDIO_VIEWPORT_H

#include "stdlib/delegate.h"
#include "studiorender/istudio_viewport.h"
#include "studiorender/studio_renderthread.h"
#include "studiorender/istudio_renderresource.h"

/**
 * @ingroup studiorender
 * @brief Studio viewport
 */
class CStudioViewport : public TRefCounted<IStudioViewport>, public TStudioRenderResource<IStudioRenderResource>
{
public:
	/**
	 * @brief Event called when a viewport index is released 
	 */
	DECLARE_MULTICAST_DELEGATE( COnReleaseViewportIndex, CStudioViewport* /* pViewport */ );
	
	/**
	 * @brief Event called when viewport's render pass is updated (called in the render thread)
	 */
	DECLARE_MULTICAST_DELEGATE( COnRenderPassUpdated, CStudioViewport* /* pViewport */ );

	/**
	 * @brief Constructor
	 */
	CStudioViewport();

	/**
	 * @brief Destructor
	 */
	~CStudioViewport();

	/**
	 * @brief Initialize viewport
	 * @param windowHandle		Window handle where will be created the viewport
	 * @param width				Viewport width
	 * @param height			Viewport height
	 * @param bUseVSync			Is it necessary to use VSync
	 */
	virtual void Init( windowHandle_t windowHandle, uint32 width, uint32 height, bool bUseVSync = false ) override;

	/**
	 * @brief Shutdown viewport
	 */
	virtual void Shutdown() override;

	/**
	 * @brief Resize viewport
	 * @param newWidth		New viewport width
	 * @param newHeight		New viewport height
	 */
	virtual void Resize( uint32 newWidth, uint32 newHeight ) override;
	
	/**
	 * @brief Update viewport logic
	 * @param deltaSeconds		Delta time since the last frame (in seconds)
	 */
	virtual void Update( float deltaSeconds ) override;

	/**
	 * @brief Draw frame
	 * @param bShouldPresent	Is need present of the frame
	 * @param bFlushRenderCmds	Is need flush render commands
	 */
	virtual void DrawFrame( bool bShouldPresent = true, bool bFlushRenderCmds = true ) override;

	/**
	 * @brief Is initialized viewport
	 * @return Return TRUE if the viewport is initialized, otherwise FALSE
	 */
	virtual bool IsInited() const override;

	/**
	 * @brief Set viewport client
	 * @param pStudioViewportClient		A new viewport client
	 */
	virtual void SetViewportClient( IStudioViewportClient* pStudioViewportClient ) override;

	/**
	 * @brief Get StudioAPI swap chain
	 * @return Return StudioAPI swap chain. If the viewport isn't initialized return NULL
	 */
	virtual IStudioAPISwapChain* GetStudioAPISwapChain() const override;

	/**
	 * @brief Get viewport client
	 * @return Return viewport client. If the viewport have not return NULL 
	 */
	virtual IStudioViewportClient* GetViewportClient() const override;

	/**
	 * @brief Get StudioAPI render pass
	 * The function returns a StudioAPI render pass for into the viewport
	 * @return Return StudioAPI render pass. If the viewport isn't initialized return NULL
	 */
	virtual IStudioAPIRenderPass* GetStudioAPIRenderPass() const override;

	/**
	 * @brief Get StudioAPI frame buffer
	 * The function returns a StudioAPI frame buffers for render into the viewport (valid only per a frame, because the viewport can use double/triple/etc buffering)
	 * @return Return StudioAPI frame buffer. If the viewport isn't initialized return NULL
	 */
	virtual IStudioAPIFrameBuffer* GetStudioAPIFrameBuffer() const override;

	/**
	 * @brief Get viewport size
	 * @return Return viewport size
	 */
	virtual ivec2_t GetSize() const override;

	/**
	 * @brief Is use VSync
	 * @return Return TRUE if the viewport using VSync, otherwise FALSE
	 */
	virtual bool IsUseVSync() const override;

	/**
	 * @brief Get the active viewport for rendering into one
	 * @warning This is only called by the render thread
	 * @return Return the active viewport, if isn't return NULL
	 */
	static FORCEINLINE CStudioViewport* R_GetActiveViewport()
	{
		Assert( Studio_IsInRenderThread() );
		return s_pActiveViewport;
	}

	/**
	 * @breif Get viewport index
	 * @return Return viewport index
	 */
	FORCEINLINE uint32 GetIndex() const
	{
		return index;
	}

	/**
	 * @brief Get event manager of release a viewport index
	 * @return Return event manager of release a viewport index
	 */
	FORCEINLINE COnReleaseViewportIndex& OnReleaseViewportIndex() const
	{
		return onReleaseViewportIndex;
	}

	/**
	 * @brief Get event manager of the viewport render pass is updated
	 * @return Return event manager of the viewport render pass is updated
	 */
	FORCEINLINE COnRenderPassUpdated& OnRenderPassUpdated() const
	{
		return onRenderPassUpdated;
	}

private:
	/**
	 * @brief Part of DrawFrame() function that executes at the render threade
	 * @warning This is only called by the render thread
	 *
	 * @param bShouldPresent	Is need present of the frame
	 */
	void R_DrawFrame( bool bShouldPresent );

	/**
	 * @brief Delegate called when the swap chain has been re-created
	 * @warning This is only called by the render thread
	 * @param pUserData				Pointer to CStudioViewport
	 * @param pStudioAPISwapChain	StudioAPI swap chain
	 * @param bChangedImageFormat	Is changed image format of the swap chain
	 */
	static void OnSwapChainReCreated( void* pUserData, IStudioAPISwapChain* pStudioAPISwapChain, bool bChangedImageFormat );

	/**
	 * @brief Initializes the StudioAPI resources used by this resource
	 * @warning This is only called by the render thread
	 *
	 * Called when the resource is initialized
	 */
	virtual void InitStudioAPI() override;

	/**
	 * @brief If the resource's StudioAPI has been initialized, then release and reinitialize it. Otherwise, do nothing
	 * @warning This is only called by the render thread
	 */
	virtual void UpdateStudioAPI() override;

	/**
	 * @brief Releases the StudioAPI resources used by this resource
	 * @warning This is only called by the render thread
	 *
	 * Called when the resource is released
	 */
	virtual void ReleaseStudioAPI() override;

	/**
	 * @brief Acquire a viewport index
	 * @return Return acquired a viewport index
	 */
	FORCEINLINE uint32 AcquireIndex()
	{
		CScopeLock		scopeLock( s_ViewportIndexMutex );
		if ( s_FreeViewportIndices.empty() )
		{
			++s_LastViewportIndex;
			return s_LastViewportIndex;
		}

		uint32		freeViewportIndex = s_FreeViewportIndices.back();
		s_FreeViewportIndices.pop_back();
		return freeViewportIndex;
	}

	/**
	 * @brief Release the viewport index
	 * @param viewportIndex		Viewport index to release
	 */
	FORCEINLINE void ReleaseIndex( uint32 viewportIndex )
	{
		CScopeLock		scopeLock( s_ViewportIndexMutex );
		s_FreeViewportIndices.emplace_back( viewportIndex );
		onReleaseViewportIndex.Broadcast( this );
	}

	bool															bUseVSync;								/**< Is it necessary to use VSync */
	uint32															index;									/**< Viewport index */
	windowHandle_t													windowHandle;							/**< Window handle of the viewport */
	IStudioViewportClient*											pStudioViewportClient;					/**< Viewport client */
	ivec2_t															size;									/**< Viewport size */
	TRefPtr<IStudioAPISwapChain>									pStudioAPISwapChain;					/**< StudioAPI swap chain */
	TRefPtr<IStudioAPIRenderPass>									pStudioAPIRenderPass;					/**< StudioAPI render pass for render into a swap chain image of the swap chain */
	IStudioAPISwapChain::IOnReCreated::funcDelegate_t*				pSwapChainReCreatedDelegate;			/**< Delegate handle 'on the swap chain render pass has been re-created' */
	mutable COnReleaseViewportIndex									onReleaseViewportIndex;					/** Event called when the viewport index is released */
	mutable COnRenderPassUpdated									onRenderPassUpdated;					/** Event called when the viewport render pass is updated */
	std::vector<TRefPtr<IStudioAPIFrameBuffer>>						studioAPIFrameBuffers;					/**< StudioAPI frame buffers for each swap chain image of the swap chain */
	static TRefPtr<CStudioViewport>									s_pActiveViewport;						/**< The active viewport */
	static CThreadMutex												s_ViewportIndexMutex;					/**< Mutex to acquire and release a viewport index */
	static uint32													s_LastViewportIndex;					/**< The last viewport index */
	static std::list<uint32>										s_FreeViewportIndices;					/**< Free viewport indices list */
};

#endif // !STUDIO_VIEWPORT_H