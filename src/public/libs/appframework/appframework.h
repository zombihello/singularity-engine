#pragma once
#include "appframework/iappsystemgroup.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class ILogOutput;


//-----------------------------------------------------------------------------
// This class is a helper class used for applications.
// It loads up the file system in preparation for using it to load other required modules
//-----------------------------------------------------------------------------
class CApplication : public CDefaultAppSystemGroup<CAppSystemGroup>
{
public:
	// NOTE: 
	//	* pLogFileName must be static string
	//	* The class creates a log file if the app was build in debug configuration or started with '-log'
	//	* The class won't create a log file if the pLogFileName is empty/null/disabled logging
	CApplication( CAppSystemGroup* pChildAppSystemGroup, const achar* pLogFileName = "" );

	// IAppSystemGroup interface
	// An installed application creation function, you should tell the group
	// the DLLs and the singleton interfaces you want to instantiate
	// Return FALSE if there's any problems and the app will abort
	virtual bool Create() override;

	// Allow the application to do some work after AppSystems are connected but
	// they aren't all Initialized
	// Return FALSE if there's any problems and the app will abort
	virtual bool PreInit() override;

	// Allow the application to do some work after AppSystems are initialized but
	// before main is run
	// Return FALSE if there's any problems and the app will abort
	virtual bool PostInit() override;

	// Main loop implemented by the application
	// Return exit code. If all ok returns zero
	virtual int32 Main() override;

	// Allow the application to do some work after all AppSystems are shut down
	virtual void PostShutdown() override;

protected:
	CAppSystemGroup*	pChildAppSystemGroup;
#if ENABLE_LOGGING
	const achar*		pLogFileName;
	ILogOutput*			pLogOutputFile;
#endif // ENABLE_LOGGING
};

#include "appframework/appframework.inl"