#include "pch_studiorender.h"
#include "materialsystem/ishader.h"
#include "materialsystem/imaterial.h"
#include "modelsystem/imodelsystem.h"
#include "modelsystem/ivertexfactory.h"
#include "modelsystem/modeltypes.h"
#include "studiorender/studioapi/istudioapi.h"
#include "studiorender/studioapi/istudioapi_cmdlist.h"
#include "studiorender/studio_resourcebindingslots.h"
#include "studiorender/studiorender.h"
#include "studiorender/studio_renderutils.h"

/*
==================
R_DrawDenormalizedQuad
==================
*/
void R_DrawDenormalizedQuad( IStudioAPICmdList* pCmdList, const studioDenormalizedQuadParams_t& params )
{
	// Set up the vertices in the target's pixel space and the texture's texel space
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( pCmdList && params.pVertexFactory && params.targetSize.x > 0 && params.targetSize.y > 0 && params.textureSize.x > 0 && params.textureSize.y > 0 );

	modelSimpleVertex_t vertices[4];
	vertices[0].position = vector4_t( params.origin.x, params.origin.y, params.clipSpaceQuadZ, 1.f );
	vertices[1].position = vector4_t( params.origin.x + params.size.x, params.origin.y, params.clipSpaceQuadZ, 1.f );
	vertices[2].position = vector4_t( params.origin.x, params.origin.y + params.size.y, params.clipSpaceQuadZ, 1.f );
	vertices[3].position = vector4_t( params.origin.x + params.size.x, params.origin.y + params.size.y, params.clipSpaceQuadZ, 1.f );

	vertices[0].texCoord = vector2_t( params.uv.x, params.uv.y );
	vertices[1].texCoord = vector2_t( params.uv.x + params.sizeUV.x, params.uv.y );
	vertices[2].texCoord = vector2_t( params.uv.x, params.uv.y + params.sizeUV.y );
	vertices[3].texCoord = vector2_t( params.uv.x + params.sizeUV.x, params.uv.y + params.sizeUV.y );

	// Convert the pixel coords into the clip space and normalize the UVs
	const studioAPIInfo_t& studioAPIInfo = g_pStudioAPI->GetInfo();
	for ( uint32 index = 0; index < ARRAYSIZE( vertices ); ++index )
	{
		modelSimpleVertex_t& vertex = vertices[index];
		vertex.position.x			= -1.f + 2.f * ( vertex.position.x - studioAPIInfo.pixelCenterOffset ) / (float)params.targetSize.x;
		vertex.position.y			= +1.f - 2.f * ( vertex.position.y - studioAPIInfo.pixelCenterOffset ) / (float)params.targetSize.y;
		vertex.texCoord.x			= vertex.texCoord.x / (float)params.textureSize.x;
		vertex.texCoord.y			= vertex.texCoord.y / (float)params.textureSize.y;
		vertex.color				= params.color;
	}
	static const uint16					 s_indices[6]  = { 0, 1, 3, 0, 3, 2 };
	static const vertexFactoryStreamUP_t s_indexStream = { (byte*)s_indices, ARRAYSIZE( s_indices ), sizeof( uint16 ) };

	// Draw the quad
	vertexFactoryStreamUP_t vertexStream = { (byte*)vertices, ARRAYSIZE( vertices ), sizeof( modelSimpleVertex_t ) };
	params.pVertexFactory->R_BindUP( pCmdList, &vertexStream, 1, &s_indexStream );
	pCmdList->DrawIndexed( 0, 0, ARRAYSIZE( s_indices ) );
}
