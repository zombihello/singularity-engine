#pragma once

//-----------------------------------------------------------------------------
// StudioRender defines
//-----------------------------------------------------------------------------
#define STUDIO_RENDERCOMMAND_BUFFER_SIZE		  ( 1024 * 1024 )
#define STUDIO_FRAMEALLOC_BLOCK_SIZE			  ( 64 * 1024 * 1024 )
#define STUDIO_FRAMEALLOC_NUM_POOLS				  1
#define STUDIO_SCENEVIEW_INITIAL_NUM_DRAWSURFACES 2048	// This is the initial allocation for max number of studioDrawSurface_t
														// in a given scene view, but it will automatically grow if needed