#include "utils/smapdoc/smap_entity.h"

/*
==================
CSMAPEntity::CSMAPEntity
==================
*/
CSMAPEntity::CSMAPEntity()
{
}

/*
==================
CSMAPEntity::CSMAPEntity
==================
*/
CSMAPEntity::CSMAPEntity( const CSMAPEntity& other )
	: classname( other.classname )
	, name( other.name )
{
}

/*
==================
CSMAPEntity::~CSMAPEntity
==================
*/
CSMAPEntity::~CSMAPEntity()
{
}
