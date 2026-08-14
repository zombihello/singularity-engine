#include "pch_studiorender.h"
#include "studiorender/studiorender.h"
#include "studiorender/studio_sceneview.h"
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

	if ( studioEntity.params.localToWorld != params.localToWorld )
	{
		studioEntity.params.localToWorld = params.localToWorld;
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
CStudioScene::FindEntityViews
==================
*/
void CStudioScene::FindEntityViews( studioSceneView_t* pSceneView ) const
{
	// TODO BS yehor.pohuliaka - Here should be frustum and occlusion culling
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_VISIBILITY );
	for ( uint32 index = 0, count = (uint32)entities.size(); index < count; ++index )
	{
		const studioEntity_t& entity	  = entities[index];
		studioEntityView_t*	  pEntityView = (studioEntityView_t*)g_studioFrameAlloc.Alloc( sizeof( studioEntityView_t ) );
		pEntityView->pEntity			  = (studioEntity_t*)&entity;
		pEntityView->localToWorld		  = entity.params.localToWorld;
		pEntityView->pNext				  = pSceneView->pEntityViews;
		pSceneView->pEntityViews		  = pEntityView;
	}
}

/*
==================
CStudioScene::AddDebugPrimitivesToSceneView
==================
*/
void CStudioScene::AddDebugPrimitivesToSceneView( studioSceneView_t* pSceneView ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_VISIBILITY );
	pSceneView->debugPrimitives.resize( debugPrimitives.size() );
	Mem_Memcpy( pSceneView->debugPrimitives.data(), debugPrimitives.data(), debugPrimitives.size() * sizeof( studioSimplePrimitive_t ) );
}

/*
==================
CStudioScene::Update
==================
*/
void CStudioScene::Update( float deltaSeconds )
{
	// Age the debug primitives and drop the expired ones
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	for ( auto it = debugPrimitives.begin(); it != debugPrimitives.end(); )
	{
		studioSimplePrimitive_t& debugPrimitive = *it;
		debugPrimitive.timeToLive -= deltaSeconds;
		if ( debugPrimitive.timeToLive <= 0.f )
		{
			it = debugPrimitives.erase( it );
		}
		else
		{
			++it;
		}
	}
}

/*
==================
CStudioScene::DrawDebugLine
==================
*/
void CStudioScene::DrawDebugLine( const vector3_t& start, const vector3_t& end, const CColor& color, float lifeTime /* = 0.f */, bool bDepthTest /* = true */ )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	studioSimplePrimitive_t& debugPrimitive = debugPrimitives.emplace_back();
	debugPrimitive.type						= STUDIO_SIMPLE_PRIMITIVE_TYPE_LINE;
	debugPrimitive.color					= color;
	debugPrimitive.timeToLive				= lifeTime;
	debugPrimitive.bDepthTest				= bDepthTest;
	debugPrimitive.line.start				= start;
	debugPrimitive.line.end					= end;
}

/*
==================
CStudioScene::DrawDebugBox
==================
*/
void CStudioScene::DrawDebugBox( const CAABB& aabb, const CColor& color, float lifeTime /* = 0.f */, bool bDepthTest /* = true */ )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	studioSimplePrimitive_t& debugPrimitive = debugPrimitives.emplace_back();
	debugPrimitive.type						= STUDIO_SIMPLE_PRIMITIVE_TYPE_BOX;
	debugPrimitive.color					= color;
	debugPrimitive.timeToLive				= lifeTime;
	debugPrimitive.bDepthTest				= bDepthTest;
	debugPrimitive.box.aabb					= aabb;
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
