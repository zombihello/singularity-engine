#pragma once
#include "tier1/fasthash.h"
#include "tier1/math/math_types.h"
#include "modelsystem/modeltypes.h"
#include "modelsystem/imodel.h"
#include "materialsystem/imaterial.h"
#include "studiorender/studio_framealloc.h"
#include "studiorender/studio_renderpasstypes.h"
#include "studiorender/studio_globalshaderparams.h"
#include "studiorender/studio_batchedsimpleelements.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
struct studioEntity_t;
class IVertexFactory;

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
// Studio surface batch key
//-----------------------------------------------------------------------------
struct studioSurfaceBatchKey_t
{
	bool operator==( const studioSurfaceBatchKey_t& other ) const;

	uint32 modelId;
	uint32 materialId;
	uint32 surfaceId;
};

struct studioSurfaceBatchKeyHash_t
{
	size operator()( const studioSurfaceBatchKey_t& key ) const;
};

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
	matrix4x3_t			localToWorld;
};

//-----------------------------------------------------------------------------
// Type-erased per-instance data for a vertex factory
//
// NOTES:
//	* Holds a tightly packed blob of instance records; the concrete record type
//	  is known only at the append site and by the consuming vertex factory
//	* One block holds exactly one instance type
//	* Allocated on the frame temporary memory, never cached
//-----------------------------------------------------------------------------
struct studioVertexFactoryInstances_t
{
	studioVertexFactoryInstances_t( IVertexFactory* pVertexFactory );

	template<typename TInstanceType>
	TInstanceType* Add( uint32 count = 1 );
	void		   R_Upload();

	bool				  IsEmpty() const;
	uint32				  GetNumInstances() const;
	vertexFactoryStream_t GetVertexFactoryStream() const;

private:
	uint32					  numInstances;
	IVertexFactory*			  pVertexFactory;
	CRefPtr<IStudioAPIBuffer> pStudioAPIBuffer;
	studioFrameVector_t<byte> data;
};

//-----------------------------------------------------------------------------
// Studio surface batch
//
// NOTES:
//	* studioSurfaceBatch_t batches all instances that share geometry and material for the render thread
//	* studioSurfaceBatch_t are always allocated and freed every frame, they are never cached
//-----------------------------------------------------------------------------
struct studioSurfaceBatch_t
{
	studioSurfaceBatch_t( IVertexFactory* pVertexFactory );

	uint32						   modelId;
	uint32						   materialId;
	uint32						   baseVertexIndex;
	uint32						   baseIndex;
	uint32						   numIndices;
	studioVertexFactoryInstances_t instances;
};

//-----------------------------------------------------------------------------
// Studio resource
//
// NOTES:
//	* studioResource_t contains a render resource that uses by a surface batch in the render thread
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
//	* `surfaceBatchIds` and `resourceIds` are uses the frame temporary memory and may be resized
//	* studioRenderPass_t are always allocated and freed every frame, they are never cached
//-----------------------------------------------------------------------------
struct studioRenderPass_t
{
	studioRenderPass_t();

	studioFrameVector_t<uint32>	 surfaceBatchIds;  // Surface batch indices that are to be rendered in the pass
	studioFrameHashSet_t<uint32> resourceIds;	   // Resource indices that are used in the pass
};

//-----------------------------------------------------------------------------
// Studio scene view
//
// NOTES:
//	* `surfaceBatches`, `resources` and `resourceDict` are uses the frame temporary memory and may be resized
//	* studioSceneView_t are allocated on the frame temporary stack memory
//-----------------------------------------------------------------------------
struct studioSceneView_t
{
	studioSceneView_t();

	studioGlobalShaderParams_t														   globalShaderParams;						   // Global shader params built from the camera view for this scene view
	studioEntityView_t*																   pEntityViews;							   // Chain of all entity views effecting view, including off screen ones casting shadows
	studioRenderPass_t																   renderPasses[STUDIO_RENDERPASS_NUM_TYPES];  // Information for each render pass
	studioFrameVector_t<studioSurfaceBatch_t*>										   surfaceBatches;							   // Surface batches are the visible surfaces of the entity views
	studioFrameVector_t<studioResource_t*>											   resources;								   // Resources are used by the surface batches
	studioFrameHashMap_t<studioResourcePtr_t, uint32>								   resourceDict;							   // Lookup table to find a resource in `resources` by hash
	studioFrameHashMap_t<studioSurfaceBatchKey_t, uint32, studioSurfaceBatchKeyHash_t> surfaceBatchDict;						   // Lookup table to find a surface batch by (model, material, surface)
	studioFrameVector_t<studioSimplePrimitive_t>									   debugPrimitives;							   // Debug primitives
};

#include "studiorender/studio_sceneview.inl"
