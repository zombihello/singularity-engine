#pragma once
#include "resourcesystem/iresource.h"
#include "game/game_resource_types.h"

//-----------------------------------------------------------------------------
// Entity descriptor interface
// By this you can create an entity
//-----------------------------------------------------------------------------
class IEntityDesc : public IResourceData
{
public:
	virtual ~IEntityDesc() {}
};

DECLARE_RESOURCE_TYPE( IEntityDesc, RESOURCE_TYPE_ENTITY_DESC );
