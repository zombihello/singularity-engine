/**
 * @file
 * @addtogroup materialsystem materialsystem
 */

#ifndef MATERIALSYSTEM_H
#define MATERIALSYSTEM_H

#include "materialsystem/imaterialsystem.h"
#include "materialsystem/texture_factory.h"
#include "materialsystem/material_factory.h"

/**
 * @ingroup materialsystem
 * @brief Application systems factory. It used for connect materialsystem's submodules (e.g: stdshaders)
 */
extern createInterfaceFn_t	g_pAppSystemFactory;

/**
 * @ingroup materialsystem
 * @brief Material system
 */
class CMaterialSystem : public CBaseAppSystem<IMaterialSystem>
{
public:
	/**
	 * @brief Connect application system
	 *
	 * @param pFactory		Pointer to interface factory
	 * @return Return TRUE if successes application system is connected, otherwise return FALSE
	 */
	virtual bool Connect( createInterfaceFn_t pFactory ) override;

	/**
	 * @brief Disconnect application system
	 */
	virtual void Disconnect() override;

	/**
	 * @brief Query interface
	 *
	 * Here's where systems can access other interfaces implemented by this object
	 *
	 * @param pInterfaceName	Interface name
	 * @return Return pointer to interface, if doesn't implement the requested interface return NULL
	 */
	virtual void* QueryInterface( const achar* pInterfaceName ) override;

	/**
	 * @brief Init application system
	 * @return Return TRUE if application system is inited
	 */
	virtual bool Init() override;

	/**
	 * @brief Shutdown application system
	 */
	virtual void Shutdown() override;

private:
	CTextureFactory			textureFactory;		/**< Texture factory */
	CMaterialFactory		materialFactory;	/**< Material factory */
};

#endif // !MATERIALSYSTEM_H