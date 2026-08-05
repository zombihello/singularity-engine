#pragma once
#include "tier1/math/aabb.h"
#include "tier1/math/color.h"
#include "resourcesystem/resourceptr.h"
#include "materialsystem/imaterial.h"
#include "modelsystem/ivertexfactory.h"
#include "modelsystem/modeltypes.h"
#include "studiorender/istudio_scene.h"
#include "studiorender/studio_framealloc.h"
#include "studiorender/studio_renderpasstypes.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStudioAPICmdList;
struct studioSceneView_t;

//-----------------------------------------------------------------------------
// Studio simple primitive types
//-----------------------------------------------------------------------------
enum studioSimplePrimitiveType_t
{
	STUDIO_SIMPLE_PRIMITIVE_TYPE_LINE,
	STUDIO_SIMPLE_PRIMITIVE_TYPE_BOX,
	STUDIO_SIMPLE_PRIMITIVE_NUM_TYPES
};

//-----------------------------------------------------------------------------
// Studio simple primitive parameters
//-----------------------------------------------------------------------------
struct studioSimpleLineParams_t
{
	vector3_t start;
	vector3_t end;
};

struct studioSimpleBoxParams_t
{
	CAABB aabb;
};

struct studioSimplePrimitive_t
{
	studioSimplePrimitive_t();

	studioSimplePrimitiveType_t type;
	CColor						color;
	float						timeToLive;
	bool						bDepthTest;
	union
	{
		studioSimpleLineParams_t line;
		studioSimpleBoxParams_t	 box;
	};
};

//-----------------------------------------------------------------------------
// Studio batcher of simple line elements
//-----------------------------------------------------------------------------
class CStudioBatchedSimpleElements
{
public:
	CStudioBatchedSimpleElements();

	void Init();
	void Shutdown();

	void R_BuildBatches( studioSceneView_t* pSceneView );
	void R_Draw( IStudioAPICmdList* pCmdList, studioRenderPassType_t renderPassType );
	bool R_IsEmpty() const;

private:
	using batch_t = studioFrameVector_t<modelSimpleVertex_t>;
	enum batchType_t
	{
		BATCH_TYPE_NO_DEPTHTEST,
		BATCH_TYPE_DEPTHTEST,
		BATCH_NUM_TYPES
	};

	void R_AddLine( batch_t& batch, const vector3_t& start, const vector3_t& end, const CColor& color );
	void R_AddBox( batch_t& batch, const CAABB& aabb, const CColor& color );
	void R_DrawBatch( IStudioAPICmdList* pCmdList, studioRenderPassType_t renderPassType, batchType_t batchType );

	CResourcePtr<IMaterial>	   pMaterial;
	CRefPtr<IMaterialResource> pMaterialResource;
	CRefPtr<IVertexFactory>	   pVertexFactory;
	uint32					   depthTestParamIndex;
	batch_t					   batches[BATCH_NUM_TYPES];
};

#include "studiorender/studio_batchedsimpleelements.inl"
