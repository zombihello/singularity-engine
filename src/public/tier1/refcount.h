#pragma once
#include <EASTL/atomic.h>

#include "tier0/defines.h"
#include "tier0/types.h"
#include "tier1/fasthash.h"
#include "tier1/threading.h"

//-----------------------------------------------------------------------------
// Standard reference counted interface
//-----------------------------------------------------------------------------
class IRefCounted
{
public:
	virtual void   AddRef()			   = 0;
	virtual void   ReleaseRef()		   = 0;
	virtual uint32 GetRefCount() const = 0;
};

//-----------------------------------------------------------------------------
// Base classes to implement reference counting
//-----------------------------------------------------------------------------
template<class TBaseClass>
class CRefCounted : public TBaseClass
{
public:
	CRefCounted();
	virtual ~CRefCounted();

	virtual void   AddRef() override;
	virtual void   ReleaseRef() override;
	virtual uint32 GetRefCount() const override;

private:
	eastl::atomic<uint32> countReferences;
};

//-----------------------------------------------------------------------------
// Reference-counting pointer
//-----------------------------------------------------------------------------
template<typename TPtrType>
class CRefPtr
{
public:
	// Hash function for STL containers
	struct hashFunction_t
	{
		size operator()( const CRefPtr& refPtr ) const;
	};

	CRefPtr();
	CRefPtr( TPtrType* pPtr );
	CRefPtr( const CRefPtr& other );
	template<typename TBasePtrType>
	CRefPtr( const CRefPtr<TBasePtrType>& copy );
	~CRefPtr();

	bool	  IsValid() const;
	uint32	  GetRefCount() const;
	TPtrType* GetRawPtr() const;

	template<typename TBasePtrType>
	CRefPtr&   operator=( const CRefPtr<TBasePtrType>& copy );
	CRefPtr&   operator=( TPtrType* pPtr );
	CRefPtr&   operator=( const CRefPtr& copy );
	bool	   operator==( const CRefPtr& right ) const;
	bool	   operator==( TPtrType* pRight ) const;
	bool	   operator!=( const CRefPtr& right ) const;
	bool	   operator!=( TPtrType* pRight ) const;
			   operator bool() const;
			   operator ptrint() const;
			   operator uptrint() const;
			   operator TPtrType&() const;
			   operator TPtrType*() const;
			   operator TPtrType*&();
	TPtrType*  operator->() const;
	TPtrType&  operator*();
	TPtrType** operator&();

private:
	TPtrType* pPtr;
};

#include "tier1/refcount.inl"
