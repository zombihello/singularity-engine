#pragma once
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EASTL/unordered_map.h>

#include "tier0/tier0.h"
#include "appframework/iappsystem.h"

//-----------------------------------------------------------------------------
// Handle to a DLL
//-----------------------------------------------------------------------------
typedef int32 appModule_t;
enum
{
	APP_MODULE_INVALID = (appModule_t)~0
};

//-----------------------------------------------------------------------------
// Application system group state
//-----------------------------------------------------------------------------
enum appSystemGroupState_t
{
	APPSYSTEM_GROUP_STATE_NONE,
	APPSYSTEM_GROUP_STATE_CREATED,
	APPSYSTEM_GROUP_STATE_CONNECTED,
	APPSYSTEM_GROUP_STATE_INITIALIZED,
	APPSYSTEM_GROUP_STATE_POSTINITIALIZED,
	APPSYSTEM_GROUP_STATE_COUNT
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
// This class represents a group of app systems that all have the same lifetime
// that need to be connected/initialized, etc. in a well-defined order
//-----------------------------------------------------------------------------
class CAppSystemGroup
{
public:
	CAppSystemGroup();
	virtual ~CAppSystemGroup();

	void Startup();
	void ConnectSystems();
	void InitSystems();
	void ShutdownSystems();
	void DisconnectSystems();
	void Shutdown();

	void*				  FindSystem( const char* pInterfaceName ) const;
	appSystemGroupState_t GetState() const;

protected:
	virtual bool		Create()		= 0;
	virtual void		Destroy()		= 0;
	virtual const char* GetName() const = 0;

	// Methods to load modules
	appModule_t LoadModule( const char* pDLLName );
	appModule_t LoadModule( createInterfaceFn_t pFactory );

	// Method to add various global singleton systems
	IAppSystem* AddSystem( appModule_t module, const char* pInterfaceName );
	void		AddSystem( IAppSystem* pAppSystem, const char* pInterfaceName );
	bool		AddSystems( appSystemInfo_t* pAppSystems );	 // Make sure the last appSystemInfo_t has a NULL module name

private:
	struct module_t
	{
		dllHandle_t			handle;
		createInterfaceFn_t pFactoryFn;
		eastl::string		name;
	};

	const char* FindSystemName( int32 sysIndex ) const;
	void		ReportFailure( int32 errorStage, int32 sysIndex = -1 ) const;

	eastl::vector<module_t>						modules;
	eastl::vector<IAppSystem*>					systems;
	eastl::unordered_map<eastl::string, uint32> systemDict;
	appSystemGroupState_t						state;
};

//-----------------------------------------------------------------------------
// Helper empty decorator implementation of an IAppSystemGroup
//-----------------------------------------------------------------------------
template<class TBaseClass>
class CDefaultAppSystemGroup : public TBaseClass
{
	typedef TBaseClass BaseClass;

protected:
	virtual bool Create() override;
	virtual void Destroy() override;
};

#include "appframework/appsystemgroup.inl"
