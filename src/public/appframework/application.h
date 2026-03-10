#pragma once
#include "tier0/tier0.h"
#include "tier0/icrashdump_handler.h"
#include "tier1/tier1_logoutputs.h"
#include "appframework/appsystemgroup_tier1.h"
#include "appframework/appsystemgroup.h"
#if ENABLE_LOGGING
	#include "tier0/ilogger.h"
#endif	// ENABLE_LOGGING

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IConVarsOverrider;
class ICvarAccessor;

//-----------------------------------------------------------------------------
// Constants and types
//-----------------------------------------------------------------------------
enum applicationType_t
{
	APPLICATION_TYPE_CONSOLE,
	APPLICATION_TYPE_WINDOW
};

struct appInfo_t
{
	const char*		   pAppName;
	applicationType_t  appType;
	uint32			   baseConVarFlags;
	IConVarsOverrider* pConVarsOverrider;
	ICvarAccessor*	   pCvarAccessor;
};

//-----------------------------------------------------------------------------
// Application
//-----------------------------------------------------------------------------
class CApplication
{
public:
	CApplication( const char* pCommandLine, appInstanceHandle_t hInstance = NULL );

	// Executes the full application lifecycle:
	// calls Init(), then Main(), and finally Shutdown().
	// Returns the exit code from Main(); crash the application if initialization fails
	int32				Run();
	appInstanceHandle_t GetAppInstance() const;

	// Returns the class factory function for the application
	static createInterfaceFn_t GetFactory();

protected:
	virtual void  Init();
	virtual int32 Main() = 0;
	virtual void  Shutdown();

	void  AddGroup( CAppSystemGroup* pSystemGroup );
	void  RemoveGroup( CAppSystemGroup* pSystemGroup );
	void* FindSystem( const char* pInterfaceName ) const;

	// Methods to get some information about the application
	bool							  IsConsoleApp() const;
	bool							  IsWindowApp() const;
	virtual const appInfo_t&		  GetAppInfo() const	   = 0;
	virtual const crashDumpAppInfo_t& GetCrashDumpInfo() const = 0;

#if ENABLE_LOGGING
	static CLogOutputStdOut& GetLogOutputStdOut();
	static CLogOutputFile&	 GetLogOutputFile();
#endif	// ENABLE_LOGGING

private:
	friend void* AppCreateInterfaceFn( const char* pName );

	const char*						pCommandLine;
	appInstanceHandle_t				appInstance;
	CAppSystemGroupTier1			tier1SystemGroup;
	eastl::vector<CAppSystemGroup*> systemGroups;
};

#include "appframework/application.inl"
