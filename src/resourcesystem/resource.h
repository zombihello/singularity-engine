#pragma once
#include "resourcesystem/iresource.h"

//-----------------------------------------------------------------------------
// Resource
//-----------------------------------------------------------------------------
class CResource : public CRefCounted<IResource>
{
public:
	CResource( const char* pPath, IRefCounted* pData, resourceType_t type, bool bProcedural = false );

	// IResource interface
	// If the data isn't valid return the default resource or NULL if it isn't exist in the system
	virtual IRefCounted*   GetData() const override;
	virtual resourceType_t GetType() const override;

	void		 SetData( IRefCounted* pData, resourceType_t type );
	bool		 IsProcedural() const;
	const char* GetPath() const;

private:
	bool				 bProcedural;
	resourceType_t		 type;
	eastl::string			 path;
	CRefPtr<IRefCounted> pData;
};

#include "resourcesystem/resource.inl"