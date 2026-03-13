#pragma once
#include "studiorender/studioapi/istudioapi_shader.h"
#include "studiorender/istudio_renderresource.h"
#include "studiorender/studio_vertextypes.h"

//-----------------------------------------------------------------------------
// Studio vertex declaration for each vertex type
//-----------------------------------------------------------------------------
class CStudioVertexDeclarations : public CStudioRenderResource<IStudioRenderResource, true>
{
public:
	FORCEINLINE void Init()
	{
		Assert( !IsInitedResource() );
		Studio_BeginInitResource( this );
	}

	FORCEINLINE void Destroy()
	{
		Assert( IsInitedResource() );
		Studio_BeginReleaseResource( this );
	}

	FORCEINLINE IStudioAPIVertexDeclaration* GetStudioAPIVertexDeclaration( studioVertexType_t vertexType ) const
	{
		Assert( vertexType < STUDIO_VERTEX_NUM_TYPES );
		return pStudioAPIVertexDeclarations[vertexType];
	}

private:
	// IStudioRenderResource interface
	virtual void InitStudioAPI() override;
	virtual void ReleaseStudioAPI() override;

	CRefPtr<IStudioAPIVertexDeclaration> pStudioAPIVertexDeclarations[STUDIO_VERTEX_NUM_TYPES];
};
extern CStudioVertexDeclarations g_StudioVertexDeclarations;