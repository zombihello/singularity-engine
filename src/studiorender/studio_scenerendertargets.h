#pragma once
#include "tier1/math/math.h"
#include "resourcesystem/resourceptr.h"
#include "materialsystem/itexture.h"

//-----------------------------------------------------------------------------
// Studio scene render target types
//-----------------------------------------------------------------------------
enum studioSceneRenderTargetType_t
{
	STUDIO_SCENE_RENDERTARGET_TYPE_SCENECOLOR_LDR,	// Final scene color (LDR, for display/presentation)
	STUDIO_SCENE_RENDERTARGET_TYPE_SCENEDEPTH,		// Scene depth buffer
	STUDIO_SCENE_RENDERTARGET_NUM_TYPES
};

//-----------------------------------------------------------------------------
// Studio scene render targets
//
// Render targets are registered as `ITexture` resources (under names `__rt_*`)
// so materials/shaders can reference them
//-----------------------------------------------------------------------------
class CStudioSceneRenderTargets
{
public:
	CStudioSceneRenderTargets();

	void Init();
	void Shutdown();
	bool Allocate( uint32 width, uint32 height );

	CResourcePtr<ITexture> GetTexture( studioSceneRenderTargetType_t type ) const;
	ITextureResource*	   GetTextureResource( studioSceneRenderTargetType_t type ) const;
	studioAPIPixelFormat_t GetTextureFormat( studioSceneRenderTargetType_t type ) const;
	const vector2i_t&	   GetBufferSize() const;

private:
	struct renderTargetInfo_t
	{
		const char*			   pName;
		studioAPIPixelFormat_t pixelFormat;
		uint32				   usageFlags;
	};

	struct renderTarget_t
	{
		CResourcePtr<ITexture>	  pTexture;
		CRefPtr<ITextureResource> pTextureResource;
	};

	vector2i_t						bufferSize;
	renderTarget_t					renderTargets[STUDIO_SCENE_RENDERTARGET_NUM_TYPES];
	static const renderTargetInfo_t s_renderTargetInfos[STUDIO_SCENE_RENDERTARGET_NUM_TYPES];
};

#include "studiorender/studio_scenerendertargets.inl"
