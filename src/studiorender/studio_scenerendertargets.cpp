#include "pch_studiorender.h"
#include "resourcesystem/iresourcesystem.h"
#include "studiorender/studio_scenerendertargets.h"

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
const CStudioSceneRenderTargets::renderTargetInfo_t CStudioSceneRenderTargets::s_renderTargetInfos[STUDIO_SCENE_RENDERTARGET_NUM_TYPES] = {
	{ "__rt_scenecolor_ldr", STUDIOAPI_PIXEL_FORMAT_RGBA8, STUDIOAPI_TEXTURE_USAGE_FLAG_RENDER_TARGET | STUDIOAPI_TEXTURE_USAGE_FLAG_TEXTURE },	 // STUDIO_SCENE_RENDERTARGET_TYPE_SCENECOLOR_LDR
	{ "__rt_scenedepth", STUDIOAPI_PIXEL_FORMAT_DEPTH32F, STUDIOAPI_TEXTURE_USAGE_FLAG_RENDER_TARGET }											 // STUDIO_SCENE_RENDERTARGET_TYPE_SCENEDEPTH
};

/*
==================
CStudioSceneRenderTargets::CStudioSceneRenderTargets
==================
*/
CStudioSceneRenderTargets::CStudioSceneRenderTargets()
	: bufferSize( 0, 0 )
{
}

/*
==================
CStudioSceneRenderTargets::Init
==================
*/
void CStudioSceneRenderTargets::Init()
{
	// Create the render targets as ITexture resources
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	IResourceTypeMgr* pTexturesMgr = g_pResourceSystem->GetResourceManagerForType<ITexture>();
	for ( uint32 index = 0; index < STUDIO_SCENE_RENDERTARGET_NUM_TYPES; ++index )
	{
		renderTarget_t& renderTarget  = renderTargets[index];
		renderTarget.pTexture		  = pTexturesMgr->CreateResource( s_renderTargetInfos[index].pName );
		renderTarget.pTextureResource = renderTarget.pTexture->GetStudioResource();
	}
}

/*
==================
CStudioSceneRenderTargets::Shutdown
==================
*/
void CStudioSceneRenderTargets::Shutdown()
{
	// Remove all the render targets
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	IResourceTypeMgr* pTexturesMgr = g_pResourceSystem->GetResourceManagerForType<ITexture>();
	for ( uint32 index = 0; index < STUDIO_SCENE_RENDERTARGET_NUM_TYPES; ++index )
	{
		renderTarget_t& renderTarget = renderTargets[index];
		pTexturesMgr->RemoveResource( renderTarget.pTexture );
		renderTarget.pTexture		  = NULL;
		renderTarget.pTextureResource = NULL;
	}
}

/*
==================
CStudioSceneRenderTargets::Allocate
==================
*/
bool CStudioSceneRenderTargets::Allocate( uint32 width, uint32 height )
{
	// Reallocate textures only when the requested size exceeds the current allocation
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	if ( bufferSize.x >= width && bufferSize.y >= height )
	{
		return false;
	}
	bufferSize.x = S_Max<int32>( width, bufferSize.x );
	bufferSize.y = S_Max<int32>( height, bufferSize.y );

	textureMipMap_t				 mipmap0			  = {};
	studioAPISamplerCreateInfo_t studioAPISamplerInfo = {};
	mipmap0.sizeX									  = (uint32)bufferSize.x;
	mipmap0.sizeY									  = (uint32)bufferSize.y;
	mipmap0.sizeZ									  = 1;

	studioAPISamplerInfo.filer		  = STUDIOAPI_SAMPLER_FILTER_POINT;
	studioAPISamplerInfo.addressModeU = STUDIOAPI_SAMPLER_ADDRESS_MODE_CLAMP;
	studioAPISamplerInfo.addressModeV = STUDIOAPI_SAMPLER_ADDRESS_MODE_CLAMP;
	studioAPISamplerInfo.addressModeW = STUDIOAPI_SAMPLER_ADDRESS_MODE_CLAMP;
	studioAPISamplerInfo.minLod		  = S_MinValue<float>();
	studioAPISamplerInfo.maxLod		  = S_MaxValue<float>();

	// Resize each render target
	for ( uint32 index = 0; index < STUDIO_SCENE_RENDERTARGET_NUM_TYPES; ++index )
	{
		const renderTarget_t&	  renderTarget	   = renderTargets[index];
		const renderTargetInfo_t& renderTargetInfo = s_renderTargetInfos[index];
		renderTarget.pTexture->Init( STUDIOAPI_TEXTURE_TYPE_2D, renderTargetInfo.pixelFormat, 1, &mipmap0, 1, renderTargetInfo.usageFlags, studioAPISamplerInfo );
	}

	// We are done
	return true;
}
