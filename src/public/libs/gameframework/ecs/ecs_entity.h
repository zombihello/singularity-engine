#ifndef ECS_ENTITY_H
#define ECS_ENTITY_H

#include "gameframework/ecs/ecs_core.h"
#include "gameframework/ientity.h"

//-----------------------------------------------------------------------------
// ECS entity
//-----------------------------------------------------------------------------
class CEcsEntity : public TRefCounted<IEntity>
{
public:
	CEcsEntity( ecsEntity_t ecsEntity );
	~CEcsEntity();

private:
	ecsEntity_t		ecsEntity;
};

#endif // !ECS_ENTITYDESC_H
