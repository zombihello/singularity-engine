#include "pch_resourcesystem.h"
#include "resourcesystem/resource.h"
#include "resourcesystem/resourcetypemgr.h"
#include "resourcesystem/resourcesystem.h"

/*
==================
CResource::CResource
==================
*/
CResource::CResource( CResourceTypeMgr* pOwner, const char* pName, resourceType_t type, uint8 flags /* = RESOURCE_TYPE_NONE */ )
	: type( type )
	, bInLruList( false )
	, pData( NULL )
	, pOwner( pOwner )
	, lastUsedFrame( g_resourceSystem.GetFrameNumber() )
	, name( pName )
{
	CResource::flags.store( RESOURCE_FLAG_ANONYMOUS | flags, eastl::memory_order_release );
	bPendingMarkUsed.store( false, eastl::memory_order_release );
}

/*
==================
CResource::~CResource
==================
*/
CResource::~CResource()
{
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
void CResource::Uncache( bool bIgnorePermanent )
{
	if ( Ensure( pOwner ) )
	{
		pOwner->UncacheResource( this, bIgnorePermanent );
	}
}

/*
==================
CResource::Uncache
==================
*/
void CResource::Uncache()
{
	Uncache( false );
}

/*
==================
CResource::MarkUsed
==================
*/
void CResource::MarkUsed()
{
	if ( Ensure( pOwner ) )
	{
		pOwner->MarkUsedResource( this );
	}
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
	return ( CResource::flags.load( eastl::memory_order_relaxed ) & flags ) == flags;
}

/*
==================
CResource::HasAnyFlags
==================
*/
bool CResource::HasAnyFlags( uint8 flags ) const
{
	return ( CResource::flags.load( eastl::memory_order_relaxed ) & flags ) != 0;
}

/*
==================
CResource::GetData
==================
*/
IResourceData* CResource::GetData() const
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

/*
==================
CResource::OnCached
==================
*/
IResource::IOnCached* CResource::OnCached() const
{
	return &onCached;
}

/*
==================
CResource::OnUncached
==================
*/
IResource::IOnUncached* CResource::OnUncached() const
{
	return &onUncached;
}
