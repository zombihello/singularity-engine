#include "pch_shaderlib.h"
#include "utils/shadercache/shadercache.h"
#include "studiorender/studioapi/istudioapi_barrier.h"
#include "studiorender/istudiorender.h"
#include "materialsystem/shaderlib/shader_base.h"
#include "materialsystem/shaderlib/shaderlib.h"

/*
==================
CBaseShader::CBaseShader
==================
*/
CBaseShader::CBaseShader()
{
	g_ShaderLib.InsertShader( this );
}

/*
==================
CBaseShader::Init
==================
*/
void CBaseShader::Init( const shaderInitParams_t& shaderInitParams )
{
	// Calculate number pipelines for each shader type and copy index offsets
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

	// Initialize the shader instance
	OnInitInstance();
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
CBaseShader::Shutdown
==================
*/
void CBaseShader::Shutdown()
{
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
CBaseShader::R_UpdateBuffers
==================
*/
void CBaseShader::R_UpdateBuffers( IStudioAPICmdContext* pStudioAPICmdContext, CRefPtr<IStudioAPIBuffer>* pStudioAPIBuffers, IMaterialVar** pParams ) const
{
}

/*
==================
CBaseShader::R_Barrier
==================
*/
void CBaseShader::R_Barrier( IStudioAPICmdList* pStudioAPICmdList, IMaterialVar** pParams, IStudioAPIBuffer** pStudioAPIBuffers ) const
{
}

/*
==================
CBaseShader::R_PrepareForDraw
==================
*/
void CBaseShader::R_PrepareForDraw( IStudioAPICmdList* pStudioAPICmdList, studioRenderPassType_t renderPassType, IMaterialVar** pParams, IStudioAPIBuffer** pStudioAPConstantIBuffers /* = NULL */ )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Select a shader combination
	shaderComboInfo_t comboInfo = {};
	R_SelectCombo( pParams, comboInfo );

	// Get a render pipeline or bake it
	uint64					  pipelineIdx			   = GetPipelineIndex( comboInfo.cacheIndices );
	IStudioAPIRenderPipeline* pStudioAPIRenderPipeline = pStudioRenderPipelineSet->R_GetStudioAPIRenderPipeline( renderPassType, pipelineIdx );
	if ( !pStudioAPIRenderPipeline )
	{
		// Initialize an information about bake the render pipeline
		studioBakeRenderPipelineParams_t studioBakeParams = {};
		studioBakeParams.pipelineIdx					  = pipelineIdx;
		studioBakeParams.renderPassType					  = renderPassType;
		studioBakeParams.vertexType						  = comboInfo.vertexType;
		for ( uint32 shaderIdx = 0; shaderIdx < STUDIOAPI_SHADER_NUM_DRAW_TYPES; ++shaderIdx )
		{
			const shaderCacheInfoInternal_t& cacheInfo = cacheInfos[shaderIdx];
			if ( cacheInfo.bValid )
			{
				studioBakeParams.pStudioAPIShaders[shaderIdx] = g_pShaderMgr->GetStudioAPIShader( g_ShaderLib.GetIndex(),
																								  (studioAPIShaderType_t)shaderIdx,
																								  comboInfo.cacheIndices[shaderIdx] + cacheInfo.indexOffset );
			}
		}

		// Bake the render pipeline
		pStudioAPIRenderPipeline = pStudioRenderPipelineSet->R_BakeRenderPipeline( studioBakeParams );
	}

	// Set the render pipeline
	Assert( pStudioAPIRenderPipeline && pStudioAPICmdList );
	pStudioAPICmdList->SetRenderPipeline( pStudioAPIRenderPipeline );

	// Prepare the shader for draw
	R_OnDraw( pStudioAPICmdList, pParams, pStudioAPConstantIBuffers );
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
