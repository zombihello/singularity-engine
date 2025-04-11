/**
 * @file
 * @addtogroup resource_tools resource_tools
 */

#ifndef IMATERIAL_TOOL_H
#define IMATERIAL_TOOL_H

#include "stdlib/math/math.h"
#include "appframework/iappsystem.h"

/**
 * @ingroup resource_tools
 * @brief Material tool interface version
 */
#define MATERIAL_TOOL_INTERFACE_VERSION "SMaterialTool001"

/**
 * @ingroup resource_tools
 * @brief Material tool variable type
 */
enum resourceToolMaterialVarType_t
{
	RESOURCE_TOOL_MATERIAL_VAR_TYPE_UNDEFINED,		/**< Undefined */
	RESOURCE_TOOL_MATERIAL_VAR_TYPE_BOOL,			/**< Bool */
	RESOURCE_TOOL_MATERIAL_VAR_TYPE_INT,			/**< Integer */
	RESOURCE_TOOL_MATERIAL_VAR_TYPE_FLOAT,			/**< Float */
	RESOURCE_TOOL_MATERIAL_VAR_TYPE_VECTOR_2D,		/**< Vector 2D */
	RESOURCE_TOOL_MATERIAL_VAR_TYPE_VECTOR_3D,		/**< Vector 3D */
	RESOURCE_TOOL_MATERIAL_VAR_TYPE_VECTOR_4D,		/**< Vector 4D */
	RESOURCE_TOOL_MATERIAL_VAR_TYPE_MATRIX,			/**< Matrix */
	RESOURCE_TOOL_MATERIAL_VAR_TYPE_STRING,			/**< String */
	RESOURCE_TOOL_MATERIAL_VAR_TYPE_TEXTURE,		/**< Texture */
	RESOURCE_TOOL_MATERIAL_VAR_TYPE_MATERIAL		/**< Material */
};

/**
 * @ingroup resource_tools
 * @brief Material variable
 */
struct resourceToolMaterialVar_t
{
	const achar*					pName;				/**< Variable name */
	resourceToolMaterialVarType_t	type;				/**< Variable type */
	union
	{
		bool						boolValue;			/**< Bool value */
		int32						intValue;			/**< Integer value */
		float						floatValue;			/**< Float value */
		vec2_t						vector2DValue;		/**< Vector 2D value */
		vec3_t						vector3DValue;		/**< Vector 3D value */
		vec4_t						vector4DValue;		/**< Vector 4D value */
		matrix_t					matrixValue;		/**< Matrix value */
		const achar*				pStringValue;		/**< String value */
		const achar*				pTextureValue;		/**< Texture value */
		const achar*				pMaterialValue;		/**< Material value */
	};
};

/**
 * @ingroup resource_tools
 * @brief Compile material parameters
 */
struct resourceToolCompileMaterialParams_t
{
	const achar*						pDestPath;		/**< Path to a destination material (without file extension) */
	const achar*						pShaderName;	/**< Shader name */
	const resourceToolMaterialVar_t*	pVars;			/**< Material variables */
	uint32								numVars;		/**< Material variables number */
};

/**
 * @ingroup resource_tools
 * @brief Material tool interface
 */
class IMaterialTool : public IAppSystem
{
public:
	/**
	 * @brief Compile a material and save in the file system
	 * @param compileParams		Compile parameters
	 * @return Return TRUE if the material successfully compiled, otherwise FALSE
	 */
	virtual bool CompileMaterial( const resourceToolCompileMaterialParams_t& compileParams ) const = 0;
};

#endif // !IMATERIAL_TOOL_H