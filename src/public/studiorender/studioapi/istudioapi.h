#pragma once
#include "appframework/iappsystem.h"
#include "studiorender/studioapi/istudioapi_shader.h"
#include "studiorender/studioapi/istudioapi_renderpipeline.h"
#include "studiorender/studioapi/istudioapi_framebuffer.h"
#include "studiorender/studioapi/istudioapi_swapchain.h"
#include "studiorender/studioapi/istudioapi_renderpass.h"
#include "studiorender/studioapi/istudioapi_buffer.h"
#include "studiorender/studioapi/istudioapi_cmdlist.h"
#include "studiorender/studioapi/istudioapi_texture.h"

//-----------------------------------------------------------------------------
// Studio API information
//-----------------------------------------------------------------------------
enum studioAPIGPUVendorId_t
{
	STUDIOAPI_GPU_VENDOR_ID_UNKNOWN		= 0xFFFFFFFF,
	STUDIOAPI_GPU_VENDOR_ID_NOT_QUERIED = 0,  // GPU vendor Id isn't queried
	STUDIOAPI_GPU_VENDOR_ID_AMD			= 0x1002,
	STUDIOAPI_GPU_VENDOR_ID_IMGTEC		= 0x1010,
	STUDIOAPI_GPU_VENDOR_ID_NVIDIA		= 0x10DE,
	STUDIOAPI_GPU_VENDOR_ID_ARM			= 0x13B5,
	STUDIOAPI_GPU_VENDOR_ID_BROADCOM	= 0x14E4,
	STUDIOAPI_GPU_VENDOR_ID_QUALCOMM	= 0x5143,
	STUDIOAPI_GPU_VENDOR_ID_INTEL		= 0x8086,
	STUDIOAPI_GPU_VENDOR_ID_APPLE		= 0x106B,
	STUDIOAPI_GPU_VENDOR_ID_VIVANTE		= 0x7a05,
	STUDIOAPI_GPU_VENDOR_ID_VERISILICON = 0x1EB1,
	STUDIOAPI_GPU_VENDOR_ID_SAMSUNG_AMD = 0x144D,
	STUDIOAPI_GPU_VENDOR_ID_MICROSOFT	= 0x1414,
	STUDIOAPI_GPU_VENDOR_ID_KAZAN		= 0x10003,
	STUDIOAPI_GPU_VENDOR_ID_CODEPLAY	= 0x10004,
	STUDIOAPI_GPU_VENDOR_ID_MESA		= 0x10005
};

struct studioAPIInfo_t
{
	bool				   bSupportMultiBuffer;
	studioAPIGPUVendorId_t gpuVendorId;
	float				   pixelCenterOffset;
	const char*			   pShaderPlatform;
};

//-----------------------------------------------------------------------------
// Studio API interface
//-----------------------------------------------------------------------------
#define STUDIOAPI_INTERFACE_VERSION "SStudioAPI001"
class IStudioAPI : public IAppSystem
{
public:
	// Acquire/release thread ownership
	virtual void AcquireThreadOwnership() = 0;
	virtual void ReleaseThreadOwnership() = 0;

	// Create Studio API resources
	virtual CRefPtr<IStudioAPICmdContext>		 CreateCmdContext( studioAPIQueueType_t queueType ) const																																																						= 0;
	virtual CRefPtr<IStudioAPICmdList>			 CreateCmdList( IStudioAPICmdContext* pCmdContext ) const																																																						= 0;
	virtual CRefPtr<IStudioAPICmdListBatch>		 CreateCmdListBatch( IStudioAPICmdContext* pCmdContext ) const																																																					= 0;
	virtual CRefPtr<IStudioAPISwapChain>		 CreateSwapChain( windowHandle_t windowHandle, uint32 width, uint32 height, bool bUseVSync = false ) const																																										= 0;
	virtual CRefPtr<IStudioAPIShader>			 CreateVertexShader( const char* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const char* pDebugName = "" ) const																						= 0;
	virtual CRefPtr<IStudioAPIShader>			 CreateHullShader( const char* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const char* pDebugName = "" ) const																							= 0;
	virtual CRefPtr<IStudioAPIShader>			 CreateDomainShader( const char* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const char* pDebugName = "" ) const																						= 0;
	virtual CRefPtr<IStudioAPIShader>			 CreateGeometryShader( const char* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const char* pDebugName = "" ) const																						= 0;
	virtual CRefPtr<IStudioAPIShader>			 CreatePixelShader( const char* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const char* pDebugName = "" ) const																						= 0;
	virtual CRefPtr<IStudioAPIShader>			 CreateComputeShader( const char* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const char* pDebugName = "" ) const																						= 0;
	virtual CRefPtr<IStudioAPIBoundShaderState>	 FindOrCreateBoundShaderState( IStudioAPIVertexDeclaration* pVertexDeclaration, IStudioAPIShader* pVertexShader, IStudioAPIShader* pPixelShader, IStudioAPIShader* pHullShader = NULL, IStudioAPIShader* pDomainShader = NULL, IStudioAPIShader* pGeometryShader = NULL ) const = 0;
	virtual CRefPtr<IStudioAPIRenderPipeline>	 CreateRenderPipeline( const studioAPIRenderPipelineCreateInfo_t& createInfo, const char* pDebugName = "" ) const																																								= 0;
	virtual CRefPtr<IStudioAPIVertexDeclaration> CreateVertexDeclaration( const studioAPIVertexInputBuffer_t* pVertexInputBuffers, uint32 numVertexInputBuffers, const studioAPIVertexInputAttribute_t* pVertexInputAttributes, uint32 numVertexInputAttributes ) const															= 0;
	virtual CRefPtr<IStudioAPIFrameBuffer>		 CreateFrameBuffer( const studioAPIFrameBufferCreateInfo_t& createInfo, const char* pDebugName = "" ) const																																										= 0;
	virtual CRefPtr<IStudioAPIRenderPass>		 CreateRenderPass( const studioAPIRenderPassCreateInfo_t& createInfo, const char* pDebugName = "" ) const																																										= 0;
	virtual CRefPtr<IStudioAPIBuffer>			 CreateBuffer( const byte* pData, uint64 dataSize, uint32 dataStride, uint32 usageFlags, const char* pDebugName = "" ) const																																					= 0;
	virtual CRefPtr<IStudioAPITexture>			 CreateTexture( studioAPITextureType_t type, uint32 sizeX, uint32 sizeY, uint32 sizeZ, uint32 numLayers, uint32 numMips, uint32 usageFlags, studioAPIPixelFormat_t pixelFormat, const byte* pData = NULL, const char* pDebugName = "" ) const									= 0;
	virtual CRefPtr<IStudioAPISampler>			 CreateSampler( const studioAPISamplerCreateInfo_t& createInfo, const char* pDebugName = "" ) const																																												= 0;

	// Begin/end drawing a frame
	virtual void BeginDrawingFrame() = 0;
	virtual void EndDrawingFrame()	 = 0;

	// Functions to work with command list batches
	virtual void SubmitCmdListBatch( IStudioAPICmdListBatch* pCmdListBatch, bool bWait = false, uint64 waitTime = 0 ) = 0;
	virtual bool WaitCmdListBatch( IStudioAPICmdListBatch* pCmdListBatch, uint64 waitTime = 0 )						  = 0;

	virtual const studioAPIInfo_t& GetInfo() const												  = 0;
	virtual uint64				   GetFrameNumber() const										  = 0;
	virtual IStudioAPICmdContext*  GetImmediateCmdContext( studioAPIQueueType_t queueType ) const = 0;
};
