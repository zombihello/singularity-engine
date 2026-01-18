#pragma once
#include <string>
#include <vector>
#include <algorithm>

#include "stdlib/defines.h"
#include "stdlib/types.h"
#include "stdlib/filetools.h"

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

	void			   SetPathID( const char* pNewPathID );
	void			   SetPath( const char* pNewPath );
	const std::string& GetPathID() const;
	const std::string& GetPath() const;

private:
	std::string pathID;
	std::string path;
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

	uint32					  currentIndex;
	std::vector<CSearchPath*> searchPaths;
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
		std::reverse( std::begin( searchPaths ), std::end( searchPaths ) );
	}
};

#include "filesystem/searchpath.inl"