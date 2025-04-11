/**
 * @file
 * @addtogroup appframework appframework
 */

#ifndef IAPPSYSTEM_H
#define IAPPSYSTEM_H

#include "stdlib/interface.h"

/**
 * @ingroup appframework
 * @brief Application system interface
 */
class IAppSystem
{
public:
	/**
	 * @brief Connect application system
	 * 
	 * @param pFactory		Pointer to interface factory
	 * @return Return TRUE if successes application system is connected, otherwise return FALSE 
	 */
	virtual bool Connect( createInterfaceFn_t pFactory ) = 0;

	/**
	 * @brief Disconnect application system
	 */
	virtual void Disconnect() = 0;

	/**
	 * @brief Query interface
	 * 
	 * Here's where systems can access other interfaces implemented by this object
	 * 
	 * @param pInterfaceName	Interface name
	 * @return Return pointer to interface, if doesn't implement the requested interface return NULL
	 */
	virtual void* QueryInterface( const achar* pInterfaceName ) = 0;

	/**
	 * @brief Init application system
	 * @return Return TRUE if application system is inited
	 */
	virtual bool Init() = 0;

	/**
	 * @brief Shutdown application system
	 */
	virtual void Shutdown() = 0;
};

/**
 * @ingroup appframework
 * @brief Helper empty implementation of an IAppSystem
 */
template<class TInterface>
class CBaseAppSystem : public TInterface
{
public:
	/**
	 * @brief Connect application system
	 *
	 * @param pFactory		Pointer to interface factory
	 * @return Return TRUE if successes application system is connected, otherwise return FALSE
	 */
	virtual bool Connect( createInterfaceFn_t pFactory ) override { return true; }

	/**
	 * @brief Disconnect application system
	 */
	virtual void Disconnect() override {}

	/**
	 * @brief Query interface
	 * 
	 * Here's where systems can access other interfaces implemented by this object
	 *
	 * @param pInterfaceName	Interface name
	 * @return Return pointer to interface, if doesn't implement the requested interface return NULL
	 */
	virtual void* QueryInterface( const achar* pInterfaceName ) override { return nullptr; }

	/**
	 * @brief Init application system
	 * @return Return TRUE if application system is inited
	 */
	virtual bool Init() override { return true; }

	/**
	 * @brief Shutdown application system
	 */
	virtual void Shutdown() override {}
};

#endif // !IAPPSYSTEM_H