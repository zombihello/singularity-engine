#pragma once
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EASTL/unordered_map.h>

#include "core/core.h"
#include "appframework/iappsystem.h"
#include "stdlib/interface.h"

//-----------------------------------------------------------------------------
// Handle to a DLL
//-----------------------------------------------------------------------------
typedef int32 appModule_t;
enum
{
	APP_MODULE_INVALID = (appModule_t)~0
};

//-----------------------------------------------------------------------------
// Specifies a module + interface name for initialization
//-----------------------------------------------------------------------------
struct appSystemInfo_t
{
	const char* pModuleName;
	const char* pInterfaceName;
};

//-----------------------------------------------------------------------------
// The following methods must be implemented in your application
// although they can be empty implementations if you like
//-----------------------------------------------------------------------------
class IAppSystemGroup
{
public:
	// An installed application creation function, you should tell the group
	// the DLLs and the singleton interfaces you want to instantiate
	// Return FALSE if there's any problems and the app will abort
	virtual bool Create() = 0;

	// Allow the application to do some work after AppSystems are connected but
	// they aren't all Initialized
	// Return FALSE if there's any problems and the app will abort
	virtual bool PreInit() = 0;

	// Allow the application to do some work after AppSystems are initialized but
	// before main is run
	// Return FALSE if there's any problems and the app will abort
	virtual bool PostInit() = 0;

	// Main loop implemented by the application
	// Return exit code. If all ok returns zero
	virtual int32 Main() = 0;

	// Allow the application to do some work before all AppSystems are shut down
	virtual void PreShutdown() = 0;

	// Allow the application to do some work after all AppSystems are shut down
	virtual void PostShutdown() = 0;

	// Call an installed application destroy function, occurring after all modules are unloaded
	virtual void Destroy() = 0;
};

//-----------------------------------------------------------------------------
// This class represents a group of app systems that all have the same lifetime
// that need to be connected/initialized, etc. in a well-defined order
//-----------------------------------------------------------------------------
class CAppSystemGroup : public IAppSystemGroup
{
public:
	// Used to determine where we exited out from the system
	enum appSystemGroupStage_t
	{
		APPSYSTEM_STAGE_CREATION,
		APPSYSTEM_STAGE_CONNECTION,
		APPSYSTEM_STAGE_PREINITIALIZATION,
		APPSYSTEM_STAGE_INITIALIZATION,
		APPSYSTEM_STAGE_POSTINITIALIZATION,
		APPSYSTEM_STAGE_RUNNING,
		APPSYSTEM_STAGE_PRESHUTDOWN,
		APPSYSTEM_STAGE_SHUTDOWN,
		APPSYSTEM_STAGE_POSTSHUTDOWN,
		APPSYSTEM_STAGE_DISCONNECTION,
		APPSYSTEM_STAGE_DESTRUCTION,

		APPSYSTEM_GROUP_STAGE_COUNT,
		APPSYSTEM_STAGE_NONE,  // This means no error
	};

	CAppSystemGroup( CAppSystemGroup* pParentAppSystem = NULL );

	// Runs the app system group
	// First, modules are loaded, next they are connected, followed by initialization
	// Then Main() is run
	// Then modules are shut down, disconnected, and unloaded
	// Return exit code. If all ok returns zero
	int32 Run();

	// Post initialize and pre-shutdown
	virtual bool PostInit() override;
	virtual void PreShutdown() override;

	// Used by CApplication to set up necessary pointer if we can't do it in the constructor
	void Setup( CAppSystemGroup* pParentAppSystem );

	// Returns the stage at which the app system group ran into an error
	appSystemGroupStage_t GetCurrentStage() const;

protected:
	// These methods are meant to be called by derived classes of CAppSystemGroup

	// Methods to load modules
	appModule_t LoadModule( const char* pDLLName );
	appModule_t LoadModule( createInterfaceFn_t pFactory );

	// Method to add various global singleton systems
	IAppSystem* AddSystem( appModule_t module, const char* pInterfaceName );
	void		AddSystem( IAppSystem* pAppSystem, const char* pInterfaceName );

	// Simpler method of doing the LoadModule/AddSystem thing.
	// Make sure the last appSystemInfo_t has a NULL module name
	bool AddSystems( appSystemInfo_t* pAppSystems );

	// Method to look up a particular named system
	void* FindSystem( const char* pInterfaceName ) const;

	// Gets at a class factory for the topmost appsystem group in an appsystem stack
	static createInterfaceFn_t GetFactory();

private:
	friend void* AppSystemCreateInterfaceFn( const char* pName );
	struct module_t
	{
		dllHandle_t			handle;
		createInterfaceFn_t pFactoryFn;
		eastl::string		name;
	};

	int32 Startup();
	void  Shutdown();

	void UnloadAllModules();
	void RemoveAllSystems();

	// Method to connect/disconnect all systems
	// failureSysIndex - Indicates the index of the system that could not be connected
	bool ConnectSystems( uint32& failureSysIndex );
	void DisconnectSystems();

	// Method to initialize/shutdown all systems
	// failureSysIndex - Indicates the index of the system that could not be connected
	bool InitSystems( uint32& failureSysIndex );
	void ShutdownSystems();

	// Gets at the parent appsystem group
	CAppSystemGroup* GetParent() const;

	eastl::string FindSystemName( int32 index );
	void		  ReportFailure( int32 errorStage, int32 sysIndex = -1 );

	eastl::vector<module_t>						modules;
	eastl::vector<IAppSystem*>					systems;
	eastl::unordered_map<eastl::string, uint32> systemDict;
	CAppSystemGroup*							pParentAppSystemGroup;
	appSystemGroupStage_t						currentStage;
};

//-----------------------------------------------------------------------------
// Helper empty decorator implementation of an IAppSystemGroup
//-----------------------------------------------------------------------------
template<class CBaseClass>
class CDefaultAppSystemGroup : public CBaseClass
{
public:
	virtual bool Create();
	virtual bool PreInit();
	virtual bool PostInit();
	virtual void PreShutdown();
	virtual void PostShutdown();
	virtual void Destroy();
};

#include "appframework/iappsystemgroup.inl"
