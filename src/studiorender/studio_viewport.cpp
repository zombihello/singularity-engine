#include "pch_studiorender.h"
#include "studiorender/studio_viewport.h"
#include "studiorender/istudio_rendercmd.h"
#include "studiorender/studiorender.h"

// Static fields
CRefPtr<CStudioViewport> CStudioViewport::s_pActiveViewport;
CThreadMutex			 CStudioViewport::s_ViewportIndexMutex;
uint32					 CStudioViewport::s_LastViewportIndex = (uint32)-1;
eastl::list<uint32>		 CStudioViewport::s_FreeViewportIndices;

/*
==================
CStudioViewport::CStudioViewport
==================
*/
CStudioViewport::CStudioViewport()
	: bUseVSync( false )
	, index( AcquireIndex() )
	, windowHandle( INVALID_WINDOW_HANDLE )
	, pStudioViewportClient( NULL )
	, size( 0.f, 0.f )
	, onSwapChainReCreatedHandle( INVALID_HANDLE )
{
}

/*
==================
CStudioViewport::~CStudioViewport
==================
*/
CStudioViewport::~CStudioViewport()
{
	ReleaseIndex( index );
}

/*
==================
CStudioViewport::FinalRelease
==================
*/
void CStudioViewport::FinalRelease()
{
	if ( IsNeedDeferredDestroy() )
	{
		Studio_BeginDeleteResource( this );
	}
	else
	{
		delete this;
	}
}

/*
==================
CStudioViewport::InitStudioAPI
==================
*/
void CStudioViewport::InitStudioAPI()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Create a new swap chain
	if ( windowHandle )
	{
		pStudioAPISwapChain = g_pStudioAPI->CreateSwapChain( windowHandle, size.x, size.y, bUseVSync );
	}

	// Subscribe on a swap chain event
	if ( pStudioAPISwapChain )
	{
		onSwapChainReCreatedHandle = pStudioAPISwapChain->OnReCreated()->Subscribe( &CStudioViewport::OnSwapChainReCreated, this );
	}

	// Trigger the delegate to create a render pass and frame buffers for each swap chain image
	OnSwapChainReCreated( this, pStudioAPISwapChain, true );
}

/*
==================
CStudioViewport::ReleaseStudioAPI
==================
*/
void CStudioViewport::ReleaseStudioAPI()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Unsubscribe from a swap chain event
	if ( pStudioAPISwapChain && onSwapChainReCreatedHandle != INVALID_HANDLE )
	{
		pStudioAPISwapChain->OnReCreated()->Unsubscribe( onSwapChainReCreatedHandle );
		onSwapChainReCreatedHandle = INVALID_HANDLE;
	}

	// Release StudioAPI resources
	studioAPIFrameBuffers.clear();
	pStudioAPIRenderPass = NULL;
	pStudioAPISwapChain	 = NULL;
}

/*
==================
CStudioViewport::UpdateStudioAPI
==================
*/
void CStudioViewport::UpdateStudioAPI()
{
	PROFILER_SCOPE_FUNC();

	// Re-create the swap chain if we have another the window handle or bUseVSync flag
	if ( pStudioAPISwapChain->GetWindowHandle() != windowHandle || bUseVSync != pStudioAPISwapChain->IsUseVSync() )
	{
		ReleaseStudioAPI();
		InitStudioAPI();
		return;
	}

	// Otherwise resize one only
	pStudioAPISwapChain->Resize( size.x, size.y );
}

/*
==================
CStudioViewport::OnSwapChainRenderPassUpdated
==================
*/
void CStudioViewport::OnSwapChainReCreated( void* pUserData, IStudioAPISwapChain* pStudioAPISwapChain, bool bChangedImageFormat )
{
	// Create a new render pass if swap chain image format has been changed
	CStudioViewport* pStudioViewport = (CStudioViewport*)pUserData;
	Assert( pStudioViewport );
	if ( bChangedImageFormat )
	{
		studioAPIRenderPassCreateInfo_t	  studioAPIRenderPassCreateInfo	 = {};
		studioAPIColorRenderTargetInfo_t& studioAPIColorRenderTargetInfo = studioAPIRenderPassCreateInfo.colorRenderTargets[0];
		studioAPIColorRenderTargetInfo.pSwapChain						 = pStudioAPISwapChain;
		studioAPIColorRenderTargetInfo.bSwapChain						 = true;
		studioAPIColorRenderTargetInfo.loadOp							 = STUDIOAPI_RENDER_TARGET_LOAD_OP_CLEAR;
		studioAPIColorRenderTargetInfo.storeOp							 = STUDIOAPI_RENDER_TARGET_STORE_OP_STORE;
		pStudioViewport->pStudioAPIRenderPass							 = g_pStudioAPI->CreateRenderPass( studioAPIRenderPassCreateInfo, "SwapChain RenderPass" );
	}

	// Create new frame buffers
	pStudioViewport->studioAPIFrameBuffers.resize( pStudioAPISwapChain->GetNumImages() );
	for ( uint32 swapChainImageIdx = 0, numSwapChainImages = pStudioAPISwapChain->GetNumImages(); swapChainImageIdx < numSwapChainImages; ++swapChainImageIdx )
	{
		studioAPIFrameBufferCreateInfo_t studioAPIFrameBufferCreateInfo = {};
		studioAPIRenderTarget_t&		 studioAPIRenderTarget			= studioAPIFrameBufferCreateInfo.colorRenderTargets[0];
		studioAPIFrameBufferCreateInfo.pRenderPass						= pStudioViewport->pStudioAPIRenderPass;
		studioAPIRenderTarget.pSwapChainImage							= pStudioAPISwapChain->GetImage( swapChainImageIdx );
		studioAPIRenderTarget.flags										= STUDIOAPI_RENDER_TARGET_FLAG_SWAPCHAIN_IMAGE;
		studioAPIFrameBufferCreateInfo.size								= pStudioAPISwapChain->GetSize();
		studioAPIFrameBufferCreateInfo.bClearColor						= true;
		studioAPIFrameBufferCreateInfo.clearColor						= CLinearColor::Make( 0.f, 0.f, 0.f );
		pStudioViewport->studioAPIFrameBuffers[swapChainImageIdx]		= g_pStudioAPI->CreateFrameBuffer( studioAPIFrameBufferCreateInfo, "SwapChain FrameBuffer" );
	}

	// If a render pass was re-created broadcast the event
	if ( bChangedImageFormat )
	{
		pStudioViewport->onRenderPassUpdated.Invoke( pStudioViewport );
	}
}

/*
==================
CStudioViewport::Init
==================
*/
void CStudioViewport::Init( windowHandle_t windowHandle, uint32 width, uint32 height, bool bUseVSync /* = false */ )
{
	AssertMsg( windowHandle != INVALID_WINDOW_HANDLE, "StudioRender: Window handle must be valid" );
	bool bUpdateStudioAPISwapChain = CStudioViewport::windowHandle != windowHandle || size.x != width || size.y != height || CStudioViewport::bUseVSync != bUseVSync;

	// Update the viewport attributes
	CStudioViewport::bUseVSync	  = bUseVSync;
	CStudioViewport::windowHandle = windowHandle;
	size						  = vector2i_t( width, height );

	// Begin the StudioAPI resource if it need
	if ( bUpdateStudioAPISwapChain )
	{
		Studio_BeginUpdateResource( this );
	}
}

/*
==================
CStudioViewport::Shutdown
==================
*/
void CStudioViewport::Shutdown()
{
	windowHandle = INVALID_WINDOW_HANDLE;
	size		 = vector2i_t( 0, 0 );
	Studio_BeginReleaseResource( this );
}

/*
==================
CStudioViewport::Resize
==================
*/
void CStudioViewport::Resize( uint32 newWidth, uint32 newHeight )
{
	// Resize the viewport if only it is valid and width or height isn't munch
	if ( windowHandle != INVALID_WINDOW_HANDLE && ( size.x != newWidth || size.y != newHeight ) )
	{
		// Save the new viewport size and update StudioAPI resources
		size = vector2i_t( newWidth, newHeight );
		Studio_BeginUpdateResource( this );
	}
}

/*
==================
CStudioViewport::Update
==================
*/
void CStudioViewport::Update( float deltaSeconds )
{
	PROFILER_SCOPE_FUNC();

	// Update logic of the viewport client
	if ( pStudioViewportClient )
	{
		pStudioViewportClient->Update( deltaSeconds );
	}
}

/*
==================
CStudioViewport::DrawFrame
==================
*/
void CStudioViewport::DrawFrame( bool bShouldPresent /* = true */ )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Do nothing if the studio resource isn't yet initialized or isn't valid
	if ( !IsInitedResource() || !IsInited() )
	{
		return;
	}

	// Acquire the next swap chain image on the render thread
	UNIQUE_RENDER_COMMAND_TWOPARAMETER( CStudioViewportCmd_AcquireNextImage,
										CStudioViewport*, pStudioViewport, this,
										bool, bShouldPresent, bShouldPresent,
										{
											pStudioViewport->R_AcquireNextImage( bShouldPresent );
										} );

	// Draw a frame by the client
	if ( pStudioViewportClient )
	{
		pStudioViewportClient->DrawFrame( this );
	}

	// Present the frame on the render thread
	UNIQUE_RENDER_COMMAND_TWOPARAMETER( CStudioViewportCmd_Present,
										CStudioViewport*, pStudioViewport, this,
										bool, bShouldPresent, bShouldPresent,
										{
											pStudioViewport->R_Present( bShouldPresent );
										} );
}

/*
==================
CStudioViewport::R_AcquireNextImage
==================
*/
void CStudioViewport::R_AcquireNextImage( bool bShouldPresent )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	AssertMsg( !s_pActiveViewport, "Only into one viewport we can draw in one time" );
	s_pActiveViewport = this;

	// Don't acquire the next image if we shouldn't present it
	if ( !bShouldPresent )
	{
		return;
	}
	Assert( pStudioAPISwapChain->GetStatus() != STUDIOAPI_SWAPCHAIN_STATUS_NOT_CREATED );

	// Re-create the swap chain if it hasn't OK status
	if ( pStudioAPISwapChain->GetStatus() != STUDIOAPI_SWAPCHAIN_STATUS_OK && !pStudioAPISwapChain->ReCreate() )
	{
		studioAPISwapChainStatus_t status = pStudioAPISwapChain->GetStatus();
		DevWarning( "StudioRender: Failed to re-create the swap chain (%p). Frame: %i, Swap chain status: 0x%X", pStudioAPISwapChain, g_pStudioAPI->GetFrameNumber(), status );
		if ( status == STUDIOAPI_SWAPCHAIN_STATUS_OUT_OF_DATE )
		{
			return;
		}
	}

	// Acquire the next swap chain image
	if ( !pStudioAPISwapChain->AcquireNextImage() )
	{
		DevWarning( "StudioRender: Failed to acquire the next image. Frame: %i, Swap chain status: 0x%X", g_pStudioAPI->GetFrameNumber(), pStudioAPISwapChain->GetStatus() );
	}
}

/*
==================
CStudioViewport::R_Present
==================
*/
void CStudioViewport::R_Present( bool bShouldPresent )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	s_pActiveViewport = NULL;

	// Don't present the frame if we shouldn't do it
	if ( !bShouldPresent )
	{
		return;
	}
	Assert( pStudioAPISwapChain->GetStatus() != STUDIOAPI_SWAPCHAIN_STATUS_NOT_CREATED );

	// Present the frame
	if ( !pStudioAPISwapChain->Present() )
	{
		DevWarning( "StudioRender: Present the frame was skipped. Frame: %i, Swap chain status: 0x%X", g_pStudioAPI->GetFrameNumber(), pStudioAPISwapChain->GetStatus() );
	}
}

/*
==================
CStudioViewport::IsInited
==================
*/
bool CStudioViewport::IsInited() const
{
	return pStudioAPISwapChain.IsValid();
}

/*
==================
CStudioViewport::SetViewportClient
==================
*/
void CStudioViewport::SetViewportClient( IStudioViewportClient* pStudioViewportClient )
{
	CStudioViewport::pStudioViewportClient = pStudioViewportClient;
}

/*
==================
CStudioViewport::GetViewportClient
==================
*/
IStudioViewportClient* CStudioViewport::GetViewportClient() const
{
	return pStudioViewportClient;
}

/*
==================
CStudioViewport::GetSize
==================
*/
vector2i_t CStudioViewport::GetSize() const
{
	return size;
}

/*
==================
CStudioViewport::GetStudioAPISwapChain
==================
*/
IStudioAPISwapChain* CStudioViewport::GetStudioAPISwapChain() const
{
	return pStudioAPISwapChain;
}

/*
==================
CStudioViewport::GetStudioAPIRenderPass
==================
*/
IStudioAPIRenderPass* CStudioViewport::GetStudioAPIRenderPass() const
{
	return pStudioAPIRenderPass;
}

/*
==================
CStudioViewport::GetStudioAPIFrameBuffer
==================
*/
IStudioAPIFrameBuffer* CStudioViewport::GetStudioAPIFrameBuffer() const
{
	return pStudioAPISwapChain ? studioAPIFrameBuffers[pStudioAPISwapChain->GetCurrentImageIndex()] : NULL;
}

/*
==================
CStudioViewport::IsUseVSync
==================
*/
bool CStudioViewport::IsUseVSync() const
{
	return bUseVSync;
}
