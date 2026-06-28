#pragma once
#include "inputsystem/iinputsystem.h"
#include "studiorender/studioapi/istudioapi.h"
#include "studiorender/istudiorender.h"
#include "resourcesystem/iresourcesystem.h"
#include "materialsystem/imaterialsystem.h"
#include "modelsystem/imodelsystem.h"
#include "appframework/appsystemgroup.h"

//-----------------------------------------------------------------------------
// The application object for apps that use engine systems
//-----------------------------------------------------------------------------
class CAppSystemGroupEngine : public CDefaultAppSystemGroup<CAppSystemGroup>
{
protected:
	virtual bool		Create() override;
	virtual const char* GetName() const override;
};

#include "launcher/appsystemgroup_engine.inl"
