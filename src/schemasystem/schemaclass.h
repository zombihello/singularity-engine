#pragma once
#include "schemasystem/ischemaclass.h"
#include "schemasystem/schemacompoundtype.h"

//-----------------------------------------------------------------------------
// A described class
//-----------------------------------------------------------------------------
class CSchemaClass : public CSchemaCompoundTypeBase<ISchemaClass>
{
public:
	// ISchemaType interface
	virtual bool IsA( const ISchemaType* pType ) const override;

	// ISchemaCompoundType interface
	virtual void* Cast( void* pObject, const ISchemaType* pType ) const override;

	// ISchemaClass interface
	virtual ISchemaObject* Construct( void* pMemory ) const override;
	virtual void		   Destruct( void* pObject ) const override;
	virtual ISchemaObject* New() const override;
	virtual void		   Delete( ISchemaObject* pObject ) const override;

	virtual void* Cast( ISchemaObject* pObject, const ISchemaType* pType ) const override;

	virtual uint32					GetNumInterfaces( bool bWithParents = false ) const override;
	virtual const ISchemaInterface* GetInterface( uint32 index, bool bWithParents = false ) const override;

	CSchemaClass( const schemaClassDesc_t& classDesc );

	ptrint GetObjectOffset() const;	 // The pointer adjustment from this class to its `ISchemaObject` subobject

protected:
	// ISchemaTypeInternal interface
	virtual void OnResolve() override;
	virtual void OnUnresolve() override;

private:
	struct implementedInterface_t
	{
		const char*				pTypeName;
		ptrint					offset;
		const ISchemaInterface* pInterface;
	};

	ptrint								  objectOffset;
	uint32								  numBaseInterfaces;
	eastl::vector<implementedInterface_t> interfaces;
};

#include "schemasystem/schemaclass.inl"
