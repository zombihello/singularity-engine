#ifndef GAME_INL
#define GAME_INL

/*
==================
CGame::GetEcsWorld
==================
*/
FORCEINLINE CEcsWorld& CGame::GetEcsWorld()
{
	return ecsWorld;
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
