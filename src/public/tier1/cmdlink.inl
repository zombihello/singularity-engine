#pragma once

/*
==================
cmdLink_t::cmdLink_t
==================
*/
FORCEINLINE cmdLink_t::cmdLink_t( const char* pName, cmdFunctionFn_t pFunctionFn, const char* pDescription, uint32 flags /* = CMD_FLAG_NONE */ )
	: pNext( NULL )
	, pName( pName )
	, pDescription( pDescription )
	, pFunctionFn( pFunctionFn )
	, flags( flags )
{
	pNext						   = CCmdsLocalRegister::s_pCmdList;
	CCmdsLocalRegister::s_pCmdList = this;
}
