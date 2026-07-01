#pragma once
#include <EASTL/vector.h>

#include "tier0/tier0.h"
#include "modelsystem/modeltypes.h"

//-----------------------------------------------------------------------------
// Types to initialize SMDL data
//-----------------------------------------------------------------------------
enum smdlInitialDataFlag_t
{
	SMDL_INITIALDATA_FLAG_NONE				  = 0,
	SMDL_INITIALDATA_FLAG_REFERENCE_VERTICES  = BIT( 0 ),
	SMDL_INITIALDATA_FLAG_REFERENCE_INDICES	  = BIT( 1 ),
	SMDL_INITIALDATA_FLAG_REFERENCE_SURFACES  = BIT( 2 ),
	SMDL_INITIALDATA_FLAG_REFERENCE_MATERIALS = BIT( 3 )
};

struct smdlInitialData_t
{
	modelVertexType_t	  vertexType;
	modelIndexType_t	  indexType;
	uint32				  sizeVertices;
	uint32				  sizeIndices;
	uint32				  numSurfaces;
	uint32				  numMaterials;
	const byte*			  pVertices;
	const byte*			  pIndices;
	const modelSurface_t* pSurfaces;
	const char**		  pMaterials;
};

//-----------------------------------------------------------------------------
// Helper for work with compiled SMDL (Singularity Model) files
//-----------------------------------------------------------------------------
class CSMDLCompiledModelDoc
{
public:
	CSMDLCompiledModelDoc();
	~CSMDLCompiledModelDoc();

	// Load and save a file
	// NOTE: For use Tier1 must be connected by ConnectTier1
	bool LoadFromFile( const char* pPath );
	bool SaveFile( const char* pPath );
	void Clear();

	void SetData( const smdlInitialData_t& initialData, uint8 flags = SMDL_INITIALDATA_FLAG_NONE );

	modelVertexType_t	  GetVertexType() const;
	modelIndexType_t	  GetIndexType() const;
	uint32				  GetSizeVertices() const;
	uint32				  GetSizeIndices() const;
	uint32				  GetNumSurfaces() const;
	uint32				  GetNumMaterials() const;
	const byte*			  GetVertices() const;
	const byte*			  GetIndices() const;
	const modelSurface_t* GetSurfaces() const;
	const char**		  GetMaterials() const;

private:
	enum smdlDataFlag_t
	{
		SMDL_DATA_FLAG_NONE			 = 0,
		SMDL_DATA_FLAG_OWN_VERTICES	 = BIT( 0 ),
		SMDL_DATA_FLAG_OWN_INDICES	 = BIT( 1 ),
		SMDL_DATA_FLAG_OWN_SURFACES	 = BIT( 2 ),
		SMDL_DATA_FLAG_OWN_MATERIALS = BIT( 3 )
	};

	modelVertexType_t	  vertexType;
	modelIndexType_t	  indexType;
	uint8				  flags;
	uint32				  sizeVertices;
	uint32				  sizeIndices;
	uint32				  numSurfaces;
	uint32				  numMaterials;
	const byte*			  pVertices;
	const byte*			  pIndices;
	const modelSurface_t* pSurfaces;
	const char**		  pMaterials;
};

#include "utils/smdldoc/smdl_compiled_doc.inl"
