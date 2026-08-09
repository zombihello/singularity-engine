#pragma once

/*
==================
IModelSystem::CreateVertexFactory
==================
*/
template<class TVertexFactoryClass>
FORCEINLINE CRefPtr<TVertexFactoryClass> IModelSystem::CreateVertexFactory() const
{
	CRefPtr<IVertexFactory> pVertexFactory = CreateVertexFactory( (modelVertexType_t)vertexFactoryInfo_t<TVertexFactoryClass>::VERTEX_TYPE );
	return (TVertexFactoryClass*)pVertexFactory.GetRawPtr();
}
