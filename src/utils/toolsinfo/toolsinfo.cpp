#include "utils/interfaces/interfaces.h"
#include "tier1/template.h"
#include "filesystem/ifilesystem.h"
#include "utils/toolsinfo/toolsinfo.h"

/*
==================
CToolsInfoDoc::CToolsInfoDoc
==================
*/
CToolsInfoDoc::CToolsInfoDoc()
	: bLoaded( false )
{
}

/*
==================
CToolsInfoDoc::LoadFromStream
==================
*/
bool CToolsInfoDoc::LoadFromStream( IStreamDataReader* pStreamReader )
{
	// Clear a game info data
	PROFILER_SCOPE_FUNC();
	Clear();

	// Load key values
	CKeyValues keyValues( "toolsinfo" );
	if ( !keyValues.LoadFromStream( pStreamReader ) )
	{
		return false;
	}

	// Get some information for the file system
	CKeyValues* pFileSystem = keyValues.FindKey( "file_system" );
	if ( pFileSystem )
	{
		// Get search paths
		CKeyValues* pSearchPaths = pFileSystem->FindKey( "search_paths" );
		if ( pSearchPaths )
		{
			for ( CKeyValuesSubKeysIterator it( pSearchPaths ); it; ++it )
			{
				toolsInfoSearchPath_t& searchPath = searchPaths.emplace_back();
				searchPath.id					  = it->GetName();
				searchPath.path					  = it->GetString( NULL );
				S_FillPathPlaceholders( searchPath.path );
			}
		}
	}

	// Get resource compilers
	CKeyValues* pResourceCompilers = keyValues.FindKey( "resource_compilers" );
	if ( pResourceCompilers )
	{
		for ( CKeyValuesSubKeysIterator it( pResourceCompilers ); it; ++it )
		{
			const char* pKeyID = it->GetName();
			if ( !S_Stricmp( pKeyID, "compiler" ) )
			{
				resourceCompilers.emplace_back( it->GetString( NULL ) );
			}
			else
			{
				Warning( "ToolsInfo: Unknown 'resource_compilers/%s' in 'toolsinfo.txt' (path: '%s')", pKeyID, pStreamReader->GetPath() );
			}
		}
	}

	// We are done
	bLoaded = true;
	return bLoaded;
}
