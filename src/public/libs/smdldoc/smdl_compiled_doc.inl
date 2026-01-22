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
FORCEINLINE void CSMDLCompiledModelDoc::SetData( const eastl::vector<smdlVertex_t>& vertices, const eastl::vector<uint32>& indices, const eastl::vector<smdlSurface_t>& surfaces, const eastl::vector<eastl::string>& materials )
{
	CSMDLCompiledModelDoc::vertices	 = vertices;
	CSMDLCompiledModelDoc::indices	 = indices;
	CSMDLCompiledModelDoc::surfaces	 = surfaces;
	CSMDLCompiledModelDoc::materials = materials;
}

/*
==================
CSMDLCompiledModelDoc::SetData
==================
*/
FORCEINLINE void CSMDLCompiledModelDoc::SetData( eastl::vector<smdlVertex_t>& vertices, eastl::vector<uint32>& indices, eastl::vector<smdlSurface_t>& surfaces, eastl::vector<eastl::string>& materials )
{
	CSMDLCompiledModelDoc::vertices	 = eastl::move( vertices );
	CSMDLCompiledModelDoc::indices	 = eastl::move( indices );
	CSMDLCompiledModelDoc::surfaces	 = eastl::move( surfaces );
	CSMDLCompiledModelDoc::materials = eastl::move( materials );
}

/*
==================
CSMDLCompiledModelDoc::GetVertices
==================
*/
FORCEINLINE const eastl::vector<smdlVertex_t>& CSMDLCompiledModelDoc::GetVertices() const
{
	return vertices;
}

/*
==================
CSMDLCompiledModelDoc::GetIndices
==================
*/
FORCEINLINE const eastl::vector<uint32>& CSMDLCompiledModelDoc::GetIndices() const
{
	return indices;
}

/*
==================
CSMDLCompiledModelDoc::GetSurfaces
==================
*/
FORCEINLINE const eastl::vector<smdlSurface_t>& CSMDLCompiledModelDoc::GetSurfaces() const
{
	return surfaces;
}

/*
==================
CSMDLCompiledModelDoc::GetMaterials
==================
*/
FORCEINLINE const eastl::vector<eastl::string>& CSMDLCompiledModelDoc::GetMaterials() const
{
	return materials;
}