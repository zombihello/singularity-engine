#pragma once
#include "tier1/refcount.h"
#include "tier1/math/aabb.h"
#include "tier1/math/color.h"
#include "resourcesystem/resourceptr.h"
#include "modelsystem/imodel.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IModel;

//-----------------------------------------------------------------------------
// Studio entity id
//-----------------------------------------------------------------------------
typedef uint64 studioEntityId_t;

//-----------------------------------------------------------------------------
// Studio entity parameters
//-----------------------------------------------------------------------------
struct studioEntityParams_t
{
	CResourcePtr<IModel> pModel;
	matrix4x3_t			 localToWorld;
};

//-----------------------------------------------------------------------------
// Studio scene interface
//-----------------------------------------------------------------------------
class IStudioScene : public IRefCounted
{
public:
	virtual ~IStudioScene() {}

	virtual void Update( float deltaSeconds ) = 0;

	virtual studioEntityId_t			AddEntity( const studioEntityParams_t& params )							= 0;
	virtual void						UpdateEntity( studioEntityId_t id, const studioEntityParams_t& params ) = 0;
	virtual void						FreeEntity( studioEntityId_t id )										= 0;
	virtual const studioEntityParams_t* GetEntityParams( studioEntityId_t id ) const							= 0;

	virtual void DrawDebugLine( const vector3_t& start, const vector3_t& end, const CColor& color, float lifeTime = 0.f, bool bDepthTest = true ) = 0;
	virtual void DrawDebugBox( const CAABB& aabb, const CColor& color, float lifeTime = 0.f, bool bDepthTest = true )							  = 0;
};
