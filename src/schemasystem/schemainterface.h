#pragma once
#include "schemasystem/ischemainterface.h"
#include "schemasystem/schematypebase.h"

//-----------------------------------------------------------------------------
// A described interface
//-----------------------------------------------------------------------------
class CSchemaInterface : public CSchemaTypeBase<ISchemaInterface>
{
public:
	// ISchemaType interface
	virtual uint32 GetFlags() const override;
	virtual bool   IsA( const ISchemaType* pType ) const override;

	// ISchemaInterface interface
	virtual const ISchemaInterface* GetBaseInterface() const override;

	CSchemaInterface( const schemaInterfaceDesc_t& interfaceDesc );

protected:
	// ISchemaTypeInternal interface
	virtual void OnResolve() override;
	virtual void OnUnresolve() override;

private:
	uint32					baseFlags;
	const char*				pBaseTypeName;
	const ISchemaInterface* pBaseInterface;
};
