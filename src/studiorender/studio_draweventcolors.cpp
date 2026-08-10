#include "pch_studiorender.h"
#include "studiorender/studio_draweventcolors.h"

#if ENABLE_STUDIOAPI_EVENTS
// Draw event colors
const CColor CStudioDrawEventColors::simpleSurface( 238, 153, 26 );
const CColor CStudioDrawEventColors::staticSurface( 0, 128, 255 );
const CColor CStudioDrawEventColors::sceneItems( 128, 128, 128 );
const CColor CStudioDrawEventColors::postProcess( 0, 128, 128 );

// Lookup table of event colors keyed by vertex type
static const CColor s_eventColorVertexTypes[] = {
	CStudioDrawEventColors::simpleSurface,	// MODEL_VERTEXTYPE_SIMPLE
	CStudioDrawEventColors::staticSurface	// MODEL_VERTEXTYPE_STATIC
};
static_assert( MODEL_VERTEX_NUM_TYPES == ARRAYSIZE( s_eventColorVertexTypes ), "Array size 's_eventColorVertexTypes' must be equal to MODEL_VERTEX_NUM_TYPES" );

/*
==================
CStudioDrawEventColors::GetColorByVertexType
==================
*/
const CColor& CStudioDrawEventColors::GetColorByVertexType( modelVertexType_t vertexType )
{
	Assert( vertexType < MODEL_VERTEX_NUM_TYPES );
	return s_eventColorVertexTypes[(uint32)vertexType];
}
#endif	// ENABLE_STUDIOAPI_EVENTS
