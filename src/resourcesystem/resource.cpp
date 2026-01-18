#include "pch_resourcesystem.h"
#include "resourcesystem/resource.h"
#include "resourcesystem/resourcesystem.h"

/*
==================
CResource::CResource
==================
*/
CResource::CResource( const char* pPath, IRefCounted* pData, resourceType_t type, bool bProcedural /* = false */ )
	: bProcedural( bProcedural )
	, type( type )
	, path( pPath )
	, pData( pData )
{
}

/*
==================
CResource::GetType
==================
*/
resourceType_t CResource::GetType() const
{
	return type;
}

/*
==================
CResource::GetData
==================
*/
IRefCounted* CResource::GetData() const
{
	return pData ? pData : g_ResourceSystem.GetDefaultResource( type );
}