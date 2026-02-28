#pragma once
#include "tier1/convar.h"

//-----------------------------------------------------------------------------
// ConVars
//-----------------------------------------------------------------------------
extern CConVar window_displayId;
extern CConVar window_width;
extern CConVar window_height;
extern CConVar window_refreshRate;
extern CConVar window_mode;

//-----------------------------------------------------------------------------
// IConVars overrider
//-----------------------------------------------------------------------------
class CConVarsOverrider : public IConVarsOverrider
{
public:
	virtual void OverrideFromCommandLine() override;
};
extern CConVarsOverrider g_conVarsOverrider;
