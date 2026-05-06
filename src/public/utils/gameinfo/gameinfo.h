#pragma once
#include <EASTL/string.h>
#include <EASTL/vector.h>

#include "tier0/defines.h"
#include "tier0/types.h"
#include "tier0/iprofiler.h"
#include "tier1/filetools.h"
#include "tier1/keyvalues.h"

//-----------------------------------------------------------------------------
// Helper for work with gameinfo.txt files
//-----------------------------------------------------------------------------
struct gameInfoCrashDump_t
{
	void Clear();

	eastl::string supportEmail;
	eastl::string supportURL;
};

struct gameInfoSearchPath_t
{
	eastl::string id;
	eastl::string path;
};

struct gameInfoFileSystem_t
{
	void Clear();

	eastl::vector<gameInfoSearchPath_t> searchPaths;
};

class CGameInfoDoc
{
public:
	CGameInfoDoc();

	// Methods for load from file/buffer and clear the document
	// NOTE: For LoadFromFile must be connected Tier1
	bool LoadFromFile( const char* pPath );
	bool LoadFromBuffer( const char* pBuffer );
	bool LoadFromStream( IStreamDataReader* pStreamReader );
	void Clear();

	const eastl::string&		GetGame() const;
	const eastl::string&		GetVersion() const;
	const gameInfoCrashDump_t&	GetCrashDump() const;
	const gameInfoFileSystem_t& GetFileSystem() const;
	bool						IsLoaded() const;

private:
	bool				 bLoaded;
	eastl::string		 game;
	eastl::string		 version;
	gameInfoCrashDump_t	 crashDump;
	gameInfoFileSystem_t fileSystem;
};

#include "utils/gameinfo/gameinfo.inl"
