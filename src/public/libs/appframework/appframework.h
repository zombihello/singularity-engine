#pragma once
#include "appframework/iappsystemgroup.h"

//-----------------------------------------------------------------------------
// This class is a helper class used for applications.
// It loads up the file system in preparation for using it to load other required modules
//-----------------------------------------------------------------------------
class CApplication : public CDefaultAppSystemGroup<CAppSystemGroup>
{
public:
	CApplication( CAppSystemGroup* pChildAppSystemGroup );

	// IAppSystemGroup interface
	// An installed application creation function, you should tell the group
	// the DLLs and the singleton interfaces you want to instantiate
	// Return FALSE if there's any problems and the app will abort
	virtual bool Create() override;

	// Allow the application to do some work after AppSystems are connected but
	// they aren't all Initialized
	// Return FALSE if there's any problems and the app will abort
	virtual bool PreInit() override;

	// Main loop implemented by the application
	// Return exit code. If all ok returns zero
	virtual int32 Main() override;

	// Allow the application to do some work after all AppSystems are shut down
	virtual void PostShutdown() override;

protected:
	CAppSystemGroup*	pChildAppSystemGroup;
};

#include "appframework/appframework.inl"