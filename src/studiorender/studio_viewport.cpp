#include "pch_studiorender.h"
#include "studiorender/studio_viewport.h"
#include "studiorender/istudio_rendercmd.h"
#include "studiorender/studiorender.h"

// Static fields
TRefPtr<CStudioViewport> CStudioViewport::s_pActiveViewport;
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
	, pSwapChainReCreatedDelegate( NULL )
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
CStudioViewport::InitStudioAPI
==================
*/
void CStudioViewport::InitStudioAPI()
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// Create a new swap chain
	if ( windowHandle )
	{
		pStudioAPISwapChain = g_pStudioAPI->CreateSwapChain( windowHandle, size.x, size.y, bUseVSync );
	}

	// Subscribe on a swap chain event
	if ( pStudioAPISwapChain )
	{
		pSwapChainReCreatedDelegate = pStudioAPISwapChain->OnReCreated()->AddFunc( &CStudioViewport::OnSwapChainReCreated, this );
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
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// Unsubscribe from a swap chain event
	if ( pStudioAPISwapChain )
	{
		pStudioAPISwapChain->OnReCreated()->RemoveFunc( pSwapChainReCreatedDelegate );
		pSwapChainReCreatedDelegate = NULL;
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
	PROFILE_SCOPE();

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
		studioAPIFrameBufferCreateInfo.clearColor						= CColor::black;
		pStudioViewport->studioAPIFrameBuffers[swapChainImageIdx]		= g_pStudioAPI->CreateFrameBuffer( studioAPIFrameBufferCreateInfo, "SwapChain FrameBuffer" );
	}

	// If a render pass was re-created broadcast the event
	if ( bChangedImageFormat )
	{
		pStudioViewport->onRenderPassUpdated.Broadcast( pStudioViewport );
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
	size						  = ivec2_t( width, height );

	// Begin the StudioAPI resource if it need
	if ( bUpdateStudioAPISwapChain )
	{
		Studio_BeginUpdateResourceSafe<CStudioViewport>( this );
	}
}

/*
==================
CStudioViewport::Shutdown
==================
*/
void CStudioViewport::Shutdown()
{
	// If we have valid StudioAPI resource destroy they
	if ( pStudioAPISwapChain )
	{
		Studio_BeginReleaseResourceSafe<CStudioViewport>( this );
	}
}

/*
==================
CStudioViewport::Resize
==================
*/
void CStudioViewport::Resize( uint32 newWidth, uint32 newHeight )
{
	// Resize the viewport if only it is valid and width or height isn't munch
	if ( IsInited() && ( size.x != newWidth || size.y != newHeight ) )
	{
		// Save the new viewport size and update StudioAPI resources
		size = ivec2_t( newWidth, newHeight );
		Studio_BeginUpdateResourceSafe<CStudioViewport>( this );
	}
}

/*
==================
CStudioViewport::Update
==================
*/
void CStudioViewport::Update( float deltaSeconds )
{
	PROFILE_SCOPE();

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
void CStudioViewport::DrawFrame( bool bShouldPresent /* = true */, bool bFlushRenderCmds /* = true */ )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// Do nothing if the studio resource isn't yet initialized or isn't valid
	if ( !IsInitedResource() || !IsInited() )
	{
		return;
	}

	// Submit of geometries to the studio render
	g_StudioRender.BeginFrame();
	if ( pStudioViewportClient )
	{
		pStudioViewportClient->DrawFrame( this );
	}
	g_StudioRender.EndFrame();

	// Draw the viewport
	if ( bFlushRenderCmds )
	{
		Studio_FlushRenderCommands();
	}

	UNIQUE_RENDER_COMMAND_TWOPARAMETER( CStudioRenderCmd_DrawFrame,
										CStudioViewport*, pStudioViewport, this,
										bool, bShouldPresent, bShouldPresent,
										{
											pStudioViewport->R_DrawFrame( bShouldPresent );
										} );
}

/*
==================
CStudioViewport::R_DrawFrame
==================
*/
void CStudioViewport::R_DrawFrame( bool bShouldPresent )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// Don't draw a new frame if the swap chain is invalid
	if ( !pStudioAPISwapChain->IsValid() )
	{
		Warning( "StudioRender: Couldn't to begin drawing a new frame due to the swap chain is invalid" );
		return;
	}

	// Begin draw a new frame
	g_pStudioAPI->BeginDrawingFrame();

	// Acquire the next swap chain image and recreate it if need
	if ( !pStudioAPISwapChain->AcquireNextImage() )
	{
		if ( !pStudioAPISwapChain->ReCreate() )
		{
			Warning( "StudioRender: Failed to re-create the swap chain" );
			return;
		}

		// Retry to acquire the swap chain image
		pStudioAPISwapChain->AcquireNextImage();
	}

	// Draw the frame
	AssertMsg( !s_pActiveViewport, "Only into one viewport we can draw in one time" );
	s_pActiveViewport = this;
	if ( pStudioViewportClient )
	{
		pStudioViewportClient->R_BeginDrawFrame( this );
	}
	g_StudioRender.R_DrawFrame( this );
	if ( pStudioViewportClient )
	{
		pStudioViewportClient->R_EndDrawFrame( this );
	}
	s_pActiveViewport = NULL;

	// Present the frame and re-create it if the one is out of date
	if ( bShouldPresent && !pStudioAPISwapChain->Present() )
	{
		pStudioAPISwapChain->ReCreate();
		Warning( "StudioRender: Present of the swap chain back buffer have been skipped, because the swap chain was out of date" );
	}

	// End draw the frame
	g_pStudioAPI->EndDrawingFrame();
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
ivec2_t CStudioViewport::GetSize() const
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
