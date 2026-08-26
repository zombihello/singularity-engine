#pragma once
#include "schemasystem/ischemacompoundtype.h"
#include "schemasystem/schematypebase.h"

//-----------------------------------------------------------------------------
// Everything a described struct and a described class share
//-----------------------------------------------------------------------------
template<class TBaseClass>
class CSchemaCompoundTypeBase : public CSchemaTypeBase<TBaseClass>
{
public:
	// ISchemaType interface
	virtual uint32 GetFlags() const override;

	// ISchemaCompoundType interface
	virtual bool ReadData( void* pObject, ISchemaDataReader* pReader ) const override;
	virtual bool WriteData( const void* pObject, ISchemaDataWriter* pWriter ) const override;
	virtual void Copy( void* pDestObject, const void* pSrcObject ) const override;

	virtual const ISchemaField* FindField( const char* pName, bool bFindInParents = true ) const override;

	virtual uint32					   GetNumFields( bool bWithParents = false ) const override;
	virtual const ISchemaField*		   GetField( uint32 index, bool bWithParents = false ) const override;
	virtual const ISchemaCompoundType* GetBaseType() const override;

	CSchemaCompoundTypeBase( schemaTypeKind_t kind, const schemaStructDesc_t& structDesc );

protected:
	// ISchemaTypeInternal interface
	virtual void OnResolve() override;
	virtual void OnUnresolve() override;

	void CheckCanConstruct( bool bAllowRefCounted = true ) const;

	uint32					 GetBaseOffset() const;
	const schemaObjectOps_t* GetObjectOps() const;

private:
	const char*				   pBaseTypeName;
	const ISchemaCompoundType* pBaseType;
	ptrint					   baseOffset;
	uint32					   baseFlags;
	uint32					   numBaseFields;
	const schemaObjectOps_t*   pObjectOps;
};

#include "schemasystem/schemacompoundtype.inl"
