#include "pch_resourcesystem.h"
#include "resourcesystem/resource.h"
#include "resourcesystem/resourcetypemgr.h"
#include "resourcesystem/resourcesystem.h"

//-----------------------------------------------------------------------------
// Collects dependencies into a resource's cached dependency list
//-----------------------------------------------------------------------------
class CResourceDependencyCollector : public IResourceDependencyCollector
{
public:
	// IResourceDependencyCollector interface
	virtual void AddDependency( IResource* pResource ) override;

	CResourceDependencyCollector( eastl::vector<CRefPtr<CResource>>& dependencies );

private:
	eastl::vector<CRefPtr<CResource>>& dependencies;
};

/*
==================
CResourceDependencyCollector::CResourceDependencyCollector
==================
*/
CResourceDependencyCollector::CResourceDependencyCollector( eastl::vector<CRefPtr<CResource>>& dependencies )
	: dependencies( dependencies )
{
}

/*
==================
CResourceDependencyCollector::AddDependency
==================
*/
void CResourceDependencyCollector::AddDependency( IResource* pResource )
{
	if ( pResource )
	{
		dependencies.emplace_back( (CResource*)pResource );
	}
}

/*
==================
CResource::CResource
==================
*/
CResource::CResource( CResourceTypeMgr* pOwner, const char* pName, resourceType_t type, uint8 flags /* = RESOURCE_TYPE_NONE */ )
	: type( type )
	, bInLruList( false )
	, bHoldsApplied( false )
	, pData( NULL )
	, pOwner( pOwner )
	, lastUsedFrame( g_resourceSystem.GetFrameNumber() )
	, name( pName )
{
	CResource::flags.store( RESOURCE_FLAG_ANONYMOUS | flags, eastl::memory_order_release );
	bPendingMarkUsed.store( false, eastl::memory_order_release );
	numPermanentHolders.store( 0, eastl::memory_order_release );
}

/*
==================
CResource::~CResource
==================
*/
CResource::~CResource()
{
	Uncache( true );
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
	// Do nothing if the resource is permanent
	PROFILER_SCOPE_FUNC();
	if ( HasAnyFlags( RESOURCE_FLAG_PERMANENT ) )
	{
		return;
	}

	// Mark the resource as permanent
	AddFlags( RESOURCE_FLAG_PERMANENT );

	// Place permanent holds on all dependencies
	ApplyPermanentHolds( false );
}

/*
==================
CResource::ClearPermanent
==================
*/
void CResource::ClearPermanent()
{
	// Do nothing if the resource isn't permanent or is procedural
	PROFILER_SCOPE_FUNC();
	if ( !HasAnyFlags( RESOURCE_FLAG_PERMANENT ) || path.empty() )
	{
		return;
	}

	// Remove flag permanent in the resource
	RemoveFlags( RESOURCE_FLAG_PERMANENT );

	// If nobody else is holding this resource permanent, release the holds
	// placed on its dependencies and let it fall back under LRU tracking
	if ( !HasPermanentHolders() )
	{
		ReleasePermanentHolds( false );
		MarkUsed();
	}
}

/*
==================
CResource::RebuildDependencies
==================
*/
void CResource::RebuildDependencies()
{
	// Release any holds placed on the old dependency list
	PROFILER_SCOPE_FUNC();
	ReleasePermanentHolds( false );

	// Collect the new dependency list from the current data
	dependencies.clear();
	if ( pData )
	{
		CResourceDependencyCollector dependencyCollector( dependencies );
		pData->CollectDependencies( &dependencyCollector );
	}

	// Place holds on the new dependency list if this resource is permanent
	ApplyPermanentHolds( false );
}

/*
==================
CResource::MarkUsedDependencies
==================
*/
void CResource::MarkUsedDependencies()
{
	PROFILER_SCOPE_FUNC();
	for ( uint32 index = 0, count = (uint32)dependencies.size(); index < count; ++index )
	{
		CResource* pDependency = dependencies[index];
		if ( pDependency )
		{
			pDependency->MarkUsed();
		}
	}
}

/*
==================
CResource::ApplyPermanentHolds
==================
*/
void CResource::ApplyPermanentHolds( bool bFromDependent )
{
	// A dependent resource just became (transitively) permanent - register
	// an extra hold placed on us by it
	PROFILER_SCOPE_FUNC();
	if ( bFromDependent )
	{
		numPermanentHolders.fetch_add( 1, eastl::memory_order_relaxed );
	}

	// Do nothing if holds are already applied, or this resource isn't (transitively) permanent
	if ( bHoldsApplied || ( !HasAnyFlags( RESOURCE_FLAG_PERMANENT ) && !HasPermanentHolders() ) )
	{
		return;
	}

	// Mark holds as applied before recursing so that a cycle in the dependency
	// graph stops here instead of re-entering this resource and double-counting forever
	bHoldsApplied = true;

	// Propagate the hold onto our own dependencies
	for ( uint32 index = 0, count = (uint32)dependencies.size(); index < count; ++index )
	{
		CResource* pDependency = dependencies[index];
		if ( pDependency )
		{
			pDependency->ApplyPermanentHolds( true );
		}
	}
}

/*
==================
CResource::ReleasePermanentHolds
==================
*/
void CResource::ReleasePermanentHolds( bool bFromDependent )
{
	// A dependent resource just stopped being (transitively) permanent - remove its hold.
	// Only continue past this if it was the last hold on us and we aren't permanent by our own flag
	PROFILER_SCOPE_FUNC();
	if ( bFromDependent && ( numPermanentHolders.fetch_sub( 1, eastl::memory_order_relaxed ) != 1 || HasAnyFlags( RESOURCE_FLAG_PERMANENT ) ) )
	{
		return;
	}

	// Do nothing if no holds are currently applied to our dependencies
	if ( !bHoldsApplied )
	{
		return;
	}

	// Mark holds as released before recursing so that a cycle in the
	// dependency graph stops here instead of re-entering this resource and
	// double-releasing
	bHoldsApplied = false;

	// Release the hold from our own dependencies
	for ( uint32 index = 0, count = (uint32)dependencies.size(); index < count; ++index )
	{
		CResource* pDependency = dependencies[index];
		if ( pDependency )
		{
			pDependency->ReleasePermanentHolds( true );
		}
	}

	// This resource is no longer held, start tracking it under LRU again
	if ( bFromDependent )
	{
		MarkUsed();
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
CResource::HasPermanentHolders
==================
*/
bool CResource::HasPermanentHolders() const
{
	return numPermanentHolders.load( eastl::memory_order_relaxed ) > 0;
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
