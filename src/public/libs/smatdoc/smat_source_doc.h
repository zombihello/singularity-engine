/**
 * @file
 * @addtogroup smatdoc smatdoc
 */

#ifndef SMAT_SOURCE_DOC_H
#define SMAT_SOURCE_DOC_H

#include <string>
#include <vector>

#include "core/core.h"
#include "stdlib/jsondoc.h"
#include "smatdoc/smatvar.h"

/**
 * @ingroup smatdoc
 * @brief Helper for work with source SMAT (Singularity Material) files
 */
class CSMATSourceMaterialDoc
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
	 * @brief Load from a buffer
	 * @param pBuffer	Buffer. The buffer must be null terminated
	 * @return Return TRUE if the buffer successfully loaded, otherwise returns FALSE
	 */
	bool LoadFromBuffer( const achar* pBuffer );

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
		outputDir	= "";
		shaderName	= "";
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

	/**
	 * @brief Set path to an output directory
	 * @param pOutputDir	Path to an output directory
	 */
	FORCEINLINE void SetOutputDir( const achar* pOutputDir )
	{
		outputDir = pOutputDir;
	}

	/**
	 * @brief Get path to an output directory
	 * @return Return path to an output directory
	 */
	FORCEINLINE const achar* GetOutputDir() const
	{
		return outputDir.c_str();
	}

private:
	/**
	 * @brief Grab a data from a JSON file
	 * @param jsonDoc		JSON document
	 * @return Return TRUE if successfully got data from the JSON file, otherwise returns FALSE
	 */
	bool GrabData( const CJsonDoc& jsonDoc );

	/**
	 * @brief Grab a parameter value as number type
	 * @param jsonValue		JSON value
	 * @param value			Output value
	 * @return Return TRUE if successfully got the value, otherwise FALSE
	 */
	bool GrabValueAsNumber( const CJsonValue& jsonValue, float& value ) const;

	/**
	 * @brief Grab a parameter value as vector 2D type
	 * @param jsonValue		JSON value
	 * @param value			Output value
	 * @return Return TRUE if successfully got the value, otherwise FALSE
	 */
	bool GrabValueAsVec2( const CJsonValue& jsonValue, vec2_t& value ) const;

	/**
	 * @brief Grab a parameter value as vector 3D type
	 * @param jsonValue		JSON value
	 * @param value			Output value
	 * @return Return TRUE if successfully got the value, otherwise FALSE
	 */
	bool GrabValueAsVec3( const CJsonValue& jsonValue, vec3_t& value ) const;

	/**
	 * @brief Grab a parameter value as vector 4D type
	 * @param jsonValue		JSON value
	 * @param value			Output value
	 * @return Return TRUE if successfully got the value, otherwise FALSE
	 */
	bool GrabValueAsVec4( const CJsonValue& jsonValue, vec4_t& value ) const;

	/**
	 * @brief Grab a parameter value as matrix type
	 * @param jsonValue		JSON value
	 * @param value			Output value
	 * @return Return TRUE if successfully got the value, otherwise FALSE
	 */
	bool GrabValueAsMatrix( const CJsonValue& jsonValue, matrix_t& value ) const;

	/**
	 * @brief Grab a parameter value as string type
	 * @param jsonValue		JSON value
	 * @param value			Output value
	 * @return Return TRUE if successfully got the value, otherwise FALSE
	 */
	bool GrabValueAsString( const CJsonValue& jsonValue, std::string& value ) const;

	std::string						outputDir;		/**< Path to an output directory */
	std::string						shaderName;		/**< Shader name */
	std::vector<CSMATMaterialVar>	vars;			/**< Material variables */
};

#endif // !SMAT_SOURCE_DOC_H