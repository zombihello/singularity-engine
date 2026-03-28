#include "pch_appframework.h"
#include "tier0/iprofiler.h"
#include "appframework/application.h"
#include "appframework/appsystemgroup.h"

static const char* s_pStageLookup[] = {
	"Unknown",		   // APPSYSTEM_GROUP_STATE_NONE
	"Creation",		   // APPSYSTEM_GROUP_STATE_CREATED
	"Connection",	   // APPSYSTEM_GROUP_STATE_CONNECTED
	"Initialization",  // APPSYSTEM_GROUP_STATE_INITIALIZED
};
static_assert( ARRAYSIZE( s_pStageLookup ) == APPSYSTEM_GROUP_STATE_COUNT, "Array size 's_pStageLookup' must be equal to APPSYSTEM_GROUP_STATE_COUNT" );

/*
==================
CAppSystemGroup::ConnectSystems
==================
*/
void CAppSystemGroup::Startup()
{
	PROFILER_SCOPE_FUNC();
	if ( state == APPSYSTEM_GROUP_STATE_NONE )
	{
		// Let prepare a list of systems
		if ( !Create() )
		{
			ReportFailure( APPSYSTEM_GROUP_STATE_CREATED );
			return;
		}

		state = APPSYSTEM_GROUP_STATE_CREATED;
	}
}

/*
==================
CAppSystemGroup::ConnectSystems
==================
*/
void CAppSystemGroup::ConnectSystems()
{
	PROFILER_SCOPE_FUNC();
	Startup();
	if ( state == APPSYSTEM_GROUP_STATE_CREATED )
	{
		// Let the libraries grab any other interfaces they may need
		for ( uint32 index = 0, count = (uint32)systems.size(); index < count; ++index )
		{
			if ( !systems[index]->Connect( CApplication::GetFactory() ) )
			{
				ReportFailure( APPSYSTEM_GROUP_STATE_CONNECTED, index );
				return;
			}
		}

		state = APPSYSTEM_GROUP_STATE_CONNECTED;
	}
}

/*
==================
CAppSystemGroup::InitSystems
==================
*/
void CAppSystemGroup::InitSystems()
{
	PROFILER_SCOPE_FUNC();
	ConnectSystems();
	if ( state == APPSYSTEM_GROUP_STATE_CONNECTED )
	{
		// Call Init on all App Systems
		for ( uint32 index = 0, count = (uint32)systems.size(); index < count; ++index )
		{
			if ( !systems[index]->Init() )
			{
				for ( int32 systemRewindIndex = index; systemRewindIndex-- > 0; )  // NOTE: Shutdown in reverse order of initialization
				{
					systems[systemRewindIndex]->Shutdown();
				}

				ReportFailure( APPSYSTEM_GROUP_STATE_INITIALIZED, index );
				return;
			}
		}

		state = APPSYSTEM_GROUP_STATE_INITIALIZED;
	}
}

/*
==================
CAppSystemGroup::ShutdownSystems
==================
*/
void CAppSystemGroup::ShutdownSystems()
{
	PROFILER_SCOPE_FUNC();
	if ( state == APPSYSTEM_GROUP_STATE_INITIALIZED )
	{
		// Call Shutdown on all App Systems
		for ( int32 index = (int32)systems.size(); --index >= 0; )	// NOTE: Shutdown in reverse order of initialization
		{
			systems[index]->Shutdown();
		}

		state = APPSYSTEM_GROUP_STATE_CONNECTED;
	}
}

/*
==================
CAppSystemGroup::DisconnectSystems
==================
*/
void CAppSystemGroup::DisconnectSystems()
{
	PROFILER_SCOPE_FUNC();
	ShutdownSystems();
	if ( state == APPSYSTEM_GROUP_STATE_CONNECTED )
	{
		// Systems should disconnect from each other
		for ( int32 index = (int32)systems.size(); --index >= 0; )	// NOTE: Disconnect in reverse order of connection
		{
			systems[index]->Disconnect();
		}
		state = APPSYSTEM_GROUP_STATE_CREATED;
	}
}

/*
==================
CAppSystemGroup::Shutdown
==================
*/
void CAppSystemGroup::Shutdown()
{
	PROFILER_SCOPE_FUNC();
	DisconnectSystems();
	if ( state == APPSYSTEM_GROUP_STATE_CREATED )
	{
		// Call an installed application destroy function
		Destroy();

		// Unload all DLLs loaded in the AppCreate block
		for ( int32 index = (int32)modules.size(); --index >= 0; )	// NOTE: Iterate in reverse order so they are unloaded in opposite order from loading
		{
			if ( modules[index].handle )
			{
				Msg( "AppFramework: Unloaded module '%s'", modules[index].name.c_str() );
				Sys_DLL_UnloadModule( modules[index].handle );
			}
			else if ( modules[index].pFactoryFn )
			{
				Msg( "AppFramework: Unloaded module '%p'", modules[index].pFactoryFn );
			}
		}

		systems.clear();
		systemDict.clear();
		modules.clear();
		state = APPSYSTEM_GROUP_STATE_NONE;
	}
}

/*
==================
CAppSystemGroup::FindSystem
==================
*/
void* CAppSystemGroup::FindSystem( const char* pInterfaceName ) const
{
	PROFILER_SCOPE_FUNC();
	auto it = systemDict.find( pInterfaceName );
	if ( it != systemDict.end() )
	{
		return systems[it->second];
	}

	// If it's not an interface we know about, it could be an older
	// version of an interface, or maybe something implemented by
	// one of the instantiated interfaces...
	for ( uint32 index = 0, count = (uint32)systems.size(); index < count; ++index )
	{
		void* pInterface = systems[index]->QueryInterface( pInterfaceName );
		if ( pInterface )
		{
			return pInterface;
		}
	}
	return NULL;
}

/*
==================
CAppSystemGroup::LoadModule
==================
*/
appModule_t CAppSystemGroup::LoadModule( const char* pDLLName )
{
	// See if we already loaded it..
	PROFILER_SCOPE_FUNC();
	for ( int32 index = (int32)modules.size(); --index >= 0; )
	{
		if ( modules[index].name == pDLLName )
		{
			return index;
		}
	}

	// Otherwise we load module
	dllHandle_t dllHandle = Sys_DLL_LoadModule( pDLLName );
	if ( !dllHandle )
	{
		Warning( "AppFramework: Unable to load module '%s'!", pDLLName );
		return APP_MODULE_INVALID;
	}

	Msg( "AppFramework: Loaded module '%s'", pDLLName );
	int32 index = (int32)modules.size();
	modules.push_back( module_t{ dllHandle, NULL, pDLLName } );
	return index;
}

/*
==================
CAppSystemGroup::LoadModule
==================
*/
appModule_t CAppSystemGroup::LoadModule( createInterfaceFn_t pFactory )
{
	PROFILER_SCOPE_FUNC();
	if ( !pFactory )
	{
		Warning( "AppFramework: Unable to load module because pFactory is NULL!", pFactory );
		return APP_MODULE_INVALID;
	}

	// See if we already loaded it..
	for ( int32 index = (int32)modules.size(); --index >= 0; )
	{
		if ( modules[index].pFactoryFn == pFactory )
		{
			return index;
		}
	}

	Msg( "AppFramework: Loaded module '%p'", pFactory );
	int32 index = (int32)modules.size();
	modules.push_back( module_t{ NULL, pFactory, "" } );
	return index;
}

/*
==================
CAppSystemGroup::AddSystem
==================
*/
IAppSystem* CAppSystemGroup::AddSystem( appModule_t module, const char* pInterfaceName )
{
	PROFILER_SCOPE_FUNC();
	if ( module == APP_MODULE_INVALID )
	{
		return NULL;
	}

	if ( systemDict.find( pInterfaceName ) != systemDict.end() )
	{
		Warning( "AppFramework: Attempted to add two systems with the same interface name '%s'!", pInterfaceName );
		return systems[systemDict[pInterfaceName]];
	}

	Assert( module >= 0 && module < modules.size() );
	createInterfaceFn_t pFactoryFn = modules[module].handle ? Sys_GetFactory( modules[module].handle ) : modules[module].pFactoryFn;
	if ( !pFactoryFn )
	{
		Warning( "AppFramework: Failed to get interface factory from '%s'", modules[module].name.c_str() );
		return NULL;
	}

	void* pSystem = pFactoryFn( pInterfaceName );
	if ( !pSystem )
	{
		Warning( "AppFramework: Unable to create system '%s'!", pInterfaceName );
		return NULL;
	}

	IAppSystem* pAppSystem = (IAppSystem*)pSystem;
	int32		sysIndex   = (int32)systems.size();
	systems.push_back( pAppSystem );

	// Inserting into the dict will help us do named lookup later
	systemDict[pInterfaceName] = sysIndex;
	return pAppSystem;
}

/*
==================
CAppSystemGroup::AddSystem
==================
*/
void CAppSystemGroup::AddSystem( IAppSystem* pAppSystem, const char* pInterfaceName )
{
	PROFILER_SCOPE_FUNC();
	if ( !pAppSystem )
	{
		return;
	}

	if ( systemDict.find( pInterfaceName ) != systemDict.end() )
	{
		Warning( "AppFramework: Attempted to add two systems with the same interface name '%s'!", pInterfaceName );
		return;
	}

	int32 sysIndex = (int32)systems.size();
	systems.push_back( pAppSystem );

	// Inserting into the dict will help us do named lookup later
	systemDict[pInterfaceName] = sysIndex;
}

/*
==================
CAppSystemGroup::AddSystems
==================
*/
bool CAppSystemGroup::AddSystems( appSystemInfo_t* pAppSystems )
{
	PROFILER_SCOPE_FUNC();
	while ( pAppSystems->pModuleName[0] )
	{
		appModule_t module	= LoadModule( pAppSystems->pModuleName );
		IAppSystem* pSystem = AddSystem( module, pAppSystems->pInterfaceName );
		if ( !pSystem )
		{
			Warning( "AppFramework: Unable to load interface '%s' from '%s'", pAppSystems->pInterfaceName, pAppSystems->pModuleName );
			return false;
		}

		++pAppSystems;
	}
	return true;
}

/*
==================
CAppSystemGroup::FindSystemName
==================
*/
const char* CAppSystemGroup::FindSystemName( int32 sysIndex ) const
{
	PROFILER_SCOPE_FUNC();
	for ( auto it = systemDict.begin(), itEnd = systemDict.end(); it != itEnd; ++it )
	{
		if ( it->second == sysIndex )
		{
			return it->first.c_str();
		}
	}
	return "";
}

/*
==================
CAppSystemGroup::ReportFailure
==================
*/
void CAppSystemGroup::ReportFailure( int32 errorStage, int32 sysIndex /* = -1 */ ) const
{
	const char* pStageName = "Unknown";
	const char* pGroupName = GetName();
	if ( errorStage >= 0 && errorStage < (int32)ARRAYSIZE( s_pStageLookup ) )
	{
		pStageName = s_pStageLookup[errorStage];
	}

	// Failure happened on some a system
	if ( sysIndex != -1 )
	{
		const char* pSystemName = FindSystemName( sysIndex );
		if ( !pSystemName || !pSystemName[0] )
		{
			pSystemName = "(Unknown)";
		}

		Sys_Error( "AppFramework: System '%s' in group '%s' failed during stage '%s'", pSystemName, pGroupName, pStageName );
	}
	// Failure happened in the group
	else
	{
		Sys_Error( "AppFramework: System group '%s' failed during stage '%s'", pGroupName, pStageName );
	}
}
