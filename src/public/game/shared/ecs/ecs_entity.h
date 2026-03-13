#pragma once
#include "game/shared/ecs/ecs_core.h"
#include "game/ientity.h"
#include "game/imap.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CEcsMap;

//-----------------------------------------------------------------------------
// ECS entity
//-----------------------------------------------------------------------------
class CEcsEntity : public CRefCounted<IEntity>
{
public:
	// IEntity interface
	virtual void Destroy() override;
	virtual bool IsValid() const override;

	CEcsEntity( ecsEntity_t ecsEntity, CEcsMap* pEcsMap );
	~CEcsEntity();

private:
	static void OnMapResetedOrUnloaded( void* pUserData, IMap* pMap );
	void		DestroyEcsEntity();

	ecsEntity_t				 ecsEntity;
	CEcsMap*				 pEcsMap;
	IOnMapReseted::handle_t	 onMapResetedHandle;
	IOnMapUnloaded::handle_t onMapUnloadedHandle;
};
