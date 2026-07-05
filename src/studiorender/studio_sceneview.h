#pragma once
#include "tier1/math/math_types.h"
#include "modelsystem/imodel.h"
#include "materialsystem/imaterial.h"
#include "studiorender/studio_renderpasstypes.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
struct studioEntity_t;
struct studioDrawSurface_t;

//-----------------------------------------------------------------------------
// Studio entity view
//
// NOTES:
//	* An studioEntityView_t is created whenever a studioEntity_t is considered for inclusion
//	  in the current view, but it may still turn out to be culled.
//	* An studioEntityView_t are allocated on the frame temporary stack memory.
//	* An studioEntityView_t contains everything that the render thread needs to render it,
//	  which the main thread may be modifying simultaneously.
//	* A single studioEntity_t can generate multiple studioEntityView_t in a single frame, as when seen in a mirror
//-----------------------------------------------------------------------------
struct studioEntityView_t
{
	studioEntityView_t*	 pNext;
	studioEntity_t*		 pEntity;  // NOTE: The render thread should NOT use the pEntity, because it can change in another thread
	matrix4x4_t			 transform;
	studioDrawSurface_t* pDrawSurfaces;
};

//-----------------------------------------------------------------------------
// Studio draw surface
//
// NOTES:
//	* studioDrawSurface_t structures command the render thread to render surfaces
//	* studioDrawSurface_t are always allocated and freed every frame, they are never cached
//-----------------------------------------------------------------------------
struct studioDrawSurface_t
{
	studioDrawSurface_t*	   pNext;
	studioEntityView_t*		   pEntityView;
	CRefPtr<IModelResource>	   pModel;
	CRefPtr<IMaterialResource> pMaterial;
	uint32					   baseVertexIndex;
	uint32					   baseIndex;
	uint32					   numIndices;
	bool					   bShouldDrawSurfaceInPass[STUDIO_RENDERPASS_NUM_TYPES];
};

//-----------------------------------------------------------------------------
// Studio render pass
//-----------------------------------------------------------------------------
struct studioRenderPass_t
{
	uint32* pDrawSurfaceIds;
	uint32	numDrawSurfaces;
};

//-----------------------------------------------------------------------------
// Studio scene view
//-----------------------------------------------------------------------------
struct studioSceneView_t
{
	studioEntityView_t*	  pEntityViews;
	studioDrawSurface_t** pDrawSurfaces;
	uint32				  numDrawSurfaces;
	studioRenderPass_t	  renderPasses[STUDIO_RENDERPASS_NUM_TYPES];
};