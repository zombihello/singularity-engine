#pragma once
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EASTL/algorithm.h>

#include "tier0/defines.h"
#include "tier0/types.h"
#include "tier1/filetools.h"

//-----------------------------------------------------------------------------
// Search path
//-----------------------------------------------------------------------------
class CSearchPath
{
public:
	CSearchPath() {}
	CSearchPath( const char* pNewPathID, const char* pNewPath )
		: pathID( pNewPathID )
		, path( pNewPath )
	{
		PROFILE_SCOPE();
		if ( !S_IsAbsolutePath( pNewPath ) )
		{
			S_MakeAbsolutePath( pNewPath, path );
		}
		S_RemoveDotPathSeparators( path );
	}

	void				 SetPathID( const char* pNewPathID );
	void				 SetPath( const char* pNewPath );
	const eastl::string& GetPathID() const;
	const eastl::string& GetPath() const;

private:
	eastl::string pathID;
	eastl::string path;
};

//-----------------------------------------------------------------------------
// Class for iterate over all search paths by path ID
// If pPathID isn't valid (pPathID is NULL or pPathID is empty, or lengthPathID is zero)
// iterate over all search paths in the file system
//-----------------------------------------------------------------------------
class CSearchPathIterator
{
public:
	CSearchPathIterator( const char* pFilePath, bool bForWrite, const char* pPathID = NULL, uint32 lengthPathID = 0 );

	void		 operator++();
	void		 operator+=( uint32 offset );
	CSearchPath* operator*() const;
	CSearchPath* operator->() const;
				 operator bool() const;
	bool		 operator!() const;

protected:
	CSearchPath* GetSearchPath() const;

	uint32						currentIndex;
	eastl::vector<CSearchPath*> searchPaths;
};

//-----------------------------------------------------------------------------
// Class for reverse iterate over all search paths by path ID
// If pPathID isn't valid (pPathID is NULL or pPathID is empty, or lengthPathID is zero)
// iterate over all search paths in the file system
//-----------------------------------------------------------------------------
class CSearchPathReverseIterator : public CSearchPathIterator
{
public:
	CSearchPathReverseIterator( const char* pFilePath, bool bForWrite, const char* pPathID = NULL, uint32 lengthPathID = 0 )
		: CSearchPathIterator( pFilePath, bForWrite, pPathID, lengthPathID )
	{
		eastl::reverse( eastl::begin( searchPaths ), eastl::end( searchPaths ) );
	}
};

#include "filesystem/searchpath.inl"
