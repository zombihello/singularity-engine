#pragma once
#include "tier1/math/color.h"
#include "studiorender/studio_renderpasstypes.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStudioAPICmdList;
class IMaterialResource;

//-----------------------------------------------------------------------------
// Render utils parameters
//-----------------------------------------------------------------------------
struct studioDenormalizedQuadParams_t
{
	IVertexFactory* pVertexFactory;	 // NOTE: Vertex factory must be for MODEL_VERTEXTYPE_SIMPLE
	vector2_t		origin;
	vector2_t		size;
	vector2_t		uv;
	vector2_t		sizeUV;
	vector2i_t		targetSize;
	vector2i_t		textureSize;
	float			clipSpaceQuadZ;
	CColor			color;
};

//-----------------------------------------------------------------------------
// Generate a denormalized quad and immediately record its draw into the command list
//
// The quad is given in the target's pixel space and its UVs in the texture's texel space, which makes
// it the tool for blitting a region of a texture
//-----------------------------------------------------------------------------
void R_DrawDenormalizedQuad( IStudioAPICmdList* pCmdList, const studioDenormalizedQuadParams_t& params );
