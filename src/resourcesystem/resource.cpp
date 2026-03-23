#include "pch_resourcesystem.h"
#include "resourcesystem/resource.h"
#include "resourcesystem/resourcesystem.h"

/*
==================
CResource::CResource
==================
*/
CResource::CResource( CResourceTypeMgr* pOwner, const char* pName, resourceType_t type, uint8 flags /* = RESOURCE_TYPE_NONE */ )
	: type( type )
	, flags( RESOURCE_FLAG_ANONYMOUS | flags )
	, pData( NULL )
	, pOwner( pOwner )
	, lastUsedFrame( 0 )
	, name( pName )
{
}

/*
==================
CResource::~CResource
==================
*/
CResource::~CResource()
{
	RemoveFlags( RESOURCE_FLAG_PERMANENT );
	Uncache();
}

/*
==================
CResource::Cache
==================
*/
bool CResource::Cache()
{
	return Ensure( pOwner ) ? pOwner->CacheResource( this ) : NULL;
}

/*
==================
CResource::Uncache
==================
*/
void CResource::Uncache()
{
	if ( Ensure( pOwner ) )
	{
		pOwner->UncacheResource( this );
	}
}

/*
==================
CResource::MarkUsed
==================
*/
void CResource::MarkUsed()
{
}

/*
==================
CResource::MakePermanent
==================
*/
void CResource::MakePermanent()
{
	AddFlags( RESOURCE_FLAG_PERMANENT );
}

/*
==================
CResource::ClearPermanent
==================
*/
void CResource::ClearPermanent()
{
	if ( !path.empty() && !HasAnyFlags( RESOURCE_FLAG_DEFAULT ) )
	{
		RemoveFlags( RESOURCE_FLAG_PERMANENT );
	}
}

/*
==================
CResource::HasAllFlags
==================
*/
bool CResource::HasAllFlags( uint8 flags ) const
{
	return ( CResource::flags & flags ) == flags;
}

/*
==================
CResource::HasAnyFlags
==================
*/
bool CResource::HasAnyFlags( uint8 flags ) const
{
	return ( CResource::flags & flags ) != 0;
}

/*
==================
CResource::GetData
==================
*/
void* CResource::GetData() const
{
	return pData;
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
CResource::GetName
==================
*/
const char* CResource::GetName() const
{
	return name.c_str();
}

/*
==================
CResource::GetPath
==================
*/
const char* CResource::GetPath() const
{
	return path.c_str();
}
