#include "shaderc/shaderc.hpp"
#include "spirv_cross/spirv_cross.hpp"
#include "spirv-tools/optimizer.hpp"

#include "utils/interfaces/interfaces.h"
#include "tier1/tier1.h"
#include "tier1/strtools.h"
#include "tier1/filetools.h"
#include "tier1/streamdata_memory.h"
#include "tier0/debug.h"
#include "filesystem/ifilesystem.h"
#include "utils/shadercache/shaderreflection.h"
#include "tools/shadercompiler/ishadercompiler_backend.h"

/*
==================
SpvOptimizer_MessageConsumer
==================
*/
static void SpvOptimizer_MessageConsumer( spv_message_level_t level, const char* pSource, const spv_position_t& position, const char* pMessage )
{
	switch ( level )
	{
	case SPV_MSG_DEBUG:
#if !DEBUG
		break;
#endif	// DEBUG

	case SPV_MSG_INFO:
		Msg( "SpvOptimizer: %s[%i:%i]: %s", pSource, position.line, position.column, pMessage );
		break;

	case SPV_MSG_WARNING:
		Warning( "SpvOptimizer: %s[%i:%i]: %s", pSource, position.line, position.column, pMessage );
		break;

	case SPV_MSG_ERROR:
		Error( "SpvOptimizer: %s[%i:%i]: %s", pSource, position.line, position.column, pMessage );
		break;

	case SPV_MSG_FATAL:
	case SPV_MSG_INTERNAL_ERROR:
		Sys_Error( "SpvOptimizer Fatal Error\n%s[%i:%i]: %s", pSource, position.line, position.column, pMessage );
		break;
	}
}

/*
==================
VK_TranslateShaderVarType
==================
*/
FORCEINLINE shaderReflectionVarType_t VK_TranslateShaderVarType( const spirv_cross::SPIRType& type )
{
	switch ( type.basetype )
	{
	case spirv_cross::SPIRType::Boolean: return SHADER_REFLECTION_VAR_TYPE_BOOL;
	case spirv_cross::SPIRType::Int:
		if ( type.vecsize == 1 )
		{
			return SHADER_REFLECTION_VAR_TYPE_INT;
		}

		if ( type.vecsize == 2 )
		{
			return SHADER_REFLECTION_VAR_TYPE_IVEC2;
		}

		if ( type.vecsize == 3 )
		{
			return SHADER_REFLECTION_VAR_TYPE_IVEC3;
		}

		if ( type.vecsize == 4 )
		{
			return SHADER_REFLECTION_VAR_TYPE_IVEC4;
		}

	case spirv_cross::SPIRType::UInt: return SHADER_REFLECTION_VAR_TYPE_UINT;
	case spirv_cross::SPIRType::Float:
		if ( type.columns == 3 )
		{
			return SHADER_REFLECTION_VAR_TYPE_MAT3;
		}

		if ( type.columns == 4 )
		{
			return SHADER_REFLECTION_VAR_TYPE_MAT3;
		}

		if ( type.vecsize == 1 )
		{
			return SHADER_REFLECTION_VAR_TYPE_FLOAT;
		}

		if ( type.vecsize == 2 )
		{
			return SHADER_REFLECTION_VAR_TYPE_VEC2;
		}

		if ( type.vecsize == 3 )
		{
			return SHADER_REFLECTION_VAR_TYPE_VEC3;
		}

		if ( type.vecsize == 4 )
		{
			return SHADER_REFLECTION_VAR_TYPE_VEC4;
		}
	}

	Sys_Error( "Unknown shader variable type" );
	return SHADER_REFLECTION_VAR_TYPE_NONE;
}

/*
==================
VK_TranslateShaderDimensionType
==================
*/
FORCEINLINE studioAPIShaderDimensionType_t VK_TranslateShaderDimensionType( const spv::Dim& dimensionType )
{
	switch ( dimensionType )
	{
	case spv::Dim1D: return STUDIOAPI_SHADER_IMAGE_DIMENSION_TYPE_1D;
	case spv::Dim2D: return STUDIOAPI_SHADER_IMAGE_DIMENSION_TYPE_2D;
	case spv::Dim3D: return STUDIOAPI_SHADER_IMAGE_DIMENSION_TYPE_3D;
	case spv::DimCube: return STUDIOAPI_SHADER_IMAGE_DIMENSION_TYPE_CUBE;
	case spv::DimRect: return STUDIOAPI_SHADER_IMAGE_DIMENSION_TYPE_RECT;
	case spv::DimBuffer: return STUDIOAPI_SHADER_IMAGE_DIMENSION_TYPE_BUFFER;
	case spv::DimSubpassData: return STUDIOAPI_SHADER_IMAGE_DIMENSION_TYPE_SUBPASS_DATA;
	}

	Sys_Error( "Unknown shader dimension type" );
	return STUDIOAPI_SHADER_IMAGE_DIMENSION_TYPE_1D;
}

//-----------------------------------------------------------------------------
// Vulkan shader compiler
//-----------------------------------------------------------------------------
class CShaderCompilerBackendVk : public IShaderCompilerBackend
{
public:
	// IShaderCompilerBackend interface
	virtual bool Connect( createInterfaceFn_t pFactory ) override;
	virtual void Disconnect() override;

	virtual bool		CompileShader( const char* pSrcFileName, const char* pFunctionName, studioAPIShaderType_t type, IShaderCompilerEnvironment* pEnvironment, IShaderCompilerOutput* pOutput ) override;
	virtual const char* GetShaderPlatform() const override;

private:
	bool GrabReflect( const shaderc::SpvCompilationResult& shadercShaderData, IShaderCompilerOutput* pOutput ) const;

	shaderc::Compiler shadercCompiler; /**< Shaderc compiler */
};

EXPOSE_SINGLE_INTERFACE( CShaderCompilerBackendVk, IShaderCompilerBackend, SHADERCOMPILERBACKEND_INTERFACE_VERSION );

//-----------------------------------------------------------------------------
// An implementation of the shaderc::CompileOptions::IncluderInterface interface to access a IShaderCompilerEnvironment
//-----------------------------------------------------------------------------
class CIncludeEnvironmentVk : public shaderc::CompileOptions::IncluderInterface
{
public:
	CIncludeEnvironmentVk( IShaderCompilerEnvironment* pEnvironment )
		: pEnvironment( pEnvironment )
	{
	}

	shaderc_include_result* GetInclude( const char* pRequestedSource, shaderc_include_type includeType, const char* pRequestingSource, size_t includeDepth )
	{
		// Initialize result information
		shaderc_include_result* pShadercIncludeResult = new shaderc_include_result;
		Mem_Memzero( pShadercIncludeResult, sizeof( shaderc_include_result ) );

		// Try to find file in the file system
		TRefPtr<IStreamDataReader> pFile;

		// For absolute path we simple try to create a reader by the path
		if ( S_IsAbsolutePath( pRequestedSource ) )
		{
			pFile = g_pFileSystem->IsFileExists( pRequestedSource ) ? g_pFileSystem->CreateFileReader( pRequestedSource ) : NULL;
		}
		// Otherwise try to create a reader with using include directories
		else
		{
			for ( uint32 index = 0, count = pEnvironment->GetNumIncludeDirs(); index < count; ++index )
			{
				// Compute the full file path
				eastl::string fullPath = S_Sprintf( "%s/%s", pEnvironment->GetIncludeDir( index ), pRequestedSource );

				// We are found file!
				if ( g_pFileSystem->IsFileExists( fullPath.c_str() ) )
				{
					pFile = g_pFileSystem->CreateFileReader( fullPath.c_str() );
					break;
				}
			}
		}

		// We not found this file :(
		if ( !pFile )
		{
			Error( "ShaderCompilerBackendVk: Not found included shader file '%s'", pRequestedSource );
		}
		// Otherwise load data into pShadercIncludeResult
		else
		{
			// Copy file name
			pShadercIncludeResult->source_name_length = S_Strlen( pRequestedSource );
			pShadercIncludeResult->source_name		  = new char[pShadercIncludeResult->source_name_length + 1];
			Mem_Memzero( (char*)pShadercIncludeResult->source_name, pShadercIncludeResult->source_name_length + 1 );
			S_Strcpy( (char*)pShadercIncludeResult->source_name, pRequestedSource );

			// Copy data of file
			pShadercIncludeResult->content_length = pFile->GetSize();
			pShadercIncludeResult->content		  = new char[pShadercIncludeResult->content_length + 1];
			Mem_Memzero( (char*)pShadercIncludeResult->content, pShadercIncludeResult->content_length + 1 );
			pFile->Read( (char*)pShadercIncludeResult->content, pShadercIncludeResult->content_length );
		}

		return pShadercIncludeResult;
	}

	void ReleaseInclude( shaderc_include_result* pIncludeResult )
	{
		if ( !pIncludeResult )
		{
			return;
		}

		if ( pIncludeResult->source_name )
		{
			delete[] pIncludeResult->source_name;
		}

		if ( pIncludeResult->content )
		{
			delete[] pIncludeResult->content;
		}

		delete pIncludeResult;
	}

private:
	IShaderCompilerEnvironment* pEnvironment;
};

/*
==================
CShaderCompilerBackendVk::Connect
==================
*/
bool CShaderCompilerBackendVk::Connect( createInterfaceFn_t pFactory )
{
	return ConnectTier1( pFactory );
}

/*
==================
CShaderCompilerBackendVk::Disconnect
==================
*/
void CShaderCompilerBackendVk::Disconnect()
{
	DisconnectTier1();
}

/*
==================
CShaderCompilerBackendVk::CompileShader
==================
*/
bool CShaderCompilerBackendVk::CompileShader( const char* pSrcFileName, const char* pFunctionName, studioAPIShaderType_t type, IShaderCompilerEnvironment* pEnvironment, IShaderCompilerOutput* pOutput )
{
	// Load source shader file
	TRefPtr<IStreamDataReader> pShaderFile = g_pFileSystem->CreateFileReader( pSrcFileName );
	if ( !pShaderFile )
	{
		pOutput->SetErrorMsg( S_Sprintf( "Failed to open shader file '%s'", pSrcFileName ).c_str() );
		return false;
	}

	// Read file into memory
	uint64 fileSize = pShaderFile->GetSize();
	byte*  pBuffer	= new byte[fileSize + 1];
	Mem_Memzero( pBuffer, fileSize + 1 );
	pShaderFile->Read( pBuffer, fileSize );

	// Convert shader type to shaderc type
	shaderc_shader_kind shadercShaderKind;
	switch ( type )
	{
	case STUDIOAPI_SHADER_TYPE_VERTEX: shadercShaderKind = shaderc_vertex_shader; break;
	case STUDIOAPI_SHADER_TYPE_HULL: shadercShaderKind = shaderc_tess_control_shader; break;
	case STUDIOAPI_SHADER_TYPE_DOMAIN: shadercShaderKind = shaderc_tess_evaluation_shader; break;
	case STUDIOAPI_SHADER_TYPE_GEOMETRY: shadercShaderKind = shaderc_geometry_shader; break;
	case STUDIOAPI_SHADER_TYPE_PIXEL: shadercShaderKind = shaderc_fragment_shader; break;
	case STUDIOAPI_SHADER_TYPE_COMPUTE: shadercShaderKind = shaderc_compute_shader; break;
	default:
		pOutput->SetErrorMsg( S_Sprintf( "Unknown shader type 0x%X", type ).c_str() );

		// Free allocated memory
		delete[] pBuffer;
		return false;
	}

	// Initialize compile options
	bool					bDebugShader = pEnvironment->GetCompileFlags() & SHADERCOMPILER_DEBUG;
	shaderc::CompileOptions shadercCompileOptions;
	shadercCompileOptions.SetWarningsAsErrors();
	shadercCompileOptions.SetSourceLanguage( shaderc_source_language_hlsl );
	shadercCompileOptions.SetTargetEnvironment( shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3 );
	shadercCompileOptions.SetTargetSpirv( shaderc_spirv_version_1_6 );
	shadercCompileOptions.SetGenerateDebugInfo();
	shadercCompileOptions.SetOptimizationLevel( shaderc_optimization_level_zero );

	// Preprocessing of the shader
	// Initialize environment's defines
	for ( uint32 index = 0, count = pEnvironment->GetNumDefines(); index < count; ++index )
	{
		const char* pName  = NULL;
		const char* pValue = NULL;
		pEnvironment->GetDefine( index, pName, pValue );
		shadercCompileOptions.AddMacroDefinition( pName, pValue );
	}

	// Add platform specific defines
	shadercCompileOptions.AddMacroDefinition( "PLATFORM_VK", "1" );

	// Create class to resolve includes
	CIncludeEnvironmentVk* pIncludeEnvironmentVk = new CIncludeEnvironmentVk( pEnvironment );
	shadercCompileOptions.SetIncluder( std::unique_ptr<CIncludeEnvironmentVk>( pIncludeEnvironmentVk ) );

	// Preprocess shader file
	shaderc::PreprocessedSourceCompilationResult shadercPreprocessedSourceResult = shadercCompiler.PreprocessGlsl( (char*)pBuffer, fileSize, shadercShaderKind, pSrcFileName, shadercCompileOptions );
	if ( shadercPreprocessedSourceResult.GetCompilationStatus() != shaderc_compilation_status_success )
	{
		pOutput->SetErrorMsg( shadercPreprocessedSourceResult.GetErrorMessage().c_str() );

		// Free allocated memory
		delete[] pBuffer;
		return false;
	}

	// Compile shader into SPIRV
	shaderc::SpvCompilationResult shadercSpvCompilationResult = shadercCompiler.CompileGlslToSpv( std::string( shadercPreprocessedSourceResult.begin(), shadercPreprocessedSourceResult.end() ), shadercShaderKind, pSrcFileName, pFunctionName, shadercCompileOptions );
	if ( shadercSpvCompilationResult.GetCompilationStatus() != shaderc_compilation_status_success )
	{
		pOutput->SetErrorMsg( shadercSpvCompilationResult.GetErrorMessage().c_str() );

		// Free allocated memory
		delete[] pBuffer;
		return false;
	}

	// Grab reflection data
	if ( !GrabReflect( shadercSpvCompilationResult, pOutput ) )
	{
		pOutput->SetErrorMsg( "During grabbing the reflection data happened some errors, see logs" );

		// Free allocated memory
		delete[] pBuffer;
		return false;
	}

	// Optimize the shader if we haven't debug flag
	// Strip reflection, debug info, set max optimization level and other thinks
	if ( !bDebugShader )
	{
		std::vector<uint32> optimizedBytecode;
		spvtools::Optimizer spvOptimizer( SPV_ENV_UNIVERSAL_1_6 );
		spvOptimizer.SetMessageConsumer( SpvOptimizer_MessageConsumer );
		spvOptimizer.RegisterPerformancePasses();
		spvOptimizer.RegisterPass( spvtools::CreateStripDebugInfoPass() );
		spvOptimizer.RegisterPass( spvtools::CreateStripNonSemanticInfoPass() );
		if ( !spvOptimizer.Run( shadercSpvCompilationResult.begin(), shadercSpvCompilationResult.end() - shadercSpvCompilationResult.begin(), &optimizedBytecode ) )
		{
			pOutput->SetErrorMsg( "Failed to optimize the shader" );

			// Free allocated memory
			delete[] pBuffer;
			return false;
		}

		pOutput->SetBytecode( (byte*)optimizedBytecode.data(), optimizedBytecode.size() * sizeof( uint32 ) );
	}
	// Otherwise save original bytecode into output interface
	else
	{
		pOutput->SetBytecode(
			(byte*)shadercSpvCompilationResult.begin(),
			( shadercSpvCompilationResult.end() - shadercSpvCompilationResult.begin() ) * sizeof( uint32 ) );
	}

	return true;
}

/*
==================
CShaderCompilerBackendVk::GrabReflect
==================
*/
bool CShaderCompilerBackendVk::GrabReflect( const shaderc::SpvCompilationResult& shadercShaderData, IShaderCompilerOutput* pOutput ) const
{
	// We collect reflection data
	// Constant buffers
	bool						 bResult = true;
	CShaderReflection			 shaderReflection;
	spirv_cross::Compiler		 spirvCrossCompiler( shadercShaderData.begin(), shadercShaderData.end() - shadercShaderData.begin() );
	spirv_cross::ShaderResources spirvCrossShaderResources = spirvCrossCompiler.get_shader_resources();
	for ( uint32 constantBufferIdx = 0, numConstantBuffers = (uint32)spirvCrossShaderResources.uniform_buffers.size(); constantBufferIdx < numConstantBuffers; ++constantBufferIdx )
	{
		// Discard unused buffers
		const spirv_cross::Resource&					   spirvCrossResource	   = spirvCrossShaderResources.uniform_buffers[constantBufferIdx];
		spirv_cross::SmallVector<spirv_cross::BufferRange> spirvCrossActiveBuffers = spirvCrossCompiler.get_active_buffer_ranges( spirvCrossResource.id );
		if ( spirvCrossActiveBuffers.empty() )
		{
			continue;
		}

		eastl::vector<shaderReflectionVar_t> vars;
		const spirv_cross::SPIRType&		 spirvCrossBufferType = spirvCrossCompiler.get_type( spirvCrossResource.base_type_id );
		for ( uint32 index = 0, numMembers = (uint32)spirvCrossBufferType.member_types.size(); index < numMembers; ++index )
		{
			shaderReflectionVar_t& var = vars.emplace_back();
			var.name				   = spirvCrossCompiler.get_member_name( spirvCrossBufferType.self, index ).c_str();
			var.size				   = (uint32)spirvCrossCompiler.get_declared_struct_member_size( spirvCrossBufferType, index );
			var.offset				   = spirvCrossCompiler.type_struct_member_offset( spirvCrossBufferType, index );
			var.type				   = VK_TranslateShaderVarType( spirvCrossCompiler.get_type( spirvCrossBufferType.member_types[index] ) );
		}

		// Make sure that the binding location at the set is free
		uint32 bindingIndex		  = spirvCrossCompiler.get_decoration( spirvCrossResource.id, spv::DecorationBinding );
		uint32 descriptorSetIndex = spirvCrossCompiler.get_decoration( spirvCrossResource.id, spv::DecorationDescriptorSet );
		if ( !shaderReflection.IsFreeBindingIndex( bindingIndex, descriptorSetIndex ) )
		{
			Error( "ShaderCompilerBackendVk: Binding index isn't free (constant buffer: %s, binding index: %i, descriptor set: %i)", spirvCrossResource.name.c_str(), bindingIndex, descriptorSetIndex );
			bResult = false;
			continue;
		}

		shaderReflection.AddConstantBuffer( spirvCrossResource.name.c_str(),
											spirvCrossCompiler.get_declared_struct_size( spirvCrossCompiler.get_type( spirvCrossResource.base_type_id ) ),
											bindingIndex,
											vars.data(),
											(uint32)vars.size(),
											descriptorSetIndex );
	}

	// Storage buffers
	for ( uint32 storageBufferIdx = 0, numStorageBuffers = (uint32)spirvCrossShaderResources.storage_buffers.size(); storageBufferIdx < numStorageBuffers; ++storageBufferIdx )
	{
		// Discard unused buffers
		const spirv_cross::Resource&					   spirvCrossResource	   = spirvCrossShaderResources.uniform_buffers[storageBufferIdx];
		spirv_cross::SmallVector<spirv_cross::BufferRange> spirvCrossActiveBuffers = spirvCrossCompiler.get_active_buffer_ranges( spirvCrossResource.id );
		if ( spirvCrossActiveBuffers.empty() )
		{
			continue;
		}

		// Make sure that the binding location at the set is free
		uint32 bindingIndex		  = spirvCrossCompiler.get_decoration( spirvCrossResource.id, spv::DecorationBinding );
		uint32 descriptorSetIndex = spirvCrossCompiler.get_decoration( spirvCrossResource.id, spv::DecorationDescriptorSet );
		if ( !shaderReflection.IsFreeBindingIndex( bindingIndex, descriptorSetIndex ) )
		{
			Error( "ShaderCompilerBackendVk: Binding index isn't free (storage buffer: %s, binding index: %i, descriptor set: %i)", spirvCrossResource.name.c_str(), bindingIndex, descriptorSetIndex );
			bResult = false;
			continue;
		}

		shaderReflection.AddStorageBuffer( spirvCrossResource.name.c_str(),
										   spirvCrossCompiler.get_declared_struct_size( spirvCrossCompiler.get_type( spirvCrossResource.base_type_id ) ),
										   bindingIndex,
										   descriptorSetIndex );
	}

	// Push constant buffers
	for ( uint32 pushConstantBufferIdx = 0, numPushConstantBuffers = (uint32)spirvCrossShaderResources.push_constant_buffers.size(); pushConstantBufferIdx < numPushConstantBuffers; ++pushConstantBufferIdx )
	{
		const spirv_cross::Resource&		 spirvCrossResource	  = spirvCrossShaderResources.push_constant_buffers[pushConstantBufferIdx];
		const spirv_cross::SPIRType&		 spirvCrossBufferType = spirvCrossCompiler.get_type( spirvCrossResource.base_type_id );
		eastl::vector<shaderReflectionVar_t> vars;
		for ( uint32 index = 0, numMembers = (uint32)spirvCrossBufferType.member_types.size(); index < numMembers; ++index )
		{
			shaderReflectionVar_t& var = vars.emplace_back();
			var.name				   = spirvCrossCompiler.get_member_name( spirvCrossBufferType.self, index ).c_str();
			var.size				   = (uint32)spirvCrossCompiler.get_declared_struct_member_size( spirvCrossBufferType, index );
			var.offset				   = spirvCrossCompiler.type_struct_member_offset( spirvCrossBufferType, index );
			var.type				   = VK_TranslateShaderVarType( spirvCrossCompiler.get_type( spirvCrossBufferType.member_types[index] ) );
		}

		shaderReflection.AddPushConstantBuffer( spirvCrossResource.name.c_str(),
												spirvCrossCompiler.get_declared_struct_size( spirvCrossBufferType ),
												vars.data(),
												(uint32)vars.size() );
	}

	// Sampled images
	for ( uint32 sampledImageIdx = 0, numSampledImages = (uint32)spirvCrossShaderResources.sampled_images.size(); sampledImageIdx < numSampledImages; ++sampledImageIdx )
	{
		// Make sure that the binding location at the set is free
		const spirv_cross::Resource& spirvCrossResource		= spirvCrossShaderResources.sampled_images[sampledImageIdx];
		const spirv_cross::SPIRType& spirvCrossResourceType = spirvCrossCompiler.get_type( spirvCrossResource.type_id );
		uint32						 bindingIndex			= spirvCrossCompiler.get_decoration( spirvCrossResource.id, spv::DecorationBinding );
		uint32						 descriptorSetIndex		= spirvCrossCompiler.get_decoration( spirvCrossResource.id, spv::DecorationDescriptorSet );
		if ( !shaderReflection.IsFreeBindingIndex( bindingIndex, descriptorSetIndex ) )
		{
			Error( "ShaderCompilerBackendVk: Binding index isn't free (sampled image: %s, binding index: %i, descriptor set: %i)", spirvCrossResource.name.c_str(), bindingIndex, descriptorSetIndex );
			bResult = false;
			continue;
		}

		shaderReflection.AddSampledImage( spirvCrossResource.name.c_str(),
										  bindingIndex,
										  descriptorSetIndex,
										  VK_TranslateShaderDimensionType( spirvCrossCompiler.get_type( spirvCrossResource.base_type_id ).image.dim ),
										  !spirvCrossResourceType.array.empty() ? spirvCrossResourceType.array[0] : 1 );
	}

	// Separate images
	for ( uint32 separateImageIdx = 0, numSeparateImages = (uint32)spirvCrossShaderResources.separate_images.size(); separateImageIdx < numSeparateImages; ++separateImageIdx )
	{
		// Make sure that the binding location at the set is free
		const spirv_cross::Resource& spirvCrossResource		= spirvCrossShaderResources.separate_images[separateImageIdx];
		const spirv_cross::SPIRType& spirvCrossResourceType = spirvCrossCompiler.get_type( spirvCrossResource.type_id );
		uint32						 bindingIndex			= spirvCrossCompiler.get_decoration( spirvCrossResource.id, spv::DecorationBinding );
		uint32						 descriptorSetIndex		= spirvCrossCompiler.get_decoration( spirvCrossResource.id, spv::DecorationDescriptorSet );
		if ( !shaderReflection.IsFreeBindingIndex( bindingIndex, descriptorSetIndex ) )
		{
			Error( "ShaderCompilerBackendVk: Binding index isn't free (separate image: %s, binding index: %i, descriptor set: %i)", spirvCrossResource.name.c_str(), bindingIndex, descriptorSetIndex );
			bResult = false;
			continue;
		}

		shaderReflection.AddSeparateImage( spirvCrossResource.name.c_str(),
										   bindingIndex,
										   descriptorSetIndex,
										   VK_TranslateShaderDimensionType( spirvCrossCompiler.get_type( spirvCrossResource.base_type_id ).image.dim ),
										   !spirvCrossResourceType.array.empty() ? spirvCrossResourceType.array[0] : 1 );
	}

	// Separate samplers
	for ( uint32 separateSamplerIdx = 0, numSeparateSamplers = (uint32)spirvCrossShaderResources.separate_samplers.size(); separateSamplerIdx < numSeparateSamplers; ++separateSamplerIdx )
	{
		// Make sure that the binding location at the set is free
		const spirv_cross::Resource& spirvCrossResource		= spirvCrossShaderResources.separate_samplers[separateSamplerIdx];
		const spirv_cross::SPIRType& spirvCrossResourceType = spirvCrossCompiler.get_type( spirvCrossResource.type_id );
		uint32						 bindingIndex			= spirvCrossCompiler.get_decoration( spirvCrossResource.id, spv::DecorationBinding );
		uint32						 descriptorSetIndex		= spirvCrossCompiler.get_decoration( spirvCrossResource.id, spv::DecorationDescriptorSet );
		if ( !shaderReflection.IsFreeBindingIndex( bindingIndex, descriptorSetIndex ) )
		{
			Error( "ShaderCompilerBackendVk: Binding index isn't free (separate sampler: %s, binding index: %i, descriptor set: %i)", spirvCrossResource.name.c_str(), bindingIndex, descriptorSetIndex );
			bResult = false;
			continue;
		}

		shaderReflection.AddSeparateSampler( spirvCrossResource.name.c_str(),
											 bindingIndex,
											 descriptorSetIndex,
											 VK_TranslateShaderDimensionType( spirvCrossCompiler.get_type( spirvCrossResource.base_type_id ).image.dim ),
											 !spirvCrossResourceType.array.empty() ? spirvCrossResourceType.array[0] : 1 );
	}

	// Storage image
	for ( uint32 storageImageIdx = 0, numStorageImages = (uint32)spirvCrossShaderResources.storage_images.size(); storageImageIdx < numStorageImages; ++storageImageIdx )
	{
		// Make sure that the binding location at the set is free
		const spirv_cross::Resource& spirvCrossResource		= spirvCrossShaderResources.storage_images[storageImageIdx];
		const spirv_cross::SPIRType& spirvCrossResourceType = spirvCrossCompiler.get_type( spirvCrossResource.type_id );
		uint32						 bindingIndex			= spirvCrossCompiler.get_decoration( spirvCrossResource.id, spv::DecorationBinding );
		uint32						 descriptorSetIndex		= spirvCrossCompiler.get_decoration( spirvCrossResource.id, spv::DecorationDescriptorSet );
		if ( !shaderReflection.IsFreeBindingIndex( bindingIndex, descriptorSetIndex ) )
		{
			Error( "ShaderCompilerBackendVk: Binding index isn't free (storage image: %s, binding index: %i, descriptor set: %i)", spirvCrossResource.name.c_str(), bindingIndex, descriptorSetIndex );
			bResult = false;
			continue;
		}

		shaderReflection.AddStorageImage( spirvCrossResource.name.c_str(),
										  bindingIndex,
										  descriptorSetIndex,
										  VK_TranslateShaderDimensionType( spirvCrossCompiler.get_type( spirvCrossResource.base_type_id ).image.dim ),
										  !spirvCrossResourceType.array.empty() ? spirvCrossResourceType.array[0] : 1 );
	}

	// Serialize reflection data
	eastl::vector<byte>		reflectionData;
	CStreamDataMemoryWriter streamMemoryWriter( reflectionData );
	shaderReflection.Serialize( &streamMemoryWriter );
	pOutput->SetReflectionData( reflectionData.data(), (uint64)reflectionData.size() );
	return bResult;
}

/*
==================
CShaderCompilerBackendVk::GetShaderPlatform
==================
*/
const char* CShaderCompilerBackendVk::GetShaderPlatform() const
{
	return "vk";
}
