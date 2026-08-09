#pragma once
#include "studiorender/studioapi/istudioapi_shader.h"
#include "studiorender/studioapi/istudioapi_barrier.h"
#include "studiorender/studioapi/istudioapi_cmdlist.h"
#include "studiorender/istudio_renderresource.h"
#include "studiorender/studio_resourcebindingslots.h"
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
template<class TBaseClass, uint32 instanceStreamSlot = INVALID_INDEX>
class CVertexFactoryBase : public CRefCounted<TBaseClass>, public CStudioRenderResource<IStudioRenderResource>
{
public:
	static_assert( eastl::is_empty<typename vertexFactoryInfo_t<TBaseClass>::instance_t>::value == ( instanceStreamSlot == INVALID_INDEX ), "Empty instance type must use INVALID_INDEX; non-empty must set a slot" );

	// IVertexFactory interface
	// Initialize and shutdown the factory
	virtual void Init() override;
	virtual void Shutdown() override;

	// Configure which vertex/index buffers the factory binds
	virtual void ClearStreams() override;
	virtual void AddVertexStream( const vertexFactoryStream_t& vertexStream ) override;
	virtual void SetIndexStream( const vertexFactoryStream_t& indexStream ) override;

	// Place barriers into a command list
	virtual void R_Barrier( IStudioAPICmdList* pStudioAPICmdList ) const override;

	// Create an instance buffer for drawing
	virtual CRefPtr<IStudioAPIBuffer> R_CreateInstanceBuffer( const byte* pInstances, uint32 numInstances ) const override;

	// Bind the vertex factory's own resources (vertex/index buffers, etc) into the command list
	virtual void R_Bind( IStudioAPICmdList* pStudioAPICmdList, const vertexFactoryStream_t& instanceStream ) override;
	virtual void R_BindUP( IStudioAPICmdList* pStudioAPICmdList, const vertexFactoryStreamUP_t& instanceStream, const vertexFactoryStreamUP_t* pVertexStreams, uint32 numVertexStreams, const vertexFactoryStreamUP_t* pIndexStream = NULL ) override;

	virtual bool			  IsIndexed() const override;
	virtual bool			  IsInstanced() const override;
	virtual modelVertexType_t GetVertexType() const override;
	virtual uint32			  GetInstanceStride() const override;

	CVertexFactoryBase();

protected:
	// IRefCounted interface
	virtual void FinalRelease() override;

	// IStudioRenderResource interface
	virtual void ReleaseStudioAPI() override;

	// Create and update the factory's buffers
	void		 R_CreateConstantBuffer( uint32 size, const char* pDebugName );
	void		 R_UpdateConstantBuffer( const byte* pData, uint32 size );
	virtual void R_UpdateInstanceBuffer( byte* pData, const byte* pInstances, uint32 numInstances ) const;

private:
	CRefPtr<IStudioAPIBuffer>			 pStudioAPIConstantBuffer;
	vertexFactoryStream_t				 indexStream;
	eastl::vector<vertexFactoryStream_t> vertexStreams;
};

#include "modelsystem/vertexfactory_base.inl"
