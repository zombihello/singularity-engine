/**
 * @file
 * @addtogroup game game
 */

#ifndef GAME_H
#define GAME_H

#include <vector>

#include "appframework/iappsystemgroup.h"
#include "game/igame.h"

/**
 * @ingroup game
 * @brief Base class of the game
 */
class CGame : public IGame
{
public:
	/**
	 * @brief Initialize the game DLL
	 *
	 * @param pFactory	Pointer to interface factory
	 * @return Return TRUE if the game is initialized, otherwise returns FALSE
	 */
	virtual bool Init( createInterfaceFn_t pFactory ) override;

	/**
	 * @brief Shutdown the game DLL
	 */
	virtual void Shutdown() override;

	/**
	 * @brief Process one game frame
	 */
	virtual void FrameUpdate() override;
};

/**
 * @ingroup game
 * @brief Base class of the game IAppSystems
 */
class CGameAppSystems : public IGameAppSystems
{
public:
	/**
	 * @brief Get number of IAppSystems
	 * @return Return number of IAppSystems
	 */
	virtual uint32 GetNum() const override;

	/**
	 * @brief Get module name
	 *
	 * @param index		IAppSystem index
	 * @return Return module name
	 */
	virtual const achar* GetModuleName( uint32 index ) const override;

	/**
	 * @brief Get interface name
	 *
	 * @param index		IAppSystem index
	 * @return Return interface name
	 */
	virtual const achar* GetInterfaceName( uint32 index ) const override;

protected:
	/**
	 * @brief Add app system
	 * 
	 * @param pModuleName		module_t name
	 * @param pInterfaceName	Interface name
	 */
	FORCEINLINE void AddAppSystem( const achar* pModuleName, const achar* pInterfaceName )
	{
		appSystemInfo_t&					appSystemInfo = appSystems.emplace_back();
		appSystemInfo.pModuleName		= pModuleName;
		appSystemInfo.pInterfaceName	= pInterfaceName;
	}

private:
	std::vector<appSystemInfo_t>		appSystems;		/**< Array of app systems */
};

#endif // GAME_H