#pragma once
#include "schemasystem/ischematype.h"

//-----------------------------------------------------------------------------
// A described interface
//
// NOTES:
//	* The base of an interface is always an interface
//	* An interface has no layout of its own - it is never constructed and owns no
//	  fields, so there is nothing to cast FROM one: reaching an interface goes
//	  through the class that implements it, `ISchemaClass::Cast`
//-----------------------------------------------------------------------------
class ISchemaInterface : public ISchemaType
{
public:
	// The interface this one derives from, NULL when there is none
	virtual const ISchemaInterface* GetBaseInterface() const = 0;
};
