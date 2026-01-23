#pragma once
#include <EASTL/string.h>
#include <EASTL/vector.h>

#include "tier0/defines.h"
#include "tier0/types.h"
#include "tier1/jsondoc.h"

//-----------------------------------------------------------------------------
// Helper for work with gameinfo.txt files
//-----------------------------------------------------------------------------
struct gameInfoSearchPath_t
{
	eastl::string id;
	eastl::string path;
};

class CGameInfoDoc
{
public:
	CGameInfoDoc();

	// Methods for load from file/buffer and clear the document
	// NOTE: For LoadFromFile must be connected Tier1
	bool LoadFromFile( const char* pPath );
	bool LoadFromBuffer( const char* pBuffer, const char* pGameInfoPath );
	void Clear();

	const eastl::string&					   GetGame() const;
	const eastl::string&					   GetVersion() const;
	const eastl::string&					   GetSupportEmail() const;
	const eastl::string&					   GetSupportURL() const;
	const eastl::vector<gameInfoSearchPath_t>& GetSearchPaths() const;
	bool									   IsLoaded() const;

private:
	bool GrabData( const CJsonDoc& jsonDoc, const char* pGameInfoPath );
	void ReplaceMacros( eastl::string& string, const char* pGameInfoPath );

	bool								bLoaded;
	eastl::string						game;
	eastl::string						version;
	eastl::string						supportEmail;
	eastl::string						supportURL;
	eastl::vector<gameInfoSearchPath_t> searchPaths;
};

#include "utils/gameinfo/gameinfo.inl"
