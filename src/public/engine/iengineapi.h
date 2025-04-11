/**
 * @file
 * @addtogroup engine engine
 */

#ifndef IENGINEAPI_H
#define IENGINEAPI_H

#include "appframework/iappsystem.h"

// Forward declarations
class CAppSystemGroup;

/**
 * @ingroup engine
 * @brief Main engine interface version to launcher 
 */
#define ENGINEAPI_INTERFACE_VERSION "SEngineAPI001"

/**
 * @ingroup engine
 * @brief Startup info
 */
struct startupInfo_t
{
	void*				pAppInstance;				/**< OS application instance */
	const achar*		pGame;						/**< Root game name ("sandbox", for example) */
	CAppSystemGroup*	pParentAppSystemGroup;		/**< Parent application system group */
};

/**
 * @ingroup engine
 * @brief Main engine interface
 */
class IEngineAPI : public IAppSystem
{
public:
	/**
	 * @brief Set startup info
	 * @note This function must be called before Init()
	 *
	 * @param info	Startup info
	 */
	virtual void SetStartupInfo( const startupInfo_t& info ) = 0;

	/**
	 * @brief Run the engine
	 * @return Return exit code. If all ok returns zero
	 */
	virtual int32 Run() = 0;
};

#endif // !IENGINEAPI_H