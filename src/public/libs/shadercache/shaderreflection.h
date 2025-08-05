#pragma once
#include <string>
#include <vector>
#include <unordered_map>

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

	std::string					name;
	shaderReflectionVarType_t	type;
	uint32						size;
	uint64						offset;
};


struct shaderReflectionConstantBuffer_t
{
	void Serialize( IStreamDataWriter* pStreamWriter );
	void Deserialize( IStreamDataReader* pStreamReader );

	std::string							name;
	uint64								size;
	uint32								bindingIndex;
	std::vector<shaderReflectionVar_t>	vars;
};


struct shaderReflectionPushConstantBuffer_t
{
	void Serialize( IStreamDataWriter* pStreamWriter );
	void Deserialize( IStreamDataReader* pStreamReader );

	std::string							name;
	uint64								size;
	std::vector<shaderReflectionVar_t>	vars;
};


struct shaderReflectionPushConstantRange_t
{
	uint64		offset;
	uint64		size;
};


struct shaderReflectionStorageBuffer_t
{
	void Serialize( IStreamDataWriter* pStreamWriter );
	void Deserialize( IStreamDataReader* pStreamReader );

	std::string		name;
	uint64			size;
	uint32			bindingIndex;
};


struct shaderReflectionImageSampler_t
{
	void Serialize( IStreamDataWriter* pStreamWriter );
	void Deserialize( IStreamDataReader* pStreamReader );

	std::string						name;
	uint32							bindingIndex;
	studioAPIShaderDimensionType_t	dimensionType;
	uint32							arraySize;
};


struct shaderReflectionDescriptorSet_t
{
	void Serialize( IStreamDataWriter* pStreamWriter );
	void Deserialize( IStreamDataReader* pStreamReader );
	bool IsEmpty() const;

	std::unordered_map<uint32, shaderReflectionConstantBuffer_t>	constantBuffersDict;
	std::unordered_map<uint32, shaderReflectionStorageBuffer_t>		storageBuffersDict;
	std::unordered_map<uint32, shaderReflectionImageSampler_t>		sampledImagesDict;
	std::unordered_map<uint32, shaderReflectionImageSampler_t>		storageImagesDict;
	std::unordered_map<uint32, shaderReflectionImageSampler_t>		separateTexturesDict;
	std::unordered_map<uint32, shaderReflectionImageSampler_t>		separateSamplersDict;
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

	void AddConstantBuffer( const achar* pName, uint64 size, uint32 bindingIndex, const shaderReflectionVar_t* pVars, uint32 numVars, uint32 descriptorSetIndex );
	void AddStorageBuffer( const achar* pName, uint64 size, uint32 bindingIndex, uint32 descriptorSetIndex );
	void AddPushConstantBuffer( const achar* pName, uint64 size, const shaderReflectionVar_t* pVars, uint32 numVars );
	void AddSampledImage( const achar* pName, uint32 bindingIndex, uint32 descriptorSetIndex, studioAPIShaderDimensionType_t dimensionType, uint32 arraySize );
	void AddSeparateImage( const achar* pName, uint32 bindingIndex, uint32 descriptorSetIndex, studioAPIShaderDimensionType_t dimensionType, uint32 arraySize );
	void AddSeparateSampler( const achar* pName, uint32 bindingIndex, uint32 descriptorSetIndex, studioAPIShaderDimensionType_t dimensionType, uint32 arraySize );
	void AddStorageImage( const achar* pName, uint32 bindingIndex, uint32 descriptorSetIndex, studioAPIShaderDimensionType_t dimensionType, uint32 arraySize );

	bool IsFreeBindingIndex( uint32 bindingIndex, uint32 descriptorSetIndex ) const;
	const std::vector<shaderReflectionDescriptorSet_t>& GetDescriptorSets() const;
	const std::vector<shaderReflectionPushConstantBuffer_t>& GetPushConstantBuffers() const;
	const std::vector<shaderReflectionPushConstantRange_t>& GetPushConstantRanges() const;

private:
	std::vector<shaderReflectionDescriptorSet_t>		descriptorSets;
	std::vector<shaderReflectionPushConstantBuffer_t>	pushConstantBuffers;
	std::vector<shaderReflectionPushConstantRange_t>	pushConstantRanges;
};

#include "shadercache/shaderreflection.inl"