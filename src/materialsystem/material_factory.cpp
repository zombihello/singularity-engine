#include "pch_materialsystem.h"
#include "materialsystem/material.h"
#include "materialsystem/material_factory.h"

/*
==================
CMaterialFactory::Create
==================
*/
void* CMaterialFactory::Create() const
{
	return new CMaterial();
}

/*
==================
CMaterialFactory::Delete
==================
*/
void CMaterialFactory::Delete( void* pData ) const
{
	delete (CMaterial*)pData;
}
