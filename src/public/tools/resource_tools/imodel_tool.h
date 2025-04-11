/**
 * @file
 * @addtogroup resource_tools resource_tools
 */

#ifndef IMODEL_TOOL_H
#define IMODEL_TOOL_H

#include "appframework/iappsystem.h"
#include "stdlib/math/axis.h"

/**
 * @ingroup resource_tools
 * @brief Model tool interface version
 */
#define MODEL_TOOL_INTERFACE_VERSION "SModelTool001"

/**
 * @ingroup resource_tools
 * @brief Renamed material information
 */
struct resourceToolRenamedMaterial_t
{
	const achar*	pOriginalName;		/**< Original material name */
	const achar*	pNewName;			/**< New material name */
};

/**
 * @ingroup resource_tools
 * @brief Compile model parameters
 */
struct resourceToolCompileModelParams_t
{
	bool							bCombineModels;			/**< If it is TRUE, combines all models into a single model */
	axisUp_t						axisUp;					/**< Axis up */
	const achar*					pMaterialsDir;			/**< Path to a directory where the model materials should be located */
	const achar*					pSrcPath;				/**< Paths to source model */
	const achar*					pDestPath;				/**< Path to a destination model (without file extension) */
	resourceToolRenamedMaterial_t*	pRenamedMaterials;		/**< Renamed materials */
	uint32							numRenamedMaterials;	/**< Renamed materials number */
};

/**
 * @ingroup resource_tools
 * @brief Model tool interface
 */
class IModelTool : public IAppSystem
{
public:
	/**
	 * @brief Compile a model and save in the file system
	 * @param compileParams		Compile parameters
	 * @return Return TRUE if the model successfully compiled, otherwise FALSE
	 */
	virtual bool CompileModel( const resourceToolCompileModelParams_t& compileParams ) const = 0;
};

#endif // !IMODEL_TOOL_H