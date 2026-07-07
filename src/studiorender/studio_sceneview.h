#pragma once
#include "tier1/math/math_types.h"
#include "modelsystem/imodel.h"
#include "materialsystem/imaterial.h"
#include "studiorender/studio_framealloc.h"
#include "studiorender/studio_renderpasstypes.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
struct studioEntity_t;

//-----------------------------------------------------------------------------
// Studio resource type
//-----------------------------------------------------------------------------
enum studioResourceType_t
{
	STUDIO_RESOURCE_TYPE_MATERIAL,
	STUDIO_RESOURCE_TYPE_MODEL
};
using studioResourcePtr_t = void*;

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
	studioEntityView_t* pNext;
	studioEntity_t*		pEntity;  // NOTE: The render thread should NOT use the pEntity, because it can change in another thread
	matrix4x4_t			localToWorld;
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
	studioEntityView_t* pEntityView;
	uint32				modelId;
	uint32				materialId;
	uint32				baseVertexIndex;
	uint32				baseIndex;
	uint32				numIndices;
};

//-----------------------------------------------------------------------------
// Studio resource
//
// NOTES:
//	* studioResource_t contains a render resource that uses by a draw surface in the render thread
//	* studioResource_t are always allocated and freed every frame, they are never cached
//-----------------------------------------------------------------------------
struct studioResource_t
{
	studioResourceType_t type;
	union
	{
		IModelResource*		pModel;
		IMaterialResource*	pMaterial;
		studioResourcePtr_t pPtr;
	};
};

//-----------------------------------------------------------------------------
// Studio render pass
//
// NOTES:
//	* `drawSurfaceIds` and `resourceIds` are uses the frame temporary memory and may be resized
//	* studioRenderPass_t are always allocated and freed every frame, they are never cached
//-----------------------------------------------------------------------------
struct studioRenderPass_t
{
	studioRenderPass_t();

	studioFrameVector_t<uint32>	 drawSurfaceIds;  // Draw surface indices that are to be rendered in the pass
	studioFrameHashSet_t<uint32> resourceIds;	  // Resource indices that are used in the pass
};

//-----------------------------------------------------------------------------
// Studio scene view
// NOTES:
//	* `drawSurfaces`, `resources` and `resourceDict` are uses the frame temporary memory and may be resized
//	* studioSceneView_t are allocated on the frame temporary stack memory
//-----------------------------------------------------------------------------
struct studioSceneView_t
{
	studioSceneView_t();

	studioEntityView_t*								  pEntityViews;								  // Chain of all entity views effecting view, including off screen ones casting shadows
	studioRenderPass_t								  renderPasses[STUDIO_RENDERPASS_NUM_TYPES];  // Information for each render pass
	studioFrameVector_t<studioDrawSurface_t*>		  drawSurfaces;								  // Draw surfaces are the visible surfaces of the entity views
	studioFrameVector_t<studioResource_t*>			  resources;								  // Resources are used by the draw surfaces
	studioFrameHashMap_t<studioResourcePtr_t, uint32> resourceDict;								  // Lookup table to find a resource in `resources` by hash
};

#include "studiorender/studio_sceneview.inl"
