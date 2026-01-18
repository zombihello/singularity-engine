#pragma once
#include <string>
#include <vector>

#include "stdlib/defines.h"
#include "stdlib/types.h"
#include "stdlib/jsondoc.h"

//-----------------------------------------------------------------------------
// Helper for work with gameinfo.txt files
//-----------------------------------------------------------------------------
struct gameInfoSearchPath_t
{
	std::string id;
	std::string path;
};

class CGameInfoDoc
{
public:
	CGameInfoDoc();

	// Methods for load from file/buffer and clear the document
	// NOTE: For LoadFromFile must be connected StdLib
	bool LoadFromFile( const char* pPath );
	bool LoadFromBuffer( const char* pBuffer, const char* pGameInfoPath );
	void Clear();

	const std::string&						 GetGame() const;
	const std::string&						 GetVersion() const;
	const std::string&						 GetSupportEmail() const;
	const std::string&						 GetSupportURL() const;
	const std::vector<gameInfoSearchPath_t>& GetSearchPaths() const;
	bool									 IsLoaded() const;

private:
	bool GrabData( const CJsonDoc& jsonDoc, const char* pGameInfoPath );
	void ReplaceMacros( std::string& string, const char* pGameInfoPath );

	bool							  bLoaded;
	std::string						  game;
	std::string						  version;
	std::string						  supportEmail;
	std::string						  supportURL;
	std::vector<gameInfoSearchPath_t> searchPaths;
};

#include "gameinfo/gameinfo.inl"