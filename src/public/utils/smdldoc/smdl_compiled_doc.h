#pragma once
#include <EASTL/vector.h>

#include "tier0/tier0.h"
#include "modelsystem/modeltypes.h"

//-----------------------------------------------------------------------------
// Helper for work with compiled SMDL (Singularity Model) files
//-----------------------------------------------------------------------------
struct smdlVertex_t
{
	vector4_t position;
	vector2_t texCoord;
	vector4_t normal;
	vector4_t tangent;
	vector4_t binormal;
};

struct smdlSurface_t
{
	uint32 materialID;
	uint32 baseVertexIndex;
	uint32 firstIndex;
	uint32 numIndices;
};

class CSMDLCompiledModelDoc
{
public:
	CSMDLCompiledModelDoc();

	// Load and save a file
	// NOTE: For use Tier1 must be connected by ConnectTier1
	bool LoadFromFile( const char* pPath );
	bool SaveFile( const char* pPath );
	void Clear();

	void SetData( const eastl::vector<smdlVertex_t>& vertices, const eastl::vector<uint32>& indices, const eastl::vector<smdlSurface_t>& surfaces, const eastl::vector<eastl::string>& materials );
	void SetData( eastl::vector<smdlVertex_t>& vertices, eastl::vector<uint32>& indices, eastl::vector<smdlSurface_t>& surfaces, eastl::vector<eastl::string>& materials );

	const eastl::vector<smdlVertex_t>&	GetVertices() const;
	const eastl::vector<uint32>&		GetIndices() const;
	const eastl::vector<smdlSurface_t>& GetSurfaces() const;
	const eastl::vector<eastl::string>& GetMaterials() const;

private:
	eastl::vector<smdlVertex_t>	 vertices;
	eastl::vector<uint32>		 indices;
	eastl::vector<smdlSurface_t> surfaces;
	eastl::vector<eastl::string> materials;
};

#include "utils/smdldoc/smdl_compiled_doc.inl"
