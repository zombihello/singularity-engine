#pragma once
#include "schemasystem/ischemacompoundtype.h"

//-----------------------------------------------------------------------------
// A described struct
//
// NOTES:
//	* The base of a struct is always a struct
//	* A struct is a light type - it never gains a vtable and it lives in memory
//	  somebody else owns
//-----------------------------------------------------------------------------
class ISchemaStruct : public ISchemaCompoundType
{
public:
	// Construct/destruct an object of this type in memory the caller owns
	// NOTE: `pMemory` must be at least `GetSize()` bytes and `GetAlignment()` aligned
	virtual void Construct( void* pMemory ) const = 0;
	virtual void Destruct( void* pObject ) const  = 0;

	// The struct this one derives from, NULL when there is none
	const ISchemaStruct* GetBaseStruct() const;
};

#include "schemasystem/ischemastruct.inl"
