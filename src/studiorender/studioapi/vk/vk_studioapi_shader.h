#pragma once
#include "studiorender/studioapi/istudioapi_shader.h"
#include "studiorender/studioapi/vk/vk_studioapi_device.h"
#include "studiorender/studioapi/vk/vk_studioapi_boundshaderstate_cache.h"
#include "studiorender/studioapi/vk/vk_studioapi_descriptorsetslayout.h"


//-----------------------------------------------------------------------------
// Vulkan StudioAPI vertex declaration
//-----------------------------------------------------------------------------
class CStudioAPIVertexDeclarationVk : public TRefCounted<IStudioAPIVertexDeclaration>
{
public:
	CStudioAPIVertexDeclarationVk( const studioAPIVertexInputBuffer_t* pVertexInputBuffers, uint32 numVertexInputBuffers, const studioAPIVertexInputAttribute_t* pVertexInputAttributes, uint32 numVertexInputAttributes );

	const VkPipelineVertexInputStateCreateInfo& GetVkPipelineVertexInputStateInfo() const;

private:
	VkPipelineVertexInputStateCreateInfo			vkPipelineVertexInputStateInfo;
	std::vector<VkVertexInputBindingDescription>	vkVertexInputBindingDescriptions;
	std::vector<VkVertexInputAttributeDescription>	vkVertexInputAttributeDescriptions;
};


//-----------------------------------------------------------------------------
// Base vulkan StudioAPI shader
//-----------------------------------------------------------------------------
class CStudioAPIShaderVk : public TRefCounted<IStudioAPIShader>
{
public:
	// IStudioAPIShader interface
	virtual studioAPIShaderType_t GetType() const override;

	CStudioAPIShaderVk( studioAPIShaderType_t type, const achar* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const achar* pDebugName = "" );
	~CStudioAPIShaderVk();


	VkShaderModule GetVkShaderModule() const;
	const VkPipelineShaderStageCreateInfo& GetVkPipelineShaderStageInfo() const;
	const std::string& GetEntryPointName() const;
	const studioAPIDescriptorSetLayoutVkDict_t& GetDescriptorSetLayoutDict() const;
	const std::vector<VkPushConstantRange>& GetVkPushConstantRanges() const;

private:
	static void OnStudioAPIVkShutdown( void* pUserData );

	studioAPIShaderType_t						type;
	VkShaderModule								vkShaderModule;
	VkPipelineShaderStageCreateInfo				vkPipelineShaderStageInfo;
	COnStudioAPIVkShutdown::funcDelegate_t*		pStudioAPIVkShutdownDelegate;
	std::string									entryPointName;
	studioAPIDescriptorSetLayoutVkDict_t		descriptorSetLayoutDict;
	std::vector<VkPushConstantRange>			vkPushConstantRanges;
};


//-----------------------------------------------------------------------------
// Vertex Vulkan StudioAPI shader
//-----------------------------------------------------------------------------
class CStudioAPIVertexShaderVk : public CStudioAPIShaderVk
{
public:
	CStudioAPIVertexShaderVk( const achar* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const achar* pDebugName = "" )
		: CStudioAPIShaderVk( STUDIOAPI_SHADER_TYPE_VERTEX, pEntryPointName, pBytecode, bytecodeSize, pReflectionData, reflectionDataSize, pDebugName )
	{}
};


//-----------------------------------------------------------------------------
// Hull Vulkan StudioAPI shader
//-----------------------------------------------------------------------------
class CStudioAPIHullShaderVk : public CStudioAPIShaderVk
{
public:
	CStudioAPIHullShaderVk( const achar* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const achar* pDebugName = "" )
		: CStudioAPIShaderVk( STUDIOAPI_SHADER_TYPE_HULL, pEntryPointName, pBytecode, bytecodeSize, pReflectionData, reflectionDataSize, pDebugName )
	{}
};


//-----------------------------------------------------------------------------
// Domain Vulkan StudioAPI shader
//-----------------------------------------------------------------------------
class CStudioAPIDomainShaderVk : public CStudioAPIShaderVk
{
public:
	CStudioAPIDomainShaderVk( const achar* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const achar* pDebugName = "" )
		: CStudioAPIShaderVk( STUDIOAPI_SHADER_TYPE_DOMAIN, pEntryPointName, pBytecode, bytecodeSize, pReflectionData, reflectionDataSize, pDebugName )
	{}
};


//-----------------------------------------------------------------------------
// Geometry Vulkan StudioAPI shader
//-----------------------------------------------------------------------------
class CStudioAPIGeometryShaderVk : public CStudioAPIShaderVk
{
public:
	CStudioAPIGeometryShaderVk( const achar* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const achar* pDebugName = "" )
		: CStudioAPIShaderVk( STUDIOAPI_SHADER_TYPE_GEOMETRY, pEntryPointName, pBytecode, bytecodeSize, pReflectionData, reflectionDataSize, pDebugName )
	{}
};


//-----------------------------------------------------------------------------
// Pixel Vulkan StudioAPI shader
//-----------------------------------------------------------------------------
class CStudioAPIPixelShaderVk : public CStudioAPIShaderVk
{
public:
	CStudioAPIPixelShaderVk( const achar* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const achar* pDebugName = "" )
		: CStudioAPIShaderVk( STUDIOAPI_SHADER_TYPE_PIXEL, pEntryPointName, pBytecode, bytecodeSize, pReflectionData, reflectionDataSize, pDebugName )
	{}
};


//-----------------------------------------------------------------------------
// Compute Vulkan StudioAPI shader
//-----------------------------------------------------------------------------
class CStudioAPIComputeShaderVk : public CStudioAPIShaderVk
{
public:
	CStudioAPIComputeShaderVk( const achar* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const achar* pDebugName = "" )
		: CStudioAPIShaderVk( STUDIOAPI_SHADER_TYPE_COMPUTE, pEntryPointName, pBytecode, bytecodeSize, pReflectionData, reflectionDataSize, pDebugName )
	{}
};


//-----------------------------------------------------------------------------
// Vulkan StudioAPI bound shader state
//-----------------------------------------------------------------------------
class CStudioAPIBoundShaderStateVk : public TRefCounted<IStudioAPIBoundShaderState>
{
public:
	// IStudioAPIBoundShaderState interface
	virtual IStudioAPIVertexDeclaration* GetVertexDeclaration() const override;
	virtual IStudioAPIShader* GetVertexShader() const override;
	virtual IStudioAPIShader* GetPixelShader() const override;
	virtual IStudioAPIShader* GetHullShader() const override;
	virtual IStudioAPIShader* GetDomainShader() const override;
	virtual IStudioAPIShader* GetGeometryShader() const override;

	CStudioAPIBoundShaderStateVk( const CStudioAPIBoundShaderStateKeyVk& key, CStudioAPIVertexDeclarationVk* pVertexDeclaration, CStudioAPIVertexShaderVk* pVertexShader, CStudioAPIPixelShaderVk* pPixelShader, CStudioAPIHullShaderVk* pHullShader = NULL, CStudioAPIDomainShaderVk* pDomainShader = NULL, CStudioAPIGeometryShaderVk* pGeometryShader = NULL );
	virtual ~CStudioAPIBoundShaderStateVk();

	const CStudioAPIBoundShaderStateKeyVk& GetKey() const;
	const CStudioAPIDescriptorSetsLayoutVk& GetDescriptorSetsLayout() const;

private:
	static void OnStudioAPIVkShutdown( void* pUserData );

	CStudioAPIBoundShaderStateKeyVk				key;
	TRefPtr<CStudioAPIVertexDeclarationVk>		pVertexDeclaration;
	TRefPtr<CStudioAPIVertexShaderVk>			pVertexShader;
	TRefPtr<CStudioAPIPixelShaderVk>			pPixelShader;
	TRefPtr<CStudioAPIHullShaderVk>				pHullShader;
	TRefPtr<CStudioAPIDomainShaderVk>			pDomainShader;
	TRefPtr<CStudioAPIGeometryShaderVk>			pGeometryShader;
	CStudioAPIDescriptorSetsLayoutVk			descriptorSetsLayout;
	COnStudioAPIVkShutdown::funcDelegate_t*		pStudioAPIVkShutdownDelegate;
};

#include "studiorender/studioapi/vk/vk_studioapi_shader.inl"