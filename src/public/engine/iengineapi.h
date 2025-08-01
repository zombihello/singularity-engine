#pragma once
#include "appframework/iappsystem.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CAppSystemGroup;


//-----------------------------------------------------------------------------
// Main engine interface
//-----------------------------------------------------------------------------
struct startupInfo_t
{
	void*				pAppInstance;
	const achar*		pGame;						// Root game name ("sandbox", for example)
	CAppSystemGroup*	pParentAppSystemGroup;
};


#define ENGINEAPI_INTERFACE_VERSION			"SEngineAPI001"
class IEngineAPI : public IAppSystem
{
public:
	// This function must be called before Init()
	virtual void SetStartupInfo( const startupInfo_t& info ) = 0;
	virtual int32 Run() = 0;
};