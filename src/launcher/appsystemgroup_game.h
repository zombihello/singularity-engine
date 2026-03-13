#pragma once
#include "appframework/appsystemgroup.h"

//-----------------------------------------------------------------------------
// The application object for apps that use game systems
//-----------------------------------------------------------------------------
class CAppSystemGroupGame : public CDefaultAppSystemGroup<CAppSystemGroup>
{
public:
	CAppSystemGroupGame();

protected:
	virtual bool		Create() override;
	virtual void		Destroy() override;
	virtual const char* GetName() const override;

private:
	// Load and unload game dll
	bool GameDLL_Load();
	void GameDLL_Unload();

	dllHandle_t			gameDLLHandle;
	createInterfaceFn_t pGameFactory;
};
