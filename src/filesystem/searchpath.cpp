#include "pch_filesystem.h"
#include "filesystem/filesystem.h"
#include "filesystem/searchpath.h"

// Null search path used to absolute paths in CSearchPathIterator
static CSearchPath s_NullSearchPath( "", "" );

/*
==================
CSearchPathIterator::CSearchPathIterator
==================
*/
CSearchPathIterator::CSearchPathIterator( const char* pFilePath, bool bForWrite, const char* pPathID /* = NULL */, uint32 lengthPathID /* = 0 */ )
	: currentIndex( INVALID_INDEX )
{
	PROFILE_SCOPE();
	CFileSystem* pFileSystem = (CFileSystem*)g_pFileSystem;
	Assert( pFileSystem );

	// For absolute path we use only s_NullSearchPath (except paths with path IDs)
	if ( ( pFilePath[0] != '/' && pFilePath[1] != '/' ) && S_IsAbsolutePath( pFilePath ) )
	{
		searchPaths.emplace_back( &s_NullSearchPath );
	}
	// Otherwise it is a relative path
	else
	{
		// If we have a path ID try to find all search paths for that
		if ( pPathID && pPathID[0] != '\0' && lengthPathID > 0 )
		{
			// We iterate from end to beginning to be able to overload paths
			for ( int32 index = (int32)pFileSystem->searchPaths.size(); --index >= 0; )
			{
				CSearchPath*		 pSearchPath  = &pFileSystem->searchPaths[index];
				const eastl::string& searchPathID = pSearchPath->GetPathID();
				if ( searchPathID.size() == lengthPathID && !S_Strnicmp( searchPathID.c_str(), pPathID, lengthPathID ) )
				{
					searchPaths.emplace_back( pSearchPath );
				}
			}

			// Try to get default search path for write if we didn't found anything
			if ( bForWrite && searchPaths.empty() )
			{
				Warning( "FileSystem: Requested non-existent write path '%s'!", pPathID );
				CSearchPath* pFirstWriteSearchPath	  = NULL;
				const char*	 pDefaultWritePathID	  = "default_write_path";
				const uint32 lenghtDefaultWritePathID = 18;

				// We iterate from end to beginning to be able to overload paths
				for ( int32 index = (int32)pFileSystem->searchPaths.size(); --index >= 0; )
				{
					CSearchPath* pSearchPath = &pFileSystem->searchPaths[index];
					if ( !pFirstWriteSearchPath )
					{
						pFirstWriteSearchPath = pSearchPath;
					}

					const eastl::string& searchPathID = pSearchPath->GetPathID();
					if ( searchPathID.size() == lenghtDefaultWritePathID && !S_Strnicmp( searchPathID.c_str(), pDefaultWritePathID, lenghtDefaultWritePathID ) )
					{
						searchPaths.emplace_back( pSearchPath );
					}
				}

				// Didn't nothing to find? Okay, just add the first write search path
				if ( searchPaths.empty() && pFirstWriteSearchPath )
				{
					searchPaths.emplace_back( pFirstWriteSearchPath );
				}
			}
		}
		// Otherwise grab all search paths
		else
		{
			// We iterate from end to beginning to be able to overload paths
			for ( int32 index = (int32)pFileSystem->searchPaths.size(); --index >= 0; )
			{
				searchPaths.emplace_back( &pFileSystem->searchPaths[index] );
			}
		}
	}

	// Init current index if search paths aren't empty
	if ( !searchPaths.empty() )
	{
		currentIndex = 0;
	}
}
