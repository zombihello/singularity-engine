#pragma once

/*
==================
CGame::HasActiveMap
==================
*/
FORCEINLINE bool CGame::HasActiveMap() const
{
	return !!pActiveEcsMap;
}

/*
==================
CGame::GetActiveMap
==================
*/
FORCEINLINE IMap* CGame::GetActiveMap() const
{
	return pActiveEcsMap;
}

/*
==================
CGame::GetActiveEcsMap
==================
*/
FORCEINLINE CEcsMap* CGame::GetActiveEcsMap() const
{
	return (CEcsMap*)GetActiveMap();
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
FORCEINLINE void CGameAppSystems::AddAppSystem( const char* pModuleName, const char* pInterfaceName, gameAppSystemOrder_t order )
{
	gameAppSystemInfo_t& appSystemInfo = appSystems.emplace_back();
	appSystemInfo.order				   = order;
	appSystemInfo.pModuleName		   = pModuleName;
	appSystemInfo.pInterfaceName	   = pInterfaceName;
}
