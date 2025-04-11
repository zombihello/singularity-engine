/**
 * @file
 * @addtogroup shadercache shadercache
 */

#ifndef SHADERREFLECTION_H
#define SHADERREFLECTION_H

#include <string>
#include <vector>
#include <unordered_map>

#include "core/types.h"
#include "stdlib/istreamdata.h"
#include "studiorender/studioapi/istudioapi_shader.h"

/**
 * @ingroup shadercache
 * @brief Shader reflection variable type
 */
enum shaderReflectionVarType_t
{
	SHADER_REFLECTION_VAR_TYPE_NONE,		/**< None */
	SHADER_REFLECTION_VAR_TYPE_BOOL,		/**< Boolean */
	SHADER_REFLECTION_VAR_TYPE_INT,			/**< Integer */
	SHADER_REFLECTION_VAR_TYPE_UINT,		/**< Unsigned integer */
	SHADER_REFLECTION_VAR_TYPE_FLOAT,		/**< Float */
	SHADER_REFLECTION_VAR_TYPE_VEC2,		/**< Vector 2D */
	SHADER_REFLECTION_VAR_TYPE_VEC3,		/**< Vector 3D */
	SHADER_REFLECTION_VAR_TYPE_VEC4,		/**< Vector 4D */
	SHADER_REFLECTION_VAR_TYPE_MAT3,		/**< Matrix 3x3 */
	SHADER_REFLECTION_VAR_TYPE_MAT4,		/**< Matrix 4x4 */
	SHADER_REFLECTION_VAR_TYPE_IVEC2,		/**< Integer vector 2D */
	SHADER_REFLECTION_VAR_TYPE_IVEC3,		/**< Integer vector 3D */
	SHADER_REFLECTION_VAR_TYPE_IVEC4		/**< Integer vector 4D */
};

/**
 * @ingroup shadercache
 * @brief Shader reflection variable
 */
struct shaderReflectionVar_t
{
	/**
	 * @brief Serialize the data into a stream
	 * @param pStreamWriter		Stream data writer
	 */
	void Serialize( IStreamDataWriter* pStreamWriter );

	/**
	 * @brief Deserialize a data from a stream
	 * @param pStreamReader		Stream data reader
	 */
	void Deserialize( IStreamDataReader* pStreamReader );

	std::string					name;		/**< Name */
	shaderReflectionVarType_t	type;		/**< Type */
	uint32						size;		/**< Size in bytes */
	uint64						offset;		/**< Offset in the buffer */
};

/**
 * @ingroup shadercache
 * @brief Shader reflection constant buffer
 */
struct shaderReflectionConstantBuffer_t
{
	/**
	 * @brief Serialize the data into a stream
	 * @param pStreamWriter		Stream data writer
	 */
	void Serialize( IStreamDataWriter* pStreamWriter );

	/**
	 * @brief Deserialize a data from a stream
	 * @param pStreamReader		Stream data reader
	 */
	void Deserialize( IStreamDataReader* pStreamReader );

	std::string							name;			/**< Buffer name */
	uint64								size;			/**< Buffer size */
	uint32								bindingIndex;	/**< Binding index */
	std::vector<shaderReflectionVar_t>	vars;			/**< Variables */
};

/**
 * @ingroup shadercache
 * @brief Shader reflection push constant buffer
 */
struct shaderReflectionPushConstantBuffer_t
{
	/**
	 * @brief Serialize the data into a stream
	 * @param pStreamWriter		Stream data writer
	 */
	void Serialize( IStreamDataWriter* pStreamWriter );

	/**
	 * @brief Deserialize a data from a stream
	 * @param pStreamReader		Stream data reader
	 */
	void Deserialize( IStreamDataReader* pStreamReader );

	std::string							name;			/**< Buffer name */
	uint64								size;			/**< Buffer size */
	std::vector<shaderReflectionVar_t>	vars;			/**< Variables */
};

/**
 * @ingroup shadercache
 * @brief Shader reflection push constant range
 */
struct shaderReflectionPushConstantRange_t
{
	uint64		offset;		/**< Offset */
	uint64		size;		/**< Size */
};

/**
 * @ingroup shadercache
 * @brief Shader reflection storage buffer
 */
struct shaderReflectionStorageBuffer_t
{
	/**
	 * @brief Serialize the data into a stream
	 * @param pStreamWriter		Stream data writer
	 */
	void Serialize( IStreamDataWriter* pStreamWriter );

	/**
	 * @brief Deserialize a data from a stream
	 * @param pStreamReader		Stream data reader
	 */
	void Deserialize( IStreamDataReader* pStreamReader );

	std::string		name;			/**< Buffer name */
	uint64			size;			/**< Buffer size */
	uint32			bindingIndex;	/**< Binding index */
};

/**
 * @ingroup shadercache
 * @brief Shader reflection image sampler
 */
struct shaderReflectionImageSampler_t
{
	/**
	 * @brief Serialize the data into a stream
	 * @param pStreamWriter		Stream data writer
	 */
	void Serialize( IStreamDataWriter* pStreamWriter );

	/**
	 * @brief Deserialize a data from a stream
	 * @param pStreamReader		Stream data reader
	 */
	void Deserialize( IStreamDataReader* pStreamReader );

	std::string						name;			/**< Name */
	uint32							bindingIndex;	/**< Binding index */
	studioAPIShaderDimensionType_t	dimensionType;	/**< Dimension type */
	uint32							arraySize;		/**< Array size */
};

/**
 * @ingroup shadercache
 * @brief Shader reflection descriptor set
 */
struct shaderReflectionDescriptorSet_t
{
	/**
	 * @brief Serialize the data into a stream
	 * @param pStreamWriter		Stream data writer
	 */
	void Serialize( IStreamDataWriter* pStreamWriter );

	/**
	 * @brief Deserialize a data from a stream
	 * @param pStreamReader		Stream data reader
	 */
	void Deserialize( IStreamDataReader* pStreamReader );

	/**
	 * @brief Is empty
	 * @return Return TRUE if the descriptor set is empty, otherwise FALSE
	 */
	FORCEINLINE bool IsEmpty() const
	{
		return	constantBuffersDict.empty() && storageBuffersDict.empty() && 
				sampledImagesDict.empty() && storageImagesDict.empty() && 
				separateTexturesDict.empty() && separateSamplersDict.empty();
	}

	std::unordered_map<uint32, shaderReflectionConstantBuffer_t>	constantBuffersDict;	/**< Constant buffers */
	std::unordered_map<uint32, shaderReflectionStorageBuffer_t>		storageBuffersDict;		/**< Storage buffers */
	std::unordered_map<uint32, shaderReflectionImageSampler_t>		sampledImagesDict;		/**< Sampled images */
	std::unordered_map<uint32, shaderReflectionImageSampler_t>		storageImagesDict;		/**< Storage images */
	std::unordered_map<uint32, shaderReflectionImageSampler_t>		separateTexturesDict;	/**< Separate textures */
	std::unordered_map<uint32, shaderReflectionImageSampler_t>		separateSamplersDict;	/**< Separate samplers */
};

/**
 * @ingroup shadercache
 * @brief Shader reflection
 */
class CShaderReflection
{
public:
	/**
	 * @brief Serialize the data into a stream
	 * @param pStreamWriter		Stream data writer
	 */
	void Serialize( IStreamDataWriter* pStreamWriter );

	/**
	 * @brief Deserialize a data from a stream
	 * @param pStreamReader		Stream data reader
	 */
	void Deserialize( IStreamDataReader* pStreamReader );

	/**
	 * @brief Clear
	 */
	FORCEINLINE void Clear()
	{
		descriptorSets.clear();
		pushConstantBuffers.clear();
		pushConstantRanges.clear();
	}

	/**
	 * @brief Add a constant buffer reflection
	 * @param pName					Buffer name
	 * @param size					Buffer size
	 * @param bindingIndex			Binding index
	 * @param pVars					Variables
	 * @param numVars				Number of variables
	 * @param descriptorSetIndex	Descriptor set index
	 */
	void AddConstantBuffer( const achar* pName, uint64 size, uint32 bindingIndex, const shaderReflectionVar_t* pVars, uint32 numVars, uint32 descriptorSetIndex );

	/**
	 * @brief Add a storage buffer reflection
	 * @param pName					Buffer name
	 * @param size					Buffer size
	 * @param bindingIndex			Binding index
	 * @param pVars					Variables
	 * @param numVars				Number of variables
	 * @param descriptorSetIndex	Descriptor set index
	 */
	void AddStorageBuffer( const achar* pName, uint64 size, uint32 bindingIndex, uint32 descriptorSetIndex );

	/**
	 * @brief Add a push constant buffer
	 * @param pName			Buffer name
	 * @param size			Buffer size
	 * @param pVars			Variables
	 * @param numVars		Number of variables
	 */
	void AddPushConstantBuffer( const achar* pName, uint64 size, const shaderReflectionVar_t* pVars, uint32 numVars );

	/**
	 * @brief Add a sampled image reflection
	 * @param pName					Image name
	 * @param bindingIndex			Binding index
	 * @param descriptorSetIndex	Descriptor set index
	 * @param dimensionType			Dimension type
	 * @param arraySize				Array size
	 */
	void AddSampledImage( const achar* pName, uint32 bindingIndex, uint32 descriptorSetIndex, studioAPIShaderDimensionType_t dimensionType, uint32 arraySize );

	/**
	 * @brief Add a separate image reflection
	 * @param pName					Image name
	 * @param bindingIndex			Binding index
	 * @param descriptorSetIndex	Descriptor set index
	 * @param dimensionType			Dimension type
	 * @param arraySize				Array size
	 */
	void AddSeparateImage( const achar* pName, uint32 bindingIndex, uint32 descriptorSetIndex, studioAPIShaderDimensionType_t dimensionType, uint32 arraySize );

	/**
	 * @brief Add a separate sampler reflection
	 * @param pName					Image name
	 * @param bindingIndex			Binding index
	 * @param descriptorSetIndex	Descriptor set index
	 * @param dimensionType			Dimension type
	 * @param arraySize				Array size
	 */
	void AddSeparateSampler( const achar* pName, uint32 bindingIndex, uint32 descriptorSetIndex, studioAPIShaderDimensionType_t dimensionType, uint32 arraySize );

	/**
	 * @brief Add a storage image reflection
	 * @param pName					Image name
	 * @param bindingIndex			Binding index
	 * @param descriptorSetIndex	Descriptor set index
	 * @param dimensionType			Dimension type
	 * @param arraySize				Array size
	 */
	void AddStorageImage( const achar* pName, uint32 bindingIndex, uint32 descriptorSetIndex, studioAPIShaderDimensionType_t dimensionType, uint32 arraySize );

	/**
	 * @brief Is the binding index at the set free
	 * @param bindingIndex			Binding index
	 * @param descriptorSetIndex	Descriptor set index
	 * @return Return TRUE if the binding index at the set is free, otherwise FALSE
	 */
	FORCEINLINE bool IsFreeBindingIndex( uint32 bindingIndex, uint32 descriptorSetIndex ) const
	{
		// If we haven't the set or it is empty then the binding index is free
		if ( descriptorSetIndex >= ( uint32 )descriptorSets.size() )
		{
			return true;
		}

		const shaderReflectionDescriptorSet_t&	shaderDesciptorSet = descriptorSets[descriptorSetIndex];
		if ( shaderDesciptorSet.IsEmpty() )
		{
			return true;
		}

		// If in the set isn't used the binding index then it is free
		return	shaderDesciptorSet.constantBuffersDict.find( bindingIndex ) == shaderDesciptorSet.constantBuffersDict.end() &&
				shaderDesciptorSet.storageBuffersDict.find( bindingIndex ) == shaderDesciptorSet.storageBuffersDict.end() &&
				shaderDesciptorSet.sampledImagesDict.find( bindingIndex ) == shaderDesciptorSet.sampledImagesDict.end() &&
				shaderDesciptorSet.storageImagesDict.find( bindingIndex ) == shaderDesciptorSet.storageImagesDict.end() &&
				shaderDesciptorSet.separateTexturesDict.find( bindingIndex ) == shaderDesciptorSet.separateTexturesDict.end() &&
				shaderDesciptorSet.separateSamplersDict.find( bindingIndex ) == shaderDesciptorSet.separateSamplersDict.end();
	}

	/**
	 * @brief Get the descriptor sets
	 * @return Return the descriptor sets
	 */
	const std::vector<shaderReflectionDescriptorSet_t>& GetDescriptorSets() const
	{
		return descriptorSets;
	}

	/**
	 * @brief Get the push constant buffers
	 * @return Return the push constant buffers
	 */
	const std::vector<shaderReflectionPushConstantBuffer_t>& GetPushConstantBuffers() const
	{
		return pushConstantBuffers;
	}

	/**
	 * @brief Get the push constant ranges
	 * @return Return the push constant ranges
	 */
	const std::vector<shaderReflectionPushConstantRange_t>& GetPushConstantRanges() const
	{
		return pushConstantRanges;
	}

private:
	std::vector<shaderReflectionDescriptorSet_t>		descriptorSets;			/**< Descriptor sets */
	std::vector<shaderReflectionPushConstantBuffer_t>	pushConstantBuffers;	/**< Push constant buffers */
	std::vector<shaderReflectionPushConstantRange_t>	pushConstantRanges;		/**< Push constant ranges */
};

#endif // !SHADERREFLECTION_H