#include "pch_shaderlib.h"
#include "utils/shadercache/shadercache.h"
#include "studiorender/studioapi/istudioapi_barrier.h"
#include "studiorender/istudiorender.h"
#include "modelsystem/ivertexfactory.h"
#include "materialsystem/shaderlib/shader_base.h"
#include "materialsystem/shaderlib/shaderlib.h"

/*
==================
CBasePerMaterialContextData::Update
==================
*/
void CBasePerMaterialContextData::Update( IMaterialVar** pParams )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	OnUpdate( pParams );
	Studio_BeginUpdateResource( this );
}

/*
==================
CBasePerMaterialContextData::R_Barrier
==================
*/
void CBasePerMaterialContextData::R_Barrier( IStudioAPICmdList* pStudioAPICmdList ) const
{
}

/*
==================
CBasePerMaterialContextData::FinalRelease
==================
*/
void CBasePerMaterialContextData::FinalRelease()
{
	if ( IsNeedDeferredDestroy() )
	{
		Studio_BeginDeleteResource( this );
	}
	else
	{
		delete this;
	}
}

/*
==================
CBasePerMaterialContextData::OnUpdate
==================
*/
void CBasePerMaterialContextData::OnUpdate( IMaterialVar** pParams )
{
}

/*
==================
CBaseShader::CBaseShader
==================
*/
CBaseShader::CBaseShader()
{
	GetShaderLib().InsertShader( this );
}

/*
==================
CBaseShader::Init
==================
*/
void CBaseShader::Init( const shaderInitParams_t& shaderInitParams )
{
	// Calculate number pipelines for each shader type and copy index offsets
	PROFILER_SCOPE_FUNC();
	Mem_Memzero( cacheInfos, STUDIOAPI_SHADER_NUM_DRAW_TYPES * sizeof( shaderCacheInfoInternal_t ) );
	uint64 currentScale = 1;
	for ( uint32 shaderTypeIdx = 0; shaderTypeIdx < STUDIOAPI_SHADER_NUM_DRAW_TYPES; ++shaderTypeIdx )
	{
		const shaderCacheInfo_t*   pSrcShaderCacheInfo	= &shaderInitParams.cacheInfos[shaderTypeIdx];
		shaderCacheInfoInternal_t* pDestShaderCacheInfo = &cacheInfos[shaderTypeIdx];

		pDestShaderCacheInfo->indexOffset = pSrcShaderCacheInfo->indexOffset;
		if ( pSrcShaderCacheInfo->numCaches > 0 )
		{
			pDestShaderCacheInfo->bValid = true;
			pDestShaderCacheInfo->scale	 = currentScale;
			currentScale *= pSrcShaderCacheInfo->numCaches;
		}
		else
		{
			pDestShaderCacheInfo->bValid = false;
			pDestShaderCacheInfo->scale	 = 0;
		}
	}

	// Create a pipeline set for the shader
	pStudioRenderPipelineSet = g_pStudioRender->CreateRenderPipelineSet();

	// Initialize a lookup table to find the param index by name
	for ( uint32 frequency = 0; frequency < SHADER_PARAM_NUM_FREQUENCIES; ++frequency )
	{
		paramIndicesDict_t& paramIndicesForFrequencyDict = paramIndicesDict[frequency];
		uint32				numParams					 = GetNumParams( (shaderParamFrequency_t)frequency );
		paramIndicesForFrequencyDict.reserve( numParams );
		for ( uint32 paramIdx = 0; paramIdx < numParams; ++paramIdx )
		{
			const shaderParam_t& param = GetParam( (shaderParamFrequency_t)frequency, paramIdx );
			VerifyMsg( paramIndicesForFrequencyDict.insert( eastl::make_pair( param.pName, paramIdx ) ).second, "More than one param is named '%s', param names are case-insensitive (shader: '%s')", param.pName, GetName() );
		}
	}

	// Initialize the shader instance
	OnInitInstance();

	// Initialize default per-draw vars
	defaultPerDrawVars.resize( GetNumParams( SHADER_PARAM_FREQUENCY_PERDRAW ) );
	if ( !defaultPerDrawVars.empty() )
	{
		InitDefaultParams( defaultPerDrawVars.data() );
	}
}

/*
==================
CBaseShader::OnInitInstance
==================
*/
void CBaseShader::OnInitInstance()
{
}

/*
==================
CBaseShader::OnShutdownInstance
==================
*/
void CBaseShader::OnShutdownInstance()
{
}

/*
==================
CBaseShader::Shutdown
==================
*/
void CBaseShader::Shutdown()
{
	PROFILER_SCOPE_FUNC();
	OnShutdownInstance();

	defaultPerDrawVars.clear();
	for ( uint32 frequency = 0; frequency < SHADER_PARAM_NUM_FREQUENCIES; ++frequency )
	{
		paramIndicesDict_t& paramIndicesForFrequencyDict = paramIndicesDict[(uint32)frequency];
		paramIndicesForFrequencyDict.clear();
	}

	pStudioRenderPipelineSet = NULL;
	Mem_Memzero( cacheInfos, STUDIOAPI_SHADER_NUM_DRAW_TYPES * sizeof( shaderCacheInfoInternal_t ) );
}

/*
==================
CBaseShader::InitDefaultParams
==================
*/
void CBaseShader::InitDefaultParams( IMaterialVar** pParams ) const
{
}

/*
==================
CBaseShader::InitDefaultParams
==================
*/
void CBaseShader::InitDefaultParams( shaderPerDrawVar_t* pParams ) const
{
}

/*
==================
CBaseShader::R_ResolveRenderPipeline
==================
*/
IStudioAPIRenderPipeline* CBaseShader::R_ResolveRenderPipeline( const shaderDrawParams_t& drawParams, studioRenderPassType_t renderPassType )
{
	// Validate the draw params
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	ValidateDrawParams( drawParams );

	// Select a shader combination
	shaderComboInfo_t comboInfo = {};
	R_SelectCombo( drawParams, comboInfo );

	// Get a render pipeline or bake it
	uint64					  pipelineIdx			   = GetPipelineIndex( comboInfo.cacheIndices );
	IStudioAPIRenderPipeline* pStudioAPIRenderPipeline = pStudioRenderPipelineSet->R_GetStudioAPIRenderPipeline( renderPassType, pipelineIdx );
	if ( !pStudioAPIRenderPipeline )
	{
		// Initialize an information about bake the render pipeline
		studioBakeRenderPipelineParams_t studioBakeParams = {};
		studioBakeParams.pipelineIdx					  = pipelineIdx;
		studioBakeParams.renderPassType					  = renderPassType;
		studioBakeParams.pVertexDeclaration				  = drawParams.pVertexFactory ? drawParams.pVertexFactory->GetStudioAPIVertexDeclaration() : NULL;
		for ( uint32 shaderIdx = 0; shaderIdx < STUDIOAPI_SHADER_NUM_DRAW_TYPES; ++shaderIdx )
		{
			const shaderCacheInfoInternal_t& cacheInfo = cacheInfos[shaderIdx];
			if ( cacheInfo.bValid )
			{
				studioBakeParams.pStudioAPIShaders[shaderIdx] = g_pShaderMgr->GetStudioAPIShader( GetShaderLib().GetIndex(),
																								  (studioAPIShaderType_t)shaderIdx,
																								  comboInfo.cacheIndices[shaderIdx] + cacheInfo.indexOffset );
			}
		}

		// Bake the render pipeline
		pStudioAPIRenderPipeline = pStudioRenderPipelineSet->R_BakeRenderPipeline( studioBakeParams );
	}

	// We are done
	Assert( pStudioAPIRenderPipeline );
	return pStudioAPIRenderPipeline;
}

/*
==================
CBaseShader::R_Bind
==================
*/
void CBaseShader::R_Bind( IStudioAPICmdList* pStudioAPICmdList, const shaderDrawParams_t& drawParams )
{
	// Validate the draw params and do binding of shader resources
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	ValidateDrawParams( drawParams );
	R_OnBind( pStudioAPICmdList, drawParams );
}

/*
==================
CBaseShader::ValidateDrawParams
==================
*/
void CBaseShader::ValidateDrawParams( const shaderDrawParams_t& drawParams ) const
{
#if ENABLE_ASSERT
	// Check `pPerMaterialContextData` and `pPerDrawVars` are valid
	AssertMsg( drawParams.pPerMaterialContextData, "Per-material context data isn't supplied (shader: '%s')", GetName() );
	uint32 numPerDrawParams = GetNumParams( SHADER_PARAM_FREQUENCY_PERDRAW );
	if ( numPerDrawParams == 0 )
	{
		return;
	}
	AssertMsg( drawParams.pPerDrawVars, "Declares %i per-draw params but the renderer supplied none (shader: '%s')", numPerDrawParams, GetName() );

	// Catch a partially filled or wrongly ordered array
	for ( uint32 index = 0; index < numPerDrawParams; ++index )
	{
		const shaderParam_t		  param		 = GetParam( SHADER_PARAM_FREQUENCY_PERDRAW, index );
		const shaderPerDrawVar_t& perDrawVar = drawParams.pPerDrawVars[index];
		AssertMsg( perDrawVar.type == param.type, "Per-draw param '%s' (index %i) has type 0x%X, expected 0x%X (shader: '%s')", param.pName, index, perDrawVar.type, param.type, GetName() );
	}
#endif	// ENABLE_ASSERT
}

/*
==================
CBaseShader::R_OnBind
==================
*/
void CBaseShader::R_OnBind( IStudioAPICmdList* pStudioAPICmdList, const shaderDrawParams_t& drawParams )
{
}

/*
==================
CBaseShader::FindParamIndex
==================
*/
uint32 CBaseShader::FindParamIndex( shaderParamFrequency_t frequency, const char* pName ) const
{
	Assert( frequency < SHADER_PARAM_NUM_FREQUENCIES );
	const paramIndicesDict_t& paramIndicesForFrequencyDict = paramIndicesDict[(uint32)frequency];
	auto					  it						   = paramIndicesForFrequencyDict.find( pName );
	return it != paramIndicesForFrequencyDict.end() ? it->second : INVALID_INDEX;
}

/*
==================
CBaseShader::GetDefaultPerDrawVars
==================
*/
const shaderPerDrawVar_t* CBaseShader::GetDefaultPerDrawVars() const
{
	return defaultPerDrawVars.data();
}

/*
==================
CBaseShader::GetFallbackShader
==================
*/
const char* CBaseShader::GetFallbackShader() const
{
	return NULL;
}
