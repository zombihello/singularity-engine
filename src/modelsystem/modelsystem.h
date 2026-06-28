#pragma once
#include "modelsystem/imodelsystem.h"
#include "modelsystem/vertexdeclarations.h"

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

	// IModelSystem interface
	virtual IStudioAPIVertexDeclaration* GetStudioAPIVertexDeclaration( modelVertexType_t vertexType ) const override;

private:
	CVertexDeclarations vertexDeclarations;
};

extern CModelSystem g_modelSystem;
