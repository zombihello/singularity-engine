#include "pch_stdlib.h"
#include "stdlib/interface.h"

/**
 * @ingroup stdlib
 * @brief Global list of registered interfaces
 */
static interfaceReg_t*	s_pInterfaceRegs = nullptr;

/*
==================
SInterfaceReg::SInterfaceReg
==================
*/
interfaceReg_t::interfaceReg_t( instantiateInterfaceFn_t pFunc, const achar* pName )
	: pCreateFn( pFunc )
	, pName( pName )
	, pNext( nullptr )
{
	pNext = s_pInterfaceRegs;
	s_pInterfaceRegs = this;
}

/*
==================
CreateInterface
==================
*/
void* CreateInterface( const achar* pName )
{
	for ( interfaceReg_t* pInterfaceReg = s_pInterfaceRegs; pInterfaceReg; pInterfaceReg = pInterfaceReg->pNext )
	{
		if ( !S_Strcmp( pInterfaceReg->pName, pName ) )
		{
			return pInterfaceReg->pCreateFn();
		}
	}
	return nullptr;
}

/*
==================
Sys_GetFactory
==================
*/
createInterfaceFn_t Sys_GetFactory( dllHandle_t dllHandle )
{
	if ( dllHandle )
	{
		return ( createInterfaceFn_t )Sys_DLL_GetProcAddress( dllHandle, CREATEINTERFACE_FUNCNAME );
	}
	return NULL;
}

/*
==================
Sys_GetFactoryThis
==================
*/
createInterfaceFn_t Sys_GetFactoryThis()
{
	return &CreateInterface;
}