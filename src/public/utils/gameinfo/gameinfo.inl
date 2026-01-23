#pragma once

/*
==================
CGameInfoDoc::GetGame
==================
*/
FORCEINLINE const eastl::string& CGameInfoDoc::GetGame() const
{
	return game;
}

/*
==================
CGameInfoDoc::GetVersion
==================
*/
FORCEINLINE const eastl::string& CGameInfoDoc::GetVersion() const
{
	return version;
}

/*
==================
CGameInfoDoc::GetSupportEmail
==================
*/
FORCEINLINE const eastl::string& CGameInfoDoc::GetSupportEmail() const
{
	return supportEmail;
}

/*
==================
CGameInfoDoc::GetSupportURL
==================
*/
FORCEINLINE const eastl::string& CGameInfoDoc::GetSupportURL() const
{
	return supportURL;
}

/*
==================
CGameInfoDoc::GetSearchPaths
==================
*/
FORCEINLINE const eastl::vector<gameInfoSearchPath_t>& CGameInfoDoc::GetSearchPaths() const
{
	return searchPaths;
}

/*
==================
CGameInfoDoc::IsLoaded
==================
*/
FORCEINLINE bool CGameInfoDoc::IsLoaded() const
{
	return bLoaded;
}