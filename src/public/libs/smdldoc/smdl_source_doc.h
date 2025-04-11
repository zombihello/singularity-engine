/**
 * @file
 * @addtogroup smdldoc smdldoc
 */

#ifndef SMDL_SOURCE_DOC_H
#define SMDL_SOURCE_DOC_H

#include "core/core.h"

/**
 * @ingroup smdldoc
 * @brief Helper for work with source SMDL (Singularity Model) files
 */
class CSMDLSourceModelDoc
{
public:
	/**
	 * @brief Constructor
	 */
	CSMDLSourceModelDoc();

	/**
	 * @brief Load from a file
	 * @warning For use StdLib must be connected by ConnectStdLib
	 *
	 * @param pPath		Path to file
	 * @return Return TRUE when a file success loaded, otherwise return FALSE
	 */
	bool LoadFromFile( const achar* pPath );

	/**
	 * @brief Save the texture document
	 * @warning For use StdLib must be connected by ConnectStdLib
	 *
	 * @param pPath		Path to save
	 * @return Return TRUE if the texture was successes saved, otherwise return FALSE
	 */
	bool SaveFile( const achar* pPath );

	/**
	 * @brief Set axis up
	 * @param axisUp	Axis up
	 */
	FORCEINLINE void SetAxisUp( axisUp_t axisUp )
	{
		CSMDLSourceModelDoc::axisUp = axisUp;
	}

	/**
	 * @brief Get axis up
	 * @return Return axis up. If isn't set return AXIS_UP_DEFAULT
	 */
	FORCEINLINE axisUp_t GetAxisUp() const
	{
		return axisUp;
	}

	/**
	 * @brief Set source path
	 * @param pSourcePath	Source path
	 */
	FORCEINLINE void SetSourcePath( const achar* pSourcePath )
	{
		CSMDLSourceModelDoc::sourcePath = pSourcePath;
	}

	/**
	 * @brief Get source path
	 * @return Return source path. If isn't set return empty string
	 */
	FORCEINLINE const std::string& GetSourcePath() const
	{
		return sourcePath;
	}

	/**
	 * @brief Set materials directory
	 * @param pMaterialsDir		Path to a directory where the model materials should be located
	 */
	FORCEINLINE void SetMaterialsDir( const achar* pMaterialsDir )
	{
		CSMDLSourceModelDoc::materialsDir = pMaterialsDir;
	}

	/**
	 * @brief Get materials directory
	 * @return Return materials directory. If isn't set return 'materials/'
	 */
	FORCEINLINE const std::string& GetMaterialsDir() const
	{
		return materialsDir;
	}

	/**
	 * @brief Set output directory
	 * @param pOutputDir	Path to an output directory
	 */
	FORCEINLINE void SetOutputDir( const achar* pOutputDir )
	{
		CSMDLSourceModelDoc::outputDir = pOutputDir;
	}

	/**
	 * @brief Get output directory
	 * @return Return output directory. If isn't set return empty string
	 */
	FORCEINLINE const std::string& GetOutputDir() const
	{
		return outputDir;
	}

	/**
	 * @brief Add renamed material
	 * @param pOriginalName		Original material name
	 * @param pNewName			New material name
	 */
	FORCEINLINE void AddRenamedMaterial( const achar* pOriginalName, const achar* pNewName )
	{
		renamedMaterialsDict[pOriginalName] = pNewName;
	}

	/**
	 * @brief Remove renamed material
	 * @param pOriginalName		Original material name for which was added renamed name
	 */
	FORCEINLINE void RemoveRenamedMaterial( const achar* pOriginalName )
	{
		renamedMaterialsDict.erase( pOriginalName );
	}

	/**
	 * @brief Remove all renamed materials
	 */
	FORCEINLINE void RemoveAllRenamedMaterials()
	{
		renamedMaterialsDict.clear();
	}

	/**
	 * @brief Get renamed materials dictionary
	 * @return Return renamed materials dictionary. If aren't set return empty dictionary
	 */
	FORCEINLINE const std::unordered_map<std::string, std::string>& GetRenamedMaterials() const
	{
		return renamedMaterialsDict;
	}

	/**
	 * @brief Set combine models
	 * @param bCombineModels	If it is TRUE, combines all models into a single model
	 */
	FORCEINLINE void SetCombineModels( bool bCombineModels )
	{
		CSMDLSourceModelDoc::bCombineModels = bCombineModels;
	}

	/**
	 * @brief Is combine models
	 * @return Return TRUE if need combine models, otherwise FALSE
	 */
	FORCEINLINE bool IsCombineModels()  const
	{
		return bCombineModels;
	}

	/**
	 * @brief Clear the source model
	 */
	FORCEINLINE void Clear()
	{
		bCombineModels	= false;
		axisUp			= AXIS_UP_DEFAULT;
		sourcePath		= "";
		materialsDir	= "materials/";
		outputDir		= "";
		renamedMaterialsDict.clear();
	}

private:
	bool											bCombineModels;			/**< If it is TRUE, combines all models into a single model */
	axisUp_t										axisUp;					/**< Axis up */
	std::string										sourcePath;				/**< Source path */
	std::string										materialsDir;			/**< Path to a directory where the model materials should be located */
	std::string										outputDir;				/**< Path to an output directory */
	std::unordered_map<std::string, std::string>	renamedMaterialsDict;	/**< Dictionary of renamed materials */
};

#endif // !SMDL_SOURCE_DOC_H