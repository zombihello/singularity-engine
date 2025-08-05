#pragma once

/*
==================
CSMDLCompiledModelDoc::Clear
==================
*/
FORCEINLINE void CSMDLCompiledModelDoc::Clear()
{
	vertices.clear();
	indices.clear();
	surfaces.clear();
	materials.clear();
}

/*
==================
CSMDLCompiledModelDoc::SetData
==================
*/
FORCEINLINE void CSMDLCompiledModelDoc::SetData( const std::vector<smdlVertex_t>& vertices, const std::vector<uint32>& indices, const std::vector<smdlSurface_t>& surfaces, const std::vector<std::string>& materials )
{
	CSMDLCompiledModelDoc::vertices		= vertices;
	CSMDLCompiledModelDoc::indices		= indices;
	CSMDLCompiledModelDoc::surfaces		= surfaces;
	CSMDLCompiledModelDoc::materials	= materials;
}

/*
==================
CSMDLCompiledModelDoc::SetData
==================
*/
FORCEINLINE void CSMDLCompiledModelDoc::SetData( std::vector<smdlVertex_t>& vertices, std::vector<uint32>& indices, std::vector<smdlSurface_t>& surfaces, std::vector<std::string>& materials )
{
	CSMDLCompiledModelDoc::vertices		= std::move( vertices );
	CSMDLCompiledModelDoc::indices		= std::move( indices );
	CSMDLCompiledModelDoc::surfaces		= std::move( surfaces );
	CSMDLCompiledModelDoc::materials	= std::move( materials );
}

/*
==================
CSMDLCompiledModelDoc::GetVertices
==================
*/
FORCEINLINE const std::vector<smdlVertex_t>& CSMDLCompiledModelDoc::GetVertices() const
{
	return vertices;
}

/*
==================
CSMDLCompiledModelDoc::GetIndices
==================
*/
FORCEINLINE const std::vector<uint32>& CSMDLCompiledModelDoc::GetIndices() const
{
	return indices;
}

/*
==================
CSMDLCompiledModelDoc::GetSurfaces
==================
*/
FORCEINLINE const std::vector<smdlSurface_t>& CSMDLCompiledModelDoc::GetSurfaces() const
{
	return surfaces;
}

/*
==================
CSMDLCompiledModelDoc::GetMaterials
==================
*/
FORCEINLINE const std::vector<std::string>& CSMDLCompiledModelDoc::GetMaterials() const
{
	return materials;
}