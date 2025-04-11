/**
 * @file
 * @addtogroup stdlib stdlib
 */

#ifndef INTERFACE_H
#define INTERFACE_H

#include "core/types.h"
#include "core/platform.h"

/**
 * @ingroup stdlib
 * @brief Macro function name of create interface
 */
#define CREATEINTERFACE_FUNCNAME		"CreateInterface"

/**
 * @ingroup stdlib
 * @brief Type pointer to create interface function
 */
typedef void* ( *createInterfaceFn_t )( const achar* pName );

/**
 * @ingroup stdlib
 * @brief Type pointer to instantiate interface function
 */
typedef void* ( *instantiateInterfaceFn_t )();

/**
 * @ingroup stdlib
 * @brief Used internally to register classes
 */
struct interfaceReg_t
{
	/**
	 * @brief Constructor
	 * 
	 * @param pFunc	Pointer to instantiate interface function
	 * @param pName	Interface name
	 */
	interfaceReg_t( instantiateInterfaceFn_t pFunc, const achar* pName );

	instantiateInterfaceFn_t		pCreateFn;		/**< Pointer to instantiate interface function */
	const achar*					pName;			/**< Interface name */
	interfaceReg_t*					pNext;			/**< Pointer to next registered interface. Need for global list */
};

/**
 * @ingroup stdlib
 * @brief Macro for expose interface with custom instantiate function
 * 
 * @param FunctionName		Function name
 * @param InterfaceName		Interface name
 * @param VersionName		Version name
 */
#define EXPOSE_INTERFACE_FN( FunctionName, InterfaceName, VersionName ) \
	static interfaceReg_t		__s_Create##InterfaceName##_reg( FunctionName, VersionName );

/**
 * @ingroup stdlib
 * @brief Macro for expose interface
 * 
 * @param ClassName			Class name
 * @param InterfaceName		Interface name
 * @param VersionName		Version name
 */
#define EXPOSE_INTERFACE( ClassName, InterfaceName, VersionName ) \
	static void*				__Create##ClassName##_interface() { return ( InterfaceName* )new ClassName; } \
	static interfaceReg_t		__s_Create##InterfaceName##_reg( __Create##ClassName##_interface, VersionName );

/**
 * @ingroup stdlib
 * @brief Macro for expose interface with global variable
 * 
 * @param ClassName			Class name
 * @param InterfaceName		Interface name
 * @param VersionName		Version name
 * @param GlobalVarName		Global variable name
 */
#define EXPOSE_SINGLE_INTERFACE_GLOBALVAR( ClassName, InterfaceName, VersionName, GlobalVarName ) \
	static void*				__Create##ClassName##_interface() { return ( InterfaceName* )&GlobalVarName; } \
	static interfaceReg_t		__s_Create##InterfaceName##_reg( __Create##ClassName##_interface, VersionName );

/**
 * @ingroup stdlib
 * @brief Macro for expose single interface
 * 
 * @param ClassName			Class name
 * @param InterfaceName		Interface name
 * @param VersionName		Version name
 */
#define EXPOSE_SINGLE_INTERFACE( ClassName, InterfaceName, VersionName ) \
	static ClassName		__s_##ClassName##_singleton; \
	EXPOSE_SINGLE_INTERFACE_GLOBALVAR( ClassName, InterfaceName, VersionName, __s_##ClassName##_singleton )

/**
 * @ingroup stdlib
 * @brief Function-factory of creating interfaces
 * 
 * This function is automatically exported and allows you to access any interfaces exposed with the above macros
 * 
 * @param pName		Interface name
 * @return Return pointer to created interface. If interfaces not found return NULL
 */
extern "C" DLL_EXPORT void* CreateInterface( const achar* pName );

/**
 * @ingroup stdlib
 * @brief Get factory of interfaces in DLL
 * 
 * @param dllHandle		DLL handle
 * @return Return pointer to function of interface factory in DLL. If fail return NULL
 */
createInterfaceFn_t Sys_GetFactory( dllHandle_t dllHandle );

/**
 * @ingroup stdlib
 * @brief Get factory of interfaces this module
 * @return Return pointer to function of interface factory this module
 */
createInterfaceFn_t Sys_GetFactoryThis();

#endif // !INTERFACE_H