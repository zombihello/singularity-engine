#pragma once
#include "stdlib/refcount.h"
#include "stdlib/delegate.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IEntity;
class IEntityDesc;
class IMap;


//-----------------------------------------------------------------------------
// Map delegate interfaces
//-----------------------------------------------------------------------------
DECLARE_MULTICAST_DELEGATE_INTERFACE( IOnMapReseted, IMap* /* pMap */ );
DECLARE_MULTICAST_DELEGATE_INTERFACE( IOnMapUnloaded, IMap* /* pMap */ );


//-----------------------------------------------------------------------------
// Map interface
//-----------------------------------------------------------------------------
class IMap
{
public:
	// Spawn and destroy an entity
	virtual IEntity* SpawnEntity( IEntityDesc* pEntityDesc, const achar* pName = "" ) = 0;
	virtual void DestroyEntity( IEntity* pEntity ) = 0;

	// Resets the map to initial state to before any updates on the map
	virtual void Reset() = 0;
	virtual void Update( float deltaTime ) = 0;

	virtual IOnMapReseted* OnMapReseted() const = 0;
	virtual IOnMapUnloaded* OnMapUnloaded() const = 0;
};
