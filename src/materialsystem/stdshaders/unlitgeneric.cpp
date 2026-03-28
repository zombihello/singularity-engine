#include "pch_stdshaders.h"
#include "materialsystem/itexture.h"
#include "materialsystem/shaderlib/shader_base.h"
#include "materialsystem/stdshaders/unlitgeneric_vs_vertex.gen.h"
#include "materialsystem/stdshaders/unlitgeneric_ps_pixel.gen.h"

struct shaderParamsBuffer_t
{
	vector4_t color;
};

BEGIN_SHADER( UnlitGeneric, "Help for UnlitGeneric" )
	BEGIN_SHADER_CACHES
		SHADER_CACHE( "unlitgeneric_vs" )
		SHADER_CACHE( "unlitgeneric_ps" )
	END_SHADER_CACHES

	BEGIN_SHADER_RESOURCES
		SHADER_BUFFER( RES_BUFFER0, 0, 1, shaderParamsBuffer_t, STUDIOAPI_BUFFER_USAGE_FLAG_STATIC | STUDIOAPI_BUFFER_USAGE_FLAG_CONSTANT_BUFFER | STUDIOAPI_BUFFER_USAGE_FLAG_TRANSFER_DST )
		SHADER_TEXTURE_SAMPLER( RES_BASETEXTURE, 0, 2 )
		SHADER_TEXTURE_SAMPLER( RES_BASESAMPLER, 0, 3 )
	END_SHADER_RESOURCES

	BEGIN_SHADER_PARAMS
		SHADER_PARAM( COLOR, SHADER_PARAM_TYPE_VECTOR_4D, "Color" )
		SHADER_PARAM( BASETEXTURE, SHADER_PARAM_TYPE_TEXTURE, "Base texture" )
	END_SHADER_PARAMS

	SHADER_INIT_PARAMS
	{
		IResourceTypeMgr* pTexturesMgr = g_pResourceSystem->GetResourceManagerForType<ITexture>();
		pParams[COLOR]->SetVecValue( vector4_t( 1.f, 1.f, 1.f, 1.f ) );
		pParams[BASETEXTURE]->SetTextureValue( pTexturesMgr->GetDefaultResource() );
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
		CResourcePtr<ITexture> pTexture			   = pParams[BASETEXTURE]->GetTextureValue();
		ITextureResource*	   pTextureResource	   = pTexture->GetStudioResource();
		studioAPIBarrier_t	   studioAPIBarriers[] = {
			StudioAPI_MakeTextureBarrier( pTextureResource->GetStudioAPITexture(), STUDIOAPI_TEXTURE_LAYOUT_SHADER_RESOURCE_READONLY, STUDIOAPI_QUEUE_TYPE_GRAPHICS ),
			StudioAPI_MakeBufferBarrier( pStudioAPIBuffers[RES_BUFFER0], STUDIOAPI_BUFFER_STATE_CONSTANT_BUFFER, STUDIOAPI_QUEUE_TYPE_GRAPHICS )
		};
		pStudioAPICmdList->Barrier( studioAPIBarriers, ARRAYSIZE( studioAPIBarriers ) );
	}

	SHADER_SELECT_COMBO
	{
		// Set a vertex shader and type
		DECLARE_VERTEX_SHADER( unlitgeneric_vs );
		SET_VERTEX_TYPE( STUDIO_VERTEXTYPE_SIMPLEELEMENT );
		SET_VERTEX_SHADER( unlitgeneric_vs );

		// Set a pixel shader
		DECLARE_PIXEL_SHADER( unlitgeneric_ps );
		SET_PIXEL_SHADER( unlitgeneric_ps );
	}

	SHADER_DRAW
	{
		CResourcePtr<ITexture> pTexture			= pParams[BASETEXTURE]->GetTextureValue();
		ITextureResource*	   pTextureResource = pTexture->GetStudioResource();
		RES_BASETEXTURE.SetTexture( pStudioAPICmdList, pTextureResource->GetStudioAPITexture() );
		RES_BASESAMPLER.SetSampler( pStudioAPICmdList, pTextureResource->GetStudioAPISampler() );
		RES_BUFFER0.SetConstantBuffer( pStudioAPICmdList, pStudioAPIBuffers[RES_BUFFER0] );
	}
END_SHADER
