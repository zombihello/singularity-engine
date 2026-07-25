#include "pch_stdshaders.h"
#include "materialsystem/itexture.h"
#include "materialsystem/shaderlib/shader_base.h"
#include "materialsystem/stdshaders/unlitgeneric_vs_vertex.gen.h"
#include "materialsystem/stdshaders/unlitgeneric_ps_pixel.gen.h"

BEGIN_SHADER( UnlitGeneric, "Help for UnlitGeneric" )
	BEGIN_SHADER_CACHES
		SHADER_CACHE( "unlitgeneric_vs" )
		SHADER_CACHE( "unlitgeneric_ps" )
	END_SHADER_CACHES

	BEGIN_SHADER_BUFFER_DATA( buffer0 )
		vector4_t color;
	END_SHADER_BUFFER_DATA

	BEGIN_SHADER_RESOURCES
		SHADER_BUFFER( RES_BUFFER0, 0, 0, buffer0, STUDIOAPI_BUFFER_USAGE_FLAG_STATIC | STUDIOAPI_BUFFER_USAGE_FLAG_CONSTANT_BUFFER | STUDIOAPI_BUFFER_USAGE_FLAG_TRANSFER_DST )
		SHADER_TEXTURE_SAMPLER( RES_BASETEXTURE, 0, 1 )
		SHADER_TEXTURE_SAMPLER( RES_BASESAMPLER, 0, 2 )
	END_SHADER_RESOURCES

	BEGIN_SHADER_PARAMS
		SHADER_PARAM( COLOR, SHADER_PARAM_TYPE_VECTOR_4D, "Color" )
		SHADER_PARAM( BASETEXTURE, SHADER_PARAM_TYPE_TEXTURE, "Base texture" )
	END_SHADER_PARAMS

	BEGIN_SHADER_PERMATERIAL_CONTEXTDATA
		DECLARE_SHADER_BUFFER_DATA( buffer0 );
		CRefPtr<ITextureResource> pBaseTexture;
		CRefPtr<IStudioAPIBuffer> pStudioAPIBuffer0;

		SHADER_PERMATERIAL_CONTEXTDATA_INIT_STUDIOAPI
		{
			pStudioAPIBuffer0 = RES_BUFFER0.CreateBuffer( (byte*)&buffer0 );
		}

		SHADER_PERMATERIAL_CONTEXTDATA_UPDATE_STUDIOAPI
		{
			IStudioAPICmdContext* pStudioAPICmdContext = g_pStudioAPI->GetImmediateCmdContext( STUDIOAPI_QUEUE_TYPE_GRAPHICS );
			RES_BUFFER0.UpdateBuffer( pStudioAPICmdContext, (byte*)&buffer0, pStudioAPIBuffer0 );
		}

		SHADER_PERMATERIAL_CONTEXTDATA_RELEASE_STUDIOAPI
		{
			pStudioAPIBuffer0 = NULL;
		}
	END_SHADER_PERMATERIAL_CONTEXTDATA

	SHADER_INIT_PARAMS
	{
		IResourceTypeMgr* pTexturesMgr = g_pResourceSystem->GetResourceManagerForType<ITexture>();
		pParams[COLOR]->SetVecValue( vector4_t( 1.f, 1.f, 1.f, 1.f ) );
		pParams[BASETEXTURE]->SetTextureValue( pTexturesMgr->GetDefaultResource() );
	}

	SHADER_UPDATE_PERMATERIAL_CONTEXTDATA
	{
		DECLARE_SHADER_PERMATERIAL_CONTEXTDATA( pPerMaterialContextDataLocal );
		CResourcePtr<ITexture> pBaseTexture = pParams[BASETEXTURE]->GetTextureValue();
		if ( !pBaseTexture.IsCached() )
		{
			IResourceTypeMgr* pTexturesMgr = g_pResourceSystem->GetResourceManagerForType<ITexture>();
			pBaseTexture				   = pTexturesMgr->GetDefaultResource();
		}

		pPerMaterialContextDataLocal->pBaseTexture = pBaseTexture->GetStudioResource();
		pParams[COLOR]->GetVecValue( &pPerMaterialContextDataLocal->buffer0.color.x, 4 );
	}

	SHADER_BARRIER
	{
		// TODO BS yehor.pohuliaka - Add the ability to get the queue type from IStudioAPICmdList
		DECLARE_SHADER_PERMATERIAL_CONTEXTDATA( pPerMaterialContextDataLocal );
		studioAPIBarrier_t studioAPIBarriers[] = {
			StudioAPI_MakeTextureBarrier( pPerMaterialContextDataLocal->pBaseTexture->GetStudioAPITexture(), STUDIOAPI_TEXTURE_LAYOUT_SHADER_RESOURCE_READONLY, STUDIOAPI_QUEUE_TYPE_GRAPHICS ),
			StudioAPI_MakeBufferBarrier( pPerMaterialContextDataLocal->pStudioAPIBuffer0, STUDIOAPI_BUFFER_STATE_CONSTANT_BUFFER, STUDIOAPI_QUEUE_TYPE_GRAPHICS )
		};
		pStudioAPICmdList->Barrier( studioAPIBarriers, ARRAYSIZE( studioAPIBarriers ) );
	}

	SHADER_SELECT_COMBO
	{
		// Set a vertex shader for the model's vertex factory
		DECLARE_VERTEX_SHADER( unlitgeneric_vs );
		SET_VERTEX_FACTORY( unlitgeneric_vs, pVertexFactory );
		SET_VERTEX_SHADER( unlitgeneric_vs );

		// Set a pixel shader
		DECLARE_PIXEL_SHADER( unlitgeneric_ps );
		SET_PIXEL_SHADER( unlitgeneric_ps );
	}

	SHADER_BIND
	{
		DECLARE_SHADER_PERMATERIAL_CONTEXTDATA( pPerMaterialContextDataLocal );
		RES_BASETEXTURE.SetTexture( pStudioAPICmdList, pPerMaterialContextDataLocal->pBaseTexture->GetStudioAPITexture() );
		RES_BASESAMPLER.SetSampler( pStudioAPICmdList, pPerMaterialContextDataLocal->pBaseTexture->GetStudioAPISampler() );
		RES_BUFFER0.SetConstantBuffer( pStudioAPICmdList, pPerMaterialContextDataLocal->pStudioAPIBuffer0 );
	}
END_SHADER
