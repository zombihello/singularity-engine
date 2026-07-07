#pragma once
#include "studiorender/studioapi/istudioapi_shader.h"
#include "studiorender/istudio_renderresource.h"
#include "modelsystem/modeltypes.h"

//-----------------------------------------------------------------------------
// Vertex declaration for each vertex type
//-----------------------------------------------------------------------------
class CVertexDeclarations : public CStudioRenderResource<IStudioRenderResource, true>
{
public:
	IStudioAPIVertexDeclaration* GetStudioAPIVertexDeclaration( modelVertexType_t vertexType ) const;

private:
	// IStudioRenderResource interface
	void InitStudioAPI() override;
	void ReleaseStudioAPI() override;

	CRefPtr<IStudioAPIVertexDeclaration> pStudioAPIVertexDeclarations[MODEL_VERTEX_NUM_TYPES];
};

#include "modelsystem/vertexdeclarations.inl"