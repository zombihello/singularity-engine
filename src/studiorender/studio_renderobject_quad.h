#pragma once
#include "studiorender/istudio_renderobject.h"
#include "studiorender/studioapi/istudioapi_buffer.h"
#include "resourcesystem/resourceptr.h"
#include "materialsystem/imaterial.h"

//-----------------------------------------------------------------------------
// Quad Studio render object (FOR TEST ONLY!!)
//-----------------------------------------------------------------------------
class CStudioRenderObjectQuad : public CRefCounted<IStudioRenderObject>
{
public:
	CStudioRenderObjectQuad( IStudioAPIBuffer* pStudioAPIVertexBuffer, IStudioAPIBuffer* pStudioAPIIndexBuffer, IResource* pMaterial )
		: pStudioAPIVertexBuffer( pStudioAPIVertexBuffer )
		, pStudioAPIIndexBuffer( pStudioAPIIndexBuffer )
		, pMaterial( pMaterial )
	{
	}

	FORCEINLINE void RefreshMaterialResource()
	{
		pMaterialResource = pMaterial->GetStudioResource();
	}

	FORCEINLINE IStudioAPIBuffer*  GetStudioAPIVertexBuffer() const { return pStudioAPIVertexBuffer; }
	FORCEINLINE IStudioAPIBuffer*  GetStudioAPIIndexBuffer() const { return pStudioAPIIndexBuffer; }
	FORCEINLINE IMaterial*		   GetMaterial() const { return *pMaterial; }
	FORCEINLINE IMaterialResource* GetMaterialResource() const { return pMaterialResource; }

public:
	CRefPtr<IStudioAPIBuffer>  pStudioAPIVertexBuffer;
	CRefPtr<IStudioAPIBuffer>  pStudioAPIIndexBuffer;
	CResourcePtr<IMaterial>	   pMaterial;
	CRefPtr<IMaterialResource> pMaterialResource;
};
