/**
 * @file
 * @addtogroup shadercompiler shadercompiler
 */

#ifndef SHADERCOMPILER_ENVIRONMENT_H
#define SHADERCOMPILER_ENVIRONMENT_H

#include <string>
#include <vector>
#include <unordered_map>

#include "tools/shadercompiler/ishadercompiler_backend.h"

/**
 * @ingroup shadercompiler
 * @brief The environment used to compile shader
 */
class CShaderCompilerEnvironment : public IShaderCompilerEnvironment
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param type	shader_t type
	 */
	CShaderCompilerEnvironment( studioAPIShaderType_t type );

	/**
	 * @brief Copy constructor
	 * @param copy		Copy
	 */
	CShaderCompilerEnvironment( const CShaderCompilerEnvironment& copy );

	/**
	 * @brief Add include dir
	 * @param pPath		Path to directory where shader compiler will find include files
	 */
	FORCEINLINE void AddIncludeDir( const achar* pPath )
	{
		includeDirs.push_back( pPath );
	}

	/**
	 * @brief Add define
	 *
	 * Example usage: @code AddDefine( "MYDEF", "1" ); @endcode
	 *
	 * @param pName		Name
	 * @param pValue	Value
	 */
	FORCEINLINE void AddDefine( const achar* pName, const achar* pValue )
	{
		defines[pName] = pValue;
	}

	/**
	 * @brief Set compile flags
	 * @param flags		Compiler flags (see shaderCompilerFlags_t)
	 */
	FORCEINLINE void SetCompileFlags( uint32 flags )
	{
		compileFlags = flags;
	}

	/**
	 * @brief Get compile flags
	 * @return Return compile flags (see shaderCompilerFlags_t)
	 */
	virtual uint32 GetCompileFlags() const override;

	/**
	 * @brief Get number of include directories
	 * @return Return number of include directories
	 */
	virtual uint32 GetNumIncludeDirs() const override;

	/**
	 * @brief Get number of defines
	 * @return Return number of defines
	 */
	virtual uint32 GetNumDefines() const override;

	/**
	 * @brief Get include directory by index
	 *
	 * @param index			Include directory index
	 * @return Return path to include directory at index
	 */
	virtual const achar* GetIncludeDir( uint32 index ) const override;

	/**
	 * @brief Get define by index
	 *
	 * @param index			Define index
	 * @param pName			Output name
	 * @param pValue		Output value
	 */
	virtual void GetDefine( uint32 index, const achar*& pName, const achar*& pValue ) const override;

private:
	std::vector<std::string>						includeDirs;			/**< Include directories */
	std::unordered_map<std::string, std::string>	defines;				/**< Defines for compiling shader */
	uint32											compileFlags;			/**< Compile flags (see shaderCompilerFlags_t) */
};

#endif // !SHADERCOMPILER_ENVIRONMENT_H