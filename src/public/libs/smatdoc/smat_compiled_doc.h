/**
 * @file
 * @addtogroup smatdoc smatdoc
 */

#ifndef SMAT_COMPILED_DOC_H
#define SMAT_COMPILED_DOC_H

#include <string>
#include <vector>

#include "core/core.h"
#include "smatdoc/smatvar.h"

/**
 * @ingroup smatdoc
 * @brief Helper for work with compiled SMAT (Singularity Material) files
 */
class CSMATCompiledMaterialDoc
{
public:
	/**
	 * @brief Save the material document
	 * @warning For use StdLib must be connected by ConnectStdLib
	 *
	 * @param pPath		Path to save
	 * @return Return TRUE if the material was successes saved, otherwise return FALSE
	 */
	bool SaveFile( const achar* pPath );

	/**
	 * @brief Load from a file
	 * @warning For use StdLib must be connected by ConnectStdLib
	 *
	 * @param pPath		Path to file
	 * @return Return TRUE when the file success loaded, otherwise return FALSE
	 */
	bool LoadFromFile( const achar* pPath );

	/**
	 * @brief Set shader name
	 * @param pShaderName	Shader name
	 */
	FORCEINLINE void SetShaderName( const achar* pShaderName )
	{
		shaderName = pShaderName;
	}

	/**
	 * @brief Add variable
	 * @param var	Material variable
	 */
	FORCEINLINE void AddVar( const CSMATMaterialVar& var )
	{
		vars.emplace_back( var );
	}

	/**
	 * @brief Remove variable by index
	 * @param index		Index
	 */
	FORCEINLINE void RemoveVar( uint32 index )
	{
		Assert( index < vars.size() );
		vars.erase( vars.begin() + index );
	}

	/**
	 * @brief Clear material document
	 */
	FORCEINLINE void Clear()
	{
		shaderName = "";
		vars.clear();
	}

	/**
	 * @brief Get number of material variables
	 * @return Return number of material variables
	 */
	FORCEINLINE uint32 GetNumVars() const
	{
		return ( uint32 )vars.size();
	}

	/**
	 * @brief Get array of material variables
	 * @return Return array of material variables
	 */
	FORCEINLINE const std::vector<CSMATMaterialVar>& GetVars() const
	{
		return vars;
	}

	/**
	 * @brief Get shader name
	 * @return Return shader name. If isn't set return empty string
	 */
	FORCEINLINE const achar* GetShaderName() const
	{
		return shaderName.c_str();
	}

private:
	std::string						shaderName;		/**< Shader name */
	std::vector<CSMATMaterialVar>	vars;			/**< Material variables */
};

#endif // !SMAT_COMPILED_DOC_H