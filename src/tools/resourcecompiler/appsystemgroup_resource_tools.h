#pragma once
#include "tools/resource_tools/itexture_tool.h"
#include "tools/resource_tools/imaterial_tool.h"
#include "tools/resource_tools/imodel_tool.h"
#include "tools/resource_tools/ientitydesc_tool.h"
#include "tools/resource_tools/imap_tool.h"
#include "appframework/appsystemgroup.h"

//-----------------------------------------------------------------------------
// The application object for apps that use resource tools
//-----------------------------------------------------------------------------
class CAppSystemGroupResourceTools : public CDefaultAppSystemGroup<CAppSystemGroup>
{
protected:
	virtual bool		Create() override;
	virtual const char* GetName() const override;
};

#include "tools/resourcecompiler/appsystemgroup_resource_tools.inl"
