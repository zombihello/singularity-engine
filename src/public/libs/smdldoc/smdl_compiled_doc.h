#pragma once
#include <EASTL/vector.h>

#include "core/core.h"
#include "studiorender/studio_vertextypes.h"

//-----------------------------------------------------------------------------
// Helper for work with compiled SMDL (Singularity Model) files
//-----------------------------------------------------------------------------
struct smdlVertex_t
{
	vec4_t position;
	vec2_t texCoord;
	vec4_t normal;
	vec4_t tangent;
	vec4_t binormal;
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
	// NOTE: For use StdLib must be connected by ConnectStdLib
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

#include "smdldoc/smdl_compiled_doc.inl"
