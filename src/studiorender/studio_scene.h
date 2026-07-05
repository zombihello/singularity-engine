#pragma once
#include "studiorender/istudio_scene.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CStudioScene;

//-----------------------------------------------------------------------------
// Studio entity
//-----------------------------------------------------------------------------
struct studioEntity_t
{
	studioEntityId_t	 id;
	studioEntityParams_t params;
	CStudioScene*		 pStudioScene;
};

//-----------------------------------------------------------------------------
// Studio scene
//-----------------------------------------------------------------------------
class CStudioScene : public CRefCounted<IStudioScene>
{
public:
	// IStudioScene interface
	virtual studioEntityId_t			AddEntity( const studioEntityParams_t& params ) override;
	virtual void						UpdateEntity( studioEntityId_t id, const studioEntityParams_t& params ) override;
	virtual void						FreeEntity( studioEntityId_t id ) override;
	virtual const studioEntityParams_t* GetEntityParams( studioEntityId_t id ) const override;

	const eastl::vector<studioEntity_t>& GetEntities() const;

private:
	eastl::vector<studioEntity_t> entities;
	eastl::list<studioEntityId_t> freeEntityIdList;
};

#include "studiorender/studio_scene.inl"