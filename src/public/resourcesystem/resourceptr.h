#pragma once
#include "resourcesystem/iresource.h"

//-----------------------------------------------------------------------------
// A resource pointer
//-----------------------------------------------------------------------------
template<class TResourceClass>
class CResourcePtr
{
public:
	CResourcePtr();
	CResourcePtr( IResource* pPtr );
	CResourcePtr( const CResourcePtr& copy );

	// To bring a data in or out of memory
	bool Cache();
	void Uncache();
	bool Reload();

	// Mark the resource as used
	void MarkUsed();

	bool			IsValid() const;
	bool			IsCached() const;
	IResource*		GetResource() const;
	TResourceClass* GetData() const;

	CResourcePtr&	operator=( IResource* pPtr );
	CResourcePtr&	operator=( const CResourcePtr& copy );
	bool			operator==( const CResourcePtr& right ) const;
	bool			operator==( IResource* pRight ) const;
	bool			operator!=( const CResourcePtr& right ) const;
	bool			operator!=( IResource* pRight ) const;
					operator bool() const;
					operator ptrint() const;
					operator uptrint() const;
					operator IResource*() const;
					operator TResourceClass*() const;
	TResourceClass* operator->() const;
	TResourceClass* operator*() const;

private:
	CRefPtr<IResource> pPtr;
};

#include "resourcesystem/resourceptr.inl"
