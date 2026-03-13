#include "pch_materialsystem.h"
#include "studiorender/studioapi/istudioapi_cmdlist.h"
#include "materialsystem/shaderlib/shader_base.h"
#include "materialsystem/imaterialvar.h"
#include "materialsystem/wireframe_vs_vertex.gen.h"
#include "materialsystem/wireframe_ps_pixel.gen.h"

struct shaderParamsBuffer_t
{
	vector4_t color;
};

BEGIN_SHADER( Wireframe, "Default shader" )
	BEGIN_SHADER_CACHES
		SHADER_CACHE( "wireframe_vs" )
		SHADER_CACHE( "wireframe_ps" )
	END_SHADER_CACHES

	BEGIN_SHADER_RESOURCES
		SHADER_BUFFER( RES_BUFFER0, 0, 1, shaderParamsBuffer_t, STUDIOAPI_BUFFER_USAGE_FLAG_STATIC | STUDIOAPI_BUFFER_USAGE_FLAG_CONSTANT_BUFFER | STUDIOAPI_BUFFER_USAGE_FLAG_TRANSFER_DST )
	END_SHADER_RESOURCES

	BEGIN_SHADER_PARAMS
		SHADER_PARAM( COLOR, SHADER_PARAM_TYPE_VECTOR_4D, "Color" )
	END_SHADER_PARAMS

	SHADER_INIT_PARAMS
	{
		pParams[COLOR]->SetVecValue( vector4_t( 1.f, 1.f, 1.f, 1.f ) );
	}

	SHADER_UPDATE_BUFFERS
	{
		// Initialize buffer data
		shaderParamsBuffer_t bufferData = {};
		pParams[COLOR]->GetVecValue( &bufferData.color.x, 4 );

		// Update a buffer
		RES_BUFFER0.UpdateBuffer( pStudioAPICmdContext, (byte*)&bufferData, pStudioAPIBuffers[RES_BUFFER0] );
	}

	SHADER_BARRIER
	{
		// TODO BS yehor.pohuliaka - Add the ability to get the queue type from IStudioAPICmdList
		studioAPIBarrier_t studioAPIBarriers[] = {
			StudioAPI_MakeBufferBarrier( pStudioAPIBuffers[RES_BUFFER0], STUDIOAPI_BUFFER_STATE_CONSTANT_BUFFER, STUDIOAPI_QUEUE_TYPE_GRAPHICS )
		};
		pStudioAPICmdList->Barrier( studioAPIBarriers, ARRAYSIZE( studioAPIBarriers ) );
	}

	SHADER_SELECT_COMBO
	{
		// Set a vertex shader and type
		DECLARE_VERTEX_SHADER( wireframe_vs );
		SET_VERTEX_TYPE( STUDIO_VERTEXTYPE_SIMPLEELEMENT );
		SET_VERTEX_SHADER( wireframe_vs );

		// Set a pixel shader
		DECLARE_PIXEL_SHADER( wireframe_ps );
		SET_PIXEL_SHADER( wireframe_ps );
	}

	SHADER_DRAW
	{
		RES_BUFFER0.SetConstantBuffer( pStudioAPICmdList, pStudioAPIBuffers[RES_BUFFER0] );
	}
END_SHADER
