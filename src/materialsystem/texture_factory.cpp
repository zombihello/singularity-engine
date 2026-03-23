#include "pch_materialsystem.h"
#include "materialsystem/texture.h"
#include "materialsystem/texture_factory.h"

/*
==================
CTextureFactory::Create
==================
*/
void* CTextureFactory::Create() const
{
	return new CTexture();
}

/*
==================
CTextureFactory::Delete
==================
*/
void CTextureFactory::Delete( void* pData ) const
{
	delete (CTexture*)pData;
}
