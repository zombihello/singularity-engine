#pragma once
#include <EASTL/atomic.h>

#include "tier0/defines.h"
#include "tier0/types.h"
#include "tier1/hashing/fasthash.h"
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
class TRefCounted : public TBaseClass
{
public:
	TRefCounted()
	{
		countReferences.store( 0, eastl::memory_order_release );
	}
	virtual ~TRefCounted()
	{
		Assert( GetRefCount() == 0 );
	}

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
class TRefPtr
{
public:
	// Hash function for STL containers
	struct hashFunction_t
	{
		size_t operator()( const TRefPtr& refPtr ) const;
	};

	TRefPtr()
		: pPtr( NULL )
	{
	}
	TRefPtr( TPtrType* pPtr )
		: pPtr( pPtr )
	{
		if ( pPtr )
		{
			pPtr->AddRef();
		}
	}
	TRefPtr( const TRefPtr& copy )
		: pPtr( copy.pPtr )
	{
		if ( pPtr )
		{
			pPtr->AddRef();
		}
	}

	template<typename TBasePtrType>
	TRefPtr( const TRefPtr<TBasePtrType>& copy )
		: pPtr( (TPtrType*)( copy.GetPtr() ) )
	{
		if ( pPtr )
		{
			pPtr->AddRef();
		}
	}
	~TRefPtr()
	{
		if ( pPtr )
		{
			pPtr->ReleaseRef();
		}
	}

	void	  SafeRelease();
	bool	  IsValid() const;
	uint32	  GetRefCount() const;
	TPtrType* GetPtr() const;

	template<typename TBasePtrType>
	TRefPtr&   operator=( const TRefPtr<TBasePtrType>& copy );
	TRefPtr&   operator=( TPtrType* pPtr );
	TRefPtr&   operator=( const TRefPtr& copy );
	bool	   operator==( const TRefPtr& right ) const;
	bool	   operator==( TPtrType* pRight ) const;
	bool	   operator!=( const TRefPtr& right ) const;
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
