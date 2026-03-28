#pragma once
#include "studiorender/studioapi/istudioapi.h"
#include "resourcesystem/resourceptr.h"
#include "materialsystem/imaterial.h"

//-----------------------------------------------------------------------------
// Quad mesh
//-----------------------------------------------------------------------------
class CQuad
{
public:
	void Init( IStudioAPIBuffer* pStudioAPIVertexBuffer, IStudioAPIBuffer* pStudioAPIIndexBuffer, IResource* pMaterial );
	void Shutdown();

	FORCEINLINE IStudioAPIBuffer* GetStudioAPIVertexBuffer() const { return pStudioAPIVertexBuffer; }
	FORCEINLINE IStudioAPIBuffer* GetStudioAPIIndexBuffer() const { return pStudioAPIIndexBuffer; }
	FORCEINLINE IResource*		  GetMaterial() const { return pMaterial; }

private:
	CRefPtr<IStudioAPIBuffer> pStudioAPIVertexBuffer;
	CRefPtr<IStudioAPIBuffer> pStudioAPIIndexBuffer;
	CResourcePtr<IMaterial>	  pMaterial;
};

CQuad& Quad();
