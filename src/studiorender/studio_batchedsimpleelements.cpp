#include "pch_studiorender.h"
#include "materialsystem/ishader.h"
#include "materialsystem/imaterialvar.h"
#include "resourcesystem/iresourcesystem.h"
#include "modelsystem/imodelsystem.h"
#include "studiorender/studioapi/istudioapi_cmdlist.h"
#include "studiorender/studio_resourcebindingslots.h"
#include "studiorender/studio_sceneview.h"
#include "studiorender/studiorender.h"
#include "studiorender/studio_batchedsimpleelements.h"

/*
==================
CStudioBatchedSimpleElements::CStudioBatchedSimpleElements
==================
*/
CStudioBatchedSimpleElements::CStudioBatchedSimpleElements()
	: depthTestParamIndex( INVALID_INDEX )
{
}

/*
==================
CStudioBatchedSimpleElements::Init
==================
*/
void CStudioBatchedSimpleElements::Init()
{
	// Create a material for the batches
	PROFILER_SCOPE_FUNC();
	IResourceTypeMgr*	  pMaterialsMgr	 = g_pResourceSystem->GetResourceManagerForType<IMaterial>();
	materialInitialData_t matInitialData = {};
	matInitialData.pShaderName			 = "simpleelements";
	pMaterial							 = pMaterialsMgr->CreateResource( "__simple_element" );
	pMaterial->Init( matInitialData );
	pMaterialResource = pMaterial->GetStudioResource();

	// Precache 'depthtest' parameter index in the shader
	IShader* pShader	= pMaterial->GetShader();
	depthTestParamIndex = pShader->FindParamIndex( SHADER_PARAM_FREQUENCY_PERDRAW, "depthtest" );
	if ( depthTestParamIndex == INVALID_INDEX )
	{
		Warning( "StudioRender: Shader '%s' doesn't declare the per-draw param 'depthtest'", pShader->GetName() );
	}

	// Create a vertex factory for the batches
	pVertexFactory = g_pModelSystem->CreateVertexFactory( MODEL_VERTEXTYPE_SIMPLE );
	pVertexFactory->Init();
}

/*
==================
CStudioBatchedSimpleElements::Shutdown
==================
*/
void CStudioBatchedSimpleElements::Shutdown()
{
	PROFILER_SCOPE_FUNC();
	pVertexFactory->Shutdown();
	for ( uint32 batchType = 0; batchType < BATCH_NUM_TYPES; ++batchType )
	{
		batches[batchType].reset_lose_memory();
	}

	pVertexFactory		= NULL;
	pMaterialResource	= NULL;
	pMaterial			= NULL;
	depthTestParamIndex = INVALID_INDEX;
}

/*
==================
CStudioBatchedSimpleElements::R_AddLine
==================
*/
void CStudioBatchedSimpleElements::R_AddLine( batch_t& batch, const vector3_t& start, const vector3_t& end, const CColor& color )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	modelSimpleVertex_t& startVertex = batch.emplace_back();
	startVertex.position			 = vector4_t( start, 1.f );
	startVertex.texCoord			 = vector2_t( 0.f, 0.f );
	startVertex.color				 = color;

	modelSimpleVertex_t& endVertex = batch.emplace_back();
	endVertex.position			   = vector4_t( end, 1.f );
	endVertex.texCoord			   = vector2_t( 0.f, 0.f );
	endVertex.color				   = color;
}

/*
==================
CStudioBatchedSimpleElements::R_AddBox
==================
*/
void CStudioBatchedSimpleElements::R_AddBox( batch_t& batch, const CAABB& aabb, const CColor& color )
{
	// Expand the box into its eight corners
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	const vector3_t& minLocation = aabb.GetMin();
	const vector3_t& maxLocation = aabb.GetMax();
	vector3_t		 corners[8]	 = {
		vector3_t( minLocation.x, minLocation.y, minLocation.z ),  // 0
		vector3_t( maxLocation.x, minLocation.y, minLocation.z ),  // 1
		vector3_t( maxLocation.x, minLocation.y, maxLocation.z ),  // 2
		vector3_t( minLocation.x, minLocation.y, maxLocation.z ),  // 3
		vector3_t( minLocation.x, maxLocation.y, minLocation.z ),  // 4
		vector3_t( maxLocation.x, maxLocation.y, minLocation.z ),  // 5
		vector3_t( maxLocation.x, maxLocation.y, maxLocation.z ),  // 6
		vector3_t( minLocation.x, maxLocation.y, maxLocation.z )   // 7
	};

	// Bottom face, top face and the vertical edges between them
	for ( uint32 index = 0; index < 4; ++index )
	{
		uint32 nextIndex = ( index + 1 ) % 4;
		R_AddLine( batch, corners[index], corners[nextIndex], color );
		R_AddLine( batch, corners[4 + index], corners[4 + nextIndex], color );
		R_AddLine( batch, corners[index], corners[4 + index], color );
	}
}

/*
==================
CStudioBatchedSimpleElements::R_BuildBatches
==================
*/
void CStudioBatchedSimpleElements::R_BuildBatches( studioSceneView_t* pSceneView )
{
	// Drop the previous frame's buffers instead of reusing them: they point into a frame allocator
	// pool that has already been recycled, so writing into them would corrupt somebody else's data
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( pSceneView );
	for ( uint32 batchType = 0; batchType < BATCH_NUM_TYPES; ++batchType )
	{
		batches[batchType].reset_lose_memory();
	}

	// Turn every primitive into lines
	const studioFrameVector_t<studioSimplePrimitive_t>& debugPrimitives = pSceneView->debugPrimitives;
	for ( uint32 index = 0, count = (uint32)debugPrimitives.size(); index < count; ++index )
	{
		const studioSimplePrimitive_t& debugPrimitive = debugPrimitives[index];
		uint32						   batchId		  = debugPrimitive.bDepthTest && depthTestParamIndex != INVALID_INDEX ? BATCH_TYPE_DEPTHTEST : BATCH_TYPE_NO_DEPTHTEST;
		batch_t&					   batch		  = batches[batchId];
		switch ( debugPrimitive.type )
		{
		case STUDIO_SIMPLE_PRIMITIVE_TYPE_LINE: R_AddLine( batch, debugPrimitive.line.start, debugPrimitive.line.end, debugPrimitive.color ); break;
		case STUDIO_SIMPLE_PRIMITIVE_TYPE_BOX: R_AddBox( batch, debugPrimitive.box.aabb, debugPrimitive.color ); break;
		default:
			AssertMsg( false, "Unknown studio simple primitive type 0x%X", debugPrimitive.type );
			break;
		}
	}
}

/*
==================
CStudioBatchedSimpleElements::R_DrawBatch
==================
*/
void CStudioBatchedSimpleElements::R_DrawBatch( IStudioAPICmdList* pCmdList, studioRenderPassType_t renderPassType, batchType_t batchType )
{
	// Do nothing if the batch is empty
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( batchType < BATCH_NUM_TYPES );
	const batch_t& batch = batches[batchType];
	if ( batch.empty() )
	{
		return;
	}

	// Take the shader's default per-draw vars and override the depth test for the batch
	IShader*			pShader		   = pMaterialResource->GetShader();
	uint32				numPerDrawVars = pShader->GetNumParams( SHADER_PARAM_FREQUENCY_PERDRAW );
	shaderPerDrawVar_t* pPerDrawVars   = (shaderPerDrawVar_t*)Mem_Alloca( numPerDrawVars * sizeof( shaderPerDrawVar_t ) );
	Mem_Memcpy( pPerDrawVars, pShader->GetDefaultPerDrawVars(), numPerDrawVars * sizeof( shaderPerDrawVar_t ) );
	if ( depthTestParamIndex != INVALID_INDEX )
	{
		pPerDrawVars[depthTestParamIndex].SetBoolValue( batchType == BATCH_TYPE_DEPTHTEST );
	}

	// Draw the whole batch
	vertexFactoryStreamUP_t vertexStream = { (byte*)batch.data(), (uint32)batch.size(), sizeof( modelSimpleVertex_t ) };
	shaderDrawParams_t		drawParams	 = { pMaterialResource->GetPerMaterialContextData(), pPerDrawVars, pVertexFactory };
	pCmdList->SetRenderPipeline( pShader->R_ResolveRenderPipeline( drawParams, renderPassType ) );
	pCmdList->SetConstantBuffer( 0, STUDIO_RESOURCE_BINDING_SLOT_GLOBAL_CB, g_StudioRender.GetStudioAPIGlobalConstantBuffer() );
	pVertexFactory->R_BindUP( pCmdList, &vertexStream, 1 );
	pShader->R_Bind( pCmdList, drawParams );
	pCmdList->Draw( 0, (uint32)batch.size() );
}
