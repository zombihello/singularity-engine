#pragma once

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
FORCEINLINE void CGameAppSystems::AddAppSystem( const achar* pModuleName, const achar* pInterfaceName, gameAppSystemOrder_t order )
{
	gameAppSystemInfo_t&			appSystemInfo = appSystems.emplace_back();
	appSystemInfo.order				= order;
	appSystemInfo.pModuleName		= pModuleName;
	appSystemInfo.pInterfaceName	= pInterfaceName;
}
