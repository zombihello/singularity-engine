#pragma once
#include "utils/sentdoc/sent_compiled_doc.h"
#include "tier1/guid.h"
#include "game/shared/ecs/ecs_core.h"
#include "game/shared/ecs/ecs_component_factory.h"
#include "game/shared/ecs/ecs_map.h"
#include "game/ientitydesc.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CEcsMap;

//-----------------------------------------------------------------------------
// ECS entity descriptor
// By this you can create an ECS entity
//-----------------------------------------------------------------------------
class CEcsEntityDesc : public IEntityDesc
{
public:
	CEcsEntityDesc();
	CEcsEntityDesc( const CSENTCompiledEntityDescDoc& sentCompiledDoc );
	~CEcsEntityDesc();

	void Init( const CSENTCompiledEntityDescDoc& sentCompiledDoc );
	void Clear();

	ecsEntity_t GetEcsPrefab( CEcsMap* pEcsMap );

private:
	struct ecsPrefab_t
	{
		CEcsMap*				 pEcsMap;
		ecsEntity_t				 ecsEntity;
		IOnMapReseted::handle_t	 onMapResetedHandle;
		IOnMapUnloaded::handle_t onMapUnloadedHandle;
	};

	uint32		CreateEcsPrefab( CEcsMap* pEcsMap, const char* pName, uint32 ecsPrefabIdx = INVALID_INDEX ) const;
	static void OnMapResetedOrUnloaded( void* pUserData, IMap* pMap );

	mutable eastl::vector<ecsPrefab_t>			 ecsPrefabs;
	eastl::vector<CRefPtr<IEcsComponentFactory>> ecsComponentFactories;
	CGuid										 guid;
	mutable uint32								 lastUsedEcsPrefabIdx;
};

DECLARE_RESOURCE_TYPE( CEcsEntityDesc, RESOURCE_TYPE_ENTITY_DESC );
