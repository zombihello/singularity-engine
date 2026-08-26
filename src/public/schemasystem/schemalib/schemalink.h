#pragma once
#include "schemasystem/ischemasystem.h"

//-----------------------------------------------------------------------------
// `schemaTypeLink_t` is a convenient way to get a described type registered in the
// schema system without having to add an explicit call somewhere in a startup
// function. The `BEGIN_SCHEMA_*` macros declare a static variable with the
// parameters and it will get executed before `main()`
//-----------------------------------------------------------------------------
struct schemaTypeLink_t
{
	// Registers one described type of a local module
	using schemaRegisterFn_t = void ( * )( schemaScopeId_t scopeId );

	schemaTypeLink_t( schemaRegisterFn_t pRegisterFn );

	schemaTypeLink_t*  pNext;
	schemaRegisterFn_t pRegisterFn;
};

//-----------------------------------------------------------------------------
// Register described types implemented in a local module
//-----------------------------------------------------------------------------
class CSchemasLocalRegister
{
	friend struct schemaTypeLink_t;
	friend void LinkSchemas( const char* pScopeName );
	friend void UnlinkSchemas();

protected:
	static schemaTypeLink_t* s_pTypeList;  // Statically constructed list of `schemaTypeLink_t`, used to register them in the schema system
	static schemaScopeId_t	 s_scopeId;
};

//-----------------------------------------------------------------------------
// Link and unlink the described types of a local module
//
// NOTES:
//  * Call them only once `ConnectTier2` has given us the schema system
//	* Unlinking closes the scope, which is refused while another module
//	  still names a type of it - so a module has to unlink AFTER everything
//	  built on top of it did
//-----------------------------------------------------------------------------
void LinkSchemas( const char* pScopeName );
void UnlinkSchemas();
