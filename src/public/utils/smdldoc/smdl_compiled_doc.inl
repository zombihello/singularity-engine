#pragma once

/*
==================
CSMDLCompiledModelDoc::GetVertexType
==================
*/
FORCEINLINE modelVertexType_t CSMDLCompiledModelDoc::GetVertexType() const
{
	return vertexType;
}

/*
==================
CSMDLCompiledModelDoc::GetIndexType
==================
*/
FORCEINLINE modelIndexType_t CSMDLCompiledModelDoc::GetIndexType() const
{
	return indexType;
}

/*
==================
CSMDLCompiledModelDoc::GetSizeVertices
==================
*/
FORCEINLINE uint32 CSMDLCompiledModelDoc::GetSizeVertices() const
{
	return sizeVertices;
}

/*
==================
CSMDLCompiledModelDoc::GetSizeIndices
==================
*/
FORCEINLINE uint32 CSMDLCompiledModelDoc::GetSizeIndices() const
{
	return sizeIndices;
}

/*
==================
CSMDLCompiledModelDoc::GetNumSurfaces
==================
*/
FORCEINLINE uint32 CSMDLCompiledModelDoc::GetNumSurfaces() const
{
	return numSurfaces;
}

/*
==================
CSMDLCompiledModelDoc::GetNumMaterials
==================
*/
FORCEINLINE uint32 CSMDLCompiledModelDoc::GetNumMaterials() const
{
	return numMaterials;
}

/*
==================
CSMDLCompiledModelDoc::GetVertices
==================
*/
FORCEINLINE const byte* CSMDLCompiledModelDoc::GetVertices() const
{
	return pVertices;
}

/*
==================
CSMDLCompiledModelDoc::GetIndices
==================
*/
FORCEINLINE const byte* CSMDLCompiledModelDoc::GetIndices() const
{
	return pIndices;
}

/*
==================
CSMDLCompiledModelDoc::GetSurfaces
==================
*/
FORCEINLINE const modelSurface_t* CSMDLCompiledModelDoc::GetSurfaces() const
{
	return pSurfaces;
}

/*
==================
CSMDLCompiledModelDoc::GetMaterials
==================
*/
FORCEINLINE const char** CSMDLCompiledModelDoc::GetMaterials() const
{
	return pMaterials;
}