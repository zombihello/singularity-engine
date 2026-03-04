#pragma once
#include "cvar/icvar.h"
#include "filesystem/ifilesystem.h"
#include "appframework/appsystemgroup.h"

//-----------------------------------------------------------------------------
// The application object for apps that use tier1
//-----------------------------------------------------------------------------
class CAppSystemGroupTier1 : public TDefaultAppSystemGroup<CAppSystemGroup>
{
protected:
	virtual bool		Create() override;
	virtual const char* GetName() const override;
};

#include "appframework/appsystemgroup_tier1.inl"
