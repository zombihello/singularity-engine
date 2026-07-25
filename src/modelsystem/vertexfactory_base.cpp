#include "pch_modelsystem.h"
#include "studiorender/studioapi/istudioapi_cmdlist.h"
#include "studiorender/studioapi/istudioapi_barrier.h"
#include "modelsystem/vertexfactory_base.h"

/*
==================
CVertexDeclarationBase::ReleaseStudioAPI
==================
*/
void CVertexDeclarationBase::ReleaseStudioAPI()
{
	pStudioAPIVertexDeclaration = NULL;
}

/*
==================
CVertexFactoryBase::R_Barrier
==================
*/
void CVertexFactoryBase::R_Barrier( IStudioAPICmdList* pStudioAPICmdList ) const
{
	// Allocate memory on the stack for barriers info
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	uint32 numBarriers = (uint32)vertexStreams.size();
	if ( IsIndexed() )
	{
		numBarriers += 1;
	}
	studioAPIBarrier_t* pBarriers = (studioAPIBarrier_t*)Mem_Alloca( numBarriers * sizeof( studioAPIBarrier_t ) );

	// Make barriers for vertex buffers
	// TODO BS yehor.pohuliaka - Maybe in the future we should implement buffer barriers for specific offset/size in a buffer
	for ( uint32 index = 0, count = (uint32)vertexStreams.size(); index < count; ++index )
	{
		const vertexFactoryStream_t& vertexStream = vertexStreams[index];
		Assert( vertexStream.pStudioAPIBuffer );
		pBarriers[index] = StudioAPI_MakeBufferBarrier( vertexStream.pStudioAPIBuffer, STUDIOAPI_BUFFER_STATE_VERTEX_BUFFER, STUDIOAPI_QUEUE_TYPE_GRAPHICS );
	}

	// Make a barrier for an index buffer (if it exists) and send the barriers into the command list
	if ( IsIndexed() )
	{
		pBarriers[numBarriers - 1] = StudioAPI_MakeBufferBarrier( indexStream.pStudioAPIBuffer, STUDIOAPI_BUFFER_STATE_INDEX_BUFFER, STUDIOAPI_QUEUE_TYPE_GRAPHICS );
	}
	pStudioAPICmdList->Barrier( pBarriers, numBarriers );
}

/*
==================
CVertexFactoryBase::R_Bind
==================
*/
void CVertexFactoryBase::R_Bind( IStudioAPICmdList* pStudioAPICmdList )
{
	// Set all vertex buffers
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	for ( uint32 index = 0, count = (uint32)vertexStreams.size(); index < count; ++index )
	{
		const vertexFactoryStream_t& vertexStream = vertexStreams[index];
		Assert( vertexStream.pStudioAPIBuffer );
		pStudioAPICmdList->SetVertexBuffer( index, vertexStream.pStudioAPIBuffer, vertexStream.offset );
	}

	// Set the index buffer
	if ( IsIndexed() )
	{
		pStudioAPICmdList->SetIndexBuffer( indexStream.pStudioAPIBuffer, indexStream.offset );
	}
}

/*
==================
CVertexFactoryBase::FinalRelease
==================
*/
void CVertexFactoryBase::FinalRelease()
{
	if ( IsNeedDeferredDestroy() )
	{
		Studio_BeginDeleteResource( this );
	}
	else
	{
		delete this;
	}
}

/*
==================
CVertexFactoryBase::IsIndexed
==================
*/
bool CVertexFactoryBase::IsIndexed() const
{
	return !!indexStream.pStudioAPIBuffer;
}
