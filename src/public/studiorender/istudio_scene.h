#pragma once
#include "tier1/refcount.h"
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
};

//-----------------------------------------------------------------------------
// Studio scene interface
//-----------------------------------------------------------------------------
class IStudioScene : public IRefCounted
{
public:
	virtual ~IStudioScene() {}

	virtual studioEntityId_t			AddEntity( const studioEntityParams_t& params )							= 0;
	virtual void						UpdateEntity( studioEntityId_t id, const studioEntityParams_t& params ) = 0;
	virtual void						FreeEntity( studioEntityId_t id )										= 0;
	virtual const studioEntityParams_t* GetEntityParams( studioEntityId_t id ) const							= 0;
};