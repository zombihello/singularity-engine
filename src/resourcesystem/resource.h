#pragma once
#include <EASTL/string.h>
#include <EASTL/list.h>

#include "tier1/template.h"
#include "resourcesystem/iresource.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CResourceTypeMgr;

//-----------------------------------------------------------------------------
// A resource
//-----------------------------------------------------------------------------
class CResource : public CRefCounted<IResource>
{
public:
	friend CResourceTypeMgr;

	// IResource interface
	// To bring a data in or out of memory
	virtual bool Cache() override;
	virtual void Uncache() override;

	// Mark the resource as used
	virtual void MarkUsed() override;

	// Set/clear permanent flag
	virtual void MakePermanent() override;
	virtual void ClearPermanent() override;

	virtual bool		   HasAllFlags( uint8 flags ) const override;
	virtual bool		   HasAnyFlags( uint8 flags ) const override;
	virtual void*		   GetData() const override;
	virtual resourceType_t GetType() const override;
	virtual const char*	   GetName() const override;
	virtual const char*	   GetPath() const override;

	CResource( CResourceTypeMgr* pOwner, const char* pName, resourceType_t type, uint8 flags = RESOURCE_TYPE_NONE );
	~CResource();

	void ChangeData( const char* pPath, void* pData );
	void Uncache( bool bIgnorePermanent );
	void AddFlags( uint8 flags );
	void RemoveFlags( uint8 flags );

private:
	resourceType_t							type;
	eastl::atomic<uint8>					flags;
	eastl::atomic<bool>						bPendingMarkUsed;
	bool									bInLruList;
	void*									pData;
	CResourceTypeMgr*						pOwner;
	uint64									lastUsedFrame; 
	eastl::string							name;
	eastl::string							path;
	eastl::list<CResource*>::const_iterator lruIt;
};

#include "resourcesystem/resource.inl"