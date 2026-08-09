#pragma once

/*
==================
IVertexFactory::IsA
==================
*/
template<class TVertexFactoryClass>
FORCEINLINE bool IVertexFactory::IsA() const
{
	return GetVertexType() == (modelVertexType_t)vertexFactoryInfo_t<TVertexFactoryClass>::VERTEX_TYPE;
}

/*
==================
IVertexFactory::As
==================
*/
template<class TVertexFactoryClass>
FORCEINLINE TVertexFactoryClass* IVertexFactory::As() const
{
	return Ensure( IsA<TVertexFactoryClass>() ) ? (TVertexFactoryClass*)this : NULL;
}
