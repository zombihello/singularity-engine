#include "pch_stdshaders.h"
#include "materialsystem/shaderlib/shader_base.h"
#include "materialsystem/stdshaders/simpleelements_vs_vertex.gen.h"
#include "materialsystem/stdshaders/simpleelements_ps_pixel.gen.h"

BEGIN_SHADER( SimpleElements, "Help for SimpleElements" )
	BEGIN_SHADER_CACHES
		SHADER_CACHE( "simpleelements_vs" )
		SHADER_CACHE( "simpleelements_ps" )
	END_SHADER_CACHES

	BEGIN_SHADER_PARAMS
		SHADER_PARAM( DEPTHTEST, SHADER_PARAM_TYPE_BOOL, SHADER_PARAM_FREQUENCY_PERDRAW, "Enable depth testing" )
	END_SHADER_PARAMS

	BEGIN_SHADER_PERMATERIAL_CONTEXTDATA
	END_SHADER_PERMATERIAL_CONTEXTDATA

	SHADER_INIT_PERDRAW_PARAMS
	{
		pParams[DEPTHTEST].SetBoolValue( true );
	}

	SHADER_INIT_DEFAULT_RENDERSTATE
	{
		renderState.inputAssemblyState.topology = STUDIOAPI_PRIMITIVE_TOPOLOGY_LINE_LIST;
		renderState.rasterizerState.cullMode	= STUDIOAPI_RASTERIZER_CULL_MODE_NONE;
		renderState.depthState.bTestEnable		= true;
		renderState.depthState.bWriteEnable		= false;
	}

	SHADER_SELECT_COMBO
	{
		// Set a vertex shader for the vertex factory
		DECLARE_VERTEX_SHADER( simpleelements_vs );
		SET_VERTEX_FACTORY( simpleelements_vs, drawParams.pVertexFactory );
		SET_VERTEX_SHADER( simpleelements_vs );

		// Set a pixel shader
		DECLARE_PIXEL_SHADER( simpleelements_ps );
		SET_PIXEL_SHADER( simpleelements_ps );

		// The renderer decides per draw whether the lines are occluded by the scene
		studioRenderState_t renderState	   = GetDefaultRenderState();
		renderState.depthState.bTestEnable = drawParams.pPerDrawVars[DEPTHTEST].GetBoolValue();
		SET_RENDER_STATE( FindOrCreateRenderState( renderState ) );
	}
END_SHADER
