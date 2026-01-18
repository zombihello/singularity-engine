#include "smapdoc/smap_entity.h"

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
	: entityDesc( other.entityDesc )
	, name( other.name )
{
	Copy( other );
}

/*
==================
CSMAPEntity::~CSMAPEntity
==================
*/
CSMAPEntity::~CSMAPEntity()
{
}