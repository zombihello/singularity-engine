#include "vertexfactory_base.h"
#pragma once

/*
==================
CVertexDeclarationBase::GetStudioAPIVertexDeclaration
==================
*/
FORCEINLINE IStudioAPIVertexDeclaration* CVertexDeclarationBase::GetStudioAPIVertexDeclaration() const
{
	return pStudioAPIVertexDeclaration;
}

/*
==================
CVertexDeclarationBase::CVertexFactoryBase
==================
*/
template<class TBaseClass, uint32 instanceStreamSlot>
CVertexFactoryBase<TBaseClass, instanceStreamSlot /*= INVALID_INDEX*/>::CVertexFactoryBase()
{
	Mem_Memzero( &indexStream, sizeof( vertexFactoryStream_t ) );
}

/*
==================
CVertexFactoryBase::Init
==================
*/
template<class TBaseClass, uint32 instanceStreamSlot /*= INVALID_INDEX*/>
void CVertexFactoryBase<TBaseClass, instanceStreamSlot>::Init()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Studio_BeginUpdateResource( this );
}

/*
==================
CVertexFactoryBase::Shutdown
==================
*/
template<class TBaseClass, uint32 instanceStreamSlot /*= INVALID_INDEX*/>
void CVertexFactoryBase<TBaseClass, instanceStreamSlot>::Shutdown()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Studio_BeginReleaseResource( this );
}

/*
==================
CVertexFactoryBase::Shutdown
==================
*/
template<class TBaseClass, uint32 instanceStreamSlot>
void CVertexFactoryBase<TBaseClass, instanceStreamSlot /*= INVALID_INDEX*/>::ReleaseStudioAPI()
{
	pStudioAPIConstantBuffer = NULL;
}

/*
==================
CVertexFactoryBase::ClearStreams
==================
*/
template<class TBaseClass, uint32 instanceStreamSlot /*= INVALID_INDEX*/>
void CVertexFactoryBase<TBaseClass, instanceStreamSlot>::ClearStreams()
{
	Mem_Memzero( &indexStream, sizeof( vertexFactoryStream_t ) );
	vertexStreams.clear();
}

/*
==================
CVertexFactoryBase::AddVertexStream
==================
*/
template<class TBaseClass, uint32 instanceStreamSlot /*= INVALID_INDEX*/>
void CVertexFactoryBase<TBaseClass, instanceStreamSlot>::AddVertexStream( const vertexFactoryStream_t& vertexStream )
{
	vertexStreams.emplace_back( vertexStream );
}

/*
==================
CVertexFactoryBase::SetIndexStream
==================
*/
template<class TBaseClass, uint32 instanceStreamSlot /*= INVALID_INDEX*/>
void CVertexFactoryBase<TBaseClass, instanceStreamSlot>::SetIndexStream( const vertexFactoryStream_t& indexStream )
{
	CVertexFactoryBase::indexStream = indexStream;
}

/*
==================
CVertexFactoryBase::R_CreateConstantBuffer
==================
*/
template<class TBaseClass, uint32 instanceStreamSlot /*= INVALID_INDEX*/>
FORCEINLINE void CVertexFactoryBase<TBaseClass, instanceStreamSlot>::R_CreateConstantBuffer( uint32 size, const char* pDebugName )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( !pStudioAPIConstantBuffer && size > 0 );
	pStudioAPIConstantBuffer = g_pStudioAPI->CreateBuffer( NULL, size, size, STUDIOAPI_BUFFER_USAGE_FLAG_DYNAMIC | STUDIOAPI_BUFFER_USAGE_FLAG_CONSTANT_BUFFER | STUDIOAPI_BUFFER_USAGE_FLAG_TRANSFER_DST, pDebugName );
}

/*
==================
CVertexFactoryBase::R_UpdateConstantBuffer
==================
*/
template<class TBaseClass, uint32 instanceStreamSlot /*= INVALID_INDEX*/>
FORCEINLINE void CVertexFactoryBase<TBaseClass, instanceStreamSlot>::R_UpdateConstantBuffer( const byte* pData, uint32 size )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( pStudioAPIConstantBuffer && pData && size <= pStudioAPIConstantBuffer->GetSize() );
	pStudioAPIConstantBuffer->UpdateData( g_pStudioAPI->GetImmediateCmdContext( STUDIOAPI_QUEUE_TYPE_GRAPHICS ), (byte*)pData, size );
}

/*
==================
CVertexFactoryBase::R_UpdateInstanceBuffer
==================
*/
template<class TBaseClass, uint32 instanceStreamSlot /*= INVALID_INDEX*/>
FORCEINLINE void CVertexFactoryBase<TBaseClass, instanceStreamSlot>::R_UpdateInstanceBuffer( byte* pData, const byte* pInstances, uint32 numInstances ) const
{
}

/*
==================
CVertexFactoryBase::R_Barrier
==================
*/
template<class TBaseClass, uint32 instanceStreamSlot /*= INVALID_INDEX*/>
void CVertexFactoryBase<TBaseClass, instanceStreamSlot>::R_Barrier( IStudioAPICmdList* pStudioAPICmdList ) const
{
	// Allocate memory on the stack for barriers info
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	uint32 numBarriers = (uint32)vertexStreams.size();
	if ( IsIndexed() )
	{
		++numBarriers;
	}
	if ( pStudioAPIConstantBuffer )
	{
		++numBarriers;
	}
	uint32				barrierIndex = 0;
	studioAPIBarrier_t* pBarriers	 = (studioAPIBarrier_t*)Mem_Alloca( numBarriers * sizeof( studioAPIBarrier_t ) );

	// Make barriers for vertex buffers
	// TODO BS yehor.pohuliaka - Maybe in the future we should implement buffer barriers for specific offset/size in a buffer
	for ( uint32 index = 0, count = (uint32)vertexStreams.size(); index < count; ++index )
	{
		const vertexFactoryStream_t& vertexStream = vertexStreams[index];
		Assert( vertexStream.pStudioAPIBuffer );
		pBarriers[barrierIndex++] = StudioAPI_MakeBufferBarrier( vertexStream.pStudioAPIBuffer, STUDIOAPI_BUFFER_STATE_VERTEX_BUFFER, STUDIOAPI_QUEUE_TYPE_GRAPHICS );
	}

	// Make a barrier for an index buffer (if it exists)
	if ( IsIndexed() )
	{
		pBarriers[barrierIndex++] = StudioAPI_MakeBufferBarrier( indexStream.pStudioAPIBuffer, STUDIOAPI_BUFFER_STATE_INDEX_BUFFER, STUDIOAPI_QUEUE_TYPE_GRAPHICS );
	}

	// Make a barrier for an constant buffer (if it exists) and send the barriers into the command list
	if ( pStudioAPIConstantBuffer )
	{
		pBarriers[barrierIndex++] = StudioAPI_MakeBufferBarrier( pStudioAPIConstantBuffer, STUDIOAPI_BUFFER_STATE_CONSTANT_BUFFER, STUDIOAPI_QUEUE_TYPE_GRAPHICS );
	}
	pStudioAPICmdList->Barrier( pBarriers, numBarriers );
}

/*
==================
CVertexFactoryBase::R_SetupInstances
==================
*/
template<class TBaseClass, uint32 instanceStreamSlot>
CRefPtr<IStudioAPIBuffer> CVertexFactoryBase<TBaseClass, instanceStreamSlot>::R_CreateInstanceBuffer( const byte* pInstances, uint32 numInstances ) const
{
	// Create a instance buffer and upload data into it
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	if constexpr ( instanceStreamSlot != INVALID_INDEX )
	{
		Assert( pInstances && numInstances > 0 );
		studioAPIMappedBufferData_t mappedInstanceData = {};
		uint32						instanceStride	   = GetInstanceStride();
		uint64						bufferSize		   = (uint64)instanceStride * numInstances;
		CRefPtr<IStudioAPIBuffer>	pStudioAPIBuffer   = g_pStudioAPI->CreateBuffer( NULL, bufferSize, instanceStride, STUDIOAPI_BUFFER_USAGE_FLAG_VOLATILE | STUDIOAPI_BUFFER_USAGE_FLAG_VERTEX_BUFFER, "VertexFactory Instances" );
		pStudioAPIBuffer->MapMemory( bufferSize, 0, mappedInstanceData );
		R_UpdateInstanceBuffer( mappedInstanceData.pData, pInstances, numInstances );
		pStudioAPIBuffer->UnmapMemory( mappedInstanceData );
		return pStudioAPIBuffer;
	}
	return NULL;
}

/*
==================
CVertexFactoryBase::R_Bind
==================
*/
template<class TBaseClass, uint32 instanceStreamSlot /*= INVALID_INDEX*/>
void CVertexFactoryBase<TBaseClass, instanceStreamSlot>::R_Bind( IStudioAPICmdList* pStudioAPICmdList, const vertexFactoryStream_t& instanceStream )
{
	// Set the constant buffer
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	if ( pStudioAPIConstantBuffer )
	{
		pStudioAPICmdList->SetConstantBuffer( 0, STUDIO_RESOURCE_BINDING_SLOT_VERTEXFACTORY_CB, pStudioAPIConstantBuffer );
	}

	// Set all vertex buffers
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

	// Set the instance buffer
	if constexpr ( instanceStreamSlot != INVALID_INDEX )
	{
		if ( instanceStream.pStudioAPIBuffer )
		{
			Assert( instanceStreamSlot >= (uint32)vertexStreams.size() );
			pStudioAPICmdList->SetVertexBuffer( instanceStreamSlot, instanceStream.pStudioAPIBuffer, instanceStream.offset );
		}
	}
}

/*
==================
CVertexFactoryBase::R_BindUP
==================
*/
template<class TBaseClass, uint32 instanceStreamSlot /*= INVALID_INDEX*/>
void CVertexFactoryBase<TBaseClass, instanceStreamSlot>::R_BindUP( IStudioAPICmdList* pStudioAPICmdList, const vertexFactoryStreamUP_t& instanceStream, const vertexFactoryStreamUP_t* pVertexStreams, uint32 numVertexStreams, const vertexFactoryStreamUP_t* pIndexStream /* = NULL */ )
{
	// Set the constant buffer
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	if ( pStudioAPIConstantBuffer )
	{
		pStudioAPICmdList->SetConstantBuffer( 0, STUDIO_RESOURCE_BINDING_SLOT_VERTEXFACTORY_CB, pStudioAPIConstantBuffer );
	}

	// Set all vertex buffers
	Assert( pVertexStreams || numVertexStreams == 0 );
	for ( uint32 index = 0; index < numVertexStreams; ++index )
	{
		const vertexFactoryStreamUP_t& vertexStream = pVertexStreams[index];
		pStudioAPICmdList->SetVertexBufferUP( index, vertexStream.pData, vertexStream.numElements, vertexStream.stride );
	}

	// Set the index buffer
	if ( pIndexStream )
	{
		pStudioAPICmdList->SetIndexBufferUP( pIndexStream->pData, pIndexStream->numElements, pIndexStream->stride );
	}

	// Set the instance buffer
	if constexpr ( instanceStreamSlot != INVALID_INDEX )
	{
		if ( instanceStream.pData && instanceStream.numElements > 0 )
		{
			Assert( instanceStreamSlot >= (uint32)vertexStreams.size() );
			pStudioAPICmdList->SetVertexBufferUP( instanceStreamSlot, instanceStream.pData, instanceStream.numElements, instanceStream.stride );
		}
	}
}

/*
==================
CVertexFactoryBase::IsIndexed
==================
*/
template<class TBaseClass, uint32 instanceStreamSlot /*= INVALID_INDEX*/>
bool CVertexFactoryBase<TBaseClass, instanceStreamSlot>::IsIndexed() const
{
	return !!indexStream.pStudioAPIBuffer;
}

/*
==================
CVertexFactoryBase::IsInstanced
==================
*/
template<class TBaseClass, uint32 instanceStreamSlot /*= INVALID_INDEX*/>
bool CVertexFactoryBase<TBaseClass, instanceStreamSlot>::IsInstanced() const
{
	return instanceStreamSlot != INVALID_INDEX;
}

/*
==================
CVertexFactoryBase::GetVertexType
==================
*/
template<class TBaseClass, uint32 instanceStreamSlot /*= INVALID_INDEX*/>
modelVertexType_t CVertexFactoryBase<TBaseClass, instanceStreamSlot>::GetVertexType() const
{
	return (modelVertexType_t)vertexFactoryInfo_t<TBaseClass>::VERTEX_TYPE;
}

/*
==================
CVertexFactoryBase::GetVertexType
==================
*/
template<class TBaseClass, uint32 instanceStreamSlot /*= INVALID_INDEX*/>
uint32 CVertexFactoryBase<TBaseClass, instanceStreamSlot>::GetInstanceStride() const
{
	return sizeof( vertexFactoryInfo_t<TBaseClass>::instance_t );
}

/*
==================
CVertexFactoryBase::FinalRelease
==================
*/
template<class TBaseClass, uint32 instanceStreamSlot /*= INVALID_INDEX*/>
void CVertexFactoryBase<TBaseClass, instanceStreamSlot>::FinalRelease()
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
