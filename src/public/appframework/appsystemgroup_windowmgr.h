#pragma once
#include "appframework/iwindowmgr.h"
#include "appframework/appsystemgroup.h"

//-----------------------------------------------------------------------------
// The application object for apps that use IWindowMgr
//-----------------------------------------------------------------------------
class CAppSystemGroupWindowMgr : public TDefaultAppSystemGroup<CAppSystemGroup>
{
protected:
	virtual bool		Create() override;
	virtual const char* GetName() const override;
};

#include "appframework/appsystemgroup_windowmgr.inl"
