#pragma once
#include <vector>

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

	void SetData( const std::vector<smdlVertex_t>& vertices, const std::vector<uint32>& indices, const std::vector<smdlSurface_t>& surfaces, const std::vector<std::string>& materials );
	void SetData( std::vector<smdlVertex_t>& vertices, std::vector<uint32>& indices, std::vector<smdlSurface_t>& surfaces, std::vector<std::string>& materials );

	const std::vector<smdlVertex_t>&  GetVertices() const;
	const std::vector<uint32>&		  GetIndices() const;
	const std::vector<smdlSurface_t>& GetSurfaces() const;
	const std::vector<std::string>&	  GetMaterials() const;

private:
	std::vector<smdlVertex_t>  vertices;
	std::vector<uint32>		   indices;
	std::vector<smdlSurface_t> surfaces;
	std::vector<std::string>   materials;
};

#include "smdldoc/smdl_compiled_doc.inl"