#include "pch_schemalib.h"
#include "schemasystem/schemalib/schemalink.h"

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
schemaTypeLink_t* CSchemasLocalRegister::s_pTypeList = NULL;
schemaScopeId_t	  CSchemasLocalRegister::s_scopeId	 = INVALID_INDEX;

/*
==================
schemaTypeLink_t::schemaTypeLink_t
==================
*/
schemaTypeLink_t::schemaTypeLink_t( schemaRegisterFn_t pRegisterFn )
	: pNext( CSchemasLocalRegister::s_pTypeList )
	, pRegisterFn( pRegisterFn )
{
	CSchemasLocalRegister::s_pTypeList = this;
}

/*
==================
LinkSchemas
==================
*/
void LinkSchemas( const char* pScopeName )
{
	if ( !g_pSchemaSystem || CSchemasLocalRegister::s_scopeId != INVALID_INDEX )
	{
		return;
	}

	CSchemasLocalRegister::s_scopeId = g_pSchemaSystem->OpenScope( pScopeName );
	for ( schemaTypeLink_t* pCurLink = CSchemasLocalRegister::s_pTypeList; pCurLink; pCurLink = pCurLink->pNext )
	{
		pCurLink->pRegisterFn( CSchemasLocalRegister::s_scopeId );
	}
}

/*
==================
UnlinkSchemas
==================
*/
void UnlinkSchemas()
{
	if ( !g_pSchemaSystem || CSchemasLocalRegister::s_scopeId == INVALID_INDEX )
	{
		return;
	}

	g_pSchemaSystem->CloseScope( CSchemasLocalRegister::s_scopeId );
	CSchemasLocalRegister::s_scopeId = INVALID_INDEX;
}
