/**
 * @file
 * @addtogroup shadercompiler shadercompiler
 */

#ifndef SHADERCOMPILE_H
#define SHADERCOMPILE_H

#include <string>
#include <vector>

#include "core/types.h"
#include "core/memory.h"

/**
 * @ingroup shadercompiler
 * @brief Shader flag
 */
struct shaderFlag_t
{
	/**
	 * @brief Constructor
	 */
	shaderFlag_t()
		: minValue( 0 )
		, maxValue( 0 )
		, bSetDefault( false )
		, defaultValue( 0 )
	{}

	std::string		name;			/**< Flag name */
	int32			minValue;		/**< Minimum value */
	int32			maxValue;		/**< Maximum value */
	bool			bSetDefault;	/**< Is default value set */
	int32			defaultValue;	/**< Default value */
};

/**
 * @ingroup shadercompiler
 * @brief Shader define
 */
struct shaderDefine_t
{
	std::string		name;	/**< Define name */
	std::string		value;	/**< Define value */
};

/**
 * @ingroup shadercompiler
 * @brief Information about shader
 */
struct shader_t
{
	/**
	 * @brief Calculate maximum number of flag combinations
	 */
	void CalcNumFlagCombos();

	uint32						numFlagCombos;	/**< Maximum number of flag combinations */
	studioAPIShaderType_t		type;			/**< Shader type */
	std::string					source;			/**< Path to shader source file */
	std::string					mainFunc;		/**< Main function */
	std::vector<shaderFlag_t>	flags;			/**< Shader flags */
	std::vector<shaderDefine_t>	defines;		/**< Shader defines */
};

/**
 * @ingroup shadercompiler
 * @brief Convert string to shader type
 * @param pShaderTypeName	Shader type in string view
 * @param shaderType		Converted shader type
 * @return Return TRUE if converted, otherwise returns FALSE
 */
bool ConvStringToShaderType( const achar* pShaderTypeName, studioAPIShaderType_t& shaderType );

/**
 * @ingroup shadercompiler
 * @brief Convert shader type to string
 * @param shaderType		Shader type
 * @param pShaderTypeName	Converted shader type into string view (DO NOT edit!)
 */
void ConvShaderTypeToString( studioAPIShaderType_t shaderType, const achar*& pShaderTypeName );

#endif // !SHADERCOMPILE_H