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
CVertexFactoryBase::ClearStreams
==================
*/
FORCEINLINE void CVertexFactoryBase::ClearStreams()
{
	Mem_Memzero( &indexStream, sizeof( vertexFactoryStream_t ) );
	vertexStreams.clear();
}

/*
==================
CVertexFactoryBase::AddVertexStream
==================
*/
FORCEINLINE void CVertexFactoryBase::AddVertexStream( const vertexFactoryStream_t& vertexStream )
{
	vertexStreams.emplace_back( vertexStream );
}

/*
==================
CVertexFactoryBase::SetIndexStream
==================
*/
FORCEINLINE void CVertexFactoryBase::SetIndexStream( const vertexFactoryStream_t& indexStream )
{
	CVertexFactoryBase::indexStream = indexStream;
}
