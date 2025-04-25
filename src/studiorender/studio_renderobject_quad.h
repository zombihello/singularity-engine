#ifndef STUDIO_RENDEROBJECT_QUAD_H
#define STUDIO_RENDEROBJECT_QUAD_H

#include "studiorender/istudio_renderobject.h"
#include "studiorender/studioapi/istudioapi_buffer.h"
#include "materialsystem/imaterial.h"

//-----------------------------------------------------------------------------
// Quad Studio render object (FOR TEST ONLY!!)
//-----------------------------------------------------------------------------
class CStudioRenderObjectQuad : public TRefCounted<IStudioRenderObject>
{
public:
	CStudioRenderObjectQuad( IStudioAPIBuffer* pStudioAPIVertexBuffer, IStudioAPIBuffer* pStudioAPIIndexBuffer, IMaterial* pMaterial )
		: pStudioAPIVertexBuffer( pStudioAPIVertexBuffer )
		, pStudioAPIIndexBuffer( pStudioAPIIndexBuffer )
		, pMaterial( pMaterial )
	{}

	FORCEINLINE IStudioAPIBuffer*	GetStudioAPIVertexBuffer() const	{ return pStudioAPIVertexBuffer; }
	FORCEINLINE IStudioAPIBuffer*	GetStudioAPIIndexBuffer() const		{ return pStudioAPIIndexBuffer; }
	FORCEINLINE IMaterial*			GetMaterial() const					{ return pMaterial; }

public:
	TRefPtr<IStudioAPIBuffer>		pStudioAPIVertexBuffer;
	TRefPtr<IStudioAPIBuffer>		pStudioAPIIndexBuffer;
	TRefPtr<IMaterial>				pMaterial;
};

#endif // !STUDIO_RENDEROBJECT_QUAD_H
