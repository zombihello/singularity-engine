#pragma once
#include "resourcesystem/resourceptr.h"
#include "modelsystem/imodel.h"

//-----------------------------------------------------------------------------
// Quad mesh
//-----------------------------------------------------------------------------
class CQuad
{
public:
	void Init( IResource* pModel );
	void Shutdown();

	FORCEINLINE IResource* GetModel() const { return pModel; }

private:
	CResourcePtr<IModel> pModel;
};

CQuad& Quad();
