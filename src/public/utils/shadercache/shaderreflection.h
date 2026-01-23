#pragma once
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EASTL/unordered_map.h>

#include "stdlib/types.h"
#include "stdlib/istreamdata.h"
#include "studiorender/studioapi/istudioapi_shader.h"

//-----------------------------------------------------------------------------
// Shader reflection types
//-----------------------------------------------------------------------------
enum shaderReflectionVarType_t
{
	SHADER_REFLECTION_VAR_TYPE_NONE,
	SHADER_REFLECTION_VAR_TYPE_BOOL,
	SHADER_REFLECTION_VAR_TYPE_INT,
	SHADER_REFLECTION_VAR_TYPE_UINT,
	SHADER_REFLECTION_VAR_TYPE_FLOAT,
	SHADER_REFLECTION_VAR_TYPE_VEC2,
	SHADER_REFLECTION_VAR_TYPE_VEC3,
	SHADER_REFLECTION_VAR_TYPE_VEC4,
	SHADER_REFLECTION_VAR_TYPE_MAT3,
	SHADER_REFLECTION_VAR_TYPE_MAT4,
	SHADER_REFLECTION_VAR_TYPE_IVEC2,
	SHADER_REFLECTION_VAR_TYPE_IVEC3,
	SHADER_REFLECTION_VAR_TYPE_IVEC4
};

struct shaderReflectionVar_t
{
	void Serialize( IStreamDataWriter* pStreamWriter );
	void Deserialize( IStreamDataReader* pStreamReader );

	eastl::string			  name;
	shaderReflectionVarType_t type;
	uint32					  size;
	uint64					  offset;
};

struct shaderReflectionConstantBuffer_t
{
	void Serialize( IStreamDataWriter* pStreamWriter );
	void Deserialize( IStreamDataReader* pStreamReader );

	eastl::string						 name;
	uint64								 size;
	uint32								 bindingIndex;
	eastl::vector<shaderReflectionVar_t> vars;
};

struct shaderReflectionPushConstantBuffer_t
{
	void Serialize( IStreamDataWriter* pStreamWriter );
	void Deserialize( IStreamDataReader* pStreamReader );

	eastl::string						 name;
	uint64								 size;
	eastl::vector<shaderReflectionVar_t> vars;
};

struct shaderReflectionPushConstantRange_t
{
	uint64 offset;
	uint64 size;
};

struct shaderReflectionStorageBuffer_t
{
	void Serialize( IStreamDataWriter* pStreamWriter );
	void Deserialize( IStreamDataReader* pStreamReader );

	eastl::string name;
	uint64		  size;
	uint32		  bindingIndex;
};

struct shaderReflectionImageSampler_t
{
	void Serialize( IStreamDataWriter* pStreamWriter );
	void Deserialize( IStreamDataReader* pStreamReader );

	eastl::string				   name;
	uint32						   bindingIndex;
	studioAPIShaderDimensionType_t dimensionType;
	uint32						   arraySize;
};

struct shaderReflectionDescriptorSet_t
{
	void Serialize( IStreamDataWriter* pStreamWriter );
	void Deserialize( IStreamDataReader* pStreamReader );
	bool IsEmpty() const;

	eastl::unordered_map<uint32, shaderReflectionConstantBuffer_t> constantBuffersDict;
	eastl::unordered_map<uint32, shaderReflectionStorageBuffer_t>  storageBuffersDict;
	eastl::unordered_map<uint32, shaderReflectionImageSampler_t>   sampledImagesDict;
	eastl::unordered_map<uint32, shaderReflectionImageSampler_t>   storageImagesDict;
	eastl::unordered_map<uint32, shaderReflectionImageSampler_t>   separateTexturesDict;
	eastl::unordered_map<uint32, shaderReflectionImageSampler_t>   separateSamplersDict;
};

//-----------------------------------------------------------------------------
// Shader reflection
//-----------------------------------------------------------------------------
class CShaderReflection
{
public:
	void Serialize( IStreamDataWriter* pStreamWriter );
	void Deserialize( IStreamDataReader* pStreamReader );
	void Clear();

	void AddConstantBuffer( const char* pName, uint64 size, uint32 bindingIndex, const shaderReflectionVar_t* pVars, uint32 numVars, uint32 descriptorSetIndex );
	void AddStorageBuffer( const char* pName, uint64 size, uint32 bindingIndex, uint32 descriptorSetIndex );
	void AddPushConstantBuffer( const char* pName, uint64 size, const shaderReflectionVar_t* pVars, uint32 numVars );
	void AddSampledImage( const char* pName, uint32 bindingIndex, uint32 descriptorSetIndex, studioAPIShaderDimensionType_t dimensionType, uint32 arraySize );
	void AddSeparateImage( const char* pName, uint32 bindingIndex, uint32 descriptorSetIndex, studioAPIShaderDimensionType_t dimensionType, uint32 arraySize );
	void AddSeparateSampler( const char* pName, uint32 bindingIndex, uint32 descriptorSetIndex, studioAPIShaderDimensionType_t dimensionType, uint32 arraySize );
	void AddStorageImage( const char* pName, uint32 bindingIndex, uint32 descriptorSetIndex, studioAPIShaderDimensionType_t dimensionType, uint32 arraySize );

	bool													   IsFreeBindingIndex( uint32 bindingIndex, uint32 descriptorSetIndex ) const;
	const eastl::vector<shaderReflectionDescriptorSet_t>&	   GetDescriptorSets() const;
	const eastl::vector<shaderReflectionPushConstantBuffer_t>& GetPushConstantBuffers() const;
	const eastl::vector<shaderReflectionPushConstantRange_t>&  GetPushConstantRanges() const;

private:
	eastl::vector<shaderReflectionDescriptorSet_t>		descriptorSets;
	eastl::vector<shaderReflectionPushConstantBuffer_t> pushConstantBuffers;
	eastl::vector<shaderReflectionPushConstantRange_t>	pushConstantRanges;
};

#include "utils/shadercache/shaderreflection.inl"
