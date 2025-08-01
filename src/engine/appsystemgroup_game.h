#pragma once
#include "appframework/iappsystemgroup.h"

//-----------------------------------------------------------------------------
// Inner loop: initialize, shutdown game systems
//-----------------------------------------------------------------------------
class CGameAppSystemGroup : public CAppSystemGroup
{
public:
	CGameAppSystemGroup( CAppSystemGroup* pParentAppSystem = NULL );

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

	// Allow the application to do some work before all AppSystems are shut down
	virtual void PreShutdown() override;

	// Allow the application to do some work after all AppSystems are shut down
	virtual void PostShutdown() override;

	// Call an installed application destroy function, occurring after all modules are unloaded
	virtual void Destroy() override;

	FORCEINLINE createInterfaceFn_t GetFactory() const
	{
		return CAppSystemGroup::GetFactory();
	}

private:
	bool GameDLL_Load();
	void GameDLL_Unload();

	dllHandle_t				gameDLLHandle;
	createInterfaceFn_t		pGameFactory;
};