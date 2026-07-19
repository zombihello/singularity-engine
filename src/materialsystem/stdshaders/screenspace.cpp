#include "pch_stdshaders.h"
#include "materialsystem/itexture.h"
#include "materialsystem/shaderlib/shader_base.h"
#include "materialsystem/stdshaders/screenspace_vs_vertex.gen.h"
#include "materialsystem/stdshaders/screenspace_ps_pixel.gen.h"

BEGIN_SHADER( ScreenSpace, "Help for ScreenSpace" )
	BEGIN_SHADER_CACHES
		SHADER_CACHE( "screenspace_vs" )
		SHADER_CACHE( "screenspace_ps" )
	END_SHADER_CACHES

	BEGIN_SHADER_RESOURCES
		SHADER_TEXTURE_SAMPLER( RES_BASETEXTURE, 0, 0 )
		SHADER_TEXTURE_SAMPLER( RES_BASESAMPLER, 0, 1 )
	END_SHADER_RESOURCES

	BEGIN_SHADER_PARAMS
		SHADER_PARAM( BASETEXTURE, SHADER_PARAM_TYPE_TEXTURE, "Base texture" )
	END_SHADER_PARAMS

	BEGIN_SHADER_CONTEXT_DATA
		CRefPtr<ITextureResource> pBaseTexture;
	END_SHADER_CONTEXT_DATA

	SHADER_INIT_PARAMS
	{
		IResourceTypeMgr* pTexturesMgr = g_pResourceSystem->GetResourceManagerForType<ITexture>();
		pParams[BASETEXTURE]->SetTextureValue( pTexturesMgr->GetDefaultResource() );
	}

	SHADER_UPDATE_CONTEXT_DATA
	{
		DECLARE_SHADER_CONTEXT_DATA( pScreenSpaceContextData );
		CResourcePtr<ITexture> pBaseTexture = pParams[BASETEXTURE]->GetTextureValue();
		if ( !pBaseTexture.IsCached() )
		{
			IResourceTypeMgr* pTexturesMgr = g_pResourceSystem->GetResourceManagerForType<ITexture>();
			pBaseTexture				   = pTexturesMgr->GetDefaultResource();
		}

		pScreenSpaceContextData->pBaseTexture = pBaseTexture->GetStudioResource();
	}

	SHADER_BARRIER
	{
		// TODO BS yehor.pohuliaka - Add the ability to get the queue type from IStudioAPICmdList
		DECLARE_SHADER_CONTEXT_DATA( pScreenSpaceContextData );
		studioAPIBarrier_t studioAPIBarriers[] = {
			StudioAPI_MakeTextureBarrier( pScreenSpaceContextData->pBaseTexture->GetStudioAPITexture(), STUDIOAPI_TEXTURE_LAYOUT_SHADER_RESOURCE_READONLY, STUDIOAPI_QUEUE_TYPE_GRAPHICS ),
		};
		pStudioAPICmdList->Barrier( studioAPIBarriers, ARRAYSIZE( studioAPIBarriers ) );
	}

	SHADER_SELECT_COMBO
	{
		// Set a vertex shader for the model's vertex factory
		DECLARE_VERTEX_SHADER( screenspace_vs );
		SET_VERTEX_FACTORY( screenspace_vs, pVertexFactory );
		SET_VERTEX_SHADER( screenspace_vs );

		// Set a pixel shader
		DECLARE_PIXEL_SHADER( screenspace_ps );
		SET_PIXEL_SHADER( screenspace_ps );
	}

	SHADER_DRAW
	{
		DECLARE_SHADER_CONTEXT_DATA( pPresentContextData );
		RES_BASETEXTURE.SetTexture( pStudioAPICmdList, pPresentContextData->pBaseTexture->GetStudioAPITexture() );
		RES_BASESAMPLER.SetSampler( pStudioAPICmdList, pPresentContextData->pBaseTexture->GetStudioAPISampler() );
	}
END_SHADER
