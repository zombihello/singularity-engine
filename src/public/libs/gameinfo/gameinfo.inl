#ifndef GAMEINFO_INL
#define GAMEINFO_INL

/*
==================
CGameInfoDoc::GetGame
==================
*/
FORCEINLINE const std::string& CGameInfoDoc::GetGame() const
{
	return game;
}

/*
==================
CGameInfoDoc::GetVersion
==================
*/
FORCEINLINE const std::string& CGameInfoDoc::GetVersion() const
{
	return version;
}

/*
==================
CGameInfoDoc::GetSupportEmail
==================
*/
FORCEINLINE const std::string& CGameInfoDoc::GetSupportEmail() const
{
	return supportEmail;
}

/*
==================
CGameInfoDoc::GetSupportURL
==================
*/
FORCEINLINE const std::string& CGameInfoDoc::GetSupportURL() const
{
	return supportURL;
}

/*
==================
CGameInfoDoc::GetSearchPaths
==================
*/
FORCEINLINE const std::vector<gameInfoSearchPath_t>& CGameInfoDoc::GetSearchPaths() const
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

#endif // GAMEINFO_INL