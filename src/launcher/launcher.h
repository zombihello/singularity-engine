/**
 * @file
 * @addtogroup launcher launcher
 */

#ifndef LAUNCHER_H
#define LAUNCHER_H

#include "appframework/iappsystemgroup.h"
#include "appframework/iwindowmgr.h"
#include "engine/iengineapi.h"

/**
 * @ingroup launcher
 * @brief Initialize of launcher's log output
 */
void Launcher_InitLogOutput();

/**
 * @ingroup launcher
 * @brief Singularity Engine application
 */
class CSingularityEngineApp : public CDefaultAppSystemGroup<CAppSystemGroup>
{
public:
	/**
	 * @brief Constructor
	 * @param pHInstance	Application instance handle
	 */
	CSingularityEngineApp( appInstanceHandle_t hInstance = nullptr );

	/**
	 * @brief An installed application creation function, you should tell the group
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
	virtual bool PreInit() override;

	/**
	 * @brief Main loop implemented by the application
	 * @return Return exit code. If all ok returns zero
	 */
	virtual int32 Main() override;

	/**
	 * @brief Allow the application to do some work after all AppSystems are shut down
	 */
	virtual void PostShutdown() override;

private:
	appInstanceHandle_t		hInstance;			/**< Application instance handle */
	IWindowMgr*				pWindowMgr;			/**< Window manager */
	IEngineAPI*				pEngineAPI;			/**< Engine API */
};

#endif // !LAUNCHER_H