/**
 * @file
 * @addtogroup shadercompiler shadercompiler
 */

#ifndef ISHADERCOMPILER_BACKEND_H
#define ISHADERCOMPILER_BACKEND_H

#include "core/types.h"
#include "stdlib/interface.h"
#include "studiorender/studioapi/istudioapi_shader.h"

/**
 * @ingroup shadercompiler
 * @brief Enumeration of flags for compile shader
 */
enum shaderCompilerFlags_t
{
	SHADERCOMPILER_NONE			= 0,		/**< No flags */
	SHADERCOMPILER_DEBUG		= 1 << 0,	/**< Debug shader version */
};

/**
 * @ingroup shadercompiler
 * @brief Interface of the environment used to compile shader
 */
class IShaderCompilerEnvironment
{
public:
	/**
	 * @brief Get compile flags
	 * @return Return compile flags (see shaderCompilerFlags_t)
	 */
	virtual uint32 GetCompileFlags() const = 0;

	/**
	 * @brief Get number of include directories
	 * @return Return number of include directories
	 */
	virtual uint32 GetNumIncludeDirs() const = 0;

	/**
	 * @brief Get number of defines
	 * @return Return number of defines
	 */
	virtual uint32 GetNumDefines() const = 0;

	/**
	 * @brief Get include directory by index
	 * 
	 * @param index			Include directory index
	 * @return Return path to include directory at index
	 */
	virtual const achar* GetIncludeDir( uint32 index ) const = 0;

	/**
	 * @brief Get define by index
	 *
	 * @param index			Define index
	 * @param pName			Output name
	 * @param pValue		Output value
	 */
	virtual void GetDefine( uint32 index, const achar*& pName, const achar*& pValue ) const = 0;
};

/**
 * @ingroup shadercompiler
 * @brief Interface of the output shader compile
 */
class IShaderCompilerOutput
{
public:
	/**
	 * @brief Set compiled bytecode of the shader
	 * 
	 * @param pData		Data
	 * @param size		Size
	 */
	virtual void SetBytecode( const byte* pData, uint64 size ) = 0;

	/**
	 * @brief Set error message
	 * @param pMsg		Error message
	 */
	virtual void SetErrorMsg( const achar* pMsg ) = 0;

	/**
	 * @brief Set reflection data
	 * @param pData		Data
	 * @param size		Size
	 */
	virtual void SetReflectionData( const byte* pData, uint64 size ) = 0;
};

/**
 * @ingroup shadercompiler
 * @brief Shader compiler backend interface version
 */
#define SHADERCOMPILERBACKEND_INTERFACE_VERSION "SShaderCompilerBackend001"

/**
 * @ingroup shadercompiler
 * @brief Interface of shader compiler backend
 */
class IShaderCompilerBackend
{
public:
	/**
	 * @brief Connect shader compiler backend
	 *
	 * @param pFactory		Pointer to interface factory
	 * @return Return TRUE if successes the backend is connected, otherwise return FALSE
	 */
	virtual bool Connect( createInterfaceFn_t pFactory ) = 0;

	/**
	 * @brief Disconnect shader compiler backend
	 */
	virtual void Disconnect() = 0;

	/**
	 * @brief Compile shader
	 *
	 * @param pSrcFileName		Path to source file of shader
	 * @param pFunctionName		Main function in the shader
	 * @param type				shader_t type (vertex, pixel, etc)
	 * @param pEnvironment		Environment of shader
	 * @param pOutput			Output data after compiling
	 * @return Return TRUE if compilation is succeed, otherwise returns FALSE
	 */
	virtual bool CompileShader( const achar* pSrcFileName, const achar* pFunctionName, studioAPIShaderType_t type, IShaderCompilerEnvironment* pEnvironment, IShaderCompilerOutput* pOutput ) = 0;

	/**
	 * @brief Get shader platform name
	 * @return Return shader platform name (dx11, vk, etc)
	 */
	virtual const achar* GetShaderPlatform() const = 0;
};

#endif // !ISHADERCOMPILER_BACKEND_H