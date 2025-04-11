/**
 * @file
 * @addtogroup studioapi_vk studioapi_vk
 */

#ifndef VK_STUDIOAPI_SHADER_H
#define VK_STUDIOAPI_SHADER_H

#include "studiorender/studioapi/istudioapi_shader.h"
#include "studiorender/studioapi/vk/vk_studioapi_device.h"
#include "studiorender/studioapi/vk/vk_studioapi_boundshaderstate_cache.h"
#include "studiorender/studioapi/vk/vk_studioapi_descriptorsetslayout.h"

/**
 * @ingroup studioapi_vk
 * @breif Vulkan StudioAPI vertex declaration
 */
class CStudioAPIVertexDeclarationVk : public TRefCounted<IStudioAPIVertexDeclaration>
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param pVertexInputBuffers			Vertex input buffers array
	 * @param numVertexInputBuffers			Vertex input buffers array size
	 * @param pVertexInputAttributes		Vertex input attributes array
	 * @param numVertexInputAttributes		Vertex input attributes array size
	 */
	CStudioAPIVertexDeclarationVk( const studioAPIVertexInputBuffer_t* pVertexInputBuffers, uint32 numVertexInputBuffers, const studioAPIVertexInputAttribute_t* pVertexInputAttributes, uint32 numVertexInputAttributes );

	/**
	 * @brief Get Vulkan pipeline vertex input state info
	 * @return Return Vulkan pipeline vertex input state info
	 */
	FORCEINLINE const VkPipelineVertexInputStateCreateInfo& GetVkPipelineVertexInputStateInfo() const
	{
		return vkPipelineVertexInputStateInfo;
	}

private:
	VkPipelineVertexInputStateCreateInfo			vkPipelineVertexInputStateInfo;		/**< Vulkan vertex input state info */
	std::vector<VkVertexInputBindingDescription>	vkVertexInputBindingDescriptions;	/**< Vulkan vertex input binding descriptions */
	std::vector<VkVertexInputAttributeDescription>	vkVertexInputAttributeDescriptions;	/**< Vulkan vertex input attribute descriptions */
};

/**
 * @ingroup studioapi_vk
 * @brief Base vulkan StudioAPI shader
 */
class CStudioAPIShaderVk : public TRefCounted<IStudioAPIShader>
{
public:
	/**
	 * @brief Constructor
	 * @param type					Shader Type
	 * @param pEntryPointName		Entry point name
	 * @param pBytecode				Shader code
	 * @param bytecodeSize			Shader code size
	 * @param pReflectionData		Reflection data
	 * @param reflectionDataSize	Reflection data size
	 * @param pDebugName			Debug name
	 */
	CStudioAPIShaderVk( studioAPIShaderType_t type, const achar* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const achar* pDebugName = "" );

	/**
	 * @brief Destructor
	 */
	~CStudioAPIShaderVk();

	/**
	 * @brief Get shader type
	 * @return Return shader type (see studioAPIShaderType_t)
	 */
	virtual studioAPIShaderType_t GetType() const override;

	/**
	 * @brief Get Vulkan shader module
	 * @return Return Vulkan shader module. If the shader isn't created return VK_NULL_HANDLE
	 */
	FORCEINLINE VkShaderModule GetVkShaderModule() const
	{
		return vkShaderModule;
	}

	/**
	 * @brief Get Vulkan pipeline shader stage info
	 * @return Return Vulkan pipeline shader stage info
	 */
	FORCEINLINE const VkPipelineShaderStageCreateInfo& GetVkPipelineShaderStageInfo() const
	{
		return vkPipelineShaderStageInfo;
	}

	/**
	 * @brief Get entry point name
	 * @return Return entry point name. If the shader isn't created return empty string
	 */
	FORCEINLINE const std::string& GetEntryPointName() const
	{
		return entryPointName;
	}

	/**
	 * @brief Get the descriptor set layout dictionary
	 * @return Return the descriptor set layout dictionary
	 */
	FORCEINLINE const studioAPIDescriptorSetLayoutVkDict_t& GetDescriptorSetLayoutDict() const
	{
		return descriptorSetLayoutDict;
	}

	/**
	 * @brief Get the Vulkan push constant ranges
	 * @return Return the Vulkan push constant ranges
	 */
	FORCEINLINE const std::vector<VkPushConstantRange>& GetVkPushConstantRanges() const
	{
		return vkPushConstantRanges;
	}

private:
	/**
	 * @brief Delegate called when StudioAPI Vulkan is shutdown
	 * @param pUserData		Pointer to CStudioAPIShaderVk
	 */
	static void OnStudioAPIVkShutdown( void* pUserData );

	studioAPIShaderType_t						type;								/**< Shader type */
	VkShaderModule								vkShaderModule;						/**< Vulkan shader module */
	VkPipelineShaderStageCreateInfo				vkPipelineShaderStageInfo;			/**< Vulkan pipeline shader stage info */
	COnStudioAPIVkShutdown::funcDelegate_t*		pStudioAPIVkShutdownDelegate;		/**< Delegate handle 'on StudioAPI Vulkan shutdown' */
	std::string									entryPointName;						/**< Entry point name */
	studioAPIDescriptorSetLayoutVkDict_t		descriptorSetLayoutDict;			/**< Descriptor set layout dictionary */
	std::vector<VkPushConstantRange>			vkPushConstantRanges;				/**< Vulkan push constant ranges */
};

/**
 * @ingroup studioapi_vk
 * @brief Vertex Vulkan StudioAPI shader
 */
class CStudioAPIVertexShaderVk : public CStudioAPIShaderVk
{
public:
	/**
	 * @brief Constructor
	 * @param pEntryPointName		Entry point name
	 * @param pBytecode				Shader code
	 * @param bytecodeSize			Shader code size
	 * @param pReflectionData		Reflection data
	 * @param reflectionDataSize	Reflection data size
	 * @param pDebugName			Debug name
	 */
	CStudioAPIVertexShaderVk( const achar* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const achar* pDebugName = "" )
		: CStudioAPIShaderVk( STUDIOAPI_SHADER_TYPE_VERTEX, pEntryPointName, pBytecode, bytecodeSize, pReflectionData, reflectionDataSize, pDebugName )
	{}
};

/**
 * @ingroup studioapi_vk
 * @brief Hull Vulkan StudioAPI shader
 */
class CStudioAPIHullShaderVk : public CStudioAPIShaderVk
{
public:
	/**
	 * @brief Constructor
	 * @param pEntryPointName		Entry point name
	 * @param pBytecode				Shader code
	 * @param bytecodeSize			Shader code size
	 * @param pReflectionData		Reflection data
	 * @param reflectionDataSize	Reflection data size
	 * @param pDebugName			Debug name
	 */
	CStudioAPIHullShaderVk( const achar* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const achar* pDebugName = "" )
		: CStudioAPIShaderVk( STUDIOAPI_SHADER_TYPE_HULL, pEntryPointName, pBytecode, bytecodeSize, pReflectionData, reflectionDataSize, pDebugName )
	{}
};

/**
 * @ingroup studioapi_vk
 * @brief Domain Vulkan StudioAPI shader
 */
class CStudioAPIDomainShaderVk : public CStudioAPIShaderVk
{
public:
	/**
	 * @brief Constructor
	 * @param pEntryPointName		Entry point name
	 * @param pBytecode				Shader code
	 * @param bytecodeSize			Shader code size
	 * @param pReflectionData		Reflection data
	 * @param reflectionDataSize	Reflection data size
	 * @param pDebugName			Debug name
	 */
	CStudioAPIDomainShaderVk( const achar* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const achar* pDebugName = "" )
		: CStudioAPIShaderVk( STUDIOAPI_SHADER_TYPE_DOMAIN, pEntryPointName, pBytecode, bytecodeSize, pReflectionData, reflectionDataSize, pDebugName )
	{}
};

/**
 * @ingroup studioapi_vk
 * @brief Geometry Vulkan StudioAPI shader
 */
class CStudioAPIGeometryShaderVk : public CStudioAPIShaderVk
{
public:
	/**
	 * @brief Constructor
	 * @param pEntryPointName		Entry point name
	 * @param pBytecode				Shader code
	 * @param bytecodeSize			Shader code size
	 * @param pReflectionData		Reflection data
	 * @param reflectionDataSize	Reflection data size
	 * @param pDebugName			Debug name
	 */
	CStudioAPIGeometryShaderVk( const achar* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const achar* pDebugName = "" )
		: CStudioAPIShaderVk( STUDIOAPI_SHADER_TYPE_GEOMETRY, pEntryPointName, pBytecode, bytecodeSize, pReflectionData, reflectionDataSize, pDebugName )
	{}
};

/**
 * @ingroup studioapi_vk
 * @brief Pixel Vulkan StudioAPI shader
 */
class CStudioAPIPixelShaderVk : public CStudioAPIShaderVk
{
public:
	/**
	 * @brief Constructor
	 * @param pEntryPointName		Entry point name
	 * @param pBytecode				Shader code
	 * @param bytecodeSize			Shader code size
	 * @param pReflectionData		Reflection data
	 * @param reflectionDataSize	Reflection data size
	 * @param pDebugName			Debug name
	 */
	CStudioAPIPixelShaderVk( const achar* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const achar* pDebugName = "" )
		: CStudioAPIShaderVk( STUDIOAPI_SHADER_TYPE_PIXEL, pEntryPointName, pBytecode, bytecodeSize, pReflectionData, reflectionDataSize, pDebugName )
	{}
};

/**
 * @ingroup studioapi_vk
 * @brief Compute Vulkan StudioAPI shader
 */
class CStudioAPIComputeShaderVk : public CStudioAPIShaderVk
{
public:
	/**
	 * @brief Constructor
	 * @param pEntryPointName		Entry point name
	 * @param pBytecode				Shader code
	 * @param bytecodeSize			Shader code size
	 * @param pReflectionData		Reflection data
	 * @param reflectionDataSize	Reflection data size
	 * @param pDebugName			Debug name
	 */
	CStudioAPIComputeShaderVk( const achar* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const achar* pDebugName = "" )
		: CStudioAPIShaderVk( STUDIOAPI_SHADER_TYPE_COMPUTE, pEntryPointName, pBytecode, bytecodeSize, pReflectionData, reflectionDataSize, pDebugName )
	{}
};

/**
 * @ingroup studioapi_vk
 * @brief Vulkan StudioAPI bound shader state
 */
class CStudioAPIBoundShaderStateVk : public TRefCounted<IStudioAPIBoundShaderState>
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param key					Bound shader state key
	 * @param pVertexDeclaration	Vertex declaration
	 * @param pVertexShader			Vertex shader
	 * @param pPixelShader			Pixel shader
	 * @param pHullShader			Hull shader
	 * @param pDomainShader			Domain shader
	 * @param pGeometryShader		Geometry shader
	 */
	CStudioAPIBoundShaderStateVk( const CStudioAPIBoundShaderStateKeyVk& key, CStudioAPIVertexDeclarationVk* pVertexDeclaration, CStudioAPIVertexShaderVk* pVertexShader, CStudioAPIPixelShaderVk* pPixelShader, CStudioAPIHullShaderVk* pHullShader = NULL, CStudioAPIDomainShaderVk* pDomainShader = NULL, CStudioAPIGeometryShaderVk* pGeometryShader = NULL );

	/**
	 * @brief Destructor
	 */
	virtual ~CStudioAPIBoundShaderStateVk();

	/**
	 * @brief Get vertex declaration
	 * @return Return vertex declaration. If the state hasn't one return NULL
	 */
	virtual IStudioAPIVertexDeclaration* GetVertexDeclaration() const override;

	/**
	 * @brief Get vertex shader
	 * @return Return vertex shader. If the state hasn't one return NULL
	 */
	virtual IStudioAPIShader* GetVertexShader() const override;

	/**
	 * @brief Get pixel shader
	 * @return Return pixel shader. If the state hasn't one return NULL
	 */
	virtual IStudioAPIShader* GetPixelShader() const override;

	/**
	 * @brief Get hull shader
	 * @return Return hull shader. If the state hasn't one return NULL
	 */
	virtual IStudioAPIShader* GetHullShader() const override;

	/**
	 * @brief Get domain shader
	 * @return Return domain shader. If the state hasn't one return NULL
	 */
	virtual IStudioAPIShader* GetDomainShader() const override;

	/**
	 * @brief Get geometry shader
	 * @return Return geometry shader. If the state hasn't one return NULL
	 */
	virtual IStudioAPIShader* GetGeometryShader() const override;

	/**
	 * @brief Get the bound shader state key
	 * @return Return the bound shader state key
	 */
	FORCEINLINE const CStudioAPIBoundShaderStateKeyVk& GetKey() const
	{
		return key;
	}

	/**
	 * @brief Get the descriptor sets layout
	 * @return Return the descriptor sets layout
	 */
	FORCEINLINE const CStudioAPIDescriptorSetsLayoutVk& GetDescriptorSetsLayout() const
	{
		return descriptorSetsLayout;
	}

private:
	/**
	 * @brief Delegate called when StudioAPI Vulkan is shutdown
	 * @param pUserData		Pointer to CStudioAPIBoundShaderStateVk
	 */
	static void OnStudioAPIVkShutdown( void* pUserData );

	CStudioAPIBoundShaderStateKeyVk				key;							/**< Bound shader state key in the cache */
	TRefPtr<CStudioAPIVertexDeclarationVk>		pVertexDeclaration;				/**< Vertex declaration */
	TRefPtr<CStudioAPIVertexShaderVk>			pVertexShader;					/**< Vertex shader */
	TRefPtr<CStudioAPIPixelShaderVk>			pPixelShader;					/**< Pixel shader */
	TRefPtr<CStudioAPIHullShaderVk>				pHullShader;					/**< Hull shader */
	TRefPtr<CStudioAPIDomainShaderVk>			pDomainShader;					/**< Domain shader */
	TRefPtr<CStudioAPIGeometryShaderVk>			pGeometryShader;				/**< Geometry shader */
	CStudioAPIDescriptorSetsLayoutVk			descriptorSetsLayout;			/**< Descriptor sets layout */
	COnStudioAPIVkShutdown::funcDelegate_t*		pStudioAPIVkShutdownDelegate;	/**< Delegate handle 'on StudioAPI Vulkan shutdown' */
};

#endif // !VK_STUDIOAPI_SHADER_H