#ifndef GAME_INL
#define GAME_INL

/*
==================
CGame::SetActiveMap
==================
*/
FORCEINLINE void CGame::SetActiveMap( const TResourcePtr<IMap>& pEcsMap )
{
	pActiveMap = pEcsMap;
}

/*
==================
CGame::GetActiveMap
==================
*/
FORCEINLINE IMap* CGame::GetActiveMap() const
{
	return *pActiveMap;
}

/*
==================
CGame::GetEcsComponentTypes
==================
*/
FORCEINLINE CEcsComponentTypes& CGame::GetEcsComponentTypes()
{
	return ecsComponentTypes;
}


/*
==================
CGameAppSystems::AddAppSystem
==================
*/
FORCEINLINE void CGameAppSystems::AddAppSystem( const achar* pModuleName, const achar* pInterfaceName )
{
	appSystemInfo_t&				appSystemInfo = appSystems.emplace_back();
	appSystemInfo.pModuleName		= pModuleName;
	appSystemInfo.pInterfaceName	= pInterfaceName;
}

#endif // !GAME_INL
