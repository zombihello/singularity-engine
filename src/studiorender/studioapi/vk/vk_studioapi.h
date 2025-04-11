/**
 * @file
 * @addtogroup studioapi_vk studioapi_vk
 */

#ifndef VK_STUDIOAPI_H
#define VK_STUDIOAPI_H

#include "studiorender/studioapi/istudioapi.h"
#include "studiorender/studioapi/vk/vk_studioapi_delegates.h"
#include "studiorender/studioapi/vk/vk_studioapi_device.h"
#include "studiorender/studioapi/vk/vk_studioapi_memorymgr.h"
#include "studiorender/studioapi/vk/vk_studioapi_cmdbufferpool.h"
#include "studiorender/studioapi/vk/vk_studioapi_boundshaderstate_cache.h"
#include "studiorender/studioapi/vk/vk_studioapi_descriptorpoolsmgr.h"
#include "studiorender/studioapi/vk/vk_studioapi_buffer.h"
#include "studiorender/studioapi/vk/vk_studioapi_datauploader.h"
#include "studiorender/studioapi/vk/vk_studioapi_syncmgr.h"

// Forward declarations
class CStudioAPICmdContextVk;
class CStudioAPIDescriptorPoolsMgrVk;

/**
 * @ingroup studioapi_vk
 * @brief Vulkan studio API
 */
class CStudioAPIVk : public IStudioAPI
{
public:
	/**
	 * @brief Constructor
	 */
	CStudioAPIVk();

	/**
	 * @brief Initialize Studio API
	 *
	 * @param pFactory	Pointer to interface factory
	 * @return Return TRUE if Studio API is initialized, otherwise returns FALSE
	 */
	virtual bool Init( createInterfaceFn_t pFactory ) override;

	/**
	 * @brief Shutdown Studio API
	 */
	virtual void Shutdown() override;

	/**
	 * @brief Acquire thread ownership
	 */
	virtual void AcquireThreadOwnership() override;

	/**
	 * @brief Release thread ownership
	 */
	virtual void ReleaseThreadOwnership() override;
	
	/**
	 * @brief Create a command context for queue type
	 * @param queueType		GPU queue type
	 * @return Return the created command context for queue type
	 */
	virtual TRefPtr<IStudioAPICmdContext> CreateCmdContext( studioAPIQueueType_t queueType ) const override;

	/**
	 * @brief Create a command list for the command context
	 * @param pCmdContext	StudioAPI command context
	 * @return Return the created command list
	 */
	virtual TRefPtr<IStudioAPICmdList> CreateCmdList( IStudioAPICmdContext* pCmdContext ) const override;

	/**
	 * @brief Create a command list batch for the command context
	 * @param pCmdContext	StudioAPI command context
	 * @return Return the created command list batch
	 */
	virtual TRefPtr<IStudioAPICmdListBatch> CreateCmdListBatch( IStudioAPICmdContext* pCmdContext ) const override;

	/**
	 * @brief Create a swap chain
	 * @param windowHandle	OS window handle
	 * @param width			Window width
	 * @param height		Window height
	 * @param bUseVSync		Is it necessary to use VSync
	 * @return Return the created swap chain
	 */
	virtual TRefPtr<IStudioAPISwapChain> CreateSwapChain( windowHandle_t windowHandle, uint32 width, uint32 height, bool bUseVSync = false ) const override;
	
	/**
	 * @brief Create vertex shader
	 *
	 * @param pEntryPointName		Entry point name
	 * @param pBytecode				Shader code
	 * @param bytecodeSize			Shader code size
	 * @param pReflectionData		Reflection data
	 * @param reflectionDataSize	Reflection data size
	 * @param pDebugName			Debug name
	 * @return Return created vertex shader. If failed return NULL
	 */
	virtual TRefPtr<IStudioAPIShader> CreateVertexShader( const achar* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const achar* pDebugName = "" ) const override;

	/**
	 * @brief Create hull shader
	 *
	 * @param pEntryPointName		Entry point name
	 * @param pBytecode				Shader code
	 * @param bytecodeSize			Shader code size
	 * @param pReflectionData		Reflection data
	 * @param reflectionDataSize	Reflection data size
	 * @param pDebugName			Debug name
	 * @return Return created hull shader. If failed return NULL
	 */
	virtual TRefPtr<IStudioAPIShader> CreateHullShader( const achar* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const achar* pDebugName = "" ) const override;

	/**
	 * @brief Create domain shader
	 *
	 * @param pEntryPointName		Entry point name
	 * @param pBytecode				Shader code
	 * @param bytecodeSize			Shader code size
	 * @param pReflectionData		Reflection data
	 * @param reflectionDataSize	Reflection data size
	 * @param pDebugName			Debug name
	 * @return Return created domain shader. If failed return NULL
	 */
	virtual TRefPtr<IStudioAPIShader> CreateDomainShader( const achar* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const achar* pDebugName = "" ) const override;

	/**
	 * @brief Create geometry shader
	 *
	 * @param pEntryPointName		Entry point name
	 * @param pBytecode				Shader code
	 * @param bytecodeSize			Shader code size
	 * @param pReflectionData		Reflection data
	 * @param reflectionDataSize	Reflection data size
	 * @param pDebugName			Debug name
	 * @return Return created geometry shader. If failed return NULL
	 */
	virtual TRefPtr<IStudioAPIShader> CreateGeometryShader( const achar* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const achar* pDebugName = "" ) const override;

	/**
	 * @brief Create pixel shader
	 *
	 * @param pEntryPointName		Entry point name
	 * @param pBytecode				Shader code
	 * @param bytecodeSize			Shader code size
	 * @param pReflectionData		Reflection data
	 * @param reflectionDataSize	Reflection data size
	 * @param pDebugName			Debug name
	 * @return Return created pixel shader. If failed return NULL
	 */
	virtual TRefPtr<IStudioAPIShader> CreatePixelShader( const achar* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const achar* pDebugName = "" ) const override;

	/**
	 * @brief Create compute shader
	 *
	 * @param pEntryPointName		Entry point name
	 * @param pBytecode				Shader code
	 * @param bytecodeSize			Shader code size
	 * @param pReflectionData		Reflection data
	 * @param reflectionDataSize	Reflection data size
	 * @param pDebugName			Debug name
	 * @return Return created compute shader. If failed return NULL
	 */
	virtual TRefPtr<IStudioAPIShader> CreateComputeShader( const achar* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const achar* pDebugName = "" ) const override;

	/**
	 * @brief Find or create a bound shader state
	 *
	 * @param pVertexDeclaration	Vertex declaration
	 * @param pVertexShader			Vertex shader
	 * @param pPixelShader			Pixel shader
	 * @param pHullShader			Hull shader
	 * @param pDomainShader			Domain shader
	 * @param pGeometryShader		Geometry shader
	 * @return Return the already created bound shader state or created a new
	 */
	virtual TRefPtr<IStudioAPIBoundShaderState> FindOrCreateBoundShaderState( IStudioAPIVertexDeclaration* pVertexDeclaration, IStudioAPIShader* pVertexShader, IStudioAPIShader* pPixelShader, IStudioAPIShader* pHullShader = NULL, IStudioAPIShader* pDomainShader = NULL, IStudioAPIShader* pGeometryShader = NULL ) const override;

	/**
	 * @brief Create render pipeline
	 * @param createInfo	Information to create a render pipeline
	 * @param pDebugName	Debug name
	 * @return Return created the render pipeline. If failed return NULL
	 */
	virtual TRefPtr<IStudioAPIRenderPipeline> CreateRenderPipeline( const studioAPIRenderPipelineCreateInfo_t& createInfo, const achar* pDebugName = "" ) const override;

	/**
	 * @brief Create a vertex declaration
	 * @param pVertexInputBuffers			Vertex input buffers array
	 * @param numVertexInputBuffers			Vertex input buffers array size
	 * @param pVertexInputAttributes		Vertex input attributes array
	 * @param numVertexInputAttributes		Vertex input attributes array size
	 * @return Return created a vertex declaration. If failed return NULL
	 */
	virtual TRefPtr<IStudioAPIVertexDeclaration> CreateVertexDeclaration( const studioAPIVertexInputBuffer_t* pVertexInputBuffers, uint32 numVertexInputBuffers, const studioAPIVertexInputAttribute_t* pVertexInputAttributes, uint32 numVertexInputAttributes ) const override;

	/**
	 * @brief Create frame buffer
	 * @param createInfo	Information to create a frame buffer
	 * @param pDebugName	Debug name
	 * @return Return created the frame buffer. If failed return NULL
	 */
	virtual TRefPtr<IStudioAPIFrameBuffer> CreateFrameBuffer( const studioAPIFrameBufferCreateInfo_t& createInfo, const achar* pDebugName = "" ) const override;

	/**
	 * @brief Create a render pass
	 * @param createInfo	Information to create a render pass
	 * @param pDebugName	Debug name
	 * @return Return created the render pipeline. If failed return NULL
	 */
	virtual TRefPtr<IStudioAPIRenderPass> CreateRenderPass( const studioAPIRenderPassCreateInfo_t& createInfo, const achar* pDebugName = "" ) const override;

	/**
	 * @brief Create a buffer
	 * @param pData			Buffer data
	 * @param dataSize		Data size in bytes
	 * @param dataStride	Data stride
	 * @param usageFlags	Usage flags (see studioAPIBufferUsageFlag_t)
	 * @param pDebugName	Debug name
	 * @return Return created the buffer. If failed return NULL
	 */
	virtual TRefPtr<IStudioAPIBuffer> CreateBuffer( const byte* pData, uint64 dataSize, uint32 dataStride, uint32 usageFlags, const achar* pDebugName = "" ) const override;

	/**
	 * @brief Create a texture
	 * @param type			Texture type
	 * @param sizeX			Texture width
	 * @param sizeY			Texture height
	 * @param sizeZ			Texture depth
	 * @param numLayers		Layers number in the texture
	 * @param numMips		Mips number in the texture
	 * @param usageFlags	Usage flags (see studioAPITextureUsageFlag_t)
	 * @param pixelFormat	Texture pixel format
	 * @param pData			Texture data
	 * @param pDebugName	Debug name
	 * @return Return created the texture. If failed return NULL
	 */
	virtual TRefPtr<IStudioAPITexture> CreateTexture( studioAPITextureType_t type, uint32 sizeX, uint32 sizeY, uint32 sizeZ, uint32 numLayers, uint32 numMips, uint32 usageFlags, studioAPIPixelFormat_t pixelFormat, const byte* pData = NULL, const achar* pDebugName = "" ) const override;

	/**
	 * @brief Create a sampler
	 * param createInfo		Information to create a sampler
	 * @param pDebugName	Debug name
	 * @return Return created the sampler. If failed return NULL
	 */
	virtual TRefPtr<IStudioAPISampler> CreateSampler( const studioAPISamplerCreateInfo_t& createInfo, const achar* pDebugName = "" ) const override;

	/**
	 * @brief Get StudioAPI information
	 * @return Return StudioAPI information
	 */
	virtual const studioAPIInfo_t& GetInfo() const override;

	/**
	 * @brief Get the StudioAPI device
	 * @return Return the StudioAPI device
	 */
	FORCEINLINE const CStudioAPIDeviceVk& GetDevice() const
	{
		return device;
	}

	/**
	 * @brief Begin drawing a new frame
	 */
	virtual void BeginDrawingFrame() override;

	/**
	 * @brief End drawing the frame
	 */
	virtual void EndDrawingFrame() override;

	/**
	 * @brief Submit a command list batch
	 * @warning You can't submit one command list batch twice at one frame

	 * @param pCmdListBatch		StudioAPI command list batch to submit
	 * @param bWait				Is need wait the command list batch when it will be done on GPU
	 * @param waitTime			Wait time in nanoseconds. Set 0 for infinity wait
	 */
	virtual void SubmitCmdListBatch( IStudioAPICmdListBatch* pCmdListBatch, bool bWait = false, uint64 waitTime = 0 ) override;

	/**
	 * @brief Wait the command list batch when it will be done on GPU
	 * @param waitTime		Wait time in nanoseconds. Set 0 for infinity wait
	 * @return Return TRUE if the command list batch has been done, otherwise FALSE
	 */
	virtual bool WaitCmdListBatch( IStudioAPICmdListBatch* pCmdListBatch, uint64 waitTime = 0 ) override;

	/**
	 * @brief Get the current frame number
	 * @return Return the current frame number
	 */
	virtual uint64 GetFrameNumber() const override;

	/**
	 * @brief Get the immediate command context for queue type
	 * @param queueType		GPU queue type
	 * @return Return the immediate command context for queue type
	 */
	virtual IStudioAPICmdContext* GetImmediateCmdContext( studioAPIQueueType_t queueType ) const override;

	/**
	 * @brief Get the memory manager
	 * @return Return the memory manager
	 */
	FORCEINLINE const CStudioAPIMemoryMgrVk& GetMemoryMgr() const
	{
		return memoryMgr;
	}

	/**
	 * @brief Get event manager 'on StudioAPI Vulkan shutdown'
	 * @return Return event manager 'on StudioAPI Vulkan shutdown'
	 */
	FORCEINLINE COnStudioAPIVkShutdown& OnStudioAPIVkShutdown() const
	{
		return onStudioAPIVkShutdown;
	}

	/**
	 * @brief Get the bound shader state cache
	 * @return Return the bound shader state cache
	 */
	FORCEINLINE CStudioAPIBoundShaderStateCacheVk& GetBoundShaderStateCache()
	{
		return boundShaderStateCache;
	}

	/**
	 * @brief Get the descriptor pools manager
	 * @return Return the descriptor pools manager
	 */
	FORCEINLINE CStudioAPIDescriptorPoolsMgrVk& GetDescriptorPoolsMgr()
	{
		return descriptorPoolsMgr;
	}

	/**
	 * @brief Get the data uploader
	 * @return Return the data uploader
	 */
	FORCEINLINE CStudioAPIDataUploaderVk& GetDataUploader()
	{
		return dataUploader;
	}

	/**
	 * @brief Get the synchronization manager
	 * @return Return the synchronization manager
	 */
	FORCEINLINE CStudioAPISyncMgrVk& GetSyncMgr()
	{
		return syncMgr;
	}

	/**
	 * @brief Get current frame in-flight
	 * @return Return current frame in-flight
	 */
	FORCEINLINE uint32 GetCurrentFrameInFlight() const
	{
		return currentFrameInFlight;
	}

private:
	bool										bInited;					/**< Is initialized Studio API */
	uint64										frameNumber;				/**< Curent frame number */
	uint32										currentFrameInFlight;		/**< Current frame in-flight */
	CStudioAPIDeviceVk							device;						/**< Vulkan device */
	TRefPtr<CStudioAPICmdContextVk>				pGraphicsCmdContext;		/**< Graphics command context */
	TRefPtr<CStudioAPICmdContextVk>				pTransferCmdContext;		/**< Transfer command context */
	TRefPtr<CStudioAPICmdContextVk>				pComputeCmdContext;			/**< Compute command context */
	CStudioAPIMemoryMgrVk						memoryMgr;					/**< Memory manager */
	CStudioAPIDescriptorPoolsMgrVk				descriptorPoolsMgr;			/**< Descriptor pools manager */
	CStudioAPIDataUploaderVk					dataUploader;				/**< Data uploader */
	CStudioAPISyncMgrVk							syncMgr;					/**< Synchronization manager */
	studioAPIInfo_t								info;						/**< StudioAPI information */
	mutable CStudioAPIBoundShaderStateCacheVk	boundShaderStateCache;		/**< Bound shader state cache */
	mutable	COnStudioAPIVkShutdown				onStudioAPIVkShutdown;		/**< Event on StudioAPI Vulkan shutdown */
};

/**
 * @ingroup studioapi_dx11
 * @brief Vulkan Studio API
 */
extern CStudioAPIVk		g_StudioAPIVk;

#endif // !VK_STUDIOAPI_H
