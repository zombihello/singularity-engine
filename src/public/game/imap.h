#pragma once
#include "tier1/refcount.h"
#include "tier1/event.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IEntity;
class IEntityDesc;
class IMap;

//-----------------------------------------------------------------------------
// Map delegate interfaces
//-----------------------------------------------------------------------------
DECLARE_EVENT_INTERFACE( IOnMapReseted, IMap* /* pMap */ );
DECLARE_EVENT_INTERFACE( IOnMapUnloaded, IMap* /* pMap */ );

//-----------------------------------------------------------------------------
// Map interface
//-----------------------------------------------------------------------------
class IMap
{
public:
	// Spawn and destroy an entity
	virtual IEntity* SpawnEntity( IEntityDesc* pEntityDesc, const char* pName = "" ) = 0;
	virtual void	 DestroyEntity( IEntity* pEntity )								 = 0;

	// Resets the map to initial state to before any updates on the map
	virtual void Reset()				   = 0;
	virtual void Update( float deltaTime ) = 0;

	virtual IOnMapReseted*	OnMapReseted() const  = 0;
	virtual IOnMapUnloaded* OnMapUnloaded() const = 0;
};
