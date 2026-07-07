#include "utils/interfaces/interfaces.h"
#include "tier0/assert.h"
#include "tier0/ilogger.h"
#include "tier0/iprofiler.h"
#include "filesystem/ifilesystem.h"
#include "tier1/compression.h"
#include "utils/smdldoc/smdl_compiled_doc.h"

// Singularity model magic
static const uint32 s_SMDLMagicSize				 = 5;
static const char	s_SMDLMagic[s_SMDLMagicSize] = { 'S', 'M', 'D', 'L', 'C' };	 // Singularity Model Compiled
static_assert( sizeof( s_SMDLMagic ) == s_SMDLMagicSize, "Size of s_SMDLMagic must be equal to s_SMDLMagicSize" );

// Singularity model version
static const uint32 s_SMDLVersion = 1;

/*
==================
CSMDLCompiledModelDoc::CSMDLCompiledModelDoc
==================
*/
CSMDLCompiledModelDoc::CSMDLCompiledModelDoc()
	: vertexType( MODEL_VERTEX_NUM_TYPES )
	, indexType( MODEL_INDEX_NUM_TYPES )
	, flags( SMDL_DATA_FLAG_NONE )
	, sizeVertices( 0 )
	, sizeIndices( 0 )
	, numSurfaces( 0 )
	, numMaterials( 0 )
	, pVertices( NULL )
	, pIndices( NULL )
	, pSurfaces( NULL )
	, pMaterials( NULL )
{
}

/*
==================
CSMDLCompiledModelDoc::~CSMDLCompiledModelDoc
==================
*/
CSMDLCompiledModelDoc::~CSMDLCompiledModelDoc()
{
	Clear();
}

/*
==================
CSMDLCompiledModelDoc::SaveFile
==================
*/
bool CSMDLCompiledModelDoc::SaveFile( const char* pPath )
{
	// Do nothing if the file system isn't valid
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_IO );
	Assert( g_pFileSystem );

	// Try to open a file
	CRefPtr<IStreamDataWriter> pFile = g_pFileSystem->CreateFileWriter( pPath );
	if ( !pFile )
	{
		Error( "SMDLDoc: Failed to open file '%s'", pPath );
		return false;
	}

	// Write format magic and version
	pFile->Write( (void*)s_SMDLMagic, s_SMDLMagicSize );
	pFile->Write( (void*)&s_SMDLVersion, sizeof( uint32 ) );

	// Write vertex and index type
	pFile->Write( &vertexType, sizeof( modelVertexType_t ) );
	pFile->Write( &indexType, sizeof( modelIndexType_t ) );

	// Write materials
	pFile->Write( &numMaterials, sizeof( uint32 ) );
	for ( uint32 materialIdx = 0; materialIdx < numMaterials; ++materialIdx )
	{
		const char* pMaterial = pMaterials[materialIdx];
		uint32		strLen	  = S_Strlen( pMaterial );
		pFile->Write( &strLen, sizeof( uint32 ) );
		pFile->Write( (char*)pMaterial, strLen * sizeof( char ) );
	}

	// Write vertices
	pFile->Write( &sizeVertices, sizeof( uint32 ) );
	CompressStreamData( COMPRESSION_ZLIB, pFile, (byte*)pVertices, sizeVertices );

	// Write indices
	pFile->Write( &sizeIndices, sizeof( uint32 ) );
	CompressStreamData( COMPRESSION_ZLIB, pFile, (byte*)pIndices, sizeIndices );

	// Write surfaces
	pFile->Write( &numSurfaces, sizeof( uint32 ) );
	pFile->Write( (byte*)pSurfaces, numSurfaces * sizeof( modelSurface_t ) );

	// We are done
	return true;
}

/*
==================
CSMDLCompiledModelDoc::LoadFromFile
==================
*/
bool CSMDLCompiledModelDoc::LoadFromFile( const char* pPath )
{
	// Do nothing if the file system isn't valid
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_IO );
	Assert( g_pFileSystem );

	// Try to open a file
	CRefPtr<IStreamDataReader> pFile = g_pFileSystem->CreateFileReader( pPath );
	if ( !pFile )
	{
		Error( "SMDLDoc: Failed to open file '%s'", pPath );
		return false;
	}

	// Read format magic and version
	char   magic[s_SMDLMagicSize];
	uint32 version = 0;
	pFile->Read( magic, s_SMDLMagicSize );
	pFile->Read( &version, sizeof( uint32 ) );
	if ( S_Strncmp( magic, s_SMDLMagic, s_SMDLMagicSize ) )
	{
		Error( "SMDLDoc: Invalid magic value in '%s'. Expected '%.*s', received '%.*s'", pPath, s_SMDLMagicSize, s_SMDLMagic, s_SMDLMagicSize, magic );
		return false;
	}

	if ( version != s_SMDLVersion )
	{
		Error( "SMDLDoc: Invalid version format in '%s'. Expected %i, received %i", pPath, s_SMDLVersion, version );
		return false;
	}
	Clear();

	// Read vertex and index type
	pFile->Read( &vertexType, sizeof( modelVertexType_t ) );
	pFile->Read( &indexType, sizeof( modelIndexType_t ) );

	// Validate vertex type and index type
	if ( vertexType >= MODEL_VERTEX_NUM_TYPES )
	{
		Error( "SMDLDoc: Invalid vertex type in the model, received 0x%X", vertexType );
		return false;
	}
	if ( indexType >= MODEL_INDEX_NUM_TYPES )
	{
		Error( "SMDLDoc: Invalid index type in the model, received 0x%X", indexType );
		return false;
	}

	// Read materials
	pFile->Read( &numMaterials, sizeof( uint32 ) );
	if ( numMaterials > 0 )
	{
		flags |= SMDL_DATA_FLAG_OWN_MATERIALS;
		pMaterials = new const char*[numMaterials];
		for ( uint32 materialIdx = 0; materialIdx < numMaterials; ++materialIdx )
		{
			uint32 strLen = 0;
			pFile->Read( &strLen, sizeof( uint32 ) );
			pMaterials[materialIdx] = new char[strLen + 1];
			char* pMaterial			= (char*)pMaterials[materialIdx];
			pFile->Read( (char*)pMaterial, strLen * sizeof( char ) );
			pMaterial[strLen] = 0;
		}
	}

	// Read vertices
	pFile->Read( &sizeVertices, sizeof( uint32 ) );
	if ( sizeVertices > 0 )
	{
		flags |= SMDL_DATA_FLAG_OWN_VERTICES;
		pVertices = new byte[sizeVertices];
		UncompressStreamData( COMPRESSION_ZLIB, pFile, (byte*)pVertices, sizeVertices );
	}

	// Write indices
	pFile->Read( &sizeIndices, sizeof( uint32 ) );
	if ( sizeIndices > 0 )
	{
		flags |= SMDL_DATA_FLAG_OWN_INDICES;
		pIndices = new byte[sizeIndices];
		UncompressStreamData( COMPRESSION_ZLIB, pFile, (byte*)pIndices, sizeIndices );
	}

	// Write surfaces
	pFile->Read( &numSurfaces, sizeof( uint32 ) );
	if ( numSurfaces > 0 )
	{
		flags |= SMDL_DATA_FLAG_OWN_SURFACES;
		pSurfaces = new modelSurface_t[numSurfaces];
		pFile->Read( (byte*)pSurfaces, numSurfaces * sizeof( modelSurface_t ) );
	}

	// We are done
	return true;
}

/*
==================
CSMDLCompiledModelDoc::SetData
==================
*/
void CSMDLCompiledModelDoc::SetData( const smdlInitialData_t& initialData, uint8 flags )
{
	// Clear old data
	PROFILER_SCOPE_FUNC();
	Clear();

	// Copy all fields
	vertexType	 = initialData.vertexType;
	indexType	 = initialData.indexType;
	sizeVertices = initialData.sizeVertices;
	sizeIndices	 = initialData.sizeIndices;
	numSurfaces	 = initialData.numSurfaces;
	numMaterials = initialData.numMaterials;

	// Copy vertices
	if ( flags & SMDL_INITIALDATA_FLAG_REFERENCE_VERTICES )
	{
		pVertices = initialData.pVertices;
	}
	else
	{
		flags |= SMDL_DATA_FLAG_OWN_VERTICES;
		pVertices = new byte[sizeVertices];
		Mem_Memcpy( (byte*)pVertices, initialData.pVertices, sizeVertices );
	}

	// Copy indices
	if ( flags & SMDL_INITIALDATA_FLAG_REFERENCE_INDICES )
	{
		pIndices = initialData.pIndices;
	}
	else
	{
		flags |= SMDL_DATA_FLAG_OWN_INDICES;
		pIndices = new byte[sizeIndices];
		Mem_Memcpy( (byte*)pIndices, initialData.pIndices, sizeIndices );
	}

	// Copy surfaces
	if ( flags & SMDL_INITIALDATA_FLAG_REFERENCE_SURFACES )
	{
		pSurfaces = initialData.pSurfaces;
	}
	else
	{
		flags |= SMDL_DATA_FLAG_OWN_SURFACES;
		pSurfaces = new modelSurface_t[numSurfaces];
		Mem_Memcpy( (byte*)pSurfaces, initialData.pSurfaces, numSurfaces * sizeof( modelSurface_t ) );
	}

	// Copy materials
	if ( flags & SMDL_INITIALDATA_FLAG_REFERENCE_MATERIALS )
	{
		pMaterials = initialData.pMaterials;
	}
	else
	{
		flags |= SMDL_DATA_FLAG_OWN_MATERIALS;
		pMaterials = new const char*[numMaterials];
		for ( uint32 materialIdx = 0; materialIdx < numMaterials; ++materialIdx )
		{
			uint32 strLen			= S_Strlen( initialData.pMaterials[materialIdx] );
			pMaterials[materialIdx] = new char[strLen + 1];
			char* pMaterial			= (char*)pMaterials[materialIdx];
			Mem_Memcpy( (byte*)pMaterial, initialData.pMaterials[materialIdx], strLen * sizeof( char ) );
			pMaterial[strLen] = 0;
		}
	}
}

/*
==================
CSMDLCompiledModelDoc::Clear
==================
*/
void CSMDLCompiledModelDoc::Clear()
{
	PROFILER_SCOPE_FUNC();
	if ( pVertices && ( flags & SMDL_DATA_FLAG_OWN_VERTICES ) )
	{
		delete[] pVertices;
	}
	if ( pIndices && ( flags & SMDL_DATA_FLAG_OWN_INDICES ) )
	{
		delete[] pIndices;
	}
	if ( pSurfaces && ( flags & SMDL_DATA_FLAG_OWN_SURFACES ) )
	{
		delete[] pSurfaces;
	}
	if ( pMaterials && ( flags & SMDL_DATA_FLAG_OWN_MATERIALS ) )
	{
		for ( uint32 materialIdx = 0; materialIdx < numMaterials; ++materialIdx )
		{
			delete[] pMaterials[materialIdx];
		}
		delete[] pMaterials;
	}

	vertexType	 = MODEL_VERTEX_NUM_TYPES;
	indexType	 = MODEL_INDEX_NUM_TYPES;
	flags		 = SMDL_DATA_FLAG_NONE;
	sizeVertices = 0;
	sizeIndices	 = 0;
	numSurfaces	 = 0;
	numMaterials = 0;
	pVertices	 = NULL;
	pIndices	 = NULL;
	pSurfaces	 = NULL;
	pMaterials	 = NULL;
}