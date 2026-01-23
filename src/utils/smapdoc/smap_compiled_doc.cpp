#include "utils/interfaces/interfaces.h"
#include "tier0/profile.h"
#include "filesystem/ifilesystem.h"
#include "tier1/compression.h"
#include "utils/smapdoc/smap_compiled_doc.h"

// Singularity map magic
static const uint32 s_SMAPMagicSize				 = 5;
static const char	s_SMAPMagic[s_SMAPMagicSize] = { 'S', 'M', 'A', 'P', 'C' };	 // Singularity Map Compiled
static_assert( sizeof( s_SMAPMagic ) == s_SMAPMagicSize, "Size of s_SMAPMagic must be equal to s_SMAPMagicSize" );

// Singularity map version
static const uint32 s_SMAPVersion = 1;

/*
==================
CSMAPCompiledMapDoc::SaveFile
==================
*/
bool CSMAPCompiledMapDoc::SaveFile( const char* pPath )
{
	// Do nothing if the file system isn't valid
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );
	Assert( g_pFileSystem );

	// Try to open a file
	TRefPtr<IStreamDataWriter> pFile = g_pFileSystem->CreateFileWriter( pPath );
	if ( !pFile )
	{
		Error( "SMAPDoc: Failed to open file '%s'", pPath );
		return false;
	}

	// Write format magic and version
	pFile->Write( (void*)s_SMAPMagic, s_SMAPMagicSize );
	pFile->Write( (void*)&s_SMAPVersion, sizeof( uint32 ) );

	// Write entities
	uint32 numEntities = (uint32)entities.size();
	pFile->Write( &numEntities, sizeof( uint32 ) );
	for ( uint32 entityIdx = 0; entityIdx < numEntities; ++entityIdx )
	{
		const CSMAPEntity& entity = entities[entityIdx];

		// Write an entity descriptor
		uint32 entityDescSize = S_Strlen( entity.GetEntityDesc() );
		pFile->Write( &entityDescSize, sizeof( uint32 ) );
		pFile->Write( (void*)entity.GetEntityDesc(), entityDescSize );

		// Write a name
		uint32 nameSize = S_Strlen( entity.GetName() );
		pFile->Write( &nameSize, sizeof( uint32 ) );
		pFile->Write( (void*)entity.GetName(), nameSize );
	}

	// We are done
	return true;
}

/*
==================
CSMAPCompiledMapDoc::LoadFromFile
==================
*/
bool CSMAPCompiledMapDoc::LoadFromFile( const char* pPath )
{
	// Do nothing if the file system isn't valid
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );
	Assert( g_pFileSystem );

	// Try to open a file
	TRefPtr<IStreamDataReader> pFile = g_pFileSystem->CreateFileReader( pPath );
	if ( !pFile )
	{
		Error( "SMAPDoc: Failed to open file '%s'", pPath );
		return false;
	}

	// Read format magic and version
	char   magic[s_SMAPMagicSize];
	uint32 version = 0;
	pFile->Read( magic, s_SMAPMagicSize );
	pFile->Read( &version, sizeof( uint32 ) );
	if ( S_Strncmp( magic, s_SMAPMagic, s_SMAPMagicSize ) )
	{
		Error( "SMAPDoc: Invalid magic value in '%s'. Expected '%.*s', received '%.*s'", pPath, s_SMAPMagicSize, s_SMAPMagic, s_SMAPMagicSize, magic );
		return false;
	}

	if ( version != s_SMAPVersion )
	{
		Error( "SMAPDoc: Invalid version format in '%s'. Expected %i, received %i", pPath, s_SMAPVersion, version );
		return false;
	}
	Clear();

	// Read entities
	uint32 numEntities = 0;
	pFile->Read( &numEntities, sizeof( uint32 ) );
	entities.resize( numEntities );
	for ( uint32 entityIdx = 0; entityIdx < numEntities; ++entityIdx )
	{
		CSMAPEntity& entity = entities[entityIdx];

		// Read an entity descriptor
		eastl::string entityDesc;
		uint32		  entityDescSize = 0;
		pFile->Read( &entityDescSize, sizeof( uint32 ) );
		entityDesc.resize( entityDescSize );
		pFile->Read( entityDesc.data(), entityDescSize );
		entity.SetEntityDesc( entityDesc.c_str() );

		// Read a name
		eastl::string name;
		uint32		  nameSize = S_Strlen( entity.GetName() );
		pFile->Read( &nameSize, sizeof( uint32 ) );
		name.resize( nameSize );
		pFile->Read( name.data(), nameSize );
		entity.SetName( name.c_str() );
	}

	// We are done
	return true;
}
