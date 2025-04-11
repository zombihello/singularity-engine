/**
 * @file
 * @addtogroup studioapi studioapi
 */

#ifndef ISTUDIOAPI_H
#define ISTUDIOAPI_H

#include "stdlib/interface.h"
#include "studiorender/studioapi/istudioapi_shader.h"
#include "studiorender/studioapi/istudioapi_renderpipeline.h"
#include "studiorender/studioapi/istudioapi_framebuffer.h"
#include "studiorender/studioapi/istudioapi_swapchain.h"
#include "studiorender/studioapi/istudioapi_renderpass.h"
#include "studiorender/studioapi/istudioapi_buffer.h"
#include "studiorender/studioapi/istudioapi_cmdlist.h"
#include "studiorender/studioapi/istudioapi_texture.h"

/**
 * @ingroup studioapi
 * @brief Studio API GPU vendor id
 */
enum studioAPIGPUVendorId_t
{
	STUDIOAPI_GPU_VENDOR_ID_UNKNOWN		= 0xFFFFFFFF,	/**< Unknown */
	STUDIOAPI_GPU_VENDOR_ID_NOT_QUERIED	= 0,			/**< GPU vendor Id isn't queried */
	STUDIOAPI_GPU_VENDOR_ID_AMD			= 0x1002,		/**< AMD */
	STUDIOAPI_GPU_VENDOR_ID_IMGTEC		= 0x1010,		/**< ImgTec */
	STUDIOAPI_GPU_VENDOR_ID_NVIDIA		= 0x10DE,		/**< Nvidia */
	STUDIOAPI_GPU_VENDOR_ID_ARM			= 0x13B5,		/**< Arm */
	STUDIOAPI_GPU_VENDOR_ID_BROADCOM	= 0x14E4,		/**< Broadcom */
	STUDIOAPI_GPU_VENDOR_ID_QUALCOMM	= 0x5143,		/**< Qualcomm */
	STUDIOAPI_GPU_VENDOR_ID_INTEL		= 0x8086,		/**< Intel */
	STUDIOAPI_GPU_VENDOR_ID_APPLE		= 0x106B,		/**< Apple */
	STUDIOAPI_GPU_VENDOR_ID_VIVANTE		= 0x7a05,		/**< Vivante */
	STUDIOAPI_GPU_VENDOR_ID_VERISILICON = 0x1EB1,		/**< VeriSilicon */
	STUDIOAPI_GPU_VENDOR_ID_SAMSUNG_AMD	= 0x144D,		/**< Samsung AMD */
	STUDIOAPI_GPU_VENDOR_ID_MICROSOFT	= 0x1414,		/**< Microsoft */
	STUDIOAPI_GPU_VENDOR_ID_KAZAN		= 0x10003,		/**< Kazan */
	STUDIOAPI_GPU_VENDOR_ID_CODEPLAY	= 0x10004,		/**< Codeplay */
	STUDIOAPI_GPU_VENDOR_ID_MESA		= 0x10005		/**< Mesa */
};

/**
 * @ingroup studioapi
 * @brief Studio API information
 */
struct studioAPIInfo_t
{
	bool						bSupportMultiBuffer;	/**< Is StudioAPI support a multi buffer */
	studioAPIGPUVendorId_t		gpuVendorId;			/**< GPU vendor id */
	const achar*				pShaderPlatform;		/**< Shader platform name (dx11, vk, etc) */
};

/**
 * @ingroup studioapi
 * @brief Studio API interface version
 */
#define STUDIOAPI_INTERFACE_VERSION "SStudioAPI001"

/**
 * @ingroup studioapi
 * @brief Studio API interface
 */
class IStudioAPI
{
public:
	/**
	 * @brief Initialize Studio API
	 *
	 * @param pFactory	Pointer to interface factory
	 * @return Return TRUE if Studio API is initialized, otherwise returns FALSE
	 */
	virtual bool Init( createInterfaceFn_t pFactory ) = 0;

	/**
	 * @brief Shutdown Studio API
	 */
	virtual void Shutdown() = 0;

	/**
	 * @brief Acquire thread ownership
	 */
	virtual void AcquireThreadOwnership() = 0;

	/**
	 * @brief Release thread ownership
	 */
	virtual void ReleaseThreadOwnership() = 0;

	/**
	 * @brief Create a command context for queue type
	 * @param queueType		GPU queue type
	 * @return Return the created command context for queue type
	 */
	virtual TRefPtr<IStudioAPICmdContext> CreateCmdContext( studioAPIQueueType_t queueType ) const = 0;

	/**
	 * @brief Create a command list for the command context
	 * @param pCmdContext	StudioAPI command context
	 * @return Return the created command list
	 */
	virtual TRefPtr<IStudioAPICmdList> CreateCmdList( IStudioAPICmdContext* pCmdContext ) const = 0;

	/**
	 * @brief Create a command list batch for the command context
	 * @param pCmdContext	StudioAPI command context
	 * @return Return the created command list batch
	 */
	virtual TRefPtr<IStudioAPICmdListBatch> CreateCmdListBatch( IStudioAPICmdContext* pCmdContext ) const = 0;

	/**
	 * @brief Create a swap chain
	 * @param windowHandle	OS window handle
	 * @param width			Window width
	 * @param height		Window height
	 * @param bUseVSync		Is it necessary to use VSync
	 * @return Return the created swap chain
	 */
	virtual TRefPtr<IStudioAPISwapChain> CreateSwapChain( windowHandle_t windowHandle, uint32 width, uint32 height, bool bUseVSync = false ) const = 0;

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
	virtual TRefPtr<IStudioAPIShader> CreateVertexShader( const achar* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const achar* pDebugName = "" ) const = 0;

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
	virtual TRefPtr<IStudioAPIShader> CreateHullShader( const achar* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const achar* pDebugName = "" ) const = 0;

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
	virtual TRefPtr<IStudioAPIShader> CreateDomainShader( const achar* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const achar* pDebugName = "" ) const = 0;

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
	virtual TRefPtr<IStudioAPIShader> CreateGeometryShader( const achar* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const achar* pDebugName = "" ) const = 0;

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
	virtual TRefPtr<IStudioAPIShader> CreatePixelShader( const achar* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const achar* pDebugName = "" ) const = 0;

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
	virtual TRefPtr<IStudioAPIShader> CreateComputeShader( const achar* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const achar* pDebugName = "" ) const = 0;

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
	virtual TRefPtr<IStudioAPIBoundShaderState> FindOrCreateBoundShaderState( IStudioAPIVertexDeclaration* pVertexDeclaration, IStudioAPIShader* pVertexShader, IStudioAPIShader* pPixelShader, IStudioAPIShader* pHullShader = NULL, IStudioAPIShader* pDomainShader = NULL, IStudioAPIShader* pGeometryShader = NULL ) const = 0;

	/**
	 * @brief Create render pipeline
	 * @param createInfo	Information to create a render pipeline
	 * @param pDebugName	Debug name
	 * @return Return created the render pipeline. If failed return NULL
	 */
	virtual TRefPtr<IStudioAPIRenderPipeline> CreateRenderPipeline( const studioAPIRenderPipelineCreateInfo_t& createInfo, const achar* pDebugName = "" ) const = 0;

	/**
	 * @brief Create a vertex declaration
	 * @param pVertexInputBuffers			Vertex input buffers array
	 * @param numVertexInputBuffers			Vertex input buffers array size
	 * @param pVertexInputAttributes		Vertex input attributes array
	 * @param numVertexInputAttributes		Vertex input attributes array size
	 * @return Return created a vertex declaration. If failed return NULL
	 */
	virtual TRefPtr<IStudioAPIVertexDeclaration> CreateVertexDeclaration( const studioAPIVertexInputBuffer_t* pVertexInputBuffers, uint32 numVertexInputBuffers, const studioAPIVertexInputAttribute_t* pVertexInputAttributes, uint32 numVertexInputAttributes ) const = 0;

	/**
	 * @brief Create frame buffer
	 * @param createInfo	Information to create a frame buffer
	 * @param pDebugName	Debug name
	 * @return Return created the frame buffer. If failed return NULL
	 */
	virtual TRefPtr<IStudioAPIFrameBuffer> CreateFrameBuffer( const studioAPIFrameBufferCreateInfo_t& createInfo, const achar* pDebugName = "" ) const = 0;

	/**
	 * @brief Create a render pass
	 * @param createInfo	Information to create a render pass
	 * @param pDebugName	Debug name
	 * @return Return created the render pipeline. If failed return NULL
	 */
	virtual TRefPtr<IStudioAPIRenderPass> CreateRenderPass( const studioAPIRenderPassCreateInfo_t& createInfo, const achar* pDebugName = "" ) const = 0;

	/**
	 * @brief Create a buffer
	 * @param pData			Buffer data
	 * @param dataSize		Data size in bytes
	 * @param dataStride	Data stride
	 * @param usageFlags	Usage flags (see studioAPIBufferUsageFlag_t)
	 * @param pDebugName	Debug name
	 * @return Return created the buffer. If failed return NULL
	 */
	virtual TRefPtr<IStudioAPIBuffer> CreateBuffer( const byte* pData, uint64 dataSize, uint32 dataStride, uint32 usageFlags, const achar* pDebugName = "" ) const = 0;

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
	virtual TRefPtr<IStudioAPITexture> CreateTexture( studioAPITextureType_t type, uint32 sizeX, uint32 sizeY, uint32 sizeZ, uint32 numLayers, uint32 numMips, uint32 usageFlags, studioAPIPixelFormat_t pixelFormat, const byte* pData = NULL, const achar* pDebugName = "" ) const = 0;

	/**
	 * @brief Create a sampler
	 * param createInfo		Information to create a sampler
	 * @param pDebugName	Debug name
	 * @return Return created the sampler. If failed return NULL
	 */
	virtual TRefPtr<IStudioAPISampler> CreateSampler( const studioAPISamplerCreateInfo_t& createInfo, const achar* pDebugName = "" ) const = 0;

	/**
	 * @brief Begin drawing a new frame
	 */
	virtual void BeginDrawingFrame() = 0;

	/**
	 * @brief End drawing the frame
	 */
	virtual void EndDrawingFrame() = 0;

	/**
	 * @brief Submit a command list batch
	 * @warning You can't submit one command list batch twice at one frame

	 * @param pCmdListBatch		StudioAPI command list batch to submit
	 * @param bWait				Is need wait the command list batch when it will be done on GPU
	 * @param waitTime			Wait time in nanoseconds. Set 0 for infinity wait
	 */
	virtual void SubmitCmdListBatch( IStudioAPICmdListBatch* pCmdListBatch, bool bWait = false, uint64 waitTime = 0 ) = 0;

	/**
	 * @brief Wait the command list batch when it will be done on GPU
	 * @param waitTime		Wait time in nanoseconds. Set 0 for infinity wait
	 * @return Return TRUE if the command list batch has been done, otherwise FALSE
	 */
	virtual bool WaitCmdListBatch( IStudioAPICmdListBatch* pCmdListBatch, uint64 waitTime = 0 ) = 0;

	/**
	 * @brief Get StudioAPI information
	 * @return Return StudioAPI information
	 */
	virtual const studioAPIInfo_t& GetInfo() const = 0;

	/**
	 * @brief Get the current frame number
	 * @return Return the current frame number
	 */
	virtual uint64 GetFrameNumber() const = 0;

	/**
	 * @brief Get the immediate command context for queue type
	 * @param queueType		GPU queue type
	 * @return Return the immediate command context for queue type
	 */
	virtual IStudioAPICmdContext* GetImmediateCmdContext( studioAPIQueueType_t queueType ) const = 0;
};

#endif // !ISTUDIOAPI_H