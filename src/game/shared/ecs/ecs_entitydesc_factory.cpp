#include "pch_game_shared.h"
#include "game/shared/ecs/ecs_entitydesc.h"
#include "game/shared/ecs/ecs_entitydesc_factory.h"

/*
==================
CEcsEntityDescFactory::Create
==================
*/
void* CEcsEntityDescFactory::Create() const
{
	return new CEcsEntityDesc();
}

/*
==================
CEcsEntityDescFactory::Delete
==================
*/
void CEcsEntityDescFactory::Delete( void* pData ) const
{
	delete (CEcsEntityDesc*)pData;
}
