#ifndef ECS_ENTITY_H
#define ECS_ENTITY_H

#include "gameframework/ecs/ecs_core.h"
#include "gameframework/ientity.h"
#include "gameframework/imap.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CEcsMap;


//-----------------------------------------------------------------------------
// ECS entity
//-----------------------------------------------------------------------------
class CEcsEntity : public TRefCounted<IEntity>
{
public:
	CEcsEntity( ecsEntity_t ecsEntity, CEcsMap* pEcsMap );
	~CEcsEntity();

private:
	static void OnMapResetedOrUnloaded( void* pUserData, IMap* pMap );
	void DestroyEcsEntity( bool bRemoveEcsMapDelegates = true );

	ecsEntity_t							ecsEntity;
	CEcsMap*							pEcsMap;
	IOnMapReseted::funcDelegate_t*		pOnMapResetedDelegate;
	IOnMapUnloaded::funcDelegate_t*		pOnMapUnloadedDelegate;
};

#endif // !ECS_ENTITYDESC_H
