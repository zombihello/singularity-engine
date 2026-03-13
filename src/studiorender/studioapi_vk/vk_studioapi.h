#pragma once
#include "studiorender/studioapi/istudioapi.h"
#include "studiorender/studioapi_vk/vk_studioapi_delegates.h"
#include "studiorender/studioapi_vk/vk_studioapi_device.h"
#include "studiorender/studioapi_vk/vk_studioapi_memorymgr.h"
#include "studiorender/studioapi_vk/vk_studioapi_cmdbufferpool.h"
#include "studiorender/studioapi_vk/vk_studioapi_boundshaderstate_cache.h"
#include "studiorender/studioapi_vk/vk_studioapi_descriptorpoolsmgr.h"
#include "studiorender/studioapi_vk/vk_studioapi_buffer.h"
#include "studiorender/studioapi_vk/vk_studioapi_datauploader.h"
#include "studiorender/studioapi_vk/vk_studioapi_syncmgr.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CStudioAPICmdContextVk;
class CStudioAPIDescriptorPoolsMgrVk;

//-----------------------------------------------------------------------------
// Vulkan Studio API
//-----------------------------------------------------------------------------
class CStudioAPIVk : public CBaseAppSystem<IStudioAPI>
{
public:
	// IAppSystem interfaces
	// Here's where the app systems get to learn about each other
	virtual bool Connect( createInterfaceFn_t pFactory ) override;
	virtual void Disconnect() override;

	// Initialize and shutdown
	virtual bool Init() override;
	virtual void Shutdown() override;

	// IStudioAPI interface
	// Acquire/release thread ownership
	virtual void AcquireThreadOwnership() override;
	virtual void ReleaseThreadOwnership() override;

	// Create Studio API resources
	virtual CRefPtr<IStudioAPICmdContext>		 CreateCmdContext( studioAPIQueueType_t queueType ) const override;
	virtual CRefPtr<IStudioAPICmdList>			 CreateCmdList( IStudioAPICmdContext* pCmdContext ) const override;
	virtual CRefPtr<IStudioAPICmdListBatch>		 CreateCmdListBatch( IStudioAPICmdContext* pCmdContext ) const override;
	virtual CRefPtr<IStudioAPISwapChain>		 CreateSwapChain( windowHandle_t windowHandle, uint32 width, uint32 height, bool bUseVSync = false ) const override;
	virtual CRefPtr<IStudioAPIShader>			 CreateVertexShader( const char* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const char* pDebugName = "" ) const override;
	virtual CRefPtr<IStudioAPIShader>			 CreateHullShader( const char* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const char* pDebugName = "" ) const override;
	virtual CRefPtr<IStudioAPIShader>			 CreateDomainShader( const char* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const char* pDebugName = "" ) const override;
	virtual CRefPtr<IStudioAPIShader>			 CreateGeometryShader( const char* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const char* pDebugName = "" ) const override;
	virtual CRefPtr<IStudioAPIShader>			 CreatePixelShader( const char* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const char* pDebugName = "" ) const override;
	virtual CRefPtr<IStudioAPIShader>			 CreateComputeShader( const char* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const char* pDebugName = "" ) const override;
	virtual CRefPtr<IStudioAPIBoundShaderState>	 FindOrCreateBoundShaderState( IStudioAPIVertexDeclaration* pVertexDeclaration, IStudioAPIShader* pVertexShader, IStudioAPIShader* pPixelShader, IStudioAPIShader* pHullShader = NULL, IStudioAPIShader* pDomainShader = NULL, IStudioAPIShader* pGeometryShader = NULL ) const override;
	virtual CRefPtr<IStudioAPIRenderPipeline>	 CreateRenderPipeline( const studioAPIRenderPipelineCreateInfo_t& createInfo, const char* pDebugName = "" ) const override;
	virtual CRefPtr<IStudioAPIVertexDeclaration> CreateVertexDeclaration( const studioAPIVertexInputBuffer_t* pVertexInputBuffers, uint32 numVertexInputBuffers, const studioAPIVertexInputAttribute_t* pVertexInputAttributes, uint32 numVertexInputAttributes ) const override;
	virtual CRefPtr<IStudioAPIFrameBuffer>		 CreateFrameBuffer( const studioAPIFrameBufferCreateInfo_t& createInfo, const char* pDebugName = "" ) const override;
	virtual CRefPtr<IStudioAPIRenderPass>		 CreateRenderPass( const studioAPIRenderPassCreateInfo_t& createInfo, const char* pDebugName = "" ) const override;
	virtual CRefPtr<IStudioAPIBuffer>			 CreateBuffer( const byte* pData, uint64 dataSize, uint32 dataStride, uint32 usageFlags, const char* pDebugName = "" ) const override;
	virtual CRefPtr<IStudioAPITexture>			 CreateTexture( studioAPITextureType_t type, uint32 sizeX, uint32 sizeY, uint32 sizeZ, uint32 numLayers, uint32 numMips, uint32 usageFlags, studioAPIPixelFormat_t pixelFormat, const byte* pData = NULL, const char* pDebugName = "" ) const override;
	virtual CRefPtr<IStudioAPISampler>			 CreateSampler( const studioAPISamplerCreateInfo_t& createInfo, const char* pDebugName = "" ) const override;

	// Begin/end drawing a frame
	virtual void BeginDrawingFrame() override;
	virtual void EndDrawingFrame() override;

	// Functions to work with command list batches
	virtual void SubmitCmdListBatch( IStudioAPICmdListBatch* pCmdListBatch, bool bWait = false, uint64 waitTime = 0 ) override;
	virtual bool WaitCmdListBatch( IStudioAPICmdListBatch* pCmdListBatch, uint64 waitTime = 0 ) override;

	virtual const studioAPIInfo_t& GetInfo() const override;
	virtual uint64				   GetFrameNumber() const override;
	virtual IStudioAPICmdContext*  GetImmediateCmdContext( studioAPIQueueType_t queueType ) const override;

	CStudioAPIVk();

	const CStudioAPIDeviceVk&		   GetDevice() const;
	const CStudioAPIMemoryMgrVk&	   GetMemoryMgr() const;
	COnStudioAPIVkShutdown&			   OnStudioAPIVkShutdown() const;
	CStudioAPIBoundShaderStateCacheVk& GetBoundShaderStateCache();
	CStudioAPIDescriptorPoolsMgrVk&	   GetDescriptorPoolsMgr();
	CStudioAPIDataUploaderVk&		   GetDataUploader();
	CStudioAPISyncMgrVk&			   GetSyncMgr();
	uint32							   GetCurrentFrameInFlight() const;

private:
	uint64									  frameNumber;
	uint32									  currentFrameInFlight;
	CStudioAPIDeviceVk						  device;
	CRefPtr<CStudioAPICmdContextVk>			  pGraphicsCmdContext;
	CRefPtr<CStudioAPICmdContextVk>			  pTransferCmdContext;
	CRefPtr<CStudioAPICmdContextVk>			  pComputeCmdContext;
	CStudioAPIMemoryMgrVk					  memoryMgr;
	CStudioAPIDescriptorPoolsMgrVk			  descriptorPoolsMgr;
	CStudioAPIDataUploaderVk				  dataUploader;
	CStudioAPISyncMgrVk						  syncMgr;
	studioAPIInfo_t							  info;
	mutable CStudioAPIBoundShaderStateCacheVk boundShaderStateCache;
	mutable COnStudioAPIVkShutdown			  onStudioAPIVkShutdown;
};
extern CStudioAPIVk g_StudioAPIVk;

#include "studiorender/studioapi_vk/vk_studioapi.inl"
