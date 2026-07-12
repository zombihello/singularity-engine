#pragma once
#include "modelsystem/imodelsystem.h"
#include "modelsystem/model.h"
#include "modelsystem/model_loader.h"

//-----------------------------------------------------------------------------
// Model system
//-----------------------------------------------------------------------------
class CModelSystem : public CBaseAppSystem<IModelSystem>
{
public:
	// IAppSystem interface
	virtual bool Connect( createInterfaceFn_t pFactory ) override;
	virtual void Disconnect() override;

	// Initialize and shutdown
	virtual bool Init() override;
	virtual void Shutdown() override;

private:
	CResourceTypeFactory<CModel> modelFactory;
	CModelLoader				 modelLoader;
};

extern CModelSystem g_modelSystem;
