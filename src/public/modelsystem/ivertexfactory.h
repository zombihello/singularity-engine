#pragma once
#include "tier1/refcount.h"
#include "modelsystem/modeltypes.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStudioAPIVertexDeclaration;
class IStudioAPICmdList;
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
// Information about a vertex/index stream that lives in user memory (UP = User Pointer)
//-----------------------------------------------------------------------------
struct vertexFactoryStreamUP_t
{
	const byte* pData;
	uint32		numElements;
	uint32		stride;
};

//-----------------------------------------------------------------------------
// Vertex factory interface
//-----------------------------------------------------------------------------
class IVertexFactory : public IRefCounted
{
public:
	virtual ~IVertexFactory() {}

	// Initialize and shutdown the factory
	virtual void Init()		= 0;
	virtual void Shutdown() = 0;

	// Configure which vertex/index buffers the factory binds
	virtual void ClearStreams()												  = 0;
	virtual void AddVertexStream( const vertexFactoryStream_t& vertexStream ) = 0;
	virtual void SetIndexStream( const vertexFactoryStream_t& indexStream )	  = 0;

	// Place barriers into a command list
	virtual void R_Barrier( IStudioAPICmdList* pStudioAPICmdList ) const = 0;

	// Bind the vertex factory's own resources (vertex/index buffers, etc) into the command list
	virtual void R_Bind( IStudioAPICmdList* pStudioAPICmdList )																																  = 0;
	virtual void R_BindUP( IStudioAPICmdList* pStudioAPICmdList, const vertexFactoryStreamUP_t* pVertexStreams, uint32 numVertexStreams, const vertexFactoryStreamUP_t* pIndexStream = NULL ) = 0;

	virtual bool						 IsIndexed() const					   = 0;
	virtual modelVertexType_t			 GetVertexType() const				   = 0;
	virtual IStudioAPIVertexDeclaration* GetStudioAPIVertexDeclaration() const = 0;
};
