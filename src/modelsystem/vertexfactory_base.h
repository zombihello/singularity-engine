#pragma once
#include "studiorender/studioapi/istudioapi_shader.h"
#include "studiorender/istudio_renderresource.h"
#include "modelsystem/ivertexfactory.h"

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
	virtual void Init() override;
	virtual void Shutdown() override;

	virtual void ClearStreams() override;
	virtual void AddVertexStream( const vertexFactoryStream_t& vertexStream ) override;
	virtual void SetIndexStream( const vertexFactoryStream_t& indexStream ) override;

	virtual void R_Barrier( IStudioAPICmdList* pStudioAPICmdList ) const override;
	virtual void R_Bind( IStudioAPICmdList* pStudioAPICmdList ) override;
	virtual void R_BindUP( IStudioAPICmdList* pStudioAPICmdList, const vertexFactoryStreamUP_t* pVertexStreams, uint32 numVertexStreams, const vertexFactoryStreamUP_t* pIndexStream = NULL ) override;

	virtual bool IsIndexed() const override;

protected:
	// IRefCounted interface
	virtual void FinalRelease() override;

private:
	eastl::vector<vertexFactoryStream_t> vertexStreams;
	vertexFactoryStream_t				 indexStream;
};

#include "modelsystem/vertexfactory_base.inl"
