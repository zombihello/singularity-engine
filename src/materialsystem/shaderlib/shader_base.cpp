#include "pch_shaderlib.h"
#include "utils/shadercache/shadercache.h"
#include "studiorender/studioapi/istudioapi_barrier.h"
#include "studiorender/istudiorender.h"
#include "modelsystem/ivertexfactory.h"
#include "materialsystem/shaderlib/shader_base.h"
#include "materialsystem/shaderlib/shaderlib.h"

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
CBaseShader::OnUpdateContextData
==================
*/
void CBaseShader::OnUpdateContextData( IMaterialVar** pParams, IPerMaterialContextData* pPerMaterialContextData ) const
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
CBaseShader::R_Barrier
==================
*/
void CBaseShader::R_Barrier( IStudioAPICmdList* pStudioAPICmdList, IPerMaterialContextData* pPerMaterialContextData ) const
{
}

/*
==================
CBaseShader::R_ResolveRenderPipeline
==================
*/
IStudioAPIRenderPipeline* CBaseShader::R_ResolveRenderPipeline( IPerMaterialContextData* pPerMaterialContextData, IVertexFactory* pVertexFactory, studioRenderPassType_t renderPassType )
{
	// Select a shader combination
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	shaderComboInfo_t comboInfo = {};
	R_SelectCombo( pPerMaterialContextData, pVertexFactory, comboInfo );

	// Get a render pipeline or bake it
	uint64					  pipelineIdx			   = GetPipelineIndex( comboInfo.cacheIndices );
	IStudioAPIRenderPipeline* pStudioAPIRenderPipeline = pStudioRenderPipelineSet->R_GetStudioAPIRenderPipeline( renderPassType, pipelineIdx );
	if ( !pStudioAPIRenderPipeline )
	{
		// Initialize an information about bake the render pipeline
		studioBakeRenderPipelineParams_t studioBakeParams = {};
		studioBakeParams.pipelineIdx					  = pipelineIdx;
		studioBakeParams.renderPassType					  = renderPassType;
		studioBakeParams.pVertexDeclaration				  = pVertexFactory ? pVertexFactory->GetStudioAPIVertexDeclaration() : NULL;
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
CBaseShader::GetFallbackShader
==================
*/
const char* CBaseShader::GetFallbackShader() const
{
	return NULL;
}
