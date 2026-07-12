#pragma once
#include "studiorender/studioapi/istudioapi_shader.h"
#include "studiorender/istudio_renderresource.h"
#include "modelsystem/ivertexfactory.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStudioAPIBuffer;

//-----------------------------------------------------------------------------
// Information about a vertex/index buffer in a vertex factory
//-----------------------------------------------------------------------------
struct vertexFactoryStream_t
{
	IStudioAPIBuffer* pStudioAPIBuffer;
	uint64			  offset;
};

//-----------------------------------------------------------------------------
// Base class for a vertex declaration
//-----------------------------------------------------------------------------
class CVertexDeclarationBase : public CStudioRenderResource<IStudioRenderResource, true>
{
public:
	IStudioAPIVertexDeclaration* GetStudioAPIVertexDeclaration() const;

protected:
	// IStudioRenderResource interface
	virtual void ReleaseStudioAPI() override;

	CRefPtr<IStudioAPIVertexDeclaration> pStudioAPIVertexDeclaration;
};

//-----------------------------------------------------------------------------
// Base class for a vertex factory
//-----------------------------------------------------------------------------
class CVertexFactoryBase : public CRefCounted<IVertexFactory>, public CStudioRenderResource<IStudioRenderResource>
{
public:
	// IVertexFactory interface
	virtual void R_Barrier( IStudioAPICmdList* pStudioAPICmdList ) const override;
	virtual void R_PrepareForDraw( IStudioAPICmdList* pStudioAPICmdList, studioRenderPassType_t renderPassType ) override;

	virtual bool IsIndexed() const override;

	void ClearStreams();
	void AddVertexStream( const vertexFactoryStream_t& vertexStream );
	void SetIndexStream( const vertexFactoryStream_t& indexStream );

protected:
	// IRefCounted interface
	virtual void FinalRelease() override;

private:
	eastl::vector<vertexFactoryStream_t> vertexStreams;
	vertexFactoryStream_t				 indexStream;
};

#include "modelsystem/vertexfactory_base.inl"
