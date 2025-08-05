#pragma once
#include "resourcesystem/iresource.h"

//-----------------------------------------------------------------------------
// Resource
//-----------------------------------------------------------------------------
class CResource : public TRefCounted<IResource>
{
public:
	CResource( const achar* pPath, IRefCounted* pData, resourceType_t type, bool bProcedural = false );

	// IResource interface
	// If the data isn't valid return the default resource or NULL if it isn't exist in the system
	virtual IRefCounted* GetData() const override;
	virtual resourceType_t GetType() const override;

	void SetData( IRefCounted* pData, resourceType_t type );
	bool IsProcedural() const;
	const achar* GetPath() const;

private:
	bool					bProcedural;
	resourceType_t			type;
	std::string				path;
	TRefPtr<IRefCounted>	pData;
};

#include "resourcesystem/resource.inl"