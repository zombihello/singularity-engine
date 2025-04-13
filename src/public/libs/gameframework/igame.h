/**
 * @file
 * @addtogroup gameframework gameframework
 */

#ifndef IGAME_H
#define IGAME_H

#include <vector>

#include "stdlib/interface.h"
#include "appframework/iappsystemgroup.h"

/**
 * @ingroup gameframework
 * @brief Game interface version
 */
#define GAME_INTERFACE_VERSION "SGame001"

/**
 * @ingroup gameframework
 * @brief Game interface
 */
class IGame
{
public:
	/**
	 * @brief Initialize the game DLL
	 * 
	 * @param pFactory	Pointer to interface factory
	 * @return Return TRUE if the game is initialized, otherwise returns FALSE
	 */
	virtual bool Init( createInterfaceFn_t pFactory ) = 0;

	/**
	 * @brief Shutdown the game DLL
	 */
	virtual void Shutdown() = 0;

	/**
	 * @brief Process one game frame
	 */
	virtual void FrameUpdate() = 0;

	/**
	 * @brief Get game description
	 * @return Return string describing current module. For example: Singularity Sandbox, Nuclear Frost, etc
	 */
	virtual const achar* GetGameDescription() const = 0;
};

/**
 * @ingroup gameframework
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
 * @ingroup gameframework
 * @brief Game application systems interface version
 */
#define GAME_APPSYSTEMS_INTERFACE_VERSION "SGameAppSystems001"

/**
 * @ingroup gameframework
 * @brief Interface exposed from the game module back to the engine for specifying IAppSystems
 */
class IGameAppSystems
{
public:
	/**
	 * @brief Get number of IAppSystems
	 * @return Return number of IAppSystems
	 */
	virtual uint32 GetNum() const = 0;

	/**
	 * @brief Get module name
	 * @param index		IAppSystem index
	 * @return Return module name
	 */
	virtual const achar* GetModuleName( uint32 index ) const = 0;

	/**
	 * @brief Get interface name
	 * @param index		IAppSystem index
	 * @return Return interface name
	 */
	virtual const achar* GetInterfaceName( uint32 index ) const = 0;
};

/**
 * @ingroup gameframework
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
	 * @param index		IAppSystem index
	 * @return Return module name
	 */
	virtual const achar* GetModuleName( uint32 index ) const override;

	/**
	 * @brief Get interface name
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
		appSystemInfo_t&				appSystemInfo = appSystems.emplace_back();
		appSystemInfo.pModuleName		= pModuleName;
		appSystemInfo.pInterfaceName	= pInterfaceName;
	}

private:
	std::vector<appSystemInfo_t>		appSystems;		/**< Array of application systems */
};

#endif // !IGAME_H