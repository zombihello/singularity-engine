#ifndef ENGINE_INL
#define ENGINE_INL

/*
==================
CEngine::IsInited
==================
*/
FORCEINLINE bool CEngine::IsInited() const
{
	return bInited;
}

/*
==================
CEngine::LoadGameInfo
==================
*/
FORCEINLINE bool CEngine::LoadGameInfo( const achar* pPath )
{
	return gameinfo.LoadFromFile( pPath );
}

/*
==================
CEngine::UnloadGameInfo
==================
*/
FORCEINLINE void CEngine::UnloadGameInfo()
{
	gameinfo.Clear();
}

/*
==================
CEngine::GetGameInfo
==================
*/
FORCEINLINE const CGameInfoDoc& CEngine::GetGameInfo() const
{
	return gameinfo;
}

#endif // !ENGINE_INL
