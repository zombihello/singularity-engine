#include "pch_studiorender.h"
#include "studiorender/studio_scene.h"

/*
==================
CStudioScene::AddEntity
==================
*/
studioEntityId_t CStudioScene::AddEntity( const studioEntityParams_t& params )
{
	// If we have free indices then use it
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	studioEntityId_t id = 0;
	if ( !freeEntityIdList.empty() )
	{
		id = freeEntityIdList.front();
		freeEntityIdList.pop_front();
	}
	else
	{
		id = entities.size();
		entities.emplace_back();
	}

	// Allocate a new studio entity
	studioEntity_t& studioEntity = entities[id];
	studioEntity.params			 = params;
	studioEntity.id				 = id;
	studioEntity.pStudioScene	 = this;
	return id;
}

/*
==================
CStudioScene::UpdateEntity
==================
*/
void CStudioScene::UpdateEntity( studioEntityId_t id, const studioEntityParams_t& params )
{
	// Do nothing if the id isn't valid
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	if ( id >= entities.size() )
	{
		Error( "StudioRender: Studio entity id isn't valid (id: %i, maxId: %i)", id, S_Max<studioEntityId_t>( 0, entities.size() - 1 ) );
		return;
	}

	// Do nothing if params has invalid model pointer
	if ( !params.pModel )
	{
		Error( "StudioRender: Studio entity params has invalid model pointer" );
		return;
	}

	// All ok, update the entity
	studioEntity_t& studioEntity = entities[id];
	if ( studioEntity.params.pModel != params.pModel )
	{
		studioEntity.params.pModel = params.pModel;
	}
}

/*
==================
CStudioScene::FreeEntity
==================
*/
void CStudioScene::FreeEntity( studioEntityId_t id )
{
	// Do nothing if the id isn't valid
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	if ( id >= entities.size() )
	{
		return;
	}

	studioEntity_t& studioEntity = entities[id];
	studioEntity.params			 = studioEntityParams_t{};
	freeEntityIdList.emplace_back( id );
}

/*
==================
CStudioScene::GetEntityParams
==================
*/
const studioEntityParams_t* CStudioScene::GetEntityParams( studioEntityId_t id ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( id < entities.size() );
	return &entities[id].params;
}