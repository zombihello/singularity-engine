#include "core/profile.h"
#include "shadercache/shaderreflection.h"

/*
==================
shaderReflectionVar_t::Serialize
==================
*/
void shaderReflectionVar_t::Serialize( IStreamDataWriter* pStreamWriter )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );
	uint32		nameSize = ( uint32 )name.size();
	pStreamWriter->Write( &nameSize,				sizeof( uint32 ) );
	pStreamWriter->Write( ( byte* )name.c_str(),	nameSize * sizeof( achar ) );
	pStreamWriter->Write( &type,					sizeof( shaderReflectionVarType_t ) );
	pStreamWriter->Write( &size,					sizeof( uint32 ) );
	pStreamWriter->Write( &offset,					sizeof( uint64 ) );
}

/*
==================
shaderReflectionVar_t::Deserialize
==================
*/
void shaderReflectionVar_t::Deserialize( IStreamDataReader* pStreamReader )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );
	uint32		nameSize = 0;
	pStreamReader->Read( &nameSize,		sizeof( uint32 ) );
	name.resize( nameSize );
	pStreamReader->Read( name.data(),	nameSize * sizeof( achar ) );
	pStreamReader->Read( &type,			sizeof( shaderReflectionVarType_t ) );
	pStreamReader->Read( &size,			sizeof( uint32 ) );
	pStreamReader->Read( &offset,		sizeof( uint64 ) );
}


/*
==================
shaderReflectionConstantBuffer_t::Serialize
==================
*/
void shaderReflectionConstantBuffer_t::Serialize( IStreamDataWriter* pStreamWriter )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );
	uint32		nameSize = ( uint32 )name.size();
	pStreamWriter->Write( &nameSize,				sizeof( uint32 ) );
	pStreamWriter->Write( ( byte* )name.c_str(),	nameSize * sizeof( achar ) );
	pStreamWriter->Write( &size,					sizeof( uint64 ) );
	pStreamWriter->Write( &bindingIndex,			sizeof( uint32 ) );

	uint32		numVars = ( uint32 )vars.size();
	pStreamWriter->Write( &numVars, sizeof( uint32 ) );
	for ( uint32 varIdx = 0; varIdx < numVars; ++varIdx )
	{
		vars[varIdx].Serialize( pStreamWriter );
	}
}

/*
==================
shaderReflectionConstantBuffer_t::Deserialize
==================
*/
void shaderReflectionConstantBuffer_t::Deserialize( IStreamDataReader* pStreamReader )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );
	uint32		nameSize = 0;
	pStreamReader->Read( &nameSize,		sizeof( uint32 ) );
	name.resize( nameSize );
	pStreamReader->Read( name.data(),	nameSize * sizeof( achar ) );
	pStreamReader->Read( &size,			sizeof( uint64 ) );
	pStreamReader->Read( &bindingIndex,	sizeof( uint32 ) );

	uint32		numVars = 0;
	pStreamReader->Read( &numVars, sizeof( uint32 ) );
	vars.resize( numVars );
	for ( uint32 varIdx = 0; varIdx < numVars; ++varIdx )
	{
		vars[varIdx].Deserialize( pStreamReader );
	}
}


/*
==================
shaderReflectionPushConstantBuffer_t::Serialize
==================
*/
void shaderReflectionPushConstantBuffer_t::Serialize( IStreamDataWriter* pStreamWriter )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );
	uint32		nameSize = ( uint32 )name.size();
	pStreamWriter->Write( &nameSize,				sizeof( uint32 ) );
	pStreamWriter->Write( ( byte* )name.c_str(),	nameSize * sizeof( achar ) );
	pStreamWriter->Write( &size,					sizeof( uint64 ) );

	uint32		numVars = ( uint32 )vars.size();
	pStreamWriter->Write( &numVars, sizeof( uint32 ) );
	for ( uint32 varIdx = 0; varIdx < numVars; ++varIdx )
	{
		vars[varIdx].Serialize( pStreamWriter );
	}
}

/*
==================
shaderReflectionPushConstantBuffer_t::Deserialize
==================
*/
void shaderReflectionPushConstantBuffer_t::Deserialize( IStreamDataReader* pStreamReader )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );
	uint32		nameSize = 0;
	pStreamReader->Read( &nameSize,		sizeof( uint32 ) );
	name.resize( nameSize );
	pStreamReader->Read( name.data(),	nameSize * sizeof( achar ) );
	pStreamReader->Read( &size,			sizeof( uint64 ) );

	uint32		numVars = 0;
	pStreamReader->Read( &numVars, sizeof( uint32 ) );
	vars.resize( numVars );
	for ( uint32 varIdx = 0; varIdx < numVars; ++varIdx )
	{
		vars[varIdx].Deserialize( pStreamReader );
	}
}


/*
==================
shaderReflectionStorageBuffer_t::Serialize
==================
*/
void shaderReflectionStorageBuffer_t::Serialize( IStreamDataWriter* pStreamWriter )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );
	uint32		nameSize = ( uint32 )name.size();
	pStreamWriter->Write( &nameSize,				sizeof( uint32 ) );
	pStreamWriter->Write( ( byte* )name.c_str(),	nameSize * sizeof( achar ) );
	pStreamWriter->Write( &size,					sizeof( uint64 ) );
	pStreamWriter->Write( &bindingIndex,			sizeof( uint32 ) );
}

/*
==================
shaderReflectionStorageBuffer_t::Deserialize
==================
*/
void shaderReflectionStorageBuffer_t::Deserialize( IStreamDataReader* pStreamReader )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );
	uint32		nameSize = 0;
	pStreamReader->Read( &nameSize,		sizeof( uint32 ) );
	name.resize( nameSize );
	pStreamReader->Read( name.data(),	nameSize * sizeof( achar ) );
	pStreamReader->Read( &size,			sizeof( uint64 ) );
	pStreamReader->Read( &bindingIndex,	sizeof( uint32 ) );
}


/*
==================
shaderReflectionImageSampler_t::Serialize
==================
*/
void shaderReflectionImageSampler_t::Serialize( IStreamDataWriter* pStreamWriter )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );
	uint32		nameSize = ( uint32 )name.size();
	pStreamWriter->Write( &nameSize,				sizeof( uint32 ) );
	pStreamWriter->Write( ( byte* )name.c_str(),	nameSize * sizeof( achar ) );
	pStreamWriter->Write( &bindingIndex,			sizeof( uint32 ) );
	pStreamWriter->Write( &dimensionType,			sizeof( studioAPIShaderDimensionType_t ) );
	pStreamWriter->Write( &arraySize,				sizeof( uint32 ) );
}

/*
==================
shaderReflectionImageSampler_t::Deserialize
==================
*/
void shaderReflectionImageSampler_t::Deserialize( IStreamDataReader* pStreamReader )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );
	uint32		nameSize = 0;
	pStreamReader->Read( &nameSize,			sizeof( uint32 ) );
	name.resize( nameSize );
	pStreamReader->Read( name.data(),		nameSize * sizeof( achar ) );
	pStreamReader->Read( &bindingIndex,		sizeof( uint32 ) );
	pStreamReader->Read( &dimensionType,	sizeof( studioAPIShaderDimensionType_t ) );
	pStreamReader->Read( &arraySize,		sizeof( uint32 ) );
}


/*
==================
shaderReflectionDescriptorSet_t::Serialize
==================
*/
void shaderReflectionDescriptorSet_t::Serialize( IStreamDataWriter* pStreamWriter )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );

	// Save the constant buffer dictionary
	uint32		numConstantBuffers = ( uint32 )constantBuffersDict.size();
	pStreamWriter->Write( &numConstantBuffers, sizeof( uint32 ) );
	for ( auto it = constantBuffersDict.begin(), itEnd = constantBuffersDict.end(); it != itEnd; ++it )
	{
		pStreamWriter->Write( ( byte* )&it->first, sizeof( uint32 ) );
		it->second.Serialize( pStreamWriter );
	}

	// Save the storage buffer dictionary
	uint32		numStorageBuffers = ( uint32 )storageBuffersDict.size();
	pStreamWriter->Write( &numStorageBuffers, sizeof( uint32 ) );
	for ( auto it = storageBuffersDict.begin(), itEnd = storageBuffersDict.end(); it != itEnd; ++it )
	{
		pStreamWriter->Write( ( byte* )&it->first, sizeof( uint32 ) );
		it->second.Serialize( pStreamWriter );
	}

	// Save the sampled images dictionary
	uint32		numSampledImages = ( uint32 )sampledImagesDict.size();
	pStreamWriter->Write( &numSampledImages, sizeof( uint32 ) );
	for ( auto it = sampledImagesDict.begin(), itEnd = sampledImagesDict.end(); it != itEnd; ++it )
	{
		pStreamWriter->Write( ( byte* )&it->first, sizeof( uint32 ) );
		it->second.Serialize( pStreamWriter );
	}

	// Save the storage images dictionary
	uint32		numStorageImages = ( uint32 )storageImagesDict.size();
	pStreamWriter->Write( &numStorageImages, sizeof( uint32 ) );
	for ( auto it = storageImagesDict.begin(), itEnd = storageImagesDict.end(); it != itEnd; ++it )
	{
		pStreamWriter->Write( ( byte* )&it->first, sizeof( uint32 ) );
		it->second.Serialize( pStreamWriter );
	}

	// Save the separate textures dictionary
	uint32		numSeparateTextures = ( uint32 )separateTexturesDict.size();
	pStreamWriter->Write( &numSeparateTextures, sizeof( uint32 ) );
	for ( auto it = separateTexturesDict.begin(), itEnd = separateTexturesDict.end(); it != itEnd; ++it )
	{
		pStreamWriter->Write( ( byte* )&it->first, sizeof( uint32 ) );
		it->second.Serialize( pStreamWriter );
	}

	// Save the separate samplers dictionary
	uint32		numSeparateSamplers = ( uint32 )separateSamplersDict.size();
	pStreamWriter->Write( &numSeparateSamplers, sizeof( uint32 ) );
	for ( auto it = separateSamplersDict.begin(), itEnd = separateSamplersDict.end(); it != itEnd; ++it )
	{
		pStreamWriter->Write( ( byte* )&it->first, sizeof( uint32 ) );
		it->second.Serialize( pStreamWriter );
	}
}

/*
==================
shaderReflectionDescriptorSet_t::Deserialize
==================
*/
void shaderReflectionDescriptorSet_t::Deserialize( IStreamDataReader* pStreamReader )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );

	// Load the constant buffer dictionary
	uint32		numConstantBuffers = 0;
	pStreamReader->Read( &numConstantBuffers, sizeof( uint32 ) );
	for ( uint32 constantBufferIdx = 0; constantBufferIdx < numConstantBuffers; ++constantBufferIdx )
	{
		uint32		bindingIndex = 0;
		pStreamReader->Read( &bindingIndex, sizeof( uint32 ) );
		constantBuffersDict[bindingIndex].Deserialize( pStreamReader );
	}

	// Load the storage buffer dictionary
	uint32		numStorageBuffers = 0;
	pStreamReader->Read( &numStorageBuffers, sizeof( uint32 ) );
	for ( uint32 storageBufferIdx = 0; storageBufferIdx < numStorageBuffers; ++storageBufferIdx )
	{
		uint32		bindingIndex = 0;
		pStreamReader->Read( &bindingIndex, sizeof( uint32 ) );
		storageBuffersDict[bindingIndex].Deserialize( pStreamReader );
	}

	// Load the sampled images dictionary
	uint32		numSampledImages = 0;
	pStreamReader->Read( &numSampledImages, sizeof( uint32 ) );
	for ( uint32 sampledImageIdx = 0; sampledImageIdx < numSampledImages; ++sampledImageIdx )
	{
		uint32		bindingIndex = 0;
		pStreamReader->Read( &bindingIndex, sizeof( uint32 ) );
		sampledImagesDict[bindingIndex].Deserialize( pStreamReader );
	}

	// Load the storage images dictionary
	uint32		numStorageImages = 0;
	pStreamReader->Read( &numStorageImages, sizeof( uint32 ) );
	for ( uint32 storageImageIdx = 0; storageImageIdx < numStorageImages; ++storageImageIdx )
	{
		uint32		bindingIndex = 0;
		pStreamReader->Read( &bindingIndex, sizeof( uint32 ) );
		storageImagesDict[bindingIndex].Deserialize( pStreamReader );
	}

	// Load the separate textures dictionary
	uint32		numSeparateTextures = 0;
	pStreamReader->Read( &numSeparateTextures, sizeof( uint32 ) );
	for ( uint32 separateTextureIdx = 0; separateTextureIdx < numSeparateTextures; ++separateTextureIdx )
	{
		uint32		bindingIndex = 0;
		pStreamReader->Read( &bindingIndex, sizeof( uint32 ) );
		separateTexturesDict[bindingIndex].Deserialize( pStreamReader );
	}

	// Load the separate samplers dictionary
	uint32		numSeparateSamplers = 0;
	pStreamReader->Read( &numSeparateSamplers, sizeof( uint32 ) );
	for ( uint32 separateSamplerIdx = 0; separateSamplerIdx < numSeparateSamplers; ++separateSamplerIdx )
	{
		uint32		bindingIndex = 0;
		pStreamReader->Read( &bindingIndex, sizeof( uint32 ) );
		separateSamplersDict[bindingIndex].Deserialize( pStreamReader );
	}
}


/*
==================
CShaderReflection::Serialize
==================
*/
void CShaderReflection::Serialize( IStreamDataWriter* pStreamWriter )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );

	// Save the descriptor sets
	uint32		numDescriptorSets = ( uint32 )descriptorSets.size();
	pStreamWriter->Write( &numDescriptorSets, sizeof( uint32 ) );
	for ( uint32 descriptorSetIdx = 0; descriptorSetIdx < numDescriptorSets; ++descriptorSetIdx )
	{
		descriptorSets[descriptorSetIdx].Serialize( pStreamWriter );
	}

	// Save the push constant buffers
	uint32		numPushConstantBuffers = ( uint32 )pushConstantBuffers.size();
	pStreamWriter->Write( &numPushConstantBuffers, sizeof( uint32 ) );
	for ( uint32 pushConstantBufferIdx = 0; pushConstantBufferIdx < numPushConstantBuffers; ++pushConstantBufferIdx )
	{
		pushConstantBuffers[pushConstantBufferIdx].Serialize( pStreamWriter );
	}

	// Save the push constant ranges
	uint32		numPushConstantRanges = ( uint32 )pushConstantRanges.size();
	pStreamWriter->Write( &numPushConstantRanges, sizeof( uint32 ) );
	if ( numPushConstantRanges > 0 )
	{
		pStreamWriter->Write( pushConstantRanges.data(), numPushConstantRanges * sizeof( shaderReflectionPushConstantRange_t ) );
	}
}

/*
==================
CShaderReflection::Deserialize
==================
*/
void CShaderReflection::Deserialize( IStreamDataReader* pStreamReader )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );

	// Read descriptor sets
	uint32		numDescriptorSets = 0;
	pStreamReader->Read( &numDescriptorSets, sizeof( uint32 ) );
	descriptorSets.resize( numDescriptorSets );
	for ( uint32 descriptorSetIdx = 0; descriptorSetIdx < numDescriptorSets; ++descriptorSetIdx )
	{
		descriptorSets[descriptorSetIdx].Deserialize( pStreamReader );
	}

	// Read push constant buffers
	uint32		numPushConstantBuffers = 0;
	pStreamReader->Read( &numPushConstantBuffers, sizeof( uint32 ) );
	pushConstantBuffers.resize( numPushConstantBuffers );
	for ( uint32 pushConstantBufferIdx = 0; pushConstantBufferIdx < numPushConstantBuffers; ++pushConstantBufferIdx )
	{
		pushConstantBuffers[pushConstantBufferIdx].Deserialize( pStreamReader );
	}

	// Read push constant ranges
	uint32		numPushConstantRanges = 0;
	pStreamReader->Read( &numPushConstantRanges, sizeof( uint32 ) );
	if ( numPushConstantRanges > 0 )
	{
		pushConstantRanges.resize( numPushConstantRanges );
		pStreamReader->Read( pushConstantRanges.data(), numPushConstantRanges * sizeof( shaderReflectionPushConstantRange_t ) );
	}
}

/*
==================
CShaderReflection::AddConstantBuffer
==================
*/
void CShaderReflection::AddConstantBuffer( const achar* pName, uint64 size, uint32 bindingIndex, const shaderReflectionVar_t* pVars, uint32 numVars, uint32 descriptorSetIndex )
{
	PROFILE_SCOPE();
	if ( descriptorSetIndex >= ( uint32 )descriptorSets.size() )
	{
		descriptorSets.resize( descriptorSetIndex + 1 );
	}

	shaderReflectionDescriptorSet_t&		shaderDesciptorSet = descriptorSets[descriptorSetIndex];
	shaderReflectionConstantBuffer_t&		shaderConstantBuffer = shaderDesciptorSet.constantBuffersDict[bindingIndex];
	shaderConstantBuffer.name				= pName;
	shaderConstantBuffer.size				= size;
	shaderConstantBuffer.bindingIndex		= bindingIndex;
	for ( uint32 index = 0; index < numVars; ++index )
	{
		shaderConstantBuffer.vars.emplace_back( pVars[index] );
	}
}

/*
==================
CShaderReflection::AddStorageBuffer
==================
*/
void CShaderReflection::AddStorageBuffer( const achar* pName, uint64 size, uint32 bindingIndex, uint32 descriptorSetIndex )
{
	PROFILE_SCOPE();
	if ( descriptorSetIndex >= ( uint32 )descriptorSets.size() )
	{
		descriptorSets.resize( descriptorSetIndex + 1 );
	}

	shaderReflectionDescriptorSet_t&		shaderDesciptorSet	= descriptorSets[descriptorSetIndex];
	auto									itFind				= shaderDesciptorSet.storageBuffersDict.find( bindingIndex );
	if ( itFind == shaderDesciptorSet.storageBuffersDict.end() )
	{
		shaderReflectionStorageBuffer_t			shaderStorageBuffer;
		shaderStorageBuffer.name				= pName;
		shaderStorageBuffer.size				= size;
		shaderStorageBuffer.bindingIndex		= bindingIndex;
		shaderDesciptorSet.storageBuffersDict[bindingIndex] = shaderStorageBuffer;
	}
	else if ( size > itFind->second.size )
	{
		itFind->second.size = size;
	}
}

/*
==================
CShaderReflection::AddPushConstantBuffer
==================
*/
void CShaderReflection::AddPushConstantBuffer( const achar* pName, uint64 size, const shaderReflectionVar_t* pVars, uint32 numVars )
{
	PROFILE_SCOPE();
	uint64		bufferOffset = 0;
	if ( !pushConstantRanges.empty() )
	{
		bufferOffset = pushConstantRanges.back().offset + pushConstantRanges.back().size;
	}

	shaderReflectionPushConstantRange_t&	pushConstantRange = pushConstantRanges.emplace_back();
	pushConstantRange.size					= size - bufferOffset;
	pushConstantRange.offset				= bufferOffset;

	shaderReflectionPushConstantBuffer_t&	pushConstantBuffer = pushConstantBuffers.emplace_back();
	pushConstantBuffer.name					= pName;
	pushConstantBuffer.size					= size - bufferOffset;
	for ( uint32 index = 0; index < numVars; ++index )
	{
		const shaderReflectionVar_t&	var = pVars[index];
		shaderReflectionVar_t&			shaderVar = pushConstantBuffer.vars.emplace_back();
		shaderVar.name					= S_Sprintf( "%s.%s", pName, var.name.c_str() );
		shaderVar.size					= var.size;
		shaderVar.offset				= var.offset - bufferOffset;
		shaderVar.type					= var.type;
	}
}

/*
==================
CShaderReflection::AddSampledImage
==================
*/
void CShaderReflection::AddSampledImage( const achar* pName, uint32 bindingIndex, uint32 descriptorSetIndex, studioAPIShaderDimensionType_t dimensionType, uint32 arraySize )
{
	PROFILE_SCOPE();
	if ( descriptorSetIndex >= ( uint32 )descriptorSets.size() )
	{
		descriptorSets.resize( descriptorSetIndex + 1 );
	}

	if ( arraySize == 0 )
	{
		arraySize = 1;
	}

	shaderReflectionDescriptorSet_t&	shaderDesciptorSet = descriptorSets[descriptorSetIndex];
	shaderReflectionImageSampler_t&		shaderImageSampler = shaderDesciptorSet.sampledImagesDict[bindingIndex];
	shaderImageSampler.name				= pName;
	shaderImageSampler.bindingIndex		= bindingIndex;
	shaderImageSampler.dimensionType	= dimensionType;
	shaderImageSampler.arraySize		= arraySize;
}

/*
==================
CShaderReflection::AddSeparateImage
==================
*/
void CShaderReflection::AddSeparateImage( const achar* pName, uint32 bindingIndex, uint32 descriptorSetIndex, studioAPIShaderDimensionType_t dimensionType, uint32 arraySize )
{
	PROFILE_SCOPE();
	if ( descriptorSetIndex >= ( uint32 )descriptorSets.size() )
	{
		descriptorSets.resize( descriptorSetIndex + 1 );
	}

	if ( arraySize == 0 )
	{
		arraySize = 1;
	}

	shaderReflectionDescriptorSet_t&	shaderDesciptorSet = descriptorSets[descriptorSetIndex];
	shaderReflectionImageSampler_t&		shaderImageSampler = shaderDesciptorSet.separateTexturesDict[bindingIndex];
	shaderImageSampler.name				= pName;
	shaderImageSampler.bindingIndex		= bindingIndex;
	shaderImageSampler.dimensionType	= dimensionType;
	shaderImageSampler.arraySize		= arraySize;
}

/*
==================
CShaderReflection::AddSeparateSampler
==================
*/
void CShaderReflection::AddSeparateSampler( const achar* pName, uint32 bindingIndex, uint32 descriptorSetIndex, studioAPIShaderDimensionType_t dimensionType, uint32 arraySize )
{
	PROFILE_SCOPE();
	if ( descriptorSetIndex >= ( uint32 )descriptorSets.size() )
	{
		descriptorSets.resize( descriptorSetIndex + 1 );
	}

	if ( arraySize == 0 )
	{
		arraySize = 1;
	}

	shaderReflectionDescriptorSet_t&	shaderDesciptorSet = descriptorSets[descriptorSetIndex];
	shaderReflectionImageSampler_t&		shaderImageSampler = shaderDesciptorSet.separateSamplersDict[bindingIndex];
	shaderImageSampler.name				= pName;
	shaderImageSampler.bindingIndex		= bindingIndex;
	shaderImageSampler.dimensionType	= dimensionType;
	shaderImageSampler.arraySize		= arraySize;
}

/*
==================
CShaderReflection::AddStorageImage
==================
*/
void CShaderReflection::AddStorageImage( const achar* pName, uint32 bindingIndex, uint32 descriptorSetIndex, studioAPIShaderDimensionType_t dimensionType, uint32 arraySize )
{
	PROFILE_SCOPE();
	if ( descriptorSetIndex >= ( uint32 )descriptorSets.size() )
	{
		descriptorSets.resize( descriptorSetIndex + 1 );
	}

	if ( arraySize == 0 )
	{
		arraySize = 1;
	}

	shaderReflectionDescriptorSet_t&	shaderDesciptorSet = descriptorSets[descriptorSetIndex];
	shaderReflectionImageSampler_t&		shaderImageSampler = shaderDesciptorSet.storageImagesDict[bindingIndex];
	shaderImageSampler.name				= pName;
	shaderImageSampler.bindingIndex		= bindingIndex;
	shaderImageSampler.dimensionType	= dimensionType;
	shaderImageSampler.arraySize		= arraySize;
}