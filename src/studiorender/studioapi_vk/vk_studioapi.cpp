#include "pch_studioapi_vk.h"
#include "tier0/icommandline.h"
#include "tier1/buildnum.h"
#include "tier0/version.h"
#include "studiorender/studioapi_vk/vk_studioapi_swapchain.h"
#include "studiorender/studioapi_vk/vk_studioapi.h"
#include "studiorender/studioapi_vk/vk_studioapi_cmdcontext.h"
#include "studiorender/studioapi_vk/vk_studioapi_shader.h"
#include "studiorender/studioapi_vk/vk_studioapi_renderpipeline.h"
#include "studiorender/studioapi_vk/vk_studioapi_framebuffer.h"
#include "studiorender/studioapi_vk/vk_studioapi_renderpass.h"
#include "studiorender/studioapi_vk/vk_studioapi_buffer.h"
#include "studiorender/studioapi_vk/vk_studioapi_descriptorpoolsmgr.h"
#include "studiorender/studioapi_vk/vk_studioapi_cmdlist.h"
#include "studiorender/studioapi_vk/vk_studioapi_texture.h"

CStudioAPIVk g_StudioAPIVk;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CStudioAPIVk, IStudioAPI, STUDIOAPI_INTERFACE_VERSION, g_StudioAPIVk );

/*
==================
CStudioAPIVk::CStudioAPIVk
==================
*/
CStudioAPIVk::CStudioAPIVk()
	: frameNumber( 0 )
	, currentFrameInFlight( 0 )
	, pGraphicsCmdContext( NULL )
	, pTransferCmdContext( NULL )
	, pComputeCmdContext( NULL )
{
	Mem_Memzero( &info, sizeof( studioAPIInfo_t ) );
}

/*
==================
CStudioAPIVk::Connect
==================
*/
bool CStudioAPIVk::Connect( createInterfaceFn_t pFactory )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Connect Tier1 and register cvars
	if ( !ConnectTier1( pFactory ) )
	{
		return false;
	}
	LinkCmds();
	ConVar_Register();
	return true;
}

/*
==================
CStudioAPIVk::Disconnect
==================
*/
void CStudioAPIVk::Disconnect()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Unregister cvars and disconnect Tier1
	UnlinkCmds();
	ConVar_Unregister();
	DisconnectTier1();
}

/*
==================
CStudioAPIVk::Init
==================
*/
bool CStudioAPIVk::Init()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Initialize the StudioAPI device
	device.Init( ENGINE_VERSION_MAJOR, ENGINE_VERSION_MINOR, ENGINE_VERSION_PATCH );

	// Initialize StudioAPI information
	info.bSupportMultiBuffer = true;
	info.gpuVendorId		 = device.GetGPUVendorId();
	info.pShaderPlatform	 = STUDIOAPI_VK_SHADER_PLATFORM_NAME;

	// Initialize the StudioAPI memory manager
	memoryMgr.Init( VK_API_VERSION_1_3 );

	// Initialize the synchronization manager
	syncMgr.Init();

	// Initialize command contexts for each queue type
	// Graphics queue family
	pGraphicsCmdContext = new CStudioAPICmdContextVk( device.GetGraphicsQueue(), STUDIOAPI_VK_QUEUE_FLAG_GRAPHICS );

	// Transfer queue family
	if ( device.GetTransferQueue().GetQueueFamilyIndex() == device.GetGraphicsQueue().GetQueueFamilyIndex() )
	{
		pGraphicsCmdContext->AddSupportQueueTypes( STUDIOAPI_VK_QUEUE_FLAG_TRANSFER );
		pTransferCmdContext = pGraphicsCmdContext;
	}
	else
	{
		pTransferCmdContext = new CStudioAPICmdContextVk( device.GetTransferQueue(), STUDIOAPI_VK_QUEUE_FLAG_TRANSFER );
	}

	// Compute queue family
	if ( device.GetComputeQueue().GetQueueFamilyIndex() == device.GetGraphicsQueue().GetQueueFamilyIndex() )
	{
		pGraphicsCmdContext->AddSupportQueueTypes( STUDIOAPI_VK_QUEUE_FLAG_COMPUTE );
		pComputeCmdContext = pGraphicsCmdContext;
	}
	else if ( device.GetComputeQueue().GetQueueFamilyIndex() == device.GetTransferQueue().GetQueueFamilyIndex() )
	{
		pTransferCmdContext->AddSupportQueueTypes( STUDIOAPI_VK_QUEUE_FLAG_COMPUTE );
		pComputeCmdContext = pTransferCmdContext;
	}
	else
	{
		pComputeCmdContext = new CStudioAPICmdContextVk( device.GetComputeQueue(), STUDIOAPI_VK_QUEUE_FLAG_COMPUTE );
	}

	// Initialize command contexts
	pGraphicsCmdContext->Init();
	if ( pTransferCmdContext != pGraphicsCmdContext )
	{
		pTransferCmdContext->Init();
	}
	if ( pComputeCmdContext != pGraphicsCmdContext && pComputeCmdContext != pTransferCmdContext )
	{
		pComputeCmdContext->Init();
	}

	// Initialize the data uploader
	dataUploader.Init();

	// Initialize the descriptor pools manager
	descriptorPoolsMgr.Init();

	// We are done!
	frameNumber = 0;
	Msg( "StudioAPIVk: Studio API initialized" );
	return true;
}

/*
==================
CStudioAPIVk::Shutdown
==================
*/
void CStudioAPIVk::Shutdown()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Wait of device idle
	vkDeviceWaitIdle( device.GetVkLogicalDevice() );

	// Broadcast what we shutdown StudioAPI
	onStudioAPIVkShutdown.Invoke();
	onStudioAPIVkShutdown.Clear();

	// Clear the bound shader state cache
	boundShaderStateCache.RemoveAll();

	// Shutdown the data uploader
	dataUploader.Shutdown();

	// Shutdown command contexts
	if ( pGraphicsCmdContext )
	{
		if ( pGraphicsCmdContext == pTransferCmdContext )
		{
			pTransferCmdContext = NULL;
		}

		if ( pGraphicsCmdContext == pComputeCmdContext )
		{
			pComputeCmdContext = NULL;
		}

		pGraphicsCmdContext->Shutdown();
		pGraphicsCmdContext = NULL;
	}

	if ( pTransferCmdContext )
	{
		if ( pTransferCmdContext == pComputeCmdContext )
		{
			pComputeCmdContext = NULL;
		}

		pTransferCmdContext->Shutdown();
		pTransferCmdContext = NULL;
	}

	if ( pComputeCmdContext )
	{
		pComputeCmdContext->Shutdown();
		pComputeCmdContext = NULL;
	}

	// Shutdown the descriptor pools manager
	descriptorPoolsMgr.Shutdown();

	// Shutdown the synchronization manager
	syncMgr.Shutdown();

	// Shutdown the memory manager and the device
	memoryMgr.Shutdown();
	device.Shutdown();

	// Clear StudioAPI info and reset the current frame number
	Mem_Memzero( &info, sizeof( studioAPIInfo_t ) );
	frameNumber = 0;
	Msg( "StudioAPIVk: StudioAPI destroyed" );
}

/*
==================
CStudioAPIVk::AcquireThreadOwnership
==================
*/
void CStudioAPIVk::AcquireThreadOwnership()
{
}

/*
==================
CStudioAPIVk::ReleaseThreadOwnership
==================
*/
void CStudioAPIVk::ReleaseThreadOwnership()
{
}

/*
==================
CStudioAPIVk::CreateCmdContext
==================
*/
CRefPtr<IStudioAPICmdContext> CStudioAPIVk::CreateCmdContext( studioAPIQueueType_t queueType ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	CStudioAPICmdContextVk* pImmediateCmdContext = (CStudioAPICmdContextVk*)GetImmediateCmdContext( queueType );
	return new CStudioAPICmdContextVk( pImmediateCmdContext->GetQueue(), pImmediateCmdContext->GetSupportQueueTypes() );
}

/*
==================
CStudioAPIVk::CreateCmdList
==================
*/
CRefPtr<IStudioAPICmdList> CStudioAPIVk::CreateCmdList( IStudioAPICmdContext* pCmdContext ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	return new CStudioAPICmdListVk( (CStudioAPICmdContextVk*)pCmdContext );
}

/*
==================
CStudioAPIVk::CreateCmdListBatch
==================
*/
CRefPtr<IStudioAPICmdListBatch> CStudioAPIVk::CreateCmdListBatch( IStudioAPICmdContext* pCmdContext ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	return new CStudioAPICmdListBatchVk( (CStudioAPICmdContextVk*)pCmdContext );
}

/*
==================
CStudioAPIVk::CreateSwapChain
==================
*/
CRefPtr<IStudioAPISwapChain> CStudioAPIVk::CreateSwapChain( windowHandle_t windowHandle, uint32 width, uint32 height, bool bUseVSync /* = false */ ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	CRefPtr<CStudioAPISwapChainVk> pStudioAPISwapChain = new CStudioAPISwapChainVk( S_Sprintf( "VkSwapChain [0x%X]", windowHandle ).c_str() );
	if ( !pStudioAPISwapChain->Create( windowHandle, width, height, VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, bUseVSync ) )
	{
		Sys_Error( "Failed to create the swap chain (%ux%u)", width, height );
		return NULL;
	}

	return pStudioAPISwapChain;
}

/*
==================
CStudioAPIVk::CreateVertexShader
==================
*/
CRefPtr<IStudioAPIShader> CStudioAPIVk::CreateVertexShader( const char* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const char* pDebugName /* = "" */ ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	return new CStudioAPIVertexShaderVk( pEntryPointName, pBytecode, bytecodeSize, pReflectionData, reflectionDataSize, pDebugName );
}

/*
==================
CStudioAPIVk::CreateHullShader
==================
*/
CRefPtr<IStudioAPIShader> CStudioAPIVk::CreateHullShader( const char* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const char* pDebugName /* = "" */ ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	return new CStudioAPIHullShaderVk( pEntryPointName, pBytecode, bytecodeSize, pReflectionData, reflectionDataSize, pDebugName );
}

/*
==================
CStudioAPIVk::CreateDomainShader
==================
*/
CRefPtr<IStudioAPIShader> CStudioAPIVk::CreateDomainShader( const char* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const char* pDebugName /* = "" */ ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	return new CStudioAPIDomainShaderVk( pEntryPointName, pBytecode, bytecodeSize, pReflectionData, reflectionDataSize, pDebugName );
}

/*
==================
CStudioAPIVk::CreateGeometryShader
==================
*/
CRefPtr<IStudioAPIShader> CStudioAPIVk::CreateGeometryShader( const char* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const char* pDebugName /* = "" */ ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	return new CStudioAPIGeometryShaderVk( pEntryPointName, pBytecode, bytecodeSize, pReflectionData, reflectionDataSize, pDebugName );
}

/*
==================
CStudioAPIVk::CreatePixelShader
==================
*/
CRefPtr<IStudioAPIShader> CStudioAPIVk::CreatePixelShader( const char* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const char* pDebugName /* = "" */ ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	return new CStudioAPIPixelShaderVk( pEntryPointName, pBytecode, bytecodeSize, pReflectionData, reflectionDataSize, pDebugName );
}

/*
==================
CStudioAPIVk::CreateComputeShader
==================
*/
CRefPtr<IStudioAPIShader> CStudioAPIVk::CreateComputeShader( const char* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const char* pDebugName /* = "" */ ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	return new CStudioAPIComputeShaderVk( pEntryPointName, pBytecode, bytecodeSize, pReflectionData, reflectionDataSize, pDebugName );
}

/*
==================
CStudioAPIVk::CreateRenderPipeline
==================
*/
CRefPtr<IStudioAPIBoundShaderState> CStudioAPIVk::FindOrCreateBoundShaderState( IStudioAPIVertexDeclaration* pVertexDeclaration, IStudioAPIShader* pVertexShader, IStudioAPIShader* pPixelShader, IStudioAPIShader* pHullShader /* = NULL */, IStudioAPIShader* pDomainShader /* = NULL */, IStudioAPIShader* pGeometryShader /* = NULL */ ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	CStudioAPIBoundShaderStateKeyVk key( (CStudioAPIVertexDeclarationVk*)pVertexDeclaration,
										 (CStudioAPIVertexShaderVk*)pVertexShader,
										 (CStudioAPIPixelShaderVk*)pPixelShader,
										 (CStudioAPIHullShaderVk*)pHullShader,
										 (CStudioAPIDomainShaderVk*)pDomainShader,
										 (CStudioAPIGeometryShaderVk*)pGeometryShader );
	CStudioAPIBoundShaderStateVk*	boundShaderState = boundShaderStateCache.Find( key );
	if ( !boundShaderState )
	{
		boundShaderState = new CStudioAPIBoundShaderStateVk( key,
															 (CStudioAPIVertexDeclarationVk*)pVertexDeclaration,
															 (CStudioAPIVertexShaderVk*)pVertexShader,
															 (CStudioAPIPixelShaderVk*)pPixelShader,
															 (CStudioAPIHullShaderVk*)pHullShader,
															 (CStudioAPIDomainShaderVk*)pDomainShader,
															 (CStudioAPIGeometryShaderVk*)pGeometryShader );
		boundShaderStateCache.Add( key, boundShaderState );
	}

	return boundShaderState;
}

/*
==================
CStudioAPIVk::CreateRenderPipeline
==================
*/
CRefPtr<IStudioAPIRenderPipeline> CStudioAPIVk::CreateRenderPipeline( const studioAPIRenderPipelineCreateInfo_t& createInfo, const char* pDebugName /* = "" */ ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	return new CStudioAPIRenderPipelineVk( createInfo, pDebugName );
}

/*
==================
CStudioAPIVk::CreateVertexDeclaration
==================
*/
CRefPtr<IStudioAPIVertexDeclaration> CStudioAPIVk::CreateVertexDeclaration( const studioAPIVertexInputBuffer_t* pVertexInputBuffers, uint32 numVertexInputBuffers, const studioAPIVertexInputAttribute_t* pVertexInputAttributes, uint32 numVertexInputAttributes ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	return new CStudioAPIVertexDeclarationVk( pVertexInputBuffers, numVertexInputBuffers, pVertexInputAttributes, numVertexInputAttributes );
}

/*
==================
CStudioAPIVk::CreateVertexDeclaration
==================
*/
CRefPtr<IStudioAPIFrameBuffer> CStudioAPIVk::CreateFrameBuffer( const studioAPIFrameBufferCreateInfo_t& createInfo, const char* pDebugName /* = "" */ ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	return new CStudioAPIFrameBufferVk( createInfo, pDebugName );
}

/*
==================
CStudioAPIVk::CreateVertexDeclaration
==================
*/
CRefPtr<IStudioAPIRenderPass> CStudioAPIVk::CreateRenderPass( const studioAPIRenderPassCreateInfo_t& createInfo, const char* pDebugName /* = "" */ ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	return new CStudioAPIRenderPassVk( createInfo, pDebugName );
}

/*
==================
CStudioAPIVk::CreateBuffer
==================
*/
CRefPtr<IStudioAPIBuffer> CStudioAPIVk::CreateBuffer( const byte* pData, uint64 dataSize, uint32 dataStride, uint32 usageFlags, const char* pDebugName /* = "" */ ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	return new CStudioAPIBufferVk( pData, dataSize, dataStride, usageFlags, pDebugName );
}

/*
==================
CStudioAPIVk::CreateTexture
==================
*/
CRefPtr<IStudioAPITexture> CStudioAPIVk::CreateTexture( studioAPITextureType_t type, uint32 sizeX, uint32 sizeY, uint32 sizeZ, uint32 numLayers, uint32 numMips, uint32 usageFlags, studioAPIPixelFormat_t pixelFormat, const byte* pData /* = NULL */, const char* pDebugName /* = "" */ ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	return new CStudioAPITextureVk( type, sizeX, sizeY, sizeZ, numLayers, numMips, usageFlags, pixelFormat, pData, pDebugName );
}

/*
==================
CStudioAPIVk::CreateSampler
==================
*/
CRefPtr<IStudioAPISampler> CStudioAPIVk::CreateSampler( const studioAPISamplerCreateInfo_t& createInfo, const char* pDebugName /* = "" */ ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	return new CStudioAPISamplerVk( createInfo, pDebugName );
}

/*
==================
CStudioAPIVk::BeginDrawingFrame
==================
*/
void CStudioAPIVk::BeginDrawingFrame()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Increase the current frame number and current frame in-flight
	currentFrameInFlight = ( currentFrameInFlight + 1 ) % STUDIOAPI_VK_NUM_FRAMES_IN_FLIGHT;
	++frameNumber;

	// Wait the current frame-in-flight
	syncMgr.WaitFrameInFlight( currentFrameInFlight );

	// Free all pending resources which were added at the frame-in-flight index previously
	memoryMgr.FreePendingResources( currentFrameInFlight );

	// Free unused descriptor pool sets
	descriptorPoolsMgr.FreeUnusedPoolSets();

	// Begin a new frame in command contexts
	pGraphicsCmdContext->BeginFrame();
	pComputeCmdContext->BeginFrame();
	pTransferCmdContext->BeginFrame();
}

/*
==================
CStudioAPIVk::EndDrawingFrame
==================
*/
void CStudioAPIVk::EndDrawingFrame()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// End the frame in command contexts
	pGraphicsCmdContext->EndFrame();
	pComputeCmdContext->EndFrame();
	pTransferCmdContext->EndFrame();
}

/*
==================
CStudioAPIVk::SubmitCmdListBatch
==================
*/
void CStudioAPIVk::SubmitCmdListBatch( IStudioAPICmdListBatch* pCmdListBatch, bool bWait /* = false */, uint64 waitTime /* = 0 */ )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Flush all upload data
	dataUploader.Flush();

	// Submit the batch
	CStudioAPICmdListBatchVk* pStudioAPICmdListBatch = (CStudioAPICmdListBatchVk*)pCmdListBatch;
	CStudioAPICmdContextVk*	  pStudioAPICmdContext	 = (CStudioAPICmdContextVk*)pStudioAPICmdListBatch->GetCmdContext();
	pStudioAPICmdContext->GetCmdListBatchMgr().Submit( pStudioAPICmdListBatch, bWait, waitTime );
}

/*
==================
CStudioAPIVk::WaitCmdListBatch
==================
*/
bool CStudioAPIVk::WaitCmdListBatch( IStudioAPICmdListBatch* pCmdListBatch, uint64 waitTime /* = 0 */ )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_WAIT );
	CStudioAPICmdListBatchVk* pStudioAPICmdListBatch = (CStudioAPICmdListBatchVk*)pCmdListBatch;
	CStudioAPICmdContextVk*	  pStudioAPICmdContext	 = (CStudioAPICmdContextVk*)pStudioAPICmdListBatch->GetCmdContext();
	return pStudioAPICmdContext->GetCmdListBatchMgr().Wait( pStudioAPICmdListBatch, waitTime );
}

/*
==================
CStudioAPIVk::GetInfo
==================
*/
const studioAPIInfo_t& CStudioAPIVk::GetInfo() const
{
	return info;
}

/*
==================
CStudioAPIVk::GetFrameNumber
==================
*/
uint64 CStudioAPIVk::GetFrameNumber() const
{
	return frameNumber;
}

/*
==================
CStudioAPIVk::GetImmediateCmdContext
==================
*/
IStudioAPICmdContext* CStudioAPIVk::GetImmediateCmdContext( studioAPIQueueType_t queueType ) const
{
	switch ( queueType )
	{
	case STUDIOAPI_QUEUE_TYPE_GRAPHICS: return pGraphicsCmdContext;
	case STUDIOAPI_QUEUE_TYPE_COMPUTE: return pComputeCmdContext;
	case STUDIOAPI_QUEUE_TYPE_TRANSFER: return pTransferCmdContext;
	default:
		AssertMsg( false, "Unknown StudioAPI queue type 0x%X", queueType );
		return NULL;
	}
}
