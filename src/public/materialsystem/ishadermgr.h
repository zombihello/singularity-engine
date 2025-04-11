/**
 * @file
 * @addtogroup materialsystem materialsystem
 */

#ifndef ISHADERMGR_H
#define ISHADERMGR_H

#include "studiorender/studioapi/istudioapi_shader.h"

// Forward declarations
class IShader;

/**
 * @ingroup materialsystem
 * @brief Shader manager interface version
 */
#define SHADERMGR_INTERFACE_VERSION "SShaderMgr001"

/**
 * @ingroup materialsystem
 * @brief Shader manager interface
 */
class IShaderMgr
{
public:
	/**
	 * @brief Initialize
	 * This function initialize the manager and loads all engine shader DLLs
	 */
	virtual void Init() = 0;

	/**
	 * @brief Shutdown
	 * This function shutdown the manager and unloads all engine shader DLLs
	 */
	virtual void Shutdown() = 0;

	/**
	 * @brief Initialize for the game
	 * This function loads all game specific shader DLLs
	 */
	virtual void GameInit() = 0;

	/**
	 * @brief Shutdown for the game
	 * This function unloads all game specific shader DLLs
	 */
	virtual void GameShutdown() = 0;

	/**
	 * @brief Find a shader by name
	 * @param pShaderName		Shader name
	 * @return Return the shader instance, if not found return NULL
	 */
	virtual IShader* FindShader( const achar* pShaderName ) const = 0;

	/**
	 * @brief Get a StudioAPI shader
	 * @param shaderLibIndex	Shader library index
	 * @param shaderType		Shader type
	 * @param shaderIndex		Shader index
	 * @return Return the StudioAPI shader. If isn't return NULL
	 */
	virtual IStudioAPIShader* GetStudioAPIShader( uint32 shaderLibIndex, studioAPIShaderType_t shaderType, uint64 shaderIndex ) const = 0;
};

/**
 * @ingroup materialsystem
 * @brief Shader library interface version
 */
#define SHADERLIB_INTERFACE_VERSION "SShaderLib001"

/**
 * @ingroup materialsystem
 * @brief Shader library interface
 */
class IShaderLib
{
public:
	/**
	 * @brief Connect the shader library
	 *
	 * @param pFactory		Pointer to interface factory
	 * @return Return TRUE if successes the library is connected, otherwise return FALSE
	 */
	virtual bool Connect( createInterfaceFn_t pFactory ) = 0;

	/**
	 * @brief Disconnect the shader library
	 */
	virtual void Disconnect() = 0;

	/**
	 * @brief Initialize the shader library
	 * @param shaderLibIndex	Shader library index in the shader manager
	 */
	virtual void Init( uint32 shaderLibIndex ) = 0;

	/**
	 * @brief Shutdown the shader library
	 */
	virtual void Shutdown() = 0;

	/**
	 * @brief Get shader library name
	 * @return Return shader library name
	 */
	virtual const achar* GetName() const = 0;

	/**
	 * @brief Get number of shaders in this library
	 * @return Return number of shaders in this library
	 */
	virtual uint32 GetNumShaders() const = 0;

	/**
	 * @brief Get shader by index
	 * @param index		Index
	 * @return Return shader at index
	 */
	virtual IShader* GetShader( uint32 index ) const = 0;

	/**
	 * @brief Get the shader library index
	 * @return Return the shader library index in the shader manager
	 */
	virtual uint32 GetIndex() const = 0;
};

#endif // !ISHADERMGR_H