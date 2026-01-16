#include "pch_citadel.h"
#include "games/citadel/quad.h"

/*
==================
CQuad::Init
==================
*/
void CQuad::Init( IStudioAPIBuffer* pStudioAPIVertexBuffer, IStudioAPIBuffer* pStudioAPIIndexBuffer, IResource* pMaterial )
{
	CQuad::pStudioAPIVertexBuffer	= pStudioAPIVertexBuffer;
	CQuad::pStudioAPIIndexBuffer	= pStudioAPIIndexBuffer;
	CQuad::pMaterial				= pMaterial;
}

/*
==================
CQuad::Shutdown
==================
*/
void CQuad::Shutdown()
{
	pStudioAPIVertexBuffer	= NULL;
	pStudioAPIIndexBuffer	= NULL;
	pMaterial				= NULL;
}


/*
==================
Quad
==================
*/
CQuad& Quad()
{
	static CQuad	s_Quad;
	return s_Quad;
}