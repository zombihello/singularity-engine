#pragma once
#include "tier1/math/color.h"
#include "modelsystem/modeltypes.h"

//-----------------------------------------------------------------------------
// Draw event colors
//-----------------------------------------------------------------------------
#if ENABLE_STUDIOAPI_EVENTS
class CStudioDrawEventColors
{
public:
	static const CColor simpleSurface;
	static const CColor staticSurface;
	static const CColor sceneItems;
	static const CColor postProcess;

	static const CColor& GetColorByVertexType( modelVertexType_t vertexType );
};
#endif	// ENABLE_STUDIOAPI_EVENTS
