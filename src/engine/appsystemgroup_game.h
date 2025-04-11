/**
 * @file
 * @addtogroup engine engine
 */

#ifndef APPSYSTEMGROUP_GAME_H
#define APPSYSTEMGROUP_GAME_H

#include "appframework/iappsystemgroup.h"

/**
 * @ingroup engine
 * @brief Inner loop: initialize, shutdown game systems
 */
class CGameAppSystemGroup : public CAppSystemGroup
{
public:
	/**
	 * @brief Constructor
	 * @param pParentAppSystem		Parent application system
	 */
	CGameAppSystemGroup( CAppSystemGroup* pParentAppSystem = NULL );

	/**
	 * @brief An installed application creation function,
	 *
	 * An installed application creation function, you should tell the group
	 * the DLLs and the singleton interfaces you want to instantiate.
	 *
	 * @return Return FALSE if there's any problems and the app will abort
	 */
	virtual bool Create() override;

	/**
	 * @brief Allow the application to do some work after AppSystems are connected
	 *
	 * Allow the application to do some work after AppSystems are connected but
	 * they aren't all Initialized
	 *
	 * @return Return FALSE if there's any problems and the app will abort
	 */
	virtual bool PreInit() override
	{
		return true;
	}

	/**
	 * @brief Allow the application to do some work after AppSystems are initialized
	 *
	 * Allow the application to do some work after AppSystems are initialized but
	 * before main is run
	 *
	 * @return Return FALSE if there's any problems and the app will abort
	 */
	virtual bool PostInit() override;

	/**
	 * @brief Main loop implemented by the application
	 * @return Return exit code. If all ok returns zero
	 */
	virtual int32 Main() override;

	/**
	 * @brief Allow the application to do some work before all AppSystems are shut down
	 */
	virtual void PreShutdown() override;

	/**
	 * @brief Allow the application to do some work after all AppSystems are shut down
	 */
	virtual void PostShutdown() override
	{}

	/**
	 * @brief Call an installed application destroy function, occurring after all modules are unloaded
	 */
	virtual void Destroy() override;

	/**
	 * @brief Get appsystemgroup's interface factory
	 * @return Return appsystemgroup's interface factory
	 */
	FORCEINLINE createInterfaceFn_t GetFactory() const
	{
		return CAppSystemGroup::GetFactory();
	}

private:
	/**
	 * @brief Load game dll
	 * @return Return TRUE if game dll loaded, otherwise returns FALSE
	 */
	bool GameDLL_Load();

	/**
	 * @brief Unload game dll
	 */
	void GameDLL_Unload();

	dllHandle_t				gameDLLHandle;	/**< Game DLL handle */
	createInterfaceFn_t		pGameFactory;	/**< Game interface factory */
};

#endif // !APPSYSTEMGROUP_GAME_H