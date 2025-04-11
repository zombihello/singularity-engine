/**
 * @file
 * @addtogroup smdldoc smdldoc
 */

#ifndef SMDL_COMPILED_DOC_H
#define SMDL_COMPILED_DOC_H

#include <vector>

#include "core/core.h"
#include "studiorender/studio_vertextypes.h"

/**
 * @ingroup smdldoc
 * @brief SMDL vertex
 */
struct smdlVertex_t
{
	vec4_t		position;		/**< Position vertex */
	vec2_t		texCoord;		/**< Texture coords */
	vec4_t		normal;			/**< Normal */
	vec4_t		tangent;		/**< Tangent */
	vec4_t		binormal;		/**< Binormal */
};

/**
 * @ingroup smdldoc
 * @brief SMDL surface
 */
struct smdlSurface_t
{
	uint32		materialID;			/**< Material ID */
	uint32		baseVertexIndex;	/**< First vertex index in the vertex buffer */
	uint32		firstIndex;			/**< First index in the index buffer */
	uint32		numIndices;			/**< Indices number in the surface */
};

/**
 * @ingroup smdldoc
 * @brief Helper for work with compiled SMDL (Singularity Model) files
 */
class CSMDLCompiledModelDoc
{
public:
	/**
	 * @brief Constructor
	 */
	CSMDLCompiledModelDoc();

	/**
	 * @brief Load from a file
	 * @warning For use StdLib must be connected by ConnectStdLib
	 *
	 * @param pPath		Path to file
	 * @return Return TRUE when a file success loaded, otherwise return FALSE
	 */
	bool LoadFromFile( const achar* pPath );

	/**
	 * @brief Save the texture document
	 * @warning For use StdLib must be connected by ConnectStdLib
	 *
	 * @param pPath		Path to save
	 * @return Return TRUE if the texture was successes saved, otherwise return FALSE
	 */
	bool SaveFile( const achar* pPath );

	/**
	 * @brief Set data
	 * @param vertices		Vertices
	 * @param indices		Indices
	 * @param surfaces		Surfaces
	 * @param materials		Materials
	 */
	FORCEINLINE void SetData( const std::vector<smdlVertex_t>& vertices, const std::vector<uint32>& indices, const std::vector<smdlSurface_t>& surfaces, const std::vector<std::string>& materials )
	{
		CSMDLCompiledModelDoc::vertices		= vertices;
		CSMDLCompiledModelDoc::indices		= indices;
		CSMDLCompiledModelDoc::surfaces		= surfaces;
		CSMDLCompiledModelDoc::materials	= materials;
	}

	/**
	 * @brief Set data
	 * @param vertices		Vertices
	 * @param indices		Indices
	 * @param surfaces		Surfaces
	 * @param materials		Materials
	 */
	FORCEINLINE void SetData( std::vector<smdlVertex_t>& vertices, std::vector<uint32>& indices, std::vector<smdlSurface_t>& surfaces, std::vector<std::string>& materials )
	{
		CSMDLCompiledModelDoc::vertices		= std::move( vertices );
		CSMDLCompiledModelDoc::indices		= std::move( indices );
		CSMDLCompiledModelDoc::surfaces		= std::move( surfaces );
		CSMDLCompiledModelDoc::materials	= std::move( materials );
	}

	/**
	 * @brief Get vertices
	 * @return Return vertices
	 */
	FORCEINLINE const std::vector<smdlVertex_t>& GetVertices() const
	{
		return vertices;
	}

	/**
	 * @brief Get indices
	 * @return Return indices
	 */
	FORCEINLINE const std::vector<uint32>& GetIndices() const
	{
		return indices;
	}

	/**
	 * @brief Get surfaces
	 * @return Return surfaces
	 */
	FORCEINLINE const std::vector<smdlSurface_t>& GetSurfaces() const
	{
		return surfaces;
	}

	/**
	 * @brief Get materials
	 * @return Return materials
	 */
	FORCEINLINE const std::vector<std::string>& GetMaterials() const
	{
		return materials;
	}

private:
	std::vector<smdlVertex_t>			vertices;		/**< Vertices array */
	std::vector<uint32>					indices;		/**< Indices array */
	std::vector<smdlSurface_t>			surfaces;		/**< Model surfaces */
	std::vector<std::string>			materials;		/**< Materials array */
};

#endif // !SMDL_COMPILED_DOC_H