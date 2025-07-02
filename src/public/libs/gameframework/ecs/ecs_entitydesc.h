#ifndef ECS_ENTITYDESC_H
#define ECS_ENTITYDESC_H

#include "sentdoc/sent_compiled_doc.h"
#include "gameframework/ecs/ecs_core.h"
#include "gameframework/ecs/ecs_component_factory.h"
#include "gameframework/ecs/ecs_map.h"
#include "gameframework/ientitydesc.h"

//-----------------------------------------------------------------------------
// ECS entity descriptor
// By this you can create an ECS entity
//-----------------------------------------------------------------------------
class CEcsEntityDesc : public TRefCounted<IEntityDesc>
{
public:
	// IEntityDesc interface
	virtual IEntity* Create( IMap* pMap, const achar* pName = "" ) const override;

	CEcsEntityDesc();
	CEcsEntityDesc( const CSENTCompiledEntityDescDoc& sentCompiledDoc );
	~CEcsEntityDesc();

	void Clear();

private:
	struct ecsPrefab_t
	{
		CEcsMap*						pEcsMap;
		ecsEntity_t						ecsEntity;
		IOnMapReseted::funcDelegate_t*	pOnMapResetedDelegate;
		IOnMapUnloaded::funcDelegate_t*	pOnMapUnloadedDelegate;
	};

	void Init( const CSENTCompiledEntityDescDoc& sentCompiledDoc );
	uint32 CreateEcsPrefab( CEcsMap* pEcsMap, const achar* pName, uint32 ecsPrefabIdx = INVALID_INDEX ) const;
	static void OnMapResetedOrUnloaded( void* pUserData, IMap* pMap );

	mutable std::vector<ecsPrefab_t>				ecsPrefabs;
	std::vector<TRefPtr<IEcsComponentFactory>>		ecsComponentFactories;
	CGuid											guid;
	mutable uint32									lastUsedEcsPrefabIdx;
};

#endif // !ECS_ENTITYDESC_H
