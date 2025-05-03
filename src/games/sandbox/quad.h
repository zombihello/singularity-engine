#ifndef QUAD_H
#define QUAD_H

#include "studiorender/studioapi/istudioapi.h"
#include "resourcesystem/iresource.h"
#include "materialsystem/imaterial.h"

//-----------------------------------------------------------------------------
// Quad mesh
//-----------------------------------------------------------------------------
class CQuad
{
public:
	void Init( IStudioAPIBuffer* pStudioAPIVertexBuffer, IStudioAPIBuffer* pStudioAPIIndexBuffer, IResource* pMaterial );
	void Shutdown();

	FORCEINLINE IStudioAPIBuffer* GetStudioAPIVertexBuffer() const		{ return pStudioAPIVertexBuffer; }
	FORCEINLINE IStudioAPIBuffer* GetStudioAPIIndexBuffer() const		{ return pStudioAPIIndexBuffer; }
	FORCEINLINE IResource* GetMaterial() const							{ return pMaterial ? pMaterial : NULL; }

private:
	TRefPtr<IStudioAPIBuffer>	pStudioAPIVertexBuffer;
	TRefPtr<IStudioAPIBuffer>	pStudioAPIIndexBuffer;
	TResourcePtr<IMaterial>		pMaterial;
};
CQuad& Quad();

#endif // !QUAD_H
