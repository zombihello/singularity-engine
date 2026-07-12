#include "pch_materialsystem.h"
#include "studiorender/studioapi/istudioapi_cmdlist.h"
#include "materialsystem/shaderlib/shader_base.h"
#include "materialsystem/imaterialvar.h"
#include "materialsystem/wireframe_vs_vertex.gen.h"
#include "materialsystem/wireframe_ps_pixel.gen.h"

BEGIN_SHADER( Wireframe, "Default shader" )
	BEGIN_SHADER_CACHES
		SHADER_CACHE( "wireframe_vs" )
		SHADER_CACHE( "wireframe_ps" )
	END_SHADER_CACHES

	BEGIN_SHADER_BUFFER_DATA( buffer0 )
		vector4_t color;
	END_SHADER_BUFFER_DATA

	BEGIN_SHADER_RESOURCES
		SHADER_BUFFER( RES_BUFFER0, 0, 1, buffer0, STUDIOAPI_BUFFER_USAGE_FLAG_STATIC | STUDIOAPI_BUFFER_USAGE_FLAG_CONSTANT_BUFFER | STUDIOAPI_BUFFER_USAGE_FLAG_TRANSFER_DST )
	END_SHADER_RESOURCES

	BEGIN_SHADER_PARAMS
		SHADER_PARAM( COLOR, SHADER_PARAM_TYPE_VECTOR_4D, "Color" )
	END_SHADER_PARAMS

	BEGIN_SHADER_CONTEXT_DATA
		DECLARE_SHADER_BUFFER_DATA( buffer0 );
		CRefPtr<IStudioAPIBuffer> pStudioAPIBuffer0;

		SHADER_CONTEXT_DATA_INIT_STUDIO_API
		{
			pStudioAPIBuffer0 = RES_BUFFER0.CreateBuffer( (byte*)&buffer0 );
		}

		SHADER_CONTEXT_DATA_UPDATE_STUDIO_API
		{
			IStudioAPICmdContext* pStudioAPICmdContext = g_pStudioAPI->GetImmediateCmdContext( STUDIOAPI_QUEUE_TYPE_GRAPHICS );
			RES_BUFFER0.UpdateBuffer( pStudioAPICmdContext, (byte*)&buffer0, pStudioAPIBuffer0 );
		}

		SHADER_CONTEXT_DATA_RELEASE_STUDIO_API
		{
			pStudioAPIBuffer0 = NULL;
		}
	END_SHADER_CONTEXT_DATA

	SHADER_INIT_PARAMS
	{
		pParams[COLOR]->SetVecValue( vector4_t( 1.f, 1.f, 1.f, 1.f ) );
	}

	SHADER_UPDATE_CONTEXT_DATA
	{
		DECLARE_SHADER_CONTEXT_DATA( pWireframeContextData );
		pParams[COLOR]->GetVecValue( &pWireframeContextData->buffer0.color.x, 4 );
	}

	SHADER_BARRIER
	{
		// TODO BS yehor.pohuliaka - Add the ability to get the queue type from IStudioAPICmdList
		DECLARE_SHADER_CONTEXT_DATA( pWireframeContextData );
		studioAPIBarrier_t studioAPIBarriers[] = {
			StudioAPI_MakeBufferBarrier( pWireframeContextData->pStudioAPIBuffer0, STUDIOAPI_BUFFER_STATE_CONSTANT_BUFFER, STUDIOAPI_QUEUE_TYPE_GRAPHICS )
		};
		pStudioAPICmdList->Barrier( studioAPIBarriers, ARRAYSIZE( studioAPIBarriers ) );
	}

	SHADER_SELECT_COMBO
	{
		// Set a vertex shader for the model's vertex factory
		DECLARE_VERTEX_SHADER( wireframe_vs );
		SET_VERTEX_FACTORY( wireframe_vs, pVertexFactory );
		SET_VERTEX_SHADER( wireframe_vs );

		// Set a pixel shader
		DECLARE_PIXEL_SHADER( wireframe_ps );
		SET_PIXEL_SHADER( wireframe_ps );
	}

	SHADER_DRAW
	{
		DECLARE_SHADER_CONTEXT_DATA( pWireframeContextData );
		RES_BUFFER0.SetConstantBuffer( pStudioAPICmdList, pWireframeContextData->pStudioAPIBuffer0 );
	}
END_SHADER
