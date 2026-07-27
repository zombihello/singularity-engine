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

	// Initialize the default render state
	InitDefaultRenderState();

	// Initialize a lookup table to find the param index by name
	for ( uint32 frequency = 0; frequency < SHADER_PARAM_NUM_FREQUENCIES; ++frequency )
	{
		paramIdsDict_t& paramIdsForFrequencyDict = paramIdsDict[frequency];
		uint32			numParams				 = GetNumParams( (shaderParamFrequency_t)frequency );
		paramIdsForFrequencyDict.reserve( numParams );
		for ( uint32 paramIdx = 0; paramIdx < numParams; ++paramIdx )
		{
			const shaderParam_t& param = GetParam( (shaderParamFrequency_t)frequency, paramIdx );
			VerifyMsg( paramIdsForFrequencyDict.insert( eastl::make_pair( param.pName, paramIdx ) ).second, "More than one param is named '%s', param names are case-insensitive (shader: '%s')", param.pName, GetName() );
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
CBaseShader::InitDefaultRenderState
==================
*/
void CBaseShader::InitDefaultRenderState()
{
	// Create the default render state
	PROFILER_SCOPE_FUNC();
	Assert( renderStates.empty() && renderStatesDict.empty() );
	studioRenderState_t studioDefaultRenderState					  = {};
	studioDefaultRenderState.inputAssemblyState.topology			  = STUDIOAPI_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	studioDefaultRenderState.rasterizerState.fillMode				  = STUDIOAPI_RASTERIZER_FILL_MODE_SOLID;
	studioDefaultRenderState.rasterizerState.cullMode				  = STUDIOAPI_RASTERIZER_CULL_MODE_CW;
	studioDefaultRenderState.rasterizerState.lineWidth				  = 1.f;
	studioDefaultRenderState.rasterizerState.bDepthBiasEnable		  = false;
	studioDefaultRenderState.rasterizerState.depthBiasConstantFactor  = 0.f;
	studioDefaultRenderState.rasterizerState.depthBiasSlopeFactor	  = 0.f;
	studioDefaultRenderState.rasterizerState.depthBiasClamp			  = 0.f;
	studioDefaultRenderState.depthState.bTestEnable					  = true;
	studioDefaultRenderState.depthState.bWriteEnable				  = true;
	studioDefaultRenderState.depthState.bBoundsTestEnable			  = false;
	studioDefaultRenderState.depthState.compareOp					  = STUDIOAPI_COMPARE_OP_LESS;
	studioDefaultRenderState.depthState.minBounds					  = 0.f;
	studioDefaultRenderState.depthState.maxBounds					  = 1.f;
	studioDefaultRenderState.stencilState.bTestEnable				  = false;
	studioDefaultRenderState.stencilState.front						  = studioAPIStencilOpStateInfo_t{ STUDIOAPI_STENCIL_OP_KEEP, STUDIOAPI_STENCIL_OP_KEEP, STUDIOAPI_STENCIL_OP_KEEP, STUDIOAPI_COMPARE_OP_ALWAYS, 0xFF, 0xFF, 0 };
	studioDefaultRenderState.stencilState.back						  = studioDefaultRenderState.stencilState.front;
	studioDefaultRenderState.colorBlendAttachment.bBlendEnable		  = false;
	studioDefaultRenderState.colorBlendAttachment.srcColorBlendFactor = STUDIOAPI_BLEND_FACTOR_ONE;
	studioDefaultRenderState.colorBlendAttachment.dstColorBlendFactor = STUDIOAPI_BLEND_FACTOR_ZERO;
	studioDefaultRenderState.colorBlendAttachment.colorBlendOp		  = STUDIOAPI_BLEND_OP_ADD;
	studioDefaultRenderState.colorBlendAttachment.srcAlphaBlendFactor = STUDIOAPI_BLEND_FACTOR_ONE;
	studioDefaultRenderState.colorBlendAttachment.dstAlphaBlendFactor = STUDIOAPI_BLEND_FACTOR_ZERO;
	studioDefaultRenderState.colorBlendAttachment.alphaBlendOp		  = STUDIOAPI_BLEND_OP_ADD;
	studioDefaultRenderState.colorBlendAttachment.colorWriteMask	  = STUDIOAPI_COLOR_COMPONENT_FLAG_R | STUDIOAPI_COLOR_COMPONENT_FLAG_G | STUDIOAPI_COLOR_COMPONENT_FLAG_B | STUDIOAPI_COLOR_COMPONENT_FLAG_A;
	studioDefaultRenderState.blendConstants							  = vector4_t( 0.f, 0.f, 0.f, 0.f );

	// Let the shader override it
	OnInitDefaultRenderState( studioDefaultRenderState );

	// Add the state into the storage
	renderStates.emplace_back( studioDefaultRenderState );
	renderStatesDict.insert( eastl::make_pair( FastHash( &studioDefaultRenderState, sizeof( studioRenderState_t ) ), 0 ) );
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
CBaseShader::OnInitDefaultRenderState
==================
*/
void CBaseShader::OnInitDefaultRenderState( studioRenderState_t& renderState ) const
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

	renderStates.clear();
	renderStatesDict.clear();
	defaultPerDrawVars.clear();
	for ( uint32 frequency = 0; frequency < SHADER_PARAM_NUM_FREQUENCIES; ++frequency )
	{
		paramIdsDict_t& paramIdsForFrequencyDict = paramIdsDict[(uint32)frequency];
		paramIdsForFrequencyDict.clear();
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
CBaseShader::FindOrCreateRenderState
==================
*/
uint64 CBaseShader::FindOrCreateRenderState( const studioRenderState_t& renderState )
{
	// Try to find already created the render state
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	hash renderStateHash = FastHash( &renderState, sizeof( studioRenderState_t ) );
	auto it				 = renderStatesDict.find( renderStateHash );
	if ( it != renderStatesDict.end() )
	{
		return it->second;
	}

	// Otherwise add the new into the storage
	uint64 renderStateIdx = (uint64)renderStates.size();
	renderStates.emplace_back( renderState );
	renderStatesDict.insert( eastl::make_pair( renderStateHash, renderStateIdx ) );
	return renderStateIdx;
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
	AssertMsg( comboInfo.renderStateIdx < (uint64)renderStates.size(), "Selected render state %i is out of range [0;%i) (shader: '%s')", comboInfo.renderStateIdx, (uint64)renderStates.size(), GetName() );

	// Get a render pipeline or bake it
	uint64					  shaderComboIdx		   = GetShaderComboIndex( comboInfo.cacheIds );
	IStudioAPIRenderPipeline* pStudioAPIRenderPipeline = pStudioRenderPipelineSet->R_GetStudioAPIRenderPipeline( renderPassType, comboInfo.renderStateIdx, shaderComboIdx );
	if ( !pStudioAPIRenderPipeline )
	{
		// Initialize an information about bake the render pipeline
		studioBakeRenderPipelineParams_t studioBakeParams = {};
		studioBakeParams.shaderComboIdx					  = shaderComboIdx;
		studioBakeParams.renderStateIdx					  = comboInfo.renderStateIdx;
		studioBakeParams.renderState					  = renderStates[comboInfo.renderStateIdx];
		studioBakeParams.renderPassType					  = renderPassType;
		studioBakeParams.pVertexDeclaration				  = drawParams.pVertexFactory ? drawParams.pVertexFactory->GetStudioAPIVertexDeclaration() : NULL;
		for ( uint32 shaderIdx = 0; shaderIdx < STUDIOAPI_SHADER_NUM_DRAW_TYPES; ++shaderIdx )
		{
			const shaderCacheInfoInternal_t& cacheInfo = cacheInfos[shaderIdx];
			if ( cacheInfo.bValid )
			{
				studioBakeParams.pStudioAPIShaders[shaderIdx] = g_pShaderMgr->GetStudioAPIShader( GetShaderLib().GetIndex(),
																								  (studioAPIShaderType_t)shaderIdx,
																								  comboInfo.cacheIds[shaderIdx] + cacheInfo.indexOffset );
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
	const paramIdsDict_t& paramIdsForFrequencyDict = paramIdsDict[(uint32)frequency];
	auto				  it					   = paramIdsForFrequencyDict.find( pName );
	return it != paramIdsForFrequencyDict.end() ? it->second : INVALID_INDEX;
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
