#pragma once
#include "resourcesystem/resourceptr.h"
#include "materialsystem/imaterial.h"
#include "modelsystem/imodel.h"

//-----------------------------------------------------------------------------
// Quad mesh
//-----------------------------------------------------------------------------
class CQuad
{
public:
	void Init( IResource* pModel, IResource* pMaterial );
	void Shutdown();

	FORCEINLINE IResource* GetModel() const { return pModel; }
	FORCEINLINE IResource* GetMaterial() const { return pMaterial; }

private:
	CResourcePtr<IModel>	pModel;
	CResourcePtr<IMaterial> pMaterial;
};

CQuad& Quad();
