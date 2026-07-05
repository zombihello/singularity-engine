#include "pch_citadel.h"
#include "game/citadel/quad.h"

/*
==================
CQuad::Init
==================
*/
void CQuad::Init( IResource* pModel )
{
	CQuad::pModel = pModel;
}

/*
==================
CQuad::Shutdown
==================
*/
void CQuad::Shutdown()
{
	pModel = NULL;
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
