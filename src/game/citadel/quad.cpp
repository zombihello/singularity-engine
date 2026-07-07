#include "pch_citadel.h"
#include "game/citadel/quad.h"

/*
==================
CQuad::Init
==================
*/
void CQuad::Init( IResource* pModel, IResource* pMaterial )
{
	CQuad::pModel	 = pModel;
	CQuad::pMaterial = pMaterial;
}

/*
==================
CQuad::Shutdown
==================
*/
void CQuad::Shutdown()
{
	pModel	  = NULL;
	pMaterial = NULL;
}

/*
==================
Quad
==================
*/
CQuad& Quad()
{
	static CQuad s_Quad;
	return s_Quad;
}
