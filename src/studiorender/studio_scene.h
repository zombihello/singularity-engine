#pragma once
#include "studiorender/istudio_scene.h"
#include "studiorender/studio_batchedsimpleelements.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CStudioScene;
struct studioSceneView_t;

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
	virtual void Update( float deltaSeconds ) override;

	virtual studioEntityId_t			AddEntity( const studioEntityParams_t& params ) override;
	virtual void						UpdateEntity( studioEntityId_t id, const studioEntityParams_t& params ) override;
	virtual void						FreeEntity( studioEntityId_t id ) override;
	virtual const studioEntityParams_t* GetEntityParams( studioEntityId_t id ) const override;

	virtual void DrawDebugLine( const vector3_t& start, const vector3_t& end, const CColor& color, float lifeTime = 0.f, bool bDepthTest = true ) override;
	virtual void DrawDebugBox( const CAABB& aabb, const CColor& color, float lifeTime = 0.f, bool bDepthTest = true ) override;

	void								 FindEntityViews( studioSceneView_t* pSceneView ) const;
	void								 AddDebugPrimitivesToSceneView( studioSceneView_t* pSceneView ) const;
	const eastl::vector<studioEntity_t>& GetEntities() const;

private:
	eastl::vector<studioEntity_t>		   entities;
	eastl::list<studioEntityId_t>		   freeEntityIdList;
	eastl::vector<studioSimplePrimitive_t> debugPrimitives;
};

#include "studiorender/studio_scene.inl"
