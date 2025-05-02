#include "interfaces/interfaces.h"
#include "core/profile.h"
#include "filesystem/ifilesystem.h"
#include "stdlib/compression/zlib.h"
#include "smdldoc/smdl_compiled_doc.h"

// Singularity model magic
static const uint32		s_SMDLMagicSize					= 5;
static const achar		s_SMDLMagic[s_SMDLMagicSize]	= { 'S', 'M', 'D', 'L', 'C' };		// Singularity Model Compiled
static_assert( sizeof( s_SMDLMagic ) == s_SMDLMagicSize, "Size of s_SMDLMagic must be equal to s_SMDLMagicSize" );

// Singularity model version
static const uint32		s_SMDLVersion					= 1;


/*
==================
CSMDLCompiledModelDoc::CSMDLCompiledModelDoc
==================
*/
CSMDLCompiledModelDoc::CSMDLCompiledModelDoc()
{}

/*
==================
CSMDLCompiledModelDoc::SaveFile
==================
*/
bool CSMDLCompiledModelDoc::SaveFile( const achar* pPath )
{
	// Do nothing if the file system isn't valid
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );
	Assert( g_pFileSystem );
	
	// Try to open a file
	TRefPtr<IStreamDataWriter>	pFile = g_pFileSystem->CreateFileWriter( pPath );
	if ( !pFile )
	{
		Error( "SMDLDoc: Failed to open file '%s'", pPath );
		return false;
	}

	// Write format magic and version
	pFile->Write( ( void* )s_SMDLMagic, s_SMDLMagicSize );
	pFile->Write( ( void* )&s_SMDLVersion, sizeof( uint32 ) );

	// Write materials
	uint32		numMaterials = ( uint32 )materials.size();
	pFile->Write( &numMaterials, sizeof( uint32 ) );
	for ( uint32 materialIdx = 0; materialIdx < numMaterials; ++materialIdx )
	{
		const std::string&		material	= materials[materialIdx];
		uint32					sizeString	= ( uint32 )material.size();
		pFile->Write( &sizeString, sizeof( uint32 ) );
		pFile->Write( ( achar* )material.data(), sizeString * sizeof( achar ) );
	}

	// Write vertices
	uint32		numVertices = ( uint32 )vertices.size();
	pFile->Write( &numVertices, sizeof( uint32 ) );
	CZLib::Compress( pFile, ( byte* )vertices.data(), numVertices * sizeof( smdlVertex_t ) );

	// Write indices
	uint32		numIndices = ( uint32 )indices.size();
	pFile->Write( &numIndices, sizeof( uint32 ) );
	CZLib::Compress( pFile, ( byte* )indices.data(), numIndices * sizeof( uint32 ) );

	// Write surfaces
	uint32		numSurfaces = ( uint32 )surfaces.size();
	pFile->Write( &numSurfaces, sizeof( uint32 ) );
	pFile->Write( surfaces.data(), numSurfaces * sizeof( smdlSurface_t ) );

	// We are done
	return true;
}

/*
==================
CSMDLCompiledModelDoc::LoadFromFile
==================
*/
bool CSMDLCompiledModelDoc::LoadFromFile( const achar* pPath )
{
	// Do nothing if the file system isn't valid
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );
	Assert( g_pFileSystem );
	
	// Try to open a file
	TRefPtr<IStreamDataReader>	pFile = g_pFileSystem->CreateFileReader( pPath );
	if ( !pFile )
	{
		Error( "SMDLDoc: Failed to open file '%s'", pPath );
		return false;
	}

	// Read format magic and version
	achar	magic[s_SMDLMagicSize];
	uint32	version = 0;
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

	// Read materials
	uint32		numMaterials = 0;
	pFile->Read( &numMaterials, sizeof( uint32 ) );
	materials.resize( numMaterials );
	for ( uint32 materialIdx = 0; materialIdx < numMaterials; ++materialIdx )
	{
		std::string&		material	= materials[materialIdx];
		uint32				sizeString	= 0;
		pFile->Read( &sizeString, sizeof( uint32 ) );
		material.resize( sizeString );
		pFile->Read( ( achar* )material.data(), sizeString * sizeof( achar ) );
	}

	// Read vertices
	uint32		numVertices = 0;
	pFile->Read( &numVertices, sizeof( uint32 ) );
	vertices.resize( numVertices );
	CZLib::Uncompress( pFile, ( byte* )vertices.data(), numVertices * sizeof( smdlVertex_t ) );

	// Write indices
	uint32		numIndices = 0;
	pFile->Read( &numIndices, sizeof( uint32 ) );
	indices.resize( numIndices );
	CZLib::Uncompress( pFile, ( byte* )indices.data(), numIndices * sizeof( uint32 ) );

	// Write surfaces
	uint32		numSurfaces = 0;
	pFile->Read( &numSurfaces, sizeof( uint32 ) );
	surfaces.resize( numSurfaces );
	pFile->Read( surfaces.data(), numSurfaces * sizeof( smdlSurface_t ) );

	// We are done
	return true;
}