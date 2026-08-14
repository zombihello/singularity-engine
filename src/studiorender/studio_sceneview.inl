#pragma once

/*
==================
studioSurfaceBatchKey_t::operator==
==================
*/
FORCEINLINE bool studioSurfaceBatchKey_t::operator==( const studioSurfaceBatchKey_t& other ) const
{
	return modelId == other.modelId && materialId == other.materialId && surfaceId == other.surfaceId;
}

/*
==================
studioSurfaceBatchKeyHash_t::operator()
==================
*/
FORCEINLINE size studioSurfaceBatchKeyHash_t::operator()( const studioSurfaceBatchKey_t& key ) const
{
	return FastHash( key );
}

/*
==================
studioVertexFactoryInstances_t::studioVertexFactoryInstances_t
==================
*/
FORCEINLINE studioVertexFactoryInstances_t::studioVertexFactoryInstances_t( IVertexFactory* pVertexFactory )
	: numInstances( 0 )
	, pVertexFactory( pVertexFactory )
{
	// Reserve memory only if the vertex factory has an instance buffer
	Assert( pVertexFactory );
	if ( pVertexFactory->IsInstanced() )
	{
		data.reserve( STUDIO_VERTEXFACTORY_INITIAL_NUM_INSTANCES * pVertexFactory->GetInstanceStride() );
	}
}

/*
==================
studioVertexFactoryInstances_t::Add
==================
*/
template<typename TInstanceType>
FORCEINLINE TInstanceType* studioVertexFactoryInstances_t::Add( uint32 count /* = 1 */ )
{
	// If the vertex factory has an instance buffer allocate data for new instances
	Assert( (modelVertexType_t)modelInstanceInfo_t<TInstanceType>::VERTEX_TYPE == pVertexFactory->GetVertexType() );
	TInstanceType* pInstance = NULL;
	if ( pVertexFactory->IsInstanced() )
	{
		uint64 offset = (uint64)data.size();
		data.resize( offset + (uint64)count * sizeof( TInstanceType ) );
		pInstance = (TInstanceType*)( data.data() + offset );
	}

	// Increase the instance number
	numInstances += count;
	return pInstance;
}

/*
==================
studioVertexFactoryInstances_t::R_Upload
==================
*/
FORCEINLINE void studioVertexFactoryInstances_t::R_Upload()
{
	if ( !IsEmpty() && pVertexFactory->IsInstanced() && !pStudioAPIBuffer )
	{
		pStudioAPIBuffer = pVertexFactory->R_CreateInstanceBuffer( data.data(), numInstances );
	}
}

/*
==================
studioVertexFactoryInstances_t::GetNumInstances
==================
*/
FORCEINLINE uint32 studioVertexFactoryInstances_t::GetNumInstances() const
{
	return numInstances;
}

/*
==================
studioVertexFactoryInstances_t::IsEmpty
==================
*/
FORCEINLINE bool studioVertexFactoryInstances_t::IsEmpty() const
{
	return numInstances == 0;
}

/*
==================
studioVertexFactoryInstances_t::GetVertexFactoryStream
==================
*/
FORCEINLINE vertexFactoryStream_t studioVertexFactoryInstances_t::GetVertexFactoryStream() const
{
	return vertexFactoryStream_t{ pStudioAPIBuffer, 0 };
}

/*
==================
studioSurfaceBatch_t::studioSurfaceBatch_t
==================
*/
FORCEINLINE studioSurfaceBatch_t::studioSurfaceBatch_t( IVertexFactory* pVertexFactory )
	: instances( pVertexFactory )
{
}

/*
==================
studioRenderPass_t::studioRenderPass_t
==================
*/
FORCEINLINE studioRenderPass_t::studioRenderPass_t()
{
	surfaceBatchIds.reserve( STUDIO_RENDERPASS_INITIAL_NUM_SURFACEBATCHIDS );
	resourceIds.reserve( STUDIO_RENDERPASS_INITIAL_NUM_RESOURCEIDS );
}

/*
==================
studioSceneView_t::studioSceneView_t
==================
*/
FORCEINLINE studioSceneView_t::studioSceneView_t()
	: pEntityViews( NULL )
{
	surfaceBatches.reserve( STUDIO_SCENEVIEW_INITIAL_NUM_SURFACEBATCHES );
	resources.reserve( STUDIO_SCENEVIEW_INITIAL_NUM_RESOURCES );
	resourceDict.reserve( STUDIO_SCENEVIEW_INITIAL_NUM_RESOURCES );
	surfaceBatchDict.reserve( STUDIO_SCENEVIEW_INITIAL_NUM_SURFACEBATCHES );
}
