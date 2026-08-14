#pragma once

/*
==================
CBaseGame::HasActiveMap
==================
*/
FORCEINLINE bool CBaseGame::HasActiveMap() const
{
	return !!pActiveEcsMap;
}

/*
==================
CBaseGame::GetActiveMap
==================
*/
FORCEINLINE IMap* CBaseGame::GetActiveMap() const
{
	return pActiveEcsMap;
}

/*
==================
CBaseGame::GetActiveEcsMap
==================
*/
FORCEINLINE CEcsMap* CBaseGame::GetActiveEcsMap() const
{
	return (CEcsMap*)GetActiveMap();
}

/*
==================
CBaseGame::GetEcsComponentTypes
==================
*/
FORCEINLINE CEcsComponentTypes& CBaseGame::GetEcsComponentTypes()
{
	return ecsComponentTypes;
}

/*
==================
CBaseGameAppSystems::AddAppSystem
==================
*/
FORCEINLINE void CBaseGameAppSystems::AddAppSystem( const char* pModuleName, const char* pInterfaceName, gameAppSystemOrder_t order )
{
	gameAppSystemInfo_t& appSystemInfo = appSystems.emplace_back();
	appSystemInfo.order				   = order;
	appSystemInfo.pModuleName		   = pModuleName;
	appSystemInfo.pInterfaceName	   = pInterfaceName;
}
