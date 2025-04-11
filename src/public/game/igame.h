/**
 * @file
 * @addtogroup game game
 */

#ifndef IGAME_H
#define IGAME_H

#include "stdlib/interface.h"

/**
 * @ingroup game
 * @brief Game interface version
 */
#define GAME_INTERFACE_VERSION "SGame001"

/**
 * @ingroup game
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
 * @ingroup game
 * @brief Game IAppSystems interface version
 */
#define GAME_APPSYSTEMS_INTERFACE_VERSION "SGameAppSystems001"

/**
 * @ingroup game
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
	 * 
	 * @param index		IAppSystem index
	 * @return Return module name
	 */
	virtual const achar* GetModuleName( uint32 index ) const = 0;

	/**
	 * @brief Get interface name
	 * 
	 * @param index		IAppSystem index
	 * @return Return interface name
	 */
	virtual const achar* GetInterfaceName( uint32 index ) const = 0;
};

#endif // !IGAME_H