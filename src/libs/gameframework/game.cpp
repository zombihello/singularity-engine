#include "pch_gameframework.h"
#include "gameframework/igame.h"

//-----------------------------------------------------------------------------
// Base game implementation
//-----------------------------------------------------------------------------
/*
==================
CGame::Init
==================
*/
bool CGame::Init( createInterfaceFn_t pFactory )
{
	// Connect StdLib
	if ( !ConnectStdLib( pFactory ) )
	{
		return false;
	}

	// Register cvars in the system
	ConVar_Register( FCVAR_GAMEDLL );
	return true;
}

/*
==================
CGame::Shutdown
==================
*/
void CGame::Shutdown()
{
	// Unregister cvars and disconnect StdLib
	ConVar_Unregister();
	DisconnectStdLib();
}

/*
==================
CGame::FrameUpdate
==================
*/
void CGame::FrameUpdate()
{}


//-----------------------------------------------------------------------------
// Game application systems implementation
//-----------------------------------------------------------------------------
/*
==================
CGameAppSystems::GetNum
==================
*/
uint32 CGameAppSystems::GetNum() const
{
	return ( uint32 )appSystems.size();
}

/*
==================
CGameAppSystems::GetModuleName
==================
*/
const achar* CGameAppSystems::GetModuleName( uint32 index ) const
{
	return appSystems[index].pModuleName;
}

/*
==================
CGameAppSystems::GetInterfaceName
==================
*/
const achar* CGameAppSystems::GetInterfaceName( uint32 index ) const
{
	return appSystems[index].pInterfaceName;
}