#ifndef ECS_ENTITYDESC_H
#define ECS_ENTITYDESC_H

#include "sentdoc/sent_compiled_doc.h"
#include "gameframework/ecs/ecs_core.h"
#include "gameframework/ecs/ecs_component_factory.h"
#include "gameframework/ientitydesc.h"

//-----------------------------------------------------------------------------
// ECS entity descriptor
// By this you can create an ECS entity
//-----------------------------------------------------------------------------
class CEcsEntityDesc : public TRefCounted<IEntityDesc>
{
public:
	// IEntityDesc interface
	virtual IEntity* Create( const achar* pName = "" ) const override;

	CEcsEntityDesc();
	CEcsEntityDesc( const CSENTCompiledEntityDescDoc& sentCompiledDoc );
	~CEcsEntityDesc();

	void Clear();

private:
	void Init( const CSENTCompiledEntityDescDoc& sentCompiledDoc );
	void RecreateEcsArchetypeEntity() const;
	ecsEntity_t CreateEcsEntity( const achar* pName ) const;

	mutable ecsEntity_t								ecsArchetypeEntity;
	std::vector<TRefPtr<IEcsComponentFactory>>		ecsComponentFactories;
};

#endif // !ECS_ENTITYDESC_H
