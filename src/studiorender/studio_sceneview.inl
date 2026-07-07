#pragma once

/*
==================
studioRenderPass_t::studioRenderPass_t
==================
*/
FORCEINLINE studioRenderPass_t::studioRenderPass_t()
{
	drawSurfaceIds.reserve( STUDIO_RENDERPASS_INITIAL_NUM_DRAWSURFACEIDS );
	resourceIds.reserve( STUDIO_RENDERPASS_INITIAL_NUM_RESOURCEIDS );
}

/*
==================
studioSceneView_t::studioSceneView_t
==================
*/
FORCEINLINE studioSceneView_t::studioSceneView_t()
	: pEntityViews( NULL )
{
	drawSurfaces.reserve( STUDIO_SCENEVIEW_INITIAL_NUM_DRAWSURFACES );
	resources.reserve( STUDIO_SCENEVIEW_INITIAL_NUM_RESOURCES );
	resourceDict.reserve( STUDIO_SCENEVIEW_INITIAL_NUM_RESOURCES );
}
