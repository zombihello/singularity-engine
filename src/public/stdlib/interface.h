#pragma once
#include "stdlib/defines.h"
#include "stdlib/types.h"

//-----------------------------------------------------------------------------
// Interface system
//-----------------------------------------------------------------------------
#define CREATEINTERFACE_FUNCNAME "CreateInterface"

typedef void* ( *createInterfaceFn_t )( const char* pName );
typedef void* ( *instantiateInterfaceFn_t )();

// Used internally to register classes
struct interfaceReg_t
{
	interfaceReg_t( instantiateInterfaceFn_t pFunc, const char* pName );

	instantiateInterfaceFn_t pCreateFn;
	const char*			 pName;
	interfaceReg_t*			 pNext;
};

// Function-factory of creating interfaces
// This function is automatically exported and allows you to access any interfaces exposed with the above macros
extern "C" DLL_EXPORT void* CreateInterface( const char* pName );

// Get factory of interfaces in DLL
createInterfaceFn_t Sys_GetFactory( dllHandle_t dllHandle );

// Get factory of interfaces this module
createInterfaceFn_t Sys_GetFactoryThis();

//-----------------------------------------------------------------------------
// Helper macros to expose interfaces
//-----------------------------------------------------------------------------
#define EXPOSE_INTERFACE_FN( FunctionName, InterfaceName, VersionName ) \
	static interfaceReg_t __s_Create##InterfaceName##_reg( (instantiateInterfaceFn_t)FunctionName, VersionName );

#define EXPOSE_INTERFACE( ClassName, InterfaceName, VersionName )                                     \
	static void*		  __Create##ClassName##_interface() { return (InterfaceName*)new ClassName; } \
	static interfaceReg_t __s_Create##InterfaceName##_reg( __Create##ClassName##_interface, VersionName );

#define EXPOSE_SINGLE_INTERFACE_GLOBALVAR( ClassName, InterfaceName, VersionName, GlobalVarName )      \
	static void*		  __Create##ClassName##_interface() { return (InterfaceName*)&GlobalVarName; } \
	static interfaceReg_t __s_Create##InterfaceName##_reg( __Create##ClassName##_interface, VersionName );

#define EXPOSE_SINGLE_INTERFACE( ClassName, InterfaceName, VersionName ) \
	static ClassName __s_##ClassName##_singleton;                        \
	EXPOSE_SINGLE_INTERFACE_GLOBALVAR( ClassName, InterfaceName, VersionName, __s_##ClassName##_singleton )