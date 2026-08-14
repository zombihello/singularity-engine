#pragma once

/*
==================
CStudioSceneRenderTargets::GetTexture
==================
*/
FORCEINLINE CResourcePtr<ITexture> CStudioSceneRenderTargets::GetTexture( studioSceneRenderTargetType_t type ) const
{
	Assert( type < STUDIO_SCENE_RENDERTARGET_NUM_TYPES );
	return renderTargets[(uint32)type].pTexture;
}

/*
==================
CStudioSceneRenderTargets::GetTextureResource
==================
*/
FORCEINLINE ITextureResource* CStudioSceneRenderTargets::GetTextureResource( studioSceneRenderTargetType_t type ) const
{
	Assert( type < STUDIO_SCENE_RENDERTARGET_NUM_TYPES );
	return renderTargets[(uint32)type].pTextureResource;
}

/*
==================
CStudioSceneRenderTargets::GetTextureFormat
==================
*/
FORCEINLINE studioAPIPixelFormat_t CStudioSceneRenderTargets::GetTextureFormat( studioSceneRenderTargetType_t type ) const
{
	Assert( type < STUDIO_SCENE_RENDERTARGET_NUM_TYPES );
	return s_renderTargetInfos[(uint32)type].pixelFormat;
}

/*
==================
CStudioSceneRenderTargets::GetBufferSize
==================
*/
FORCEINLINE const vector2i_t& CStudioSceneRenderTargets::GetBufferSize() const
{
	return bufferSize;
}
