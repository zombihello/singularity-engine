#include "interfaces/interfaces.h"
#include "core/profile.h"
#include "filesystem/ifilesystem.h"
#include "shadercache/shadercache.h"
#include "stdlib/compression/zlib.h"

// Shader cache magic
#define SSC_MAGIC				( ( 'C'<<16 ) + ( 'S'<<8 ) + 'S' )	// Singularity Shader Cache

// Shader cache version
#define SSC_VERSION				1


/*
==================
CShaderCacheDoc::shaderCache_t::Serialize
==================
*/
void CShaderCacheDoc::shaderCache_t::Serialize( IStreamDataWriter* pStreamWriter )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );

	// Save the entry point name
	uint32		entryPointNameSize = ( uint32 )entryPointName.size();
	pStreamWriter->Write( &entryPointNameSize,				sizeof( uint32 ) );
	pStreamWriter->Write( ( byte* )entryPointName.c_str(),	entryPointNameSize * sizeof( achar ) );

	// Save the byte code
	uint64		bytecodeSize = ( uint64 )bytecode.size();
	pStreamWriter->Write( &bytecodeSize, sizeof( uint64 ) );
	CZLib::Compress( pStreamWriter, bytecode.data(), bytecodeSize );

	// Save the reflection data
	uint64		reflectionDataSize = ( uint64 )reflectionData.size();
	pStreamWriter->Write( &reflectionDataSize, sizeof( uint64 ) );
	CZLib::Compress( pStreamWriter, reflectionData.data(), reflectionDataSize );
}

/*
==================
CShaderCacheDoc::shaderCache_t::Deserialize
==================
*/
void CShaderCacheDoc::shaderCache_t::Deserialize( IStreamDataReader* pStreamReader )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );

	// Load the entry point name
	uint32		entryPointNameSize = 0;
	pStreamReader->Read( &entryPointNameSize, sizeof( uint32 ) );
	entryPointName.resize( entryPointNameSize );
	pStreamReader->Read( entryPointName.data(), entryPointNameSize * sizeof( achar ) );

	// Load the byte code
	uint64		bytecodeSize = 0;
	pStreamReader->Read( &bytecodeSize, sizeof( uint64 ) );
	bytecode.resize( bytecodeSize );
	CZLib::Uncompress( pStreamReader, bytecode.data(), bytecodeSize );

	// Load the reflection data
	uint64		reflectionDataSize = 0;
	pStreamReader->Read( &reflectionDataSize, sizeof( uint64 ) );
	reflectionData.resize( reflectionDataSize );
	CZLib::Uncompress( pStreamReader, reflectionData.data(), reflectionDataSize );
}


/*
==================
CShaderCacheDoc::CShaderCacheDoc
==================
*/
CShaderCacheDoc::CShaderCacheDoc()
	: type( STUDIOAPI_SHADER_TYPE_UNKNOWN )
{
	Clear();
}

/*
==================
CShaderCacheDoc::SaveFile
==================
*/
bool CShaderCacheDoc::SaveFile( const achar* pPath )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );

	// Do nothing if file system isn't valid
	Assert( g_pFileSystem );
	if ( !g_pFileSystem )
	{
		Error( "ShaderCacheDoc: For use CShaderCacheDoc::SaveFile StdLib must be connected by ConnectStdLib" );
		return false;
	}

	// Create shader cache file
	TRefPtr<IStreamDataWriter>	pFile = g_pFileSystem->CreateFileWriter( pPath );
	if ( !pFile )
	{
		Error( "ShaderCacheDoc: Failed to create file '%s'", pPath );
		return false;
	}

	// Write the shader cache header
	uint32	magic	= SSC_MAGIC;
	uint32	version = SSC_VERSION;
	pFile->Write( &magic,		sizeof( uint32 ) );
	pFile->Write( &version,		sizeof( uint32 ) );
	pFile->Write( &type,		sizeof( studioAPIShaderType_t ) );

	// Write the shader caches
	uint64		numCaches = ( uint64 )caches.size();
	pFile->Write( &numCaches, sizeof( uint64 ) );
	for ( uint64 cacheIdx = 0; cacheIdx < numCaches; ++cacheIdx )
	{
		caches[cacheIdx].Serialize( pFile );
	}

	// We are done
	return true;
}

/*
==================
CShaderCacheDoc::LoadFromFile
==================
*/
bool CShaderCacheDoc::LoadFromFile( const achar* pPath )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );

	// Do nothing if file system isn't valid
	Assert( g_pFileSystem );
	if ( !g_pFileSystem )
	{
		return false;
	}

	// Clear shader cache data
	Clear();

	// Try to open file
	TRefPtr<IStreamDataReader>	pFile = g_pFileSystem->CreateFileReader( pPath );
	if ( !pFile )
	{
		return false;
	}

	// Read the shader cache header
	uint32	magic	= 0;
	uint32	version = 0;
	pFile->Read( &magic, sizeof( uint32 ) );
	pFile->Read( &version, sizeof( uint32 ) );
	if ( magic != SSC_MAGIC )
	{
		Error( "ShaderCacheDoc: Invalid magic value in '%s'. Expected 0x%X, received 0x%X", pPath, SSC_MAGIC, magic );
		return false;
	}

	if ( version != SSC_VERSION )
	{
		Error( "ShaderCacheDoc: Invalid version format in '%s'. Expected %i, received %i", pPath, SSC_VERSION, version );
		return false;
	}
	pFile->Read( &type, sizeof( studioAPIShaderType_t ) );

	// Read the shader caches
	uint64		numCaches = 0;
	pFile->Read( &numCaches, sizeof( uint64 ) );
	caches.resize( numCaches );
	for ( uint64 cacheIdx = 0; cacheIdx < numCaches; ++cacheIdx )
	{
		caches[cacheIdx].Deserialize( pFile );
	}

	// We are done
	return true;
}

/*
==================
CShaderCacheDoc::Clear
==================
*/
void CShaderCacheDoc::Clear()
{
	// Clear shader type and shader caches
	type = STUDIOAPI_SHADER_TYPE_UNKNOWN;
	caches.clear();
}