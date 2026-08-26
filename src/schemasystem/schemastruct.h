#pragma once
#include "schemasystem/ischemastruct.h"
#include "schemasystem/schemacompoundtype.h"

//-----------------------------------------------------------------------------
// A described struct
//-----------------------------------------------------------------------------
class CSchemaStruct : public CSchemaCompoundTypeBase<ISchemaStruct>
{
public:
	// ISchemaType interface
	virtual bool IsA( const ISchemaType* pType ) const override;

	// ISchemaCompoundType interface
	virtual void* Cast( void* pObject, const ISchemaType* pType ) const override;

	// ISchemaStruct interface
	virtual void Construct( void* pMemory ) const override;
	virtual void Destruct( void* pObject ) const override;

	CSchemaStruct( const schemaStructDesc_t& structDesc );
};
