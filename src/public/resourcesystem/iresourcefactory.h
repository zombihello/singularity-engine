#ifndef IRESOURCEFACTORY_H
#define IRESOURCEFACTORY_H

#include "stdlib/refcount.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IResource;


//-----------------------------------------------------------------------------
// Load flags
//-----------------------------------------------------------------------------
enum resourceLoadFlag_t
{
	RESOURCE_LOAD_FLAG_NONE			= 0,
	RESOURCE_LOAD_FLAG_ONLY_FIND	= 1 << 0	// Only try to find already exists a resource
};


//-----------------------------------------------------------------------------
// Resource factory interface
// Path to the resource in the file system can be without file extension
//-----------------------------------------------------------------------------
class IResourceFactory
{
public:
	virtual TRefPtr<IRefCounted> CreateProceduralResource() const = 0;
	virtual TRefPtr<IRefCounted> LoadResource( const achar* pPath, uint32 loadFlags = RESOURCE_LOAD_FLAG_NONE ) const = 0;
	virtual void UnloadResource( IRefCounted* pResoruce ) const = 0;
	virtual TRefPtr<IResource> GetDefaultResource() const = 0;
};

#endif // !IRESOURCEFACTORY_H